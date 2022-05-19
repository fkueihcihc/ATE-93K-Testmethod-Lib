#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;
#include "../d2s_tml/d2s_video_apb.h"
#include "../d2s_tml/generic_d2s/d2sFramework.h"
using namespace V93kLimits;


#define wait_time_ms  1
/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class vdac_apb_cfg: public testmethod::TestMethod {

protected:
	int 		D2SMode;
	int			Debug_Mode;
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




		d2sFramework fr;
		d2s_vdac_apb& myD2S = d2s_vdac_apb::Instance();


		fr.registerTransactionPort(myD2S);


		d2sFrameWorkModeType::Enum d2s_mode;

		switch ( D2SMode ){
				case 0:
				case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
				case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
			}

		ON_FIRST_INVOCATION_BEGIN();

			fr.d2s_LABEL_BEGIN("video_cfg",d2s_mode);//add


				myD2S.execLabel("vdac_apb_init",710);

				myD2S.read(0xa6  ,"0xa6");
				myD2S.read(0xa1  ,"0xa1");

				myD2S.write(0xa0,0x73);
				myD2S.wait(10 ms);
				myD2S.write(0xa0,0x70);
//				myD2S.write(0xa6,0x0);//disable auto cable detect
//
//				myD2S.write(0xa1,0xc0);//modify the out of dac to 29.3mA(1.1V)

				if(Debug_Mode){
					myD2S.read(0xa6  ,"0xa6_1");
					myD2S.read(0xa1  ,"0xa1_1");
					myD2S.read(0xa0  ,"0xa0");
				}

			fr.d2s_LABEL_END();

		ON_FIRST_INVOCATION_END();

		if(Debug_Mode){
			cout << "0xa6 : "<< hex << myD2S.getReadValue("0xa6") <<endl; //
			cout << "0xa6_1 : "<< hex << myD2S.getReadValue("0xa6_1") <<endl; //

			cout << "0xa1 : "<< hex << myD2S.getReadValue("0xa1") <<endl; //
			cout << "0xa1_1 : "<< hex << myD2S.getReadValue("0xa1_1") <<endl; //
			cout << "0xa0 : "<< hex << myD2S.getReadValue("0xa0") <<endl; //
		}
//


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
REGISTER_TESTMETHOD("bm_ac_tml.vdac_apb_cfg", vdac_apb_cfg);
