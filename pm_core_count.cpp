/*
 * pm_core_count.cpp
 *
 *  Created on: April 28, 2022
 *      Author: Bobo
 */
/*	Attention:
 *  this method work with special digtial capture setup as below:
 *  Transfer Mode = Serial;
 *  Sample Length in Bits = 32;
 *  Bits per Word = 32; equal to Sample Length in Bits
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
	  int 		wait_ms;//wait time for connect
	  int  		core_num;//capture size;
	  string  	vCap;//capture variable
	  STRING 	Gen_testTable;//flag for generate testtable

  virtual void initialize()
  {
    //Add your initialization code here

	    addParameter("core_num",     "int",    &core_num)
	    .setDefault("88")
	      .setComment("the sample size of digital capture");

	    addParameter("vCap",       "string",    &vCap)
	    .setDefault("Cap_pm_core_chain")
	      .setComment("the Capture variable,must be unique");

	    addParameter("wait_ms", "int", &wait_ms)
					   .setDefault("10")
					   .setComment("wait time for connect");

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
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
   {
 	  static ARRAY_I		aiCapData;
 	  static ARRAY_I		ProcessedData;
 	  static string 		sTsName;
 	  static STRING_VECTOR 	sTestname;
 	  char tmpchar[256];

 	  //initial the array
 	  aiCapData.resize(core_num);
 	  aiCapData.init(0);
 	  ProcessedData.resize(core_num);
 	  ProcessedData.init(0);
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

 	  ON_FIRST_INVOCATION_BEGIN();
 	  	  DISCONNECT();
 	  	  WAIT_TIME(wait_ms ms);
 	  	  CONNECT();
 	  	  WAIT_TIME(wait_ms ms);
 		  DIGITAL_CAPTURE_TEST();

	  ON_FIRST_INVOCATION_END();

	  aiCapData = VECTOR(vCap).getVectors();//get back the capture data

	  ProcessedData = CapDataProcess(aiCapData);//process raw data

	  for(int i=0;i<ProcessedData.size();i++){
		  int pm_cnt = 0x0000ffff & ProcessedData[i];//bit 0~15
		  int core_id = (0xff000000 & ProcessedData[i])>>24;//bit 24~31
//		  if(debug_mode) cout<<"the pm count of core"<<dec<<i<<"="<<pm_cnt<<endl;
		  if(debug_mode) cout<<"the pm counter value of core"<<dec<<core_id<<" = "<<pm_cnt<<endl;
		  cout<<endl;
		  sprintf(tmpchar,"pm_core%d",core_id);
		  TestSet.cont(true).TEST("", tmpchar, tmLimits, pm_cnt);
	  }

     return;
   }
  /**
   *This function will be invoked once the specified parameter's value is changed.
   *@param parameterIdentifier
   *
   *Note: Test Method API should not be used in this method.
   */

	void Generate_testTable(STRING_VECTOR &temp_pins_list) {

		char temp_test[50], temp_testname[50];
		string suite;
		GET_TESTSUITE_NAME(suite);
		TestTable* testTable = TestTable::getDefaultInstance();
		testTable->isCsvLoadNeeded();
		testTable->readCsvFile();
		int KeyValue[] = { 1 };
		testTable->setLookupKeys(1, KeyValue);

		sprintf(temp_test, "%s", suite.c_str());

		for (size_t i = 0; i < temp_pins_list.size(); i++) {

			sprintf(temp_testname, "%s", temp_pins_list[i].c_str());
			if (testTable->getTestPos(temp_test) < 0) {
				testTable->addTest(temp_test);
				testTable->setValue(temp_test, "Suite name", suite.c_str());
				testTable->setValue(temp_test, "Test name", temp_testname);
				testTable->writeCsvFile();
			}
		}

		TestTable::clearDefaultInstance();

	}

	//regroup raw captured 32-bit data into right order
	ARRAY_I CapDataProcess(ARRAY_I capData) {
		ARRAY_I processedData;
		processedData.resize(capData.size());
		processedData.init(0);
		for (int i = 0; i < capData.size(); i++) {
			unsigned int tmpWord = reverse(capData[i]);
			ARRAY_I tmpByte = Word2Byte(tmpWord);
			unsigned int ProcessedWord = (tmpByte[0] << 24) + (tmpByte[1] << 16)
					+ (tmpByte[2] << 8) + tmpByte[3];
			processedData[i] = ProcessedWord;
		}
		return processedData;
	}

	//reverse raw 32-bit data,from MSB to LSB
	unsigned int reverse(register unsigned int x) {
		x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
		x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
		x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
		x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));

		return ((x >> 16) | (x << 16));

	}
	//cut 32-bit data into 4 bytes
	ARRAY_I Word2Byte(unsigned int inData) {

		ARRAY_I byte;
		byte.resize(4);
		byte.init(0);

		byte[0] = 0x000000ff & inData;
		byte[1] = (0x0000ff00 & inData) >> 8;
		byte[2] = (0x00ff0000 & inData) >> 16;
		byte[3] = (0xff000000 & inData) >> 24;

		return byte;
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
