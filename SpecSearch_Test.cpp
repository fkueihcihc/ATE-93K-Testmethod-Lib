#include "testmethod.hpp"
#include "MAPI/ErrorMap.h"
//for test method API interfaces
#include "mapi.hpp"
using namespace std;

/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class SpecSearch_Test: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
	string spec_name,mode;
  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
	addParameter("spec_name", "string", &spec_name,testmethod::TM_PARAMETER_INPUT)
				.setDefault("");
	addParameter("mode", "string", &mode,testmethod::TM_PARAMETER_INPUT)
				.setDefault("")
				.setOptions("VIH:VIL:VOH:VOL");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
	//string V_SPEC = "VScale";
	string TestSuiteName;
	GET_TESTSUITE_NAME(TestSuiteName);
	LEVEL_SPEC   Vspec=Primary.getLevelSpec();
	double Cur_V_value=Vspec.getSpecValue(spec_name);
//	cout << Cur_V_value <<endl;
	double low,high;

	if(mode == "VIH")
	{
		low = 0.3;
		high = 1;
	}
	if(mode == "VIL")
	{
		low = 0;
		high = 3;
	}
	if(mode == "VOH")
	{
		low = 1;
		high = 2;
	}
	if(mode == "VOL")
	{
		low = 0;
		high = 1;
	}
	//int step = 20;
	//int result = 0;
	string test_name;
	int pre_result = 0;  // previous result, fail by default
	int k=0;
	//int result[20];
	for(double i=Cur_V_value*low;i<=Cur_V_value*high;i=i+Cur_V_value*(high-low)/20)
	{
		//Add your test code here.
		 Vspec.change(spec_name,i);
		 FLUSH(TM::APRM);
		 CONNECT();
		ON_FIRST_INVOCATION_BEGIN();
		FUNCTIONAL_TEST();
		ON_FIRST_INVOCATION_END();
		int result = GET_FUNCTIONAL_RESULT();

		if(k ==0 )  // for first test result
			pre_result = result;

		if(result != pre_result)
		{
			if(result==0)
			{
						//cout << TestSuiteName <<": " <<spec_name << "= " << i <<": result is Fail" <<endl;
						cout << TestSuiteName <<": " <<spec_name << "= " << i- Cur_V_value*(high-low)/20 <<": result is Pass" <<endl;
						break;
			}
			else
			{
						cout << TestSuiteName <<": " <<spec_name << "= " << i <<": result is Pass" <<endl;
						break;
			}

		}

		k++;


/*
		result[k] = GET_FUNCTIONAL_RESULT();
		k=k+1;

		if(result[k+1]==0 & result[k]==1)
		{
			if(result[k]==1)
			cout << TestSuiteName <<": " <<spec_name << "= " << i <<": result is Pass to Fail" <<endl;
			else
				cout << TestSuiteName <<": " <<spec_name << "= " << i <<": result is Fail to Pass" <<endl;
		}
*/
		DISCONNECT();
	}








	 Vspec.change(spec_name,Cur_V_value);
	 Vspec.restore();
	 FLUSH(TM::APRM);
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
REGISTER_TESTMETHOD("bm_ac_tml.SpecSearch_Test", SpecSearch_Test);
