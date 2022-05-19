/*
 * bynonce_test.cpp
 *
 *  Created on: April 29, 2022
 *      Author: Bobo
 */
/*	Attention:
 *  this method work with special capture setup as below:
 *  Transfer Mode = Serial;
 *  Sample Length in Bits = 31;31 is the Tick nonce cnt's bits
 *  Bits per Word = 31;31; equal to Sample Length in Bits
 *  Inter-Sample Skip Bits  = 1;reg has 32bits,the remaining high 1 bit need discard
 *  Sample Count = 1; 1 represent only one register;
 * */

#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"

using namespace std;
using namespace V93kLimits;

/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */

class bynonce_test: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   *
   *Note: Test Method API should not be used in this method.
   */

	  int 		debug_mode;//log ctrl
	  int  		Cap_samples;//capture samples;
	  int  		cnt_bits;//counter bits;
	  int  		discard_bits;//discard bits from the remaining 32bit-reg bits;
	  int  		expect_cnt;//expect ticket count;
	  string  	vCap;//capture variable
	  string  	cap_pin;//capture pin
	  string	setup_pat;//pattern for setup
	  STRING 	Gen_testTable;//flag for generate testtable
	  STRING 	Gen_Vector_Var;//flag for auto generate vector variable

  virtual void initialize()
  {
    //Add your initialization code here

	    addParameter("setup_pat",       "string",    &setup_pat)
	    .setDefault("bynonce_setup_header_pll_PWTH_CDLY")
	      .setComment("the pattern for setup");

	    addParameter("Cap_samples",     "int",    &Cap_samples)
	    .setDefault("1")
	      .setComment("the samples of digital capture");

	    addParameter("vCap",       "string",    &vCap)
	    .setDefault("bynonce_work_xcore")
	      .setComment("the Capture variable,must be unique");

	    addParameter("cnt_bits",  "int",    &cnt_bits)
	    .setDefault("31")
	      .setComment("the ticket counter bits");

	    addParameter("discard_bits",  "int",    &discard_bits)
	    .setDefault("1")
	      .setComment("discard bits from 32bits reg");

	    addParameter("expect_cnt",  "int",    &expect_cnt)
	    .setDefault("7040")
	      .setComment("the ticket counter bits");

	    addParameter("cap_pin",       "string",    &cap_pin)
	    .setDefault("RO_RI")
	      .setComment("the Capture pin");

	    addParameter("debug_mode", "int", &debug_mode)
					   .setDefault("0")
					   .setOptions("0:1")
					   .setComment("debug mode 0:no_need_ouput, 1:output_to_ui");


		addParameter("Gen_Vector_Var",
					"string",&Gen_Vector_Var,
					testmethod::TM_PARAMETER_INPUT)
					   .setOptions("Yes:No")
					   .setDefault("No")
			  	  	   .setComment("the flag for ctrl auto generate vector variable");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
   {
 	  static ARRAY_I		aiCapData;
 	  static string 		sTsName;
 	  static STRING_VECTOR 	sTestname;
 	  char 					tmpchar[256];


 	  //initial the array
 	  aiCapData.resize(Cap_samples);
 	  aiCapData.init(0);
// 	  sTestname.resize(Cap_samples);
// 	  sTestname.clear();
 	  const string mylabel=Primary.getLabel();//get pattern for add vector variable

 	  if("Yes"==Gen_Vector_Var){
		//just run 1 time or will report error
		ON_FIRST_INVOCATION_BEGIN();
		  discard_bits = 32 - cnt_bits;//discard_bits + cnt_bits = 32(register bits)
		  sprintf(tmpchar,"DVVA \"%s\",DIGCAPT,\"%s\",SERI,,,,0,%d,%d,0,%d,%d,(%s)",vCap.c_str(),mylabel.c_str(),cnt_bits,cnt_bits,Cap_samples,discard_bits,cap_pin.c_str());
		  FW_TASK(tmpchar);
//		  cout<<tmpchar<<endl;
		ON_FIRST_INVOCATION_END();
 	  }

 	  ON_FIRST_INVOCATION_BEGIN();

 	  	  CONNECT();
 	  	  Primary.label(setup_pat);//change to setup label
 	  	  FLUSH();
 	  	  FUNCTIONAL_TEST();//run setup

 	  	  Primary.label(mylabel);//change back initial pattern name
 	  	  FLUSH();
 		  DIGITAL_CAPTURE_TEST();

	  ON_FIRST_INVOCATION_END();

	  aiCapData = VECTOR(vCap).getVectors();//get back the capture data
	  if(debug_mode)  cout<<aiCapData<<endl;

	  DOUBLE pct_return =  aiCapData[0]/expect_cnt *100;//calculate the percentage of samples/(expect nonce cnt)

	  TestSet.cont(true).TEST("", "PCT", tmLimits, pct_return);


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
REGISTER_TESTMETHOD("bm_ac_tml.bynonce_test", bynonce_test);
