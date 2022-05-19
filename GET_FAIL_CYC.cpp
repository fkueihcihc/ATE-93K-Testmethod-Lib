// GET_FAIL_CYC.cpp


//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"
#include "tml_def.h"

#include <cstring>
#include <fstream>
#include <iomanip>

using namespace std;



/**
 *Testmethod class.
 *
 *For each testsuite using this testmethod, one object of this
 *class is created.
 */

class GET_FAIL_CYC: public testmethod::TestMethod
{
protected:
//Add your class member variable here.
	string File_Path;

protected:
/**
 *Initialize the parameter interface to the testflow.
 *This method is called just once after a testsuite is created.
 *
 *Note: TestMethod API should not be used in this method.
 */
virtual void initialize()
{
  //Add your intialization code here.
	  addParameter("File_Path",
	               "string",
	               &File_Path)
	    .setDefault("/tmp");
}

/**
 *This test is invoked per site.
 */
virtual void run()
{
    // add your code
	/*
   string result;
    string count;
   string suite_name;

   int i;
    //CONNECT();
    //FUNCTIONAL_TEST();
   const string mylabel=Primary.getLabel();
  //  GET_TESTSUITE_NAME(suite_name);
  //  cerr<<suite_name<<": \n";
   cout<<"Pattern Name="<<mylabel<<endl;
  // fout<<"Pattern Name="<<mylabel<<endl;  
    FW_TASK("SQGB ACQF,0,(@)\n");
    FUNCTIONAL_TEST();
  	FW_TASK("ERCC? ERCP,0,950272,(@)\n",count);
    FW_TASK("ERCY? ALL,0,,400000,(@)\n",result);
    //FW_TASK("ERCC? ERCP,0,819200,(@)\n",count);
    //FW_TASK("ERCY? ALL,0,,8192,(@)\n",result);
        
    fprintf(stderr,"The COUNT:\n%s",count.c_str());
    PUT_DATALOG(count);
    fprintf(stderr,"The RESULT:\n%s",result.c_str());
    PUT_DATALOG(result);
    */
    //=File_Path.c_str();
    //const string mylabel=Primary.getLabel();
    char tmpchar[256];
    string tmpstr;
    const string mylabel=Primary.getLabel();
    ofstream fout;
//    tmpstr = File_Path+"/"+mylabel+"_Failcyc.txt";
    tmpstr = File_Path+"/"+"ZX6716_Failcyc.txt";
    const char *file_path = tmpstr.c_str();
   // fout.open("/tmp/PM.txt",ios_base::out|ios_base::app);
   fout.open(file_path,ios_base::out|ios_base::app);
   //fout.precision(3);
    // add your code
	//ofstream fout;
   string result;
    string count;
   string suite_name;
    //CONNECT();
    //FUNCTIONAL_TEST();
   FUNCTIONAL_TEST();
   PUT_DATALOG(mylabel);
  //  GET_TESTSUITE_NAME(suite_name);
  //  cerr<<suite_name<<": \n";
   cout<<"Pattern Name="<<mylabel<<endl;
   fout<<"Pattern Name="<<mylabel<<endl;
  // fout<<"Pattern Name="<<mylabel<<endl;  
    FW_TASK("SQGB ACQF,0,(@)\n");
    FUNCTIONAL_TEST();
    FW_TASK("ERCC? ERCP,0,950272,(@)\n",count);
    FW_TASK("ERCY? ALL,0,,40000,(@)\n",result);
    fprintf(stderr,"The COUNT:\n%s",count.c_str());
    PUT_DATALOG(count);
    fout<<count<<endl;
    fprintf(stderr,"The RESULT:\n%s",result.c_str());
   // DISCONNECT();
    PUT_DATALOG(result);
    fout<<result<<endl; 
    // DISCONNECT();
    fout.close(); 
    return;
}

/**
 *This function will be invoked once the specified parameter's value is changed.
 *@param parameterIdentifier
 *
 *Note: TestMethod API should not be used in this method.
 */
virtual void postParameterChange(const string& parameterIdentifier)
{
  //add your code
}

};
REGISTER_TESTMETHOD("bm_ac_tml.GET_FAIL_CYC",GET_FAIL_CYC);


class GET_FAIL_CYC_LOOP: public testmethod::TestMethod
{
protected:
//Add your class member variable here.
	string File_Path;
    int LOOP_N;
protected:
/**
 *Initialize the parameter interface to the testflow.
 *This method is called just once after a testsuite is created.
 *
 *Note: TestMethod API should not be used in this method.
 */
virtual void initialize()
{
  //Add your intialization code here.
	  addParameter("File_Path",
	               "string",
	               &File_Path)
	    .setDefault("/tmp");
	  addParameter("LOOP_N",
	               "int",
	               &LOOP_N)
	    .setDefault("5");
}

/**
 *This test is invoked per site.
 */
virtual void run()
{
   
    char tmpchar[256];
    string tmpstr;
    const string mylabel=Primary.getLabel();
    ofstream fout;
    tmpstr = File_Path+"/"+mylabel+"_Failcyc.txt";
    //cout<<tmpstr<<endl<<endl;
    //const char *file_path = File_Path.c_str();
   fout.open(tmpstr.c_str(),ios_base::out|ios_base::app);
   string result;
    string count;
   string suite_name;
    //CONNECT();
    //FUNCTIONAL_TEST();
   //FUNCTIONAL_TEST();
   for(int i=1;i<=LOOP_N;i++)
   {
	   PUT_DATALOG(mylabel);
	  //  GET_TESTSUITE_NAME(suite_name);
	  //  cerr<<suite_name<<": \n";
	    sprintf(tmpchar,"Loop_n = %d",i);
	    tmpstr=tmpchar;
	    PUT_DATALOG(tmpstr);  
	    cout<<"Loop_n = "<<i<<endl;
	    fout<<"Loop_n = "<<i<<endl;
	   cout<<"Pattern Name="<<mylabel<<endl;
	   fout<<"Pattern Name="<<mylabel<<endl;
	  // fout<<"Pattern Name="<<mylabel<<endl;  
	    FW_TASK("SQGB ACQF,0,(@)\n");
	    //DISCONNECT();
	    CONNECT();
	    FUNCTIONAL_TEST();
	    FW_TASK("ERCC? ERCP,0,950272,(@)\n",count);
	    FW_TASK("ERCY? ALL,0,,40000,(@)\n",result);
	    fprintf(stderr,"The COUNT:\n%s",count.c_str());
	    PUT_DATALOG(count);
	    fout<<count<<endl;
	    fprintf(stderr,"The RESULT:\n%s",result.c_str());
	   // DISCONNECT();
	    PUT_DATALOG(result);
	    fout<<result<<endl; 
	    // DISCONNECT();	   
   }

    fout.close(); 
    return;
}

/**
 *This function will be invoked once the specified parameter's value is changed.
 *@param parameterIdentifier
 *
 *Note: TestMethod API should not be used in this method.
 */
virtual void postParameterChange(const string& parameterIdentifier)
{
  //add your code
}

};
REGISTER_TESTMETHOD("bm_ac_tml.GET_FAIL_CYC_LOOP",GET_FAIL_CYC_LOOP);


class GET_FAIL_CYC_LOOP_PORT: public testmethod::TestMethod
{
protected:
//Add your class member variable here.
	string File_Path;
    int LOOP_N;
protected:
/**
 *Initialize the parameter interface to the testflow.
 *This method is called just once after a testsuite is created.
 *
 *Note: TestMethod API should not be used in this method.
 */
virtual void initialize()
{
  //Add your intialization code here.
	  addParameter("File_Path",
	               "string",
	               &File_Path)
	    .setDefault("/tmp");
	  addParameter("LOOP_N",
	               "int",
	               &LOOP_N)
	    .setDefault("5");
}

/**
 *This test is invoked per site.
 */
virtual void run()
{

    char tmpchar[256];
    string tmpstr;
    const string mylabel=Primary.getLabel();
    ofstream fout;
    tmpstr = File_Path+"/"+mylabel+"_Failcyc.txt";
    //cout<<tmpstr<<endl<<endl;
    //const char *file_path = File_Path.c_str();
   fout.open(tmpstr.c_str(),ios_base::out|ios_base::app);
   string result;
    string count;
   string suite_name;
    //CONNECT();
    //FUNCTIONAL_TEST();
   //FUNCTIONAL_TEST();
   for(int i=1;i<=LOOP_N;i++)
   {
	   PUT_DATALOG(mylabel);
	  //  GET_TESTSUITE_NAME(suite_name);
	  //  cerr<<suite_name<<": \n";
	    sprintf(tmpchar,"Loop_n = %d",i);
	    tmpstr=tmpchar;
	    PUT_DATALOG(tmpstr);
	   // cout<<"Loop_n = "<<i<<endl;
	    fout<<"Loop_n = "<<i<<endl;
	  // cout<<"Pattern Name="<<mylabel<<endl;
	   fout<<"Pattern Name="<<mylabel<<endl;
	  // fout<<"Pattern Name="<<mylabel<<endl;
	    FW_TASK("SQGB ACQF,0,(pDFT_MBIST)\n");
	    //DISCONNECT();
	    CONNECT();
	    FUNCTIONAL_TEST();
	    FW_TASK("ERCC? ERCP,0,950272,(pDFT_MBIST)\n",count);
	    FW_TASK("ERCY? ALL,0,,40000,(pDFT_MBIST)\n",result);
	    //fprintf(stderr,"The COUNT:\n%s",count.c_str());
	    PUT_DATALOG(count);
	    fout<<count<<endl;
	    //fprintf(stderr,"The RESULT:\n%s",result.c_str());
	   // DISCONNECT();
	    PUT_DATALOG(result);
	    fout<<result<<endl;
	    // DISCONNECT();
   }

    fout.close();
    return;
}

/**
 *This function will be invoked once the specified parameter's value is changed.
 *@param parameterIdentifier
 *
 *Note: TestMethod API should not be used in this method.
 */
virtual void postParameterChange(const string& parameterIdentifier)
{
  //add your code
}

};
REGISTER_TESTMETHOD("bm_ac_tml.GET_FAIL_CYC_LOOP_PORT",GET_FAIL_CYC_LOOP_PORT);


