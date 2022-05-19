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
class errormap: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
	int		Debug_Mode;
    string    sPort;
virtual void initialize()
{
  //Add your initialization code here
  //Note: Test Method API should not be used in this method!
	  	addParameter("Debug_Mode", "int", &Debug_Mode)
	                 .setDefault("1")
	                 .setOptions("0:1")
	                 .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

	  	addParameter("Port", "string", &sPort)
	                 .setDefault("@")
                   .setOptions("");

}
  /**
   *This test is invoked per site.
   */
  virtual void run()
  {

		FUNCTIONAL_TEST();

		if(Debug_Mode){

		ERROR_MAP_TEST            emap(TM::TESTER_CYCLES);
		ARRAY_BOOL                pfresult;
		ARRAY_BOOL                pfresult_all;

		STRING_VECTOR             sv = PinUtility.getDigitalPinNamesFromPinList(sPort,TM::O_PIN|TM::IO_PIN,1,0);

		STRING_VECTOR::iterator   it;
 //       emap.getErrorCount();
		for(it = sv.begin();it!=sv.end();it++){     //pin loop ..
			int j = 0;
			emap.pin(*it).getPassFailResultsInRange(0,emap.getLastCycleExecuted(),pfresult);

			if(it == sv.begin()) pfresult_all = pfresult;
          else{
				for(int i = 0; i< pfresult.size();i++) {    //cycle result loop based on one singel pin
	//				pfresult_all[i] &= pfresult[i];
				    if (pfresult[i] == 0){    //0 indicate there gets error cycle in the current pin
                        cout<<"fail pin name= "<< *it <<"    fail cycle is = "<< i << endl;   //fail cycle details for signel pin
				    	j++;                    //fail count...
				    }

				}
				    if(j > 0){
						cout<<"fail pin name= "<< *it << endl;  //cout
						cout<<"fail cycle count = "<< j << endl;
				    }
			}

		}
	}
//		cout<<"cycle = "<< pfresult_all<< endl;
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
REGISTER_TESTMETHOD("bm_ac_tml.errormap", errormap);
