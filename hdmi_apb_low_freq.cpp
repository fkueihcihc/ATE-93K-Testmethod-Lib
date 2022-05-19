#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;
#include "../d2s_tml/d2s_hdmi_apb.h"
#include "../d2s_tml/generic_d2s/d2sFramework.h"
using namespace V93kLimits;
/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class hdmi_apb_low_freq: public testmethod::TestMethod {

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
		d2s_apb& myD2S = d2s_apb::Instance();

		TransactionPort hdmi_clk("pHDMI_CLK");

		fr.registerTransactionPort(myD2S);
		fr.registerTransactionPort(hdmi_clk);

		d2sFrameWorkModeType::Enum d2s_mode;

		switch ( D2SMode ){
				case 0:
				case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
				case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
			}

		ON_FIRST_INVOCATION_BEGIN();

			fr.d2s_LABEL_BEGIN("hdmi_low_freq_cfg",d2s_mode);//add

				hdmi_clk.execLabel("HDMI_CLK",1);

				myD2S.execLabel("hdmi_apb_init",320);

				myD2S.write(0x00,0x00);//reset
				myD2S.wait(1 us);
				myD2S.write(0x00,0xc0);//release reset

				myD2S.write(0xa0  ,0x01);

				myD2S.read(0xa0,"0xa0");


				myD2S.write(0xa1  ,0x01);
				myD2S.write(0xa2  ,0xf0);
				myD2S.write(0xa3  ,0x63);
				myD2S.write(0xa4  ,0x00);
				myD2S.write(0xa5  ,0x01);
				myD2S.write(0xa6  ,0x21);
				myD2S.write(0xa0  ,0x00);
				myD2S.write(0xaa  ,0x02);
				myD2S.write(0xab  ,0x04);
				myD2S.write(0xac  ,0x14);
				myD2S.write(0xad  ,0x00);

				myD2S.wait(2000 ns);
				myD2S.wait(2000 ns);
				if(Debug_Mode){
					myD2S.read(0xa9,"0xa9");
					myD2S.read(0xaf,"0xaf");
				}
				myD2S.wait(1 ms);

				myD2S.write(0x10  ,0x1f);
				myD2S.write(0x11  ,0x00);
				myD2S.write(0x20  ,0x1f);
				myD2S.write(0x21  ,0x00);
				myD2S.write(0x30  ,0x1f);
				myD2S.write(0x31  ,0x00);
				myD2S.write(0x02  ,0xa1);
				myD2S.write(0x02  ,0xa0);
				myD2S.write(0x02  ,0xa1);

				if(Debug_Mode){
					myD2S.read(0x10,"0x10");
				}

			fr.d2s_LABEL_END();
		ON_FIRST_INVOCATION_END();

		if(Debug_Mode){
			cout << "0xa0 : "<< hex << myD2S.getReadValue("0xa0") <<endl; //
			cout << "0xa9 : "<< hex << myD2S.getReadValue("0xa9") <<endl; //
			cout << "0xaf : "<< hex << myD2S.getReadValue("0xaf") <<endl;  //
			cout << "0x10 : "<< hex << myD2S.getReadValue("0x10") <<endl;

		}



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
REGISTER_TESTMETHOD("bm_ac_tml.hdmi_apb_low_freq", hdmi_apb_low_freq);

class hdmi_apb_high_freq: public testmethod::TestMethod {

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
		d2s_apb& myD2S = d2s_apb::Instance();

		TransactionPort hdmi_clk("pHDMI_CLK");

		fr.registerTransactionPort(myD2S);
		fr.registerTransactionPort(hdmi_clk);

		d2sFrameWorkModeType::Enum d2s_mode;

		switch ( D2SMode ){
				case 0:
				case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
				case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
			}

		ON_FIRST_INVOCATION_BEGIN();

			fr.d2s_LABEL_BEGIN("hdmi_high_freq_cfg",d2s_mode);//add

				hdmi_clk.execLabel("HDMI_CLK",1);

				myD2S.execLabel("hdmi_apb_init",320);

				myD2S.write(0x00,0x00);//reset
				myD2S.wait(1 us);
				myD2S.write(0x00,0xc0);//release reset

				myD2S.write(0xa0  ,0x01);

				myD2S.read(0xa0,"0xa0");


				myD2S.write(0xa1  ,0x01);
				myD2S.write(0xa2  ,0xf0);
				myD2S.write(0xa3  ,0x63);
				myD2S.write(0xa4  ,0x00);
				myD2S.write(0xa5  ,0x01);
				myD2S.write(0xa6  ,0x21);
				myD2S.write(0xa0  ,0x00);
				myD2S.write(0xaa  ,0x02);
				myD2S.write(0xab  ,0x04);
				myD2S.write(0xac  ,0x14);
				myD2S.write(0xad  ,0x00);

				myD2S.wait(2000 ns);
				myD2S.wait(2000 ns);
				if(Debug_Mode){
					myD2S.read(0xa9,"0xa9");
					myD2S.read(0xaf,"0xaf");
				}
				myD2S.wait(1 ms);


				myD2S.write(0x02  ,0xa1);
				myD2S.write(0x02  ,0xa0);
				myD2S.write(0x02  ,0xa1);



				if(Debug_Mode){
					myD2S.read(0x10,"0x10");
				}

			fr.d2s_LABEL_END();
		ON_FIRST_INVOCATION_END();

		if(Debug_Mode){
			cout << "0xa0 :0x "<< hex << myD2S.getReadValue("0xa0") <<endl; //
			cout << "0xa9 :0x "<< hex << myD2S.getReadValue("0xa9") <<endl; //
			cout << "0xaf :0x "<< hex << myD2S.getReadValue("0xaf") <<endl;  //
			cout << "0x10 :0x "<< hex << myD2S.getReadValue("0x10") <<endl;

		}



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
REGISTER_TESTMETHOD("bm_ac_tml.hdmi_apb_high_freq", hdmi_apb_high_freq);
