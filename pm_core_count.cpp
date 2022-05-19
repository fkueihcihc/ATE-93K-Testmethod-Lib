/*
 * pm_core_count.cpp
 *
 *  Created on: April 28, 2022
 *      Author: Bobo
 */
/*	Attention:
 *  this method work with special capture setup as below:
 *  Transfer Mode = Serial;
 *  Sample Length in Bits = 16;16 is the pm counter's bits
 *  Bits per Word = 16; equal to Sample Length in Bits
 *  Inter-Sample Skip Bits  = 16;reg has 32bits,the remaining high 16bits need discard
 *  Sample Count = 88; 88 is big core number;
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

class pm_core_count: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   *
   *Note: Test Method API should not be used in this method.
   */

	  int 		debug_mode;//log ctrl
	  int  		core_num;//capture size;
	  int  		cnt_bits;//core pm counter bits;
	  int  		discard_bits;//discard bits from the remaining 32bit-reg bits;
	  string  	vCap;//capture variable
	  string  	cap_pin;//capture pin
	  STRING 	Gen_testTable;//flag for generate testtable
	  STRING 	Gen_Vector_Var;//flag for auto generate vector variable

  virtual void initialize()
  {
    //Add your initialization code here

	    addParameter("core_num",     "int",    &core_num)
	    .setDefault("88")
	      .setComment("the sample size of digital capture");

	    addParameter("vCap",       "string",    &vCap)
	    .setDefault("Cap_pm_core_chain")
	      .setComment("the Capture variable,must be unique");

	    addParameter("cnt_bits",  "int",    &cnt_bits)
	    .setDefault("16")
	      .setComment("the core pm counter bits");

	    addParameter("discard_bits",  "int",    &discard_bits)
	    .setDefault("16")
	      .setComment("discard bits from 32bits reg");

	    addParameter("cap_pin",       "string",    &cap_pin)
	    .setDefault("RO_RI")
	      .setComment("the Capture pin");

	    addParameter("debug_mode", "int", &debug_mode)
					   .setDefault("0")
					   .setOptions("0:1")
					   .setComment("debug mode 0:no_need_ouput, 1:output_to_ui");

		addParameter("Gen_testTable",
					"string",&Gen_testTable,
					testmethod::TM_PARAMETER_INPUT)
					   .setOptions("Yes:No")
					   .setDefault("No")
			  	  	   .setComment("the flag for ctrl auto generate test limit");

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
 	  char tmpchar[256];

 	  //initial the array
 	  aiCapData.resize(core_num);
 	  aiCapData.init(0);
 	  sTestname.resize(core_num);
 	  sTestname.clear();

 	  //auto generate test limit in testtalbe
		if("Yes"==Gen_testTable){
			ON_FIRST_INVOCATION_BEGIN();

			  for(int id=0;id<core_num;id++){
				  sprintf(tmpchar,"pm_core%d",id);
				  sTestname.push_back(tmpchar);
			  }
			  tmLimits.load();
			  Generate_testTable(sTestname);
			  tmLimits.load();
			ON_FIRST_INVOCATION_END();
		}
		if("Yes"==Gen_Vector_Var){
			//just run 1 time or will report error
			ON_FIRST_INVOCATION_BEGIN();
			  const string mylabel=Primary.getLabel();
			  discard_bits = 32 - cnt_bits;//discard_bits + cnt_bits = 32(register bits)
			  //step1:define vector variable
			  sprintf(tmpchar,"DVVA \"%s\",DIGCAPT,\"%s\",SERI,,,,0,%d,%d,0,%d,%d,(%s)",vCap.c_str(),mylabel.c_str(),cnt_bits,cnt_bits,core_num,discard_bits,cap_pin.c_str());
			  FW_TASK(tmpchar);
// 			  sprintf(tmpchar,"RVVA \"%s\",(@@)",vCap.c_str());
 			  //step2:active digital capture
 			  sprintf(tmpchar,"SDGC \"%s\",STD,(%s)",mylabel.c_str(),cap_pin.c_str());
 			  FW_TASK(tmpchar);

//			  cout<<tmpchar<<endl;
			ON_FIRST_INVOCATION_END();
		}

 	  ON_FIRST_INVOCATION_BEGIN();

 	  	  CONNECT();
 		  DIGITAL_CAPTURE_TEST();

	  ON_FIRST_INVOCATION_END();

	  aiCapData = VECTOR(vCap).getVectors();//get back the capture data

	  for(int id=0;id<core_num;id++){
		  if(debug_mode) cout<<"the pm count of core"<<id<<"="<<aiCapData[id]<<endl;
		  sprintf(tmpchar,"pm_core%d",id);
		  TestSet.cont(true).TEST("", tmpchar, tmLimits, aiCapData[id]);
	  }

     return;
   }
  /**
   *This function will be invoked once the specified parameter's value is changed.
   *@param parameterIdentifier
   *
   *Note: Test Method API should not be used in this method.
   */

  void Generate_testTable(STRING_VECTOR &temp_pins_list)
    {

  	char temp_test[50],temp_testname[50];
  	string suite;
  	GET_TESTSUITE_NAME(suite);
  //
  	TestTable* testTable = TestTable::getDefaultInstance();
  	testTable->isCsvLoadNeeded();
  	testTable->readCsvFile();
  //
  	int KeyValue[]={1};
  	testTable->setLookupKeys(1,KeyValue);

  	sprintf(temp_test,"%s",suite.c_str());


  	for(size_t i=0;i<temp_pins_list.size();i++)
  	{

  		sprintf(temp_testname,"%s",temp_pins_list[i].c_str());
  		if(testTable->getTestPos(temp_test)<0)
  		{
  			testTable->addTest(temp_test);
  			testTable->setValue(temp_test,"Suite name",suite.c_str());
  			testTable->setValue(temp_test,"Test name",temp_testname);
  			testTable->writeCsvFile();
  		}
  	}

  	TestTable::clearDefaultInstance();

    }

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
REGISTER_TESTMETHOD("bm_ac_tml.pm_core_count", pm_core_count);
