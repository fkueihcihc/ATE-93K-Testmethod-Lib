#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;
#include "TMUtils.h"
#include "Common.h"
using namespace V93kLimits;

/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class MIPI_test: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
	  double  	Sample_period_ns;
	  int  		Sample_size, iDebug, FFT_MODE;//, sCycle_number;
	  string  	sVecVar, sPLLLock;//, sLock, sLock_TestItem


  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
	  addParameter("Sample_Period_ns", "double", &Sample_period_ns)
	  		.setDefault("2.5")
	  	    .setComment("the sample period of digital capture");

	  addParameter("FFT_MODE","int",&FFT_MODE)
	  	    .setDefault("1")
	  	    .setComment("the FFT mode");

	  addParameter("Sample_Size",     "int",    &Sample_size)
	  	    .setDefault("1024")
	  	    .setComment("the sample size of digital capture");

	  addParameter("Capture_Signal",       "string",    &sVecVar)
	  	    .setDefault("")
	  	    .setComment("the list of test variable, separated by comma");

//	  addParameter("Lock_Signal", "string", &sPLLLock)
//	  	    .setDefault("")
//	  	    .setComment("the list of frequency test name, separated by comma");

	  addParameter("Debug_Mode", "int", &iDebug)
	  		.setDefault("1")
	  		.setOptions("0:1")
	  		.setComment("debug mode  0:no_need_ouput, 1:output_to_ui_and_signal_analyzer");
	    }



  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
  	  int           iListSize;
  	  int			site_number;
//  	  ARRAY_I		    pll_lock;
  	  ARRAY_I 		aiCapData, aiAPLLDATA, aiAPLLCAP1, aiAPLLCAP2;
  	  static string sTsName;
  	  ARRAY_D	    dFrequency;
  	  vector<string> VecVar_list,PLLLock_list;

      tokenize(sVecVar, VecVar_list, ",");
//      tokenize(sPLLLock, PLLLock_list, ",");
	  //iListSize = VecVar_list.size();
	  iListSize = VecVar_list.size();

	  //cout<<"iListSize = "<<iListSize<<endl;

	  aiCapData.resize(iListSize);
	  dFrequency.resize(iListSize);
//	  pll_lock.resize(iListSize);
		  //GET_TESTSUITE_NAME(sTsName);

  	  ON_FIRST_INVOCATION_BEGIN();
		  CONNECT();
			//FUNCTIONAL_TEST();

		  DIGITAL_CAPTURE_TEST();
//  		  DISCONNECT();
  	  ON_FIRST_INVOCATION_END();

  	      site_number = CURRENT_SITE_NUMBER()-1;
  	      cout << "site :" << site_number <<endl;
  	      for(int i =0;i<iListSize;i++)
  	      {
//  	    	  pll_lock[i] = GET_FUNCTIONAL_RESULT(PLLLock_list[i]);
  	    	  aiCapData = VECTOR(VecVar_list[i]).getVectors();
  	    	  dFrequency[i] = Frequency(aiCapData, Sample_period_ns*1e-3, FFT_MODE);
			  if(iDebug)
			  {
				  PUT_DEBUG(VecVar_list[i],"timeDomain",  aiCapData);
				  cout  << "Frequency (" << VecVar_list[i] << ") = " << dFrequency[i] << "MHz"<< endl;
			  }
			  TestSet.cont(true).TEST("", "Frequency_"+VecVar_list[i], tmLimits, dFrequency[i]);
//			  TestSet.cont(true).TEST("", "Lock_"+PLLLock_list[i], tmLimits, pll_lock[i]);
  	      }


//  		  if(iDebug)
//  		  {
//  			  cout <<"PLL_LOCK Result = "<<pll_lock[site_number]<<endl;
//  		  }
  		  //cout<<"PLLLock_list[iListSize-1]= "<<PLLLock_list[iListSize-1]<<endl;
  		  //TestSet.cont(true).TEST("", PLLLock_list[iListSize-1], tmLimits, pll_lock[site_number]);


    return;
  }

  /**
   *This function will be invoked once the specified parameter's value is changed.
   *@param parameterIdentifier
   */
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
REGISTER_TESTMETHOD("bm_ac_tml.MIPI_test", MIPI_test);
