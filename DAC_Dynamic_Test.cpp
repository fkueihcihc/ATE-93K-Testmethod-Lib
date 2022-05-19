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
class DAC_Dynamic_Test: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
   string dgtpins,rly_on,rly_off;
//   double sample_freq;
   int debug_mode,RelCycles;
  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
	addParameter("DGT_Pins",       "string",    &dgtpins)
				.setDefault("")
				.setComment("specify the dgt pins");
//	addParameter("Sampling_Frequency", "double",    &sample_freq)
//				.setDefault("")
//				.setComment("specify Sampling_Frequency");
	addParameter("debug_mode", "int",    &debug_mode)
				.setDefault("")
				.setOptions("0:1")
				.setComment("specify debug mode");
	addParameter("RelCycles", "int",    &RelCycles)
				.setDefault("")
				.setComment("specify RelCycles");
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

	ARRAY_D dgtz_waveform, spect_waveform;
	int num_pts,end_bin;
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
		DGT(dgtpins).enable();

		EXECUTE_TEST();

		DGT(dgtpins).disable();
		Routing.pin(dgtpins).disconnect();
	ON_FIRST_INVOCATION_END();

	dgtz_waveform = DGT(dgtpins).getWaveform();

	num_pts = dgtz_waveform.size();
	//end_bin = int (sample_freq / num_pts  + 0.5);


	//int spect_num = num_pts/2;
	DSP_SPECTRUM(dgtz_waveform,spect_waveform,DB,1.414,RECT,0);
	/*
	double pwrSignal = 0.0;
	pwrSignal = spect_waveform[RelCycles];
	//calc SFDR
	double pwrSpurious = 0.0;
	for(int i = RelCycles + 5; i< num_pts/2;i++)
	{
		if(spect_waveform[i]>pwrSpurious)
		{
			pwrSpurious = spect_waveform[i];
		}
	}

	double dSFDR = 10*( log10(pwrSignal/pwrSpurious));
	//calc SINAD
	double pwrSum = 0.0;
	for(int i =1;i<spect_waveform.size();i++)
	{
		pwrSum+= spect_waveform[i];
	}
	double dSINAD = 10*(log10(pwrSignal/(pwrSum-pwrSignal)));
*/
	if(debug_mode)
	{
		PUT_DEBUG(dgtpins, "orig_wave", dgtz_waveform);
		PUT_DEBUG(dgtpins, "spect_wave", spect_waveform);

	}
	double dSFDR,dSINAD;
	DYNPARtype dac_thd;

	DSP_THD(dgtz_waveform,&dac_thd,RelCycles,5,3,DB,0);
	dSFDR = dac_thd.sfdr;
	dSINAD = dac_thd.snd;
	string sTsName;
	GET_TESTSUITE_NAME(sTsName);
	if(debug_mode)
	{
		cout << sTsName<<" dSFDR :" << dSFDR <<endl;
		cout << sTsName<<" dSINAD :" << dSINAD <<endl;
	}

	TestSet.cont(true).TEST("", sTsName+"_dSFDR", tmLimits, dSFDR);
	TestSet.cont(true).TEST("", sTsName+"_dSINAD", tmLimits, dSINAD);
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
REGISTER_TESTMETHOD("bm_analog_tml.DAC_Dynamic_Test", DAC_Dynamic_Test);
