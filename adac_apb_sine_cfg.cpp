#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;
#include "../d2s_tml/d2s_audio_apb.h"
#include "../d2s_tml/generic_d2s/d2sFramework.h"
using namespace V93kLimits;


#define wait_time_ms  1
/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class adac_apb_sine_cfg: public testmethod::TestMethod {

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
		d2s_adac_apb& myD2S_Audio = d2s_adac_apb::Instance();


		fr.registerTransactionPort(myD2S_Audio);


		d2sFrameWorkModeType::Enum d2s_mode;

		switch ( D2SMode ){
				case 0:
				case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
				case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
			}

		ON_FIRST_INVOCATION_BEGIN();

			fr.d2s_LABEL_BEGIN("audio_cfg",d2s_mode);//add


				myD2S_Audio.execLabel("APB_audio_init",710);

				myD2S_Audio.write(0x06,0x2);//reserve reg for harmonic


				myD2S_Audio.write(0x23  ,0x0);//pop sound stop working
				myD2S_Audio.wait(wait_time_ms ms);

				myD2S_Audio.write(0x22  ,0xfe);//quick charge
				myD2S_Audio.wait(wait_time_ms ms);

//
				myD2S_Audio.write(0x2a  ,0x01);//precharge
				myD2S_Audio.wait(wait_time_ms ms);


				myD2S_Audio.wait(25 ms);  //wait vcm charge
//
//

////
//
				myD2S_Audio.write(0x22  ,0x02);//2*I0 charge
				myD2S_Audio.wait(wait_time_ms ms);

				myD2S_Audio.write(0x2a  ,0x41);//precharge
				myD2S_Audio.wait(wait_time_ms ms);

				myD2S_Audio.write(0x2a  ,0x63);//DAC PATH ref voltage buffer enable
				myD2S_Audio.wait(wait_time_ms ms);


				myD2S_Audio.write(0x2b  ,0xcc);//DAC module ref voltage buffer enable and initial DAC module
				myD2S_Audio.wait(wait_time_ms ms);

				myD2S_Audio.write(0x2b  ,0xee);//DAC module enable
				myD2S_Audio.wait(wait_time_ms ms);

				myD2S_Audio.write(0x2e  ,0xd1);//DRVL module enable
				myD2S_Audio.wait(wait_time_ms ms);



				myD2S_Audio.write(0x2b  ,0xff);//DAC CLOCK enable
				myD2S_Audio.wait(wait_time_ms ms);

				myD2S_Audio.write(0x2e  ,0xfc);//DRVL module enable
				myD2S_Audio.wait(wait_time_ms ms);


				myD2S_Audio.write(0x2f  ,0x18);//DRVL gain:-3dBFS
				myD2S_Audio.wait(wait_time_ms ms);

//
////
				myD2S_Audio.write(0x30  ,0x18);//DRVR gain:-3dBFS
				myD2S_Audio.wait(wait_time_ms ms);

//

//
//
				myD2S_Audio.write(0x0,0x83);//bist work mode
				myD2S_Audio.write(0x7,0x50);//sine/L,R channel
////
////
				myD2S_Audio.wait(wait_time_ms ms);

				myD2S_Audio.execLabel("APB_audio_biston",10020);//biston

				myD2S_Audio.wait(500 ms);//wait output stable

				if(Debug_Mode){
				myD2S_Audio.read(0x22  ,"0x22");
				myD2S_Audio.read(0x0 ,"0x0");
				myD2S_Audio.read(0x7  ,"0x7");

				myD2S_Audio.read(0x30,"0x30");
				myD2S_Audio.read(0x2f,"0x2f");
				myD2S_Audio.read(0x2f,"0x2b");
				myD2S_Audio.read(0x2a  ,"0x2a");
				myD2S_Audio.read(0x22  ,"0x22");
				myD2S_Audio.read(0x23  ,"0x23");
				myD2S_Audio.read(0x4  ,"0x4");
				myD2S_Audio.read(0x6  ,"0x6");
				}

			fr.d2s_LABEL_END();
		ON_FIRST_INVOCATION_END();

		if(Debug_Mode){
			cout << "0x0 : "<< hex << myD2S_Audio.getReadValue("0x0") <<endl; //
			cout << "0x7 : "<< hex << myD2S_Audio.getReadValue("0x7") <<endl; //
			cout << "0x23 : "<< hex << myD2S_Audio.getReadValue("0x23") <<endl; //
			cout << "0x22 : "<< hex << myD2S_Audio.getReadValue("0x22") <<endl; //
			cout << "0x2a : "<< hex << myD2S_Audio.getReadValue("0x2a") <<endl; //
			cout << "0x30 : "<< hex << myD2S_Audio.getReadValue("0x30") <<endl; //
			cout << "0x2f : "<< hex << myD2S_Audio.getReadValue("0x2f") <<endl; //
			cout << "0x2b : "<< hex << myD2S_Audio.getReadValue("0x2b") <<endl; //
			cout << "0x4 : "<< hex << myD2S_Audio.getReadValue("0x4") <<endl; //
			cout << "0x6 : "<< hex << myD2S_Audio.getReadValue("0x6") <<endl; //

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
REGISTER_TESTMETHOD("bm_ac_tml.adac_apb_sine_cfg", adac_apb_sine_cfg);

class adac_apb_sine_bypass_cfg: public testmethod::TestMethod {

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


		int N=8192;
		int M=21;
		int code_in=0;

		d2sFramework fr;
		d2s_adac_apb& myD2S_Audio = d2s_adac_apb::Instance();


		fr.registerTransactionPort(myD2S_Audio);


		d2sFrameWorkModeType::Enum d2s_mode;

		switch ( D2SMode ){
				case 0:
				case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
				case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
			}

		ON_FIRST_INVOCATION_BEGIN();

			fr.d2s_LABEL_BEGIN("audio_bypass_cfg",d2s_mode);//add


				myD2S_Audio.execLabel("APB_audio_inti",710);


				myD2S_Audio.write(0x23  ,0x0);//pop sound stop working
				myD2S_Audio.wait(wait_time_ms ms);

				myD2S_Audio.write(0x22  ,0xfe);//quick charge
				myD2S_Audio.wait(wait_time_ms ms);

//
				myD2S_Audio.write(0x2a  ,0x01);//precharge
				myD2S_Audio.wait(wait_time_ms ms);


				myD2S_Audio.wait(200 ms);  //wait vcm charge
//
//
//				myD2S_Audio.wait(500 ms);
//				myD2S_Audio.wait(500 ms);
//				myD2S_Audio.wait(500 ms);
//				myD2S_Audio.wait(500 ms);
//				myD2S_Audio.wait(500 ms);
//				myD2S_Audio.wait(500 ms);
//				myD2S_Audio.wait(500 ms);
//				myD2S_Audio.wait(500 ms);
//				myD2S_Audio.wait(500 ms);
//				myD2S_Audio.wait(500 ms);
//
//
				myD2S_Audio.write(0x22  ,0x02);//2*I0 charge
				myD2S_Audio.wait(wait_time_ms ms);

				myD2S_Audio.write(0x2a  ,0x41);//precharge
				myD2S_Audio.wait(wait_time_ms ms);

				myD2S_Audio.write(0x2a  ,0x63);//DAC PATH ref voltage buffer enable
				myD2S_Audio.wait(wait_time_ms ms);


				myD2S_Audio.write(0x2b  ,0xcc);//DAC module ref voltage buffer enable and initial DAC module
				myD2S_Audio.wait(wait_time_ms ms);

				myD2S_Audio.write(0x2b  ,0xee);//DAC module enable
				myD2S_Audio.wait(wait_time_ms ms);

				myD2S_Audio.write(0x2e  ,0xd1);//DRVL module enable
				myD2S_Audio.wait(wait_time_ms ms);



				myD2S_Audio.write(0x2b  ,0xff);//DAC CLOCK enable
				myD2S_Audio.wait(wait_time_ms ms);

				myD2S_Audio.write(0x2e  ,0xfc);//DRVL module enable
				myD2S_Audio.wait(wait_time_ms ms);


				myD2S_Audio.write(0x2f  ,0x18);//DRVL gain:-3dBFS
				myD2S_Audio.wait(wait_time_ms ms);

//
////
				myD2S_Audio.write(0x30  ,0x18);//DRVR gain:-3dBFS
				myD2S_Audio.wait(wait_time_ms ms);

//

//
//
				myD2S_Audio.write(0x0,0x43);// bypass mode
				myD2S_Audio.write(0x7,0x0c);//L,R channel bypass mode
////
////

				for(int i=0;i<N;i++){
					myD2S_Audio.write(0x10,0x0f&code_in);
					myD2S_Audio.write(0x11,0xff&code_in);
				}

				myD2S_Audio.read(0x22  ,"0x22");
				myD2S_Audio.read(0x0 ,"0x0");
				myD2S_Audio.read(0x7  ,"0x7");

				myD2S_Audio.read(0x30,"0x30");
				myD2S_Audio.read(0x2f,"0x2f");
				myD2S_Audio.read(0x2f,"0x2b");
				myD2S_Audio.read(0x2a  ,"0x2a");
				myD2S_Audio.read(0x22  ,"0x22");
				myD2S_Audio.read(0x23  ,"0x23");

			fr.d2s_LABEL_END();
		ON_FIRST_INVOCATION_END();

		if(Debug_Mode){
			cout << "0x0 : "<< hex << myD2S_Audio.getReadValue("0x0") <<endl; //
			cout << "0x7 : "<< hex << myD2S_Audio.getReadValue("0x7") <<endl; //
			cout << "0x23 : "<< hex << myD2S_Audio.getReadValue("0x23") <<endl; //
			cout << "0x22 : "<< hex << myD2S_Audio.getReadValue("0x22") <<endl; //
			cout << "0x2a : "<< hex << myD2S_Audio.getReadValue("0x2a") <<endl; //
			cout << "0x30 : "<< hex << myD2S_Audio.getReadValue("0x30") <<endl; //
			cout << "0x2f : "<< hex << myD2S_Audio.getReadValue("0x2f") <<endl; //
			cout << "0x2b : "<< hex << myD2S_Audio.getReadValue("0x2b") <<endl; //

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
REGISTER_TESTMETHOD("bm_ac_tml.adac_apb_sine_bypass_cfg", adac_apb_sine_bypass_cfg);
