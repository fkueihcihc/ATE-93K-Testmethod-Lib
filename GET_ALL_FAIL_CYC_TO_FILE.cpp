// GET_ALL_FAIL_CYC_TO_FILE.cpp


//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"
#include "tml_def.h"

using namespace std;

/**
 *Testmethod class.
 *
 *For each testsuite using this testmethod, one object of this
 *class is created.
 */
class GET_ALL_FAIL_CYC_TO_FILE: public testmethod::TestMethod
{
protected:
//Add your class member variable here.

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
}

/**
 *This test is invoked per site.
 */
virtual void run()
{
    // add your code
	string result;
	    string count;
	    char *start_cycle;
	    char *end_cycle;
	    char err_acq[500],ercc_str[500],ercy_str[500];
	   
	    
	    FILE *pFile;
	    pFile=fopen("/tmp/failcyc.log","w");
	    const string mylabel=Primary.getLabel();
	   //  GET_TESTSUITE_NAME(suite_name);
	   //  cerr<<suite_name<<": \n";
	  //  cout<<"Pattern Name="<<mylabel<<endl;
	// error map can only get 8192 detail information when running functional test for one time;
	// "for" loop is used to get all error information in 300x8192 cycles.
	    for (int i=0;i<1000;i++)
	    {
	        sprintf(err_acq,"SQGB ACQF,%d,(@)\n",i*8192);
	        FW_TASK(err_acq);

	        FUNCTIONAL_TEST();
	        sprintf(ercc_str,"ERCC? ERCP,%d,%d,(@)\n",i*8192,8192);
	//        sprintf(ercy_str,"ERCY? ALL,%d,,%d,(@)\n",i*8192,((i+1)*8192-1));
	  	sprintf(ercy_str,"ERCY? ALL,%d,%d,,(@)\n",i*8192,8192); 

	 
	    FW_TASK(ercc_str,count);
	    FW_TASK(ercy_str,result);
	    
	    //fprintf(pFile,"Pattern Name=",mylabel);
	    fprintf(pFile,"The COUNT: %s\n",count.c_str());
	    fprintf(pFile,"The RESULT: %s\n",result.c_str());
	    }
	    fclose(pFile);

	    
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
REGISTER_TESTMETHOD("bm_ac_tml.GET_ALL_FAIL_CYC_TO_FILE",GET_ALL_FAIL_CYC_TO_FILE);

