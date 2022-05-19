/*
 * tpc_user.cpp
 *
 *  Created on: Jul 5, 2016
 *      Author: pengbobo
 */
extern "C"{
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>  /* netdb is necessary for struct hostent */
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>

};


/*
 	fabcode	Location
	0000	SCC
    0001	JCET
	0100	AMD
	0101	NT
	1100	ZTE
	1101	ADV
  */

//#define PORT 65533  /* server port */
#define PORT 6716  /* server port */

#define MAXDATASIZE 1024

//#define HOST_IP_ADDR_ZTE "10.11.30.253"  // ZTE server intranet IP
#define HOST_IP_ADDR_ZTE "10.89.100.102"  // ZTE server intranet IP
#define HOST_IP_ADDR_AMD "10.72.2.128"  // AMD server intranet IP
//#define HOST_IP_ADDR_AMD "10.89.100.103"  // AMD server intranet IP
#define HOST_IP_ADDR_SCC "10.20.32.149"    // SCC server intranet IP;
#define HOST_IP_ADDR_JCET "172.17.255.86"    // JCET server intranet IP;
#define HOST_IP_ADDR_NT "172.15.19.253"  // NT server intranet IP;
#define HOST_IP_ADDR_ADV ""  // ADV server intranet IP;


#define CONNECT_TIMEOUT_VAL (200 ms)  // 200 ms time out

#include "testsuitecontrol_api.hpp"
#include <iostream>
#include <stdexcept>
using namespace testsuitecontrol;

#include<time.h>
#include "testmethod.hpp"
#include "Common.h"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;


using namespace V93kLimits;

/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */


class tpc_user: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */

	// static value; only one variable for all class instance.
	// must be intialized outside class declaration
    static int sockfd;    /* files descriptors as global variable in the class*/
    static unsigned int fabcode;  // 2 bit fab code: 00 SCC; 01 AMD; 10 reserved; 11 ZTE lab
    static int CountCalling;
    string chip_code;
  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
	  CountCalling = 0;
	  addParameter("chip_code","string",&chip_code,testmethod::TM_PARAMETER_INPUT)
						   .setOptions("6716:6718:6716V2")
		  	  	   	   	   .setDefault("");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
    {
	  INT flag_connect=0;
	  ARRAY_I sites,chip_id;

	  initializeSession();
	  INT Is_first_connect=0;
//	  flag_connect=0;
	  GET_USER_FLAG("Is_first_connect",&Is_first_connect);



	  ON_FIRST_INVOCATION_BEGIN();

	  	  CountCalling++;
	  	  printf("calling count is %d \n", CountCalling);
	  	  if(Is_first_connect){
	  		flag_connect = ConnectChipIDServer(); // ConnectSrv();  // establish server connection
	  		if(flag_connect==0) SET_USER_FLAG("Is_first_connect",0);
	  	  }
	  	  else{
	  		cout<<"***************************"<<endl;
	  		flag_connect = ConnectChipIDServer_Block();
	  	  }

	  	  if(flag_connect==1){
	  		  cout<<"Error!!! the server maybe has down or internet is disconnect or the IP address is wrong!"<<endl;
	  		  exitProcess(3);
	  	  }
	  	  int num_of_sites = GET_ACTIVE_SITES(sites);
		  chip_id.resize(num_of_sites);
		  chip_id.init(0);
		  flag_connect=GetChipID(num_of_sites,chip_id);  // get chip id from server


		  FOR_EACH_SITE_BEGIN();
		    int chip_index;
		  	int site_index=CURRENT_SITE_NUMBER();
		  	for(size_t i=0;i<sites.size();i++){
		  		if(sites[i]==site_index){
		  			chip_index=i;
		  			break;
		  		}
		  	}

		  	cout<<"the left keys ="<<dec<<268435456-chip_id[chip_index]<<endl;

		  	chip_id32[chip_index]=chip_id[chip_index]|(fabcode << 28);  // aign the fab code at bit 31,30,29,28

		  	cout<<"chip_id32["<<chip_index<<"]=0x"<<hex<<chip_id32[chip_index]<<"of site"<<site_index<<endl;

		  	SET_USER_DOUBLE("chip_id",chip_id32[chip_index]);

		  FOR_EACH_SITE_END();

		  if(flag_connect==1){
			  cout<<"Error!!! the server maybe has down or internet is disconnect or the IP address is wrong!"<<endl;
			  exitProcess(3);
		  }

		  DetachChipIDServer(); // close server connection
	  ON_FIRST_INVOCATION_END();

	  return;

    }


int ConnectSrv(const char * ip_addr)
{
	int flags; // file descriptor flags
	int n; // connect return value
	struct timeval tval;  // timeout value to wait for the connect
	fd_set rset, wset;

    struct sockaddr_in server;
	struct timeval t_start,t_end;

	long cost_time = 0;
	static int SrvConFlag = 0;  // flag to indicate if server is already connected. 1=connected, 0=not connected

	gettimeofday(&t_start, NULL);
    long start = ((long)t_start.tv_sec)*1000+(long)t_start.tv_usec/1000;

    printf("sockfd is %d at beginning\n", sockfd);

	SrvConFlag = 0;
	if(SrvConFlag == 0)
	{
		if((sockfd=socket(AF_INET,SOCK_STREAM, 0))==-1)
		 {
			 printf("socket() error\n");
			 return 1;
		 }
	     printf("sockfd generated is %d \n", sockfd);
		 flags = fcntl(sockfd, F_GETFL, 0);
		 fcntl(sockfd, F_SETFL, flags|O_NONBLOCK);  // non blocking connection
		 FD_ZERO(&rset);
		 FD_SET(sockfd, &rset);
		 wset = rset;
		 tval.tv_sec = 0;  // all zero; do not wait; wait outside
		 tval.tv_usec = 0;
	}

	printf("enter connection process, ip address: %s\n", ip_addr);

    bzero(&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    if((inet_pton(AF_INET, ip_addr, &server.sin_addr))!=1)
    {
        printf("server ip address error\n");
        return 1;
    }

    //printf("ip %s \n", inet_ntoa(server.sin_addr));


	if((SrvConFlag == 0) && ((n=connect(sockfd, (struct sockaddr *)&server, sizeof(server)))< 0))
	{
		//printf ("connecting zte %d \n", n );
		if(errno != EINPROGRESS)
		{
			printf ("error num is %d\n", errno);
			return 1;
		}
	}
		if((SrvConFlag == 0) && (n == 0))
		{
	    	SrvConFlag = 1;
		}
		else
		{
			WAIT_TIME(CONNECT_TIMEOUT_VAL);
			if((n = select(sockfd + 1, &rset, &wset, NULL, &tval)) == 0)
			{
				// time out
				close(sockfd);
				printf("Time out! sockfd is %d\n", sockfd);

			}
			else
			{
				printf ("connection established on %s with descriptor count %d\n", ip_addr, n);
				SrvConFlag = 1;
				if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset))
				{

				}
				else
					printf("select error: sockfd not set\n");

			}
		}
///////////////////////////////////

	if(SrvConFlag == 1)
	{
		gettimeofday(&t_end, NULL);
		long end = ((long)t_end.tv_sec)*1000+(long)t_end.tv_usec/1000;
		  //calculate time slot
		cost_time = end - start;
//		printf("connect sucessfully after %ldms\n",cost_time);
		fcntl(sockfd, F_SETFL, flags); // restore flags
	}
	else
		return 1;

    return 0;
}

void DetachChipIDServer()
{
	   close(sockfd);  // do not close the socket after one touch down
}

int ConnectChipIDServer()
{
    //int sockfd;    /* files descriptors */

	struct timeval t_start,t_end;

	long start_time = 0;
	long end_time = 0;
	long cost_time = 0;

	gettimeofday(&t_start, NULL);
	start_time = ((long)t_start.tv_sec)*1000+(long)t_start.tv_usec/1000;

	/*
	 	fabcode	Location
		0000	SCC
	    0001	JCET
		0100	AMD
		0101	NT
		1100	ZTE
		1101	ADV
	  */

	if(ConnectSrv(HOST_IP_ADDR_SCC) == 0)
	{
		fabcode = 0;//0000
		SET_USER_STRING("Server_IP_ADDR",HOST_IP_ADDR_SCC);
	}
	else if(ConnectSrv(HOST_IP_ADDR_JCET) == 0)
	{
		fabcode = 1;//0001
		SET_USER_STRING("Server_IP_ADDR",HOST_IP_ADDR_JCET);
	}
	else if(ConnectSrv(HOST_IP_ADDR_AMD) == 0)
	{
		fabcode = 4;//0100
		SET_USER_STRING("Server_IP_ADDR",HOST_IP_ADDR_AMD);
	}
	else if(ConnectSrv(HOST_IP_ADDR_NT) == 0)
	{
		fabcode = 5;//0101
		SET_USER_STRING("Server_IP_ADDR",HOST_IP_ADDR_NT);
	}
	else if(ConnectSrv(HOST_IP_ADDR_ZTE) == 0)
	{
		fabcode = 12;//1100
		SET_USER_STRING("Server_IP_ADDR",HOST_IP_ADDR_ZTE);
	}
	else if(ConnectSrv(HOST_IP_ADDR_ADV) == 0)
	{
		fabcode = 13;//1101
		SET_USER_STRING("Server_IP_ADDR",HOST_IP_ADDR_ADV);
	}
	else{
		return 1;
	}

	gettimeofday(&t_end, NULL);
	end_time = ((long)t_end.tv_sec)*1000+(long)t_end.tv_usec/1000;
	cost_time = end_time - start_time;
	printf("connect sucessfully after %ldms\n",cost_time);
	printf ("fab code is %d\n", fabcode);

    return 0;
}

int GetChipID(INT num_of_sites,ARRAY_I &chip_id)
{
	int num;
    unsigned char buf[MAXDATASIZE];    /* buf will store received text */
    char str[] = "chip_id\n";
    //get chip_id for each active sites
    printf("sending requeset... \n");
    for(int i=0;i<num_of_sites;i++){
//		if((num=send(sockfd,str,sizeof(str),0))==-1){
    	if((num=send(sockfd,chip_code.c_str(),sizeof(chip_code.c_str()),0))==-1){
			printf("send() error\n");
			return 1;
		}
		if((num=recv(sockfd,buf,MAXDATASIZE,0))==-1)
		{
			printf("recv() error\n");
			return 1;
		}
		chip_id[i]=(buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|(buf[0]);
		printf("site %d :\nchip_id=%d\n",i+1,chip_id[i]);

    }



    return 0;
}

int ConnectChipIDServer_Block()
{
    //int sockfd;    /* files descriptors */
	const char * ip_addr;
	int n; // connect return value
//	struct timeval tval;  // timeout value to wait for the connect

    struct sockaddr_in server;
	struct timeval t_start,t_end;

	long start_time = 0;
	long end_time = 0;
	long cost_time = 0;

	STRING tmp_ip_addr;
	GET_USER_STRING("Server_IP_ADDR",tmp_ip_addr);

	/*
	 	fabcode	Location
		0000	SCC
	    0001	JCET
		0100	AMD
		0101	NT
		1100	ZTE
		1101	ADV
	  */
	if(tmp_ip_addr==HOST_IP_ADDR_SCC){
		fabcode = 0;//0000
	}
	else if(tmp_ip_addr==HOST_IP_ADDR_JCET)
	{
		fabcode = 1;//0001
	}
	else if(tmp_ip_addr == HOST_IP_ADDR_AMD)
	{
		fabcode = 4;//0100
	}
	else if(tmp_ip_addr== HOST_IP_ADDR_NT)
	{
		fabcode = 5;//0101
	}
	else if(tmp_ip_addr== HOST_IP_ADDR_ZTE)
	{
		fabcode = 12;//1100
	}
	else if(tmp_ip_addr == HOST_IP_ADDR_ADV)
	{
		fabcode = 13;//1101
	}
	else {
		return 1;
	}
	ip_addr=tmp_ip_addr.c_str();

	gettimeofday(&t_start, NULL);
	start_time = ((long)t_start.tv_sec)*1000+(long)t_start.tv_usec/1000;

	if((sockfd=socket(AF_INET,SOCK_STREAM, 0))==-1){
		 printf("socket() error\n");
		 return 1;
	}
	printf("sockfd generated is %d \n", sockfd);

	int flags = fcntl(sockfd,F_GETFL,0);
	fcntl(sockfd,F_SETFL,flags & ~O_NONBLOCK);//block mode
	printf("enter connection process, ip address: %s\n", ip_addr);
	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	if((inet_pton(AF_INET, ip_addr, &server.sin_addr))!=1)
	{
		printf("server ip address error\n");
		return 1;
	}
	if((n=connect(sockfd, (struct sockaddr *)&server, sizeof(server)))< 0){
		if(errno != EINPROGRESS)
		{
			printf ("error num is %d\n", errno);
			return 1;
		}
	}
	gettimeofday(&t_end, NULL);
	end_time = ((long)t_end.tv_sec)*1000+(long)t_end.tv_usec/1000;
	cost_time = end_time - start_time;
	printf("connect sucessfully after %ldms\n",cost_time);
	printf ("fab code is %d\n", fabcode);

    return 0;
}

  virtual void postParameterChange(const string& parameterIdentifier)
  {
    //Add your code
    //Note: Test Method API should not be used in this method!
    return;
  }

  /**
   *This function will be invoked once the Select Test Method Dialog is opened.
   */
  virtual const string getComment() const
  {
    string comment = " please add your comment for this method.";
    return comment;
  }
};

// intialize the static variable; must do;
int tpc_user::sockfd = 0;
unsigned int tpc_user::fabcode = 0;
int tpc_user::CountCalling = 0;

REGISTER_TESTMETHOD("bm_ac_tml.tpc_user", tpc_user);





