/*
 * DelDigCapVar.cpp
 *
 *	this method is for auto-deleting digital capture variable
 *  Created on: May 25, 2022
 *      Author: pengbobo
 *      mail:bobo.peng@bitmain.com
 */

#include "testmethod.hpp"
#include "mapi.hpp"



class DelDigCapVar: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   *
   *Note: Test Method API should not be used in this method.
   */

	  string  	vCap;//capture variable



  virtual void initialize()
  {
    //Add your initialization code here

	    addParameter("vCap",       "string",    &vCap)
	    .setDefault("")
	      .setComment("the Capture variable,must be unique");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
   {

 	  char tmpchar[256];


 	  //just run 1 time or will report error
 	  ON_FIRST_INVOCATION_BEGIN();

		  sprintf(tmpchar,"RVVA \"%s\",(@@)",vCap.c_str());
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
REGISTER_TESTMETHOD("bm_miscellaneous_tml.DelDigCapVar", DelDigCapVar);


