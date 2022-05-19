#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>


//#define PORT 65533  //6718
#define PORT 6716//6716

#define BACKLOG 1
#define MAXRECVLEN 100

int main(int argc, char *argv[])
{
    char buf[MAXRECVLEN];
    int listenfd, connectfd;  /* socket descriptors */
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    socklen_t addrlen;
    static unsigned int chip_id = 0;
    unsigned int total=pow(2.0,28);//2^29
    unsigned int left=0;
    char pPath[200]="";
    FILE *fp;



    /* Create TCP socket */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        /* handle exception */
        perror("socket() error. Failed to initiate a socket");
        exit(1);
    }

    /* set socket option */
    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        /* handle exception */
        perror("Bind() error.");
        exit(1);
    }

    if(listen(listenfd, BACKLOG) == -1)
    {
        perror("listen() error. \n");
        exit(1);
    }

    addrlen = sizeof(client);

start:    printf(" *******Warning!!!!*******\n *******the chip id sever is running...*******\n *******please don't close the Konsole!!!*******\n");

    while(1){
        if((connectfd=accept(listenfd,(struct sockaddr *)&client, &addrlen))==-1)
        {
            perror("accept() error. \n");
            exit(1);
        }

        struct timeval tv;
        gettimeofday(&tv, NULL);
        printf("You got a connection from client's ip %s, port %d at time %ld.%ld\n",inet_ntoa(client.sin_addr),htons(client.sin_port), tv.tv_sec,tv.tv_usec);

        int iret=-1;
        while(1)
        {
            iret = recv(connectfd, buf, MAXRECVLEN, 0);
            if(iret>0)
            {
                printf("the receive code is %s\n", buf);

                sprintf(pPath,"./source_%s.txt",buf);
//                if(0==strcmp(buf,"6716")){
//                	pPath="./source_6716.txt";
//                }
//                else if(0==strcmp(buf,"6718")){
//                	pPath="./source_6718.txt";
//                }
                printf("%s%s\n", "the chip id file is ",pPath);
            }
            else
            {
                close(connectfd);
                break;
            }
        	/*get the inital value for chip_id from file source.txt */


            if( (fp=fopen(pPath,"r"))==NULL )
            {
                printf("open chip id file fail the expected file doesn't exist!");

                //exit(0);

		goto start;
            }
            while(fscanf(fp,"%d%*[^0-9]",&chip_id)>0)
                   printf("the latest used code is %d ",chip_id);
               fclose(fp);
               printf("\n");
            /* print client's ip and port */
            chip_id++;
            left=total-chip_id -1;
            printf("send message chip_id=%d\n", chip_id);
            printf("the number of left Keys is %d\n",left);

            /*backup the vaule in source.txt*/
            char c[50];

            sprintf(c,"cp -rf %s %s.bak",pPath,pPath);
            printf("cp -rf %s %s.bak\n", pPath,pPath);

            system(c);
//            if(0==strcmp(buf,"6716")){
//            	system("cp -rf source_6716.txt source_6716.txt.bak ");
//            }
//            else if(0==strcmp(buf,"6718")){
//            	system("cp -rf source_6718.txt source_6718.txt.bak ");
//            }

	/*overwrite the vaule in source.txt*/
       if( (fp=fopen(pPath,"w"))==NULL )
       {
           printf("open source.txt fail maybe no file created!");
           fclose(fp);
           exit(0);
       }

       fprintf(fp,"%d",chip_id);//write the int type value
       fclose(fp);

       send(connectfd, &chip_id, sizeof(unsigned int), 0); /* send to the client welcome message */
	   // send(connectfd, &left, sizeof(unsigned int), 0);

        }
    }
    close(listenfd); /* close listenfd */
    return 0;
}
