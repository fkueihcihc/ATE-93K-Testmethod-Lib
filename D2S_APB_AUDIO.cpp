#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;
#include "d2s_APB_w.h"
#include "generic_d2s/d2sFramework.h"

#define __D2SWRITEBLOCKCONFIG__     d2s_APB_w &myD2S = d2s_APB_w::Instance();
using namespace V93kLimits;
/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class D2S_APB_AUDIO: public testmethod::TestMethod {

protected:
	  int 		D2SMode;
	  int		Debug_Mode;
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
	 addParameter("D2SMode", "int", &D2SMode)
				   .setDefault("1")
				   .setOptions("1:2")
				   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");
	 addParameter("Debug_Mode", "int", &Debug_Mode)
	                 .setDefault("1")
	                 .setOptions("0:1")
	                 .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");
  }

  /**
   *This test is invoked per site.
   */
	virtual void run() {

		  __D2SWRITEBLOCKCONFIG__
		d2sFramework D2S_APB_fr;
		d2s_APB_w& d2s_AUDIO_APB = d2s_APB_w::Instance();



		D2S_APB_fr.registerTransactionPort(d2s_AUDIO_APB);

//		D2S_APB_fr.registerTransactionPort(port_freq);

		  d2sFrameWorkModeType::Enum d2s_mode;

		  switch ( D2SMode ){
					case 0:
					case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
					case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
				}

		ON_FIRST_INVOCATION_BEGIN();


		CONNECT();
		D2S_APB_fr.d2s_LABEL_BEGIN("d2s_AUDIO_APB_burst",d2s_mode);

		d2s_AUDIO_APB.writeDynamic(1,1,"d2s_AUDIO_APB_burst");//

//		d2s_AUDIO_APB.write(0x23, 0x10);
//		d2s_AUDIO_APB.write(0x23, 0x23);



		D2S_APB_fr.d2s_LABEL_END();
		ON_FIRST_INVOCATION_END();

		string TestSuiteName;
		GET_TESTSUITE_NAME(TestSuiteName);
//		TestSet.cont(true).TEST("",TestSuiteName,tmLimits, flag);

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
REGISTER_TESTMETHOD("bm_analog_tml.D2S_APB_AUDIO", D2S_APB_AUDIO);
