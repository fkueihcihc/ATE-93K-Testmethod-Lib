/*
 * hdmi_functional.cpp
 *
 *  Created on: May 31, 2016
 *      Author: pengbobo
 */

#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;
using namespace V93kLimits;


class hdmi_fun_ps1600: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   *
   *Note: Test Method API should not be used in this method.
   */

	int Debug_Mode;
	int Loop_xN;

  virtual void initialize()
  {
    //Add your initialization code here
	  addParameter("Debug_Mode", "int", &Debug_Mode)
	                 .setDefault("1")
	                 .setOptions("0:1")
	                 .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");
	  addParameter("Loop_xN", "int", &Loop_xN)
	                 .setDefault("10")
	                 .setComment("Loop N times");

  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
   {

 	  int			Flag_Pass=0;
 	  int			Flag_TX0N=0,Flag_TX1N=0,Flag_TX2N=0;
 	  int			Flag_TX0P=0,Flag_TX1P=0,Flag_TX2P=0;
 	  static string 		sTsName;


 			/********************************************************************************************
 			 * move the strobe 10 times and Capture the data if 1 time is ok, set the test pass
 			 ********************************************************************************************/


 	  GET_TESTSUITE_NAME(sTsName);

 	  Flag_Pass=0;
 	  Flag_TX0N=0,Flag_TX1N=0,Flag_TX2N=0;
 	  Flag_TX0P=0,Flag_TX1P=0,Flag_TX2P=0;

 	  double period = 1000/Primary.getSpecification().getSpecValue("Freq@p1600_HDMI_DATA")/8;//8 strobes


 	  ON_FIRST_INVOCATION_BEGIN();
 		  CONNECT();
 	  ON_FIRST_INVOCATION_END();

 	  for(int i = 0;i<Loop_xN;i++){
 		  ON_FIRST_INVOCATION_BEGIN();
			  Primary.getSpecification().change("offset@p1600_HDMI_DATA",i*period/Loop_xN);
			  if(Debug_Mode) cout<<"offset value is :"<< Primary.getSpecification().getSpecValue("offset@p1600_HDMI_DATA")<<endl;
			  FLUSH(TM::APRM);
			  FUNCTIONAL_TEST();
 		  ON_FIRST_INVOCATION_END();
		  Flag_TX0N = Flag_TX0N | GET_FUNCTIONAL_RESULT("HDMI_O_TX0N");
		  Flag_TX1N = Flag_TX1N | GET_FUNCTIONAL_RESULT("HDMI_O_TX1N");
		  Flag_TX2N = Flag_TX2N | GET_FUNCTIONAL_RESULT("HDMI_O_TX2N");
		  Flag_TX0P = Flag_TX0P | GET_FUNCTIONAL_RESULT("HDMI_O_TX0P");
		  Flag_TX1P = Flag_TX1P | GET_FUNCTIONAL_RESULT("HDMI_O_TX1P");
		  Flag_TX2P = Flag_TX2P | GET_FUNCTIONAL_RESULT("HDMI_O_TX2P");
 	  }
 	  //================= set back the original setup ==========//
 	  Primary.getSpecification().restore();
 	  FLUSH(TM::APRM);
 ///************************Compare the capture code with the expected code*************************************/

 	  Flag_Pass = Flag_TX0N & Flag_TX1N & Flag_TX2N & Flag_TX0P & Flag_TX1P & Flag_TX2P;//

 	  if(Debug_Mode){
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_Pass="<<Flag_Pass<<endl;

 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX0N="<<Flag_TX0N<<endl;
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX1N="<<Flag_TX1N<<endl;
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX2N="<<Flag_TX2N<<endl;
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX0P="<<Flag_TX0P<<endl;
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX1P="<<Flag_TX1P<<endl;
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX2P="<<Flag_TX2P<<endl;

 	  }
 	  //======Judge The Result===================//
 	  TestSet.cont(true).TEST("","flag",tmLimits,Flag_Pass);

     return;
   }
  /**
   *This function will be invoked once the specified parameter's value is changed.
   *@param parameterIdentifier
   *
   *Note: Test Method API should not be used in this method.
   */

  virtual void postParameterChange(const string& parameterIdentifier)
  {
    //Add your code

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
REGISTER_TESTMETHOD("bm_ac_tml.hdmi_fun_ps1600", hdmi_fun_ps1600);
class hdmi_fun_9g: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   *
   *Note: Test Method API should not be used in this method.
   */

	int Debug_Mode;

  virtual void initialize()
  {
    //Add your initialization code here
	  addParameter("Debug_Mode", "int", &Debug_Mode)
	                 .setDefault("1")
	                 .setOptions("0:1")
	                 .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
   {

 	  int			Flag_Pass=0;
 	  int			Flag_TX0N=0,Flag_TX1N=0,Flag_TX2N=0;
 	  int			Flag_TX0P=0,Flag_TX1P=0,Flag_TX2P=0;
 	  static string 		sTsName;


 			/********************************************************************************************
 			 * move the strobe 10 times and Capture the data if 1 time is ok, set the test pass
 			 ********************************************************************************************/


 	  GET_TESTSUITE_NAME(sTsName);

 	  double period = 1000/Primary.getSpecification().getSpecValue("Freq@pHDMI_9G_Data")/8;//8 strobes


 	  ON_FIRST_INVOCATION_BEGIN();
 		  CONNECT();
 	  ON_FIRST_INVOCATION_END();

 	  for(int i = 0;i<10;i++){
 		  ON_FIRST_INVOCATION_BEGIN();
			  Primary.getSpecification().change("offset@pHDMI_9G_Data",i*period/10);
			  FLUSH(TM::APRM);
			  FUNCTIONAL_TEST();
 		  ON_FIRST_INVOCATION_END();
		  Flag_TX0N = Flag_TX0N | GET_FUNCTIONAL_RESULT("HDMI_O_TX0N");
		  Flag_TX1N = Flag_TX1N | GET_FUNCTIONAL_RESULT("HDMI_O_TX1N");
		  Flag_TX2N = Flag_TX2N | GET_FUNCTIONAL_RESULT("HDMI_O_TX2N");
		  Flag_TX0P = Flag_TX0P | GET_FUNCTIONAL_RESULT("HDMI_O_TX0P");
		  Flag_TX1P = Flag_TX1P | GET_FUNCTIONAL_RESULT("HDMI_O_TX1P");
		  Flag_TX2P = Flag_TX2P | GET_FUNCTIONAL_RESULT("HDMI_O_TX2P");
 	  }
 	  //================= set back the original setup ==========//
 	  Primary.getSpecification().restore();
 	  FLUSH(TM::APRM);
 ///************************Compare the capture code with the expected code*************************************/

 	  Flag_Pass = Flag_TX0N & Flag_TX1N & Flag_TX2N & Flag_TX0P & Flag_TX1P & Flag_TX2P;//

 	  if(Debug_Mode){
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_Pass="<<Flag_Pass<<endl;

 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX0N="<<Flag_TX0N<<endl;
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX1N="<<Flag_TX1N<<endl;
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX2N="<<Flag_TX2N<<endl;
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX0P="<<Flag_TX0P<<endl;
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX1P="<<Flag_TX1P<<endl;
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX2P="<<Flag_TX2P<<endl;

 	  }
 	  //======Judge The Result===================//
 	  TestSet.cont(true).TEST("","flag",tmLimits,Flag_Pass);

     return;
   }
  /**
   *This function will be invoked once the specified parameter's value is changed.
   *@param parameterIdentifier
   *
   *Note: Test Method API should not be used in this method.
   */

  virtual void postParameterChange(const string& parameterIdentifier)
  {
    //Add your code

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
REGISTER_TESTMETHOD("bm_ac_tml.hdmi_fun_9g", hdmi_fun_9g);
