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
class Video_PGA_Test: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
   string dgtpins,rly_on,rly_off;
   double sample_freq, fullscale;
   int debug_mode;
  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
	addParameter("DGT_Pins",       "string",    &dgtpins)
				.setDefault("")
				.setComment("specify the dgt pins");
	addParameter("Sampling_Frequency", "double",    &sample_freq)
				.setDefault("")
				.setComment("specify Sampling_Frequency");
	addParameter("fullscale", "double",    &fullscale)
				.setDefault("")
				.setComment("specify the full scale");
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

	ARRAY_D dgtz_waveform, ramp_waveform, dac_dnla,dac_inla;
	LINEARtype dac_lin;
	double dmin,dmax,offset,gain_error,inl,dnl;
	int imin,imax,start,end,m;
    //Add your test code here.
	ON_FIRST_INVOCATION_BEGIN();

    if(rly_on!="None")
		 {
		   Routing.util(rly_on).on();
   	   FLUSH();
		 }
	 if(rly_off!="None")
	  	 {
		   Routing.util(rly_off).off();
		   FLUSH();
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
	double sum = 0;
	for(int i=0; i<dgtz_waveform.size();i++)
	{
		sum+=dgtz_waveform[i];
	}
	double value = sum / dgtz_waveform.size();
	string sTsName;
		GET_TESTSUITE_NAME(sTsName);
	if(debug_mode)
	{
		PUT_DEBUG(dgtpins, "orig_wave", dgtz_waveform);
		cout <<sTsName <<" value :" << value <<endl;

	}

	TestSet.cont(true).TEST("", sTsName, tmLimits, value);

	//DISCONNECT();
	//WAIT_TIME(3 s);
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
REGISTER_TESTMETHOD("bm_analog_tml.Video_PGA_Test", Video_PGA_Test);
