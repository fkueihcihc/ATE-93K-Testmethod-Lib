#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;
#include "../d2s_tml/d2s_DFT_JTAG.h"
#include "../d2s_tml/generic_d2s/d2sFramework.h"
using namespace V93kLimits;
/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class dft_jtag_cfg: public testmethod::TestMethod {

protected:
	int 		D2SMode;
	int			Debug_Mode;
	int			dft_mode;
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
		  	addParameter("dft_mode", "int", &dft_mode)
		                 .setDefault("0")
		                 .setOptions("0:1:2:3:4:5:6:7:8:9:10:11:12:13:14:15")
		                 .setComment("test mode select");
  }

  /**
   *This test is invoked per site.
   */
	virtual void run() {

		d2sFramework fr;
		d2s_dft_jtag& myD2S = d2s_dft_jtag::Instance();


		fr.registerTransactionPort(myD2S);


		d2sFrameWorkModeType::Enum d2s_mode;

		switch ( D2SMode ){
				case 0:
				case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
				case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
			}

		ON_FIRST_INVOCATION_BEGIN();

			CONNECT();

			char tmpchar[256];
			sprintf(tmpchar,"dft_mode_%d",dft_mode);


			fr.d2s_LABEL_BEGIN(tmpchar,d2s_mode);//add
				myD2S.execLabel("dft_jtag_reset",9);
				myD2S.write(0x1B5 ,dft_mode);
				myD2S.wait(10 us);

			fr.d2s_LABEL_END();
		ON_FIRST_INVOCATION_END();


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
REGISTER_TESTMETHOD("bm_ac_tml.dft_jtag_cfg", dft_jtag_cfg);

class dft_jtag_cfg_read: public testmethod::TestMethod {

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
		d2s_dft_jtag& myD2S = d2s_dft_jtag::Instance();


		fr.registerTransactionPort(myD2S);


		d2sFrameWorkModeType::Enum d2s_mode;

		switch ( D2SMode ){
				case 0:
				case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
				case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
			}

		ON_FIRST_INVOCATION_BEGIN();

			fr.d2s_LABEL_BEGIN("dft_jtag_cfg_read",d2s_mode);//add
				myD2S.read(0x1B5 ,"0x1B5");//
			fr.d2s_LABEL_END();
		ON_FIRST_INVOCATION_END();

		if(Debug_Mode){
			cout << "0x1B5 : "<< dec << myD2S.getReadValue("0x1B5") <<endl; //
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
REGISTER_TESTMETHOD("bm_ac_tml.dft_jtag_cfg_read", dft_jtag_cfg_read);

