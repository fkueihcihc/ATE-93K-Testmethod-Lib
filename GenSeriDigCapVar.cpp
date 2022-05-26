/*
 * GenSeriDigCapVar.cpp
 *	this method is for auto-creating digital capture variable
 *  Created on: May 25, 2022
 *      Author: pengbobo
 *      mail:bobo.peng@bitmain.com
 */

#include "testmethod.hpp"
#include "mapi.hpp"



class GenSeriDigCapVar: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   *
   *Note: Test Method API should not be used in this method.
   */
	  int  		samples;//
	  int  		bitsPerWord;//
	  string  	vCap;//capture variable
	  string  	cap_pin;//capture pin
	  string  	mylabel;//pattern label name


  virtual void initialize()
  {
    //Add your initialization code here

	    addParameter("samples",     "int",    &samples)
	    .setDefault("1")
	      .setComment("the sample size of digital capture");

	    addParameter("vCap",       "string",    &vCap)
	    .setDefault("")
	      .setComment("the Capture variable,must be unique");

	    addParameter("bitsPerWord",  "int",    &bitsPerWord)
	    .setDefault("32")
	      .setComment("the bitsPerWord for upload");

	    addParameter("cap_pin",       "string",    &cap_pin)
	    .setDefault("RO_RI")
	      .setComment("the Capture pin");

	    addParameter("mylabel",       "string",    &mylabel)
	    .setDefault("")
	      .setComment("pattern label name");

  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
   {

 	  char tmpchar[256];


 	  //just run 1 time or will report error
 	  ON_FIRST_INVOCATION_BEGIN();

		  //step1:define vector variable
		  sprintf(tmpchar,"DVVA \"%s\",DIGCAPT,\"%s\",SERI,,,,0,%d,%d,0,%d,%d,(%s)",vCap.c_str(),mylabel.c_str(),bitsPerWord,bitsPerWord,samples,0,cap_pin.c_str());
		  FW_TASK(tmpchar);
		  //step2:active digital capture
		  sprintf(tmpchar,"SDGC \"%s\",STD,(%s)",mylabel.c_str(),cap_pin.c_str());
		  FW_TASK(tmpchar);

	  ON_FIRST_INVOCATION_END();


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
REGISTER_TESTMETHOD("bm_miscellaneous_tml.GenSeriDigCapVar", GenSeriDigCapVar);



