#include "testmethod.hpp"
#include "tml_def.h"
//for test method API interfaces
#include "mapi.hpp"
#include <libcicpi.h>
#include "pmicComCode.h"
#include <string>
using namespace std;
using namespace V93kLimits;


/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class Tsensor_bandgap: public testmethod::TestMethod {

protected:
  int debug_mode;
  STRING mTEMP_P;
  STRING mTEMP_N;
  virtual void initialize()
  {
	  	addParameter("debug_mode", "int", &debug_mode)
	                 .setDefault("1")
	                 .setOptions("0:1")
	                 .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");
		addParameter("PIN_TEMP_P",
				"string",
				&mTEMP_P,
				testmethod::TM_PARAMETER_INPUT)
	  			.setDefault("TEMP_P");
		addParameter("PIN_TEMP_N",
				"string",
				&mTEMP_N,
				testmethod::TM_PARAMETER_INPUT)
	  			.setDefault("TEMP_N");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
	STRING TestsuitName;
	SPMU_TASK badc_task;
	static data Vout;
	static vector<string> sListPins;

	ON_FIRST_INVOCATION_BEGIN();
		GET_TESTSUITE_NAME(TestsuitName);
		CONNECT();
		FUNCTIONAL_TEST();
		//use high accuracy voltage meter to measure bandgap voltage
		DigPin_Relay(mTEMP_P,"AC_OFF");
		DigPin_Relay(mTEMP_N,"AC_OFF");
		VM_BADC(badc_task,mTEMP_P, Vout,debug_mode,0, 1,16);
		VM_BADC(badc_task,mTEMP_N, Vout,debug_mode,0, 1,16);
	ON_FIRST_INVOCATION_END();

	int site_index=CURRENT_SITE_NUMBER()-1;
	if(debug_mode)
	{
		cout<<"Positive Voltage of TEMP_P"<<" = "<<Vout.get(mTEMP_P, site_index, 0)<<" V"<<endl;
		cout<<"Negative Voltage of TEMP_N"<<" = "<<Vout.get(mTEMP_N, site_index, 0)<<" V"<<endl;
		cout<<"Bandgap Voltage of TEMP"<<" = "<<Vout.get(mTEMP_P, site_index, 0) - Vout.get(mTEMP_N, site_index, 0)<<" V"<<endl;
	}
	TESTSET().cont(TRUE).judgeAndLog_ParametricTest("",mTEMP_P,tmLimits,Vout.get(mTEMP_P,site_index, 0));
	TESTSET().cont(TRUE).judgeAndLog_ParametricTest("",mTEMP_N,tmLimits,Vout.get(mTEMP_P,site_index, 0));
	TESTSET().cont(TRUE).judgeAndLog_ParametricTest("","Bandgap",tmLimits,Vout.get(mTEMP_P,site_index, 0) - Vout.get(mTEMP_N, site_index, 0));

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
REGISTER_TESTMETHOD("bm_ac_tml.Tsensor_bandgap", Tsensor_bandgap);


