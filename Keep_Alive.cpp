#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;

/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class Keep_Alive_ENABLE: public testmethod::TestMethod {
protected:
  string  PortNames;

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   *
   *Note: Test Method API should not be used in this method.
   */
  virtual void initialize()
  {
    //Add your initialization code here
    addParameter("PortNames",
                 "PinString",
                 &PortNames,
                 testmethod::TM_PARAMETER_INPUT)
      .setDefault("pHDMI_CLK");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
    //Add your test code here.
	  char tmpchar[256];
	  sprintf(tmpchar,"FTCG SKPR,ON,(%s)\n",PortNames.c_str());
	  FW_TASK(tmpchar);
//	  WAIT_TIME(10 ms);
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
REGISTER_TESTMETHOD("bm_ac_tml.Keep_Alive.Keep_Alive_ENABLE", Keep_Alive_ENABLE);

class Keep_Alive_DISABLE: public testmethod::TestMethod {
protected:
  string  PortNames;

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   *
   *Note: Test Method API should not be used in this method.
   */
  virtual void initialize()
  {
    //Add your initialization code here
    addParameter("PortNames",
                 "PinString",
                 &PortNames,
                 testmethod::TM_PARAMETER_INPUT)
      .setDefault("pHDMI_CLK");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
    //Add your test code here.
	  char tmpchar[256];
	  sprintf(tmpchar,"FTCG SKPR,NEVER,(%s)\n",PortNames.c_str());
	  FW_TASK(tmpchar);
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
REGISTER_TESTMETHOD("bm_ac_tml.Keep_Alive.Keep_Alive_DISABLE", Keep_Alive_DISABLE);




class Keep_Alive: public testmethod::TestMethod {
protected:
  string  Enable_PortNames;
  string  Disable_PortNames;

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   *
   *Note: Test Method API should not be used in this method.
   */
  virtual void initialize()
  {
    //Add your initialization code here
    addParameter("Enable_PortNames",
                 "PinString",
                 &Enable_PortNames,
                 testmethod::TM_PARAMETER_INPUT)
      .setDefault("None");
    addParameter("Disable_PortNames",
                 "PinString",
                 &Disable_PortNames,
                 testmethod::TM_PARAMETER_INPUT)
      .setDefault("None");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
    //Add your test code here.
	  char tmpchar[256];
	  sprintf(tmpchar,"FTCG SKPR,ON,(%s)\n",Enable_PortNames.c_str());
	  FW_TASK(tmpchar);
	  sprintf(tmpchar,"FTCG SKPR,NEVER,(%s)\n",Disable_PortNames.c_str());
	  FW_TASK(tmpchar);
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
REGISTER_TESTMETHOD("bm_ac_tml.Keep_Alive", Keep_Alive);
