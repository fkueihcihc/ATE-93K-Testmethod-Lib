/*
 * sec_freq.cpp
 *
 *  Created on: Oct 8, 2016
 *      Author: pengbobo
 */
#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;
#include "TMUtils.h"
#include "Common.h"
using namespace V93kLimits;

/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class sec_freq: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
	  int  		iDebug;//, sCycle_number;
	  STRING 	mVec_Var;

  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
		addParameter("mVec_Var", "string", &mVec_Var,testmethod::TM_PARAMETER_INPUT)
				.setDefault("sec_cap")
				.setComment("The vector variable which captures device output signal");

	  addParameter("Debug_Mode", "int", &iDebug)
				   .setDefault("1")
				   .setOptions("0:1")
				   .setComment("debug mode  0:no_need_ouput, 1:output_to_ui_and_signal_analyzer");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
    {
  	  int			site_number;
  	  ARRAY_I 		CapData(6144);
  	  ARRAY_I 		iCapData[24];
  	  static string sTsName;
  	  ARRAY_D	    dFreq_MHz;
  	  double 		Sample_period_ns=2.5;



  	  dFreq_MHz.resize(24);

  	  CapData.init(0);
  	  dFreq_MHz.init(0);

  	  ON_FIRST_INVOCATION_BEGIN();
		  CONNECT();
		  DIGITAL_CAPTURE_TEST();
  	  ON_FIRST_INVOCATION_END();

  	      site_number = CURRENT_SITE_NUMBER()-1;
  	      CapData = VECTOR(mVec_Var).getVectors();
  	      for(int i =0;i<24;i++)
  	      {
  	    	  iCapData[i].resize(256);
  	    	  iCapData[i].init(0);
  	    	  for(int j=0;j<256;j++){
  	    		iCapData[i][j]+=CapData[i*256+j];
  	    	  }

  	    	  dFreq_MHz[i] = 20*Frequency(iCapData[i], Sample_period_ns*1e-3, 1);//sec need x20

			  if(iDebug)
			  {
//				  cout<<CapData<<endl;
				  cout<<"iCapData["<<i<<"]="<<iCapData[i]<<endl;
				  cout << "site :" << site_number <<endl;
				  PUT_DEBUG("sec","Freq",  CapData);
				  cout  << "Frequency= " << dFreq_MHz[i] << "MHz"<< endl;
			  }
  	      }

  	      double min,max;
  	      int minindex,maxindex;
  	      DSP_MINMAX(dFreq_MHz,&min,&max,&minindex,&maxindex,24);
  	      min = int(min);
		  if(iDebug)
		  {
			  cout<<"Fmin="<<min<<"MHz"<<endl;
			  cout<<"Fmax="<<max<<"MHz"<<endl;
		  }
		  SET_USER_DOUBLE("trng_slowest_fro",min);
		  TestSet.cont(true).TEST("", "Fmin", tmLimits, min);


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
REGISTER_TESTMETHOD("bm_ac_tml.sec_freq", sec_freq);
