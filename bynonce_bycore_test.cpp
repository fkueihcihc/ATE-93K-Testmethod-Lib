/*
 * bynonce_test.cpp
 *
 *  Created on: April 29, 2022
 *      Author: Bobo
 */
/*	Attention:
 *  this method work with special capture setup as below:
 *  Transfer Mode = Serial;
 *  Sample Length in Bits = 32;32 is the Tick nonce cnt's bits
 *  Bits per Word = 32;32; equal to Sample Length in Bits
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
	  int 		wait_ms;//wait time for connect
	  int  		Cap_samples;//capture samples;
	  int  		expect_cnt;//expect ticket count;
	  string  	vCap;//capture variable


  virtual void initialize()
  {
    //Add your initialization code here

	    addParameter("Cap_samples",     "int",    &Cap_samples)
	    .setDefault("1")
	      .setComment("the samples of digital capture");

	    addParameter("vCap",       "string",    &vCap)
	    .setDefault("bynonce_work_xcore")
	      .setComment("the Capture variable,must be unique");


	    addParameter("expect_cnt",  "int",    &expect_cnt)
	    .setDefault("7040")
	      .setComment("the ticket counter bits");

	    addParameter("wait_ms", "int", &wait_ms)
					   .setDefault("10")
					   .setComment("wait time for connect");

	    addParameter("debug_mode", "int", &debug_mode)
					   .setDefault("0")
					   .setOptions("0:1")
					   .setComment("debug mode 0:no_need_ouput, 1:output_to_ui");

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

 	  //initial the array
 	  aiCapData.resize(Cap_samples);
 	  aiCapData.init(0);

 	  ProcessedData.resize(Cap_samples);
 	  ProcessedData.init(0);

 	  ON_FIRST_INVOCATION_BEGIN();
 	  	  DISCONNECT();
 	  	  WAIT_TIME(wait_ms ms);
		  CONNECT();
		  WAIT_TIME(wait_ms ms);

 		  DIGITAL_CAPTURE_TEST();

	  ON_FIRST_INVOCATION_END();

	  aiCapData = VECTOR(vCap).getVectors();//get back the raw capture data

	  ProcessedData = CapDataProcess(aiCapData);//process raw data

	  for(int i=0;i<ProcessedData.size();i++){
		  if(debug_mode)cout<<"ticket count:"<<ProcessedData[i]<<endl;
		  DOUBLE pct_return =   double(ProcessedData[i])/expect_cnt *100;//calculate the percentage of samples/(expect nonce cnt)

		  TestSet.cont(true).TEST("", "PCT", tmLimits, pct_return);
	  }


     return;
   }
  /**
   *This function will be invoked once the specified parameter's value is changed.
   *@param parameterIdentifier
   *
   *Note: Test Method API should not be used in this method.
   */
  unsigned int CaptDataProcess(ARRAY_I  capData)
	{
	  unsigned int regVal= 0; //initial regVal
		for(int i = 0; i< capData.size();i++){

			if(i<=7) {regVal = regVal | capData[i]<<(24+i);} //bit0~7 shift as the bit 24~31 of regVal
			else if(7<i<=15) {regVal = regVal | capData[i]<<(8+i);}//bit8~15 shift as the bit 16~23 of regVal
			else if(16<i<=23) {regVal = regVal | capData[i]<<(i-8);}//bit16~23 shift as the bit 8~15 of regVal
		}

		return regVal;
	}

  	ARRAY_I CapDataProcess(ARRAY_I  capData){
  		ARRAY_I processedData;
  		processedData.resize(capData.size());
  		processedData.init(0);
  		for(int i=0;i<capData.size();i++){
//  			cout<<"raw data:"<<dec<<capData[i]<<endl;
  			unsigned int tmpWord = reverse(capData[i]);
//  			cout<<"reverse data:"<<hex<<tmpWord<<endl;
  			ARRAY_I tmpByte = Word2Byte(tmpWord);
//  			cout<<"cut byte:"<<tmpByte<<endl;
  			unsigned int ProcessedWord = (tmpByte[0]<<24)+(tmpByte[1]<<16)+(tmpByte[2]<<8)+tmpByte[3];
  			processedData[i]= ProcessedWord;
  		}
  		return processedData;
  	}

  	//reverse raw 32-bit data,from MSB to LSB
	unsigned int
	reverse(register unsigned int x)
	{
//		cout<<"x:0x"<<hex<<x<<endl;
	    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
	    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
	    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
	    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
//	    cout<<"processed x:0x"<<((x >> 16) | (x << 16))<<endl;
	    return((x >> 16) | (x << 16));

	}
	//cut 32-bit data into 4 bytes
  	ARRAY_I Word2Byte(unsigned int inData){

  		  ARRAY_I byte;
  		  byte.resize(4);
  		  byte.init(0);

  		  byte[0] =  0x000000ff & inData;
  		  byte[1] = (0x0000ff00 & inData)>>8;
  		  byte[2] = (0x00ff0000 & inData)>>16;
  		  byte[3] = (0xff000000 & inData)>>24;

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
REGISTER_TESTMETHOD("bm_ac_tml.bynonce_test", bynonce_test);
