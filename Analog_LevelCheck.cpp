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
class Analog_LevelCheck: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */

	  string dgtpins,rly_on,rly_off;
	   double fullscale;
	   int debug_mode;
	   int Offline_flag;

	  virtual void initialize()
	  {
	    //Add your initialization code here
	    //Note: Test Method API should not be used in this method!
		addParameter("DGT_Pins",       "string",    &dgtpins)
					.setDefault("")
					.setComment("specify the dgt pins");
		addParameter("debug_mode", "int",    &debug_mode)
					.setDefault("")
					.setOptions("0:1")
					.setComment("specify debug mode");
		addParameter("rly_on","string",&rly_on,testmethod::TM_PARAMETER_INPUT)
		            .setDefault("None");
		addParameter("rly_off","string",&rly_off,testmethod::TM_PARAMETER_INPUT)
					.setDefault("None");
	  }

	  /**
	   *This test is invoked per site.
	   */
	  virtual void run()
	  {

		  ARRAY_D dgtz_waveform, cut_waveform, dac_dnla,dac_inla;
		  double DC_level=0;

		string sTsName;
		//demo
		// orig head check

	    //Add your test code here. yes
		GET_TESTSUITE_NAME(sTsName);
		ON_FIRST_INVOCATION_BEGIN();


	    if(rly_on!="None")
			 {
			   Routing.util(rly_on).on();
	   	       FLUSH();
	   	       WAIT_TIME(1 ms);
			 }
		 if(rly_off!="None")
		  	 {
			   Routing.util(rly_off).off();
			   FLUSH();
			   WAIT_TIME(1 ms);
		  	 }


			CONNECT();
			Routing.pin(dgtpins).connect(TM::SINGLE);
			FLUSH();
			Analog.DGT(dgtpins).enable();

			EXECUTE_TEST();

			Analog.DGT(dgtpins).disable();
			Routing.pin(dgtpins).disconnect();


			ON_FIRST_INVOCATION_END();

			dgtz_waveform = DGT(dgtpins).getWaveform();
/***************************************** Generate Offline Data **********************************/
			GET_SYSTEM_FLAG("offline", &Offline_flag);
			if(Offline_flag)
			{
				dgtz_waveform.init(1);
				dgtz_waveform.resize(1024);

				cout<<"NOTE: It is offline mode!"<<endl;
			}
/**************************************************   End  ******************************************/

		for(int i=0;i<8;i++)
		{
			DC_level= DC_level + dgtz_waveform[i];
		}

		DC_level= DC_level/8;  //average
//		DSP_MINMAX(dgtz_waveform,&dmin,&dmax,&imin,&imax);
		if(debug_mode)
			cout<<sTsName<<"  "<<"DC level : "<<DC_level<<endl;

	   TestSet.cont(true).TEST("",sTsName,tmLimits,DC_level);



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
REGISTER_TESTMETHOD("bm_analog_tml.Analog_LevelCheck", Analog_LevelCheck);
