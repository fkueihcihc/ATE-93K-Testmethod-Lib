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
class Audio_ADC_Debug: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
   string cap_variable,awgpins,rly_on,rly_off;
   double sample_freq;
   int debug_mode,RelCycles,awg_mode;
  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
	addParameter("Capture_Variable",       "string",    &cap_variable)
				.setDefault("")
				.setComment("specify the capture variable");
	addParameter("AWG_Pins",       "string",    &awgpins)
				.setDefault("")
				.setComment("specify the awgpins");
	addParameter("Sampling_Frequency", "double",    &sample_freq)
				.setDefault("")
				.setComment("specify Sampling_Frequency");
	addParameter("debug_mode", "int",    &debug_mode)
				.setDefault("")
				.setOptions("0:1")
				.setComment("specify debug mode");
	addParameter("RelCycles", "int",    &RelCycles)
				.setDefault("")
				.setComment("specify RelCycles");
	addParameter("AWG_Mode", "int",    &awg_mode)
				.setDefault("")
				.setOptions("0:1")
				.setComment("specify awg mode. 0:SINGLE, 1:DIFFERENTIAL");
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

	ARRAY_I digtal_waveform,digtal_waveform_L,digtal_waveform_R;
	ARRAY_D spect_waveform_L,spect_waveform_R;

	vector<string> awgpins_list;
    tokenize(awgpins,awgpins_list, ",");
	//int num_pts,end_bin;
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
		for(int i =0;i<awgpins_list.size();i++)
		{
			if(awg_mode == 0)
			{
				Routing.pin(awgpins_list[i]).connect(TM::SINGLE);
			}
			else
			{
				Routing.pin(awgpins_list[i]).connect(TM::DIFFERENTIAL);
			}
		}
		FLUSH();
		//Routing.pin(awgpins).connect();
		for(int i =0;i<awgpins_list.size();i++)
		{
			AWG(awgpins_list[i]).enable();
		}
		DIGITAL_CAPTURE_TEST(1.0);
		for(int i =0;i<awgpins_list.size();i++)
		{
			AWG(awgpins_list[i]).disable();
			Routing.pin(awgpins_list[i]).disconnect();
		}
	ON_FIRST_INVOCATION_END();
	digtal_waveform = VECTOR(cap_variable).getVectors();

	ARRAY_I digital_waveform_16bit;
	digital_waveform_16bit.resize(digtal_waveform.size()/16);
	for(int i=0;i<digtal_waveform.size()-15;i+=16)
	{
		if((digtal_waveform[i] == 0))
		{
			digital_waveform_16bit[i/16] = (1<<15) + (digtal_waveform[i+1] << 14) + (digtal_waveform[i+2] << 13)
				                      +(digtal_waveform[i+3] << 12) + (digtal_waveform[i+4] << 11) + (digtal_waveform[i+5] << 10)
				                      +(digtal_waveform[i+6] << 9) + (digtal_waveform[i+7] << 8) + (digtal_waveform[i+8] << 7)
				                      +(digtal_waveform[i+9] << 6) + (digtal_waveform[i+10] << 5) + (digtal_waveform[i+11] << 4)
				                      +(digtal_waveform[i+12] << 3) + (digtal_waveform[i+13] << 2) + (digtal_waveform[i+14] << 1)
				                      + digtal_waveform[i+15];
		}
		if((digtal_waveform[i] == 1))
		{
			digital_waveform_16bit[i/16] = (digtal_waveform[i+1] << 14) + (digtal_waveform[i+2] << 13)
				                      +(digtal_waveform[i+3] << 12) + (digtal_waveform[i+4] << 11) + (digtal_waveform[i+5] << 10)
				                      +(digtal_waveform[i+6] << 9) + (digtal_waveform[i+7] << 8) + (digtal_waveform[i+8] << 7)
				                      +(digtal_waveform[i+9] << 6) + (digtal_waveform[i+10] << 5) + (digtal_waveform[i+11] << 4)
				                      +(digtal_waveform[i+12] << 3) + (digtal_waveform[i+13] << 2) + (digtal_waveform[i+14] << 1)
				                      + digtal_waveform[i+15];
		}
//		digital_waveform_16bit[i/24] = (digtal_waveform[i]<< 23) + (digtal_waveform[i+1] << 22) + (digtal_waveform[i+2] << 21)
//				                      +(digtal_waveform[i+3] << 20) + (digtal_waveform[i+4] << 19) + (digtal_waveform[i+5] << 18)
//				                      +(digtal_waveform[i+6] << 17) + (digtal_waveform[i+7] << 16) + (digtal_waveform[i+8] << 15)
//				                      +(digtal_waveform[i+9] << 14) + (digtal_waveform[i+10] << 13) + (digtal_waveform[i+11] << 12)
//				                      +(digtal_waveform[i+12] << 11) + (digtal_waveform[i+13] << 10) + (digtal_waveform[i+14] << 9)
//				                      +(digtal_waveform[i+15] << 8) + (digtal_waveform[i+16] << 7)+(digtal_waveform[i+17] << 6)
//				                      +(digtal_waveform[i+18] << 5)+(digtal_waveform[i+19] << 4)+(digtal_waveform[i+20] << 3)
//				                      +(digtal_waveform[i+21] << 2)+(digtal_waveform[i+22] << 1)+(digtal_waveform[i+23] );
//		digital_waveform_16bit[i/16] = (digtal_waveform[i]) + (digtal_waveform[i+1] << 1) + (digtal_waveform[i+2] << 2)
//				                      +(digtal_waveform[i+3] << 3) + (digtal_waveform[i+4] << 4) + (digtal_waveform[i+5] << 5)
//				                      +(digtal_waveform[i+6] << 6) + (digtal_waveform[i+7] << 7) + (digtal_waveform[i+8] << 8)
//				                      +(digtal_waveform[i+9] << 9) + (digtal_waveform[i+10] << 10) + (digtal_waveform[i+11] << 11)
//				                      +(digtal_waveform[i+12] << 12) + (digtal_waveform[i+13] << 13) + (digtal_waveform[i+14] << 14)
//				                      +(digtal_waveform[i+15] << 15);

	}
	digtal_waveform_L.resize(digital_waveform_16bit.size()/2);
	digtal_waveform_R.resize(digital_waveform_16bit.size()/2);
	for(int i=0;i<digital_waveform_16bit.size()-1;i+=2)
	{
		digtal_waveform_L[i/2] = digital_waveform_16bit[i];
		digtal_waveform_R[i/2] = digital_waveform_16bit[i+1];
	}


	DSP_SPECTRUM(digtal_waveform_L,spect_waveform_L,PWR,0,HANNING,0);
	DSP_SPECTRUM(digtal_waveform_R,spect_waveform_R,PWR,0,HANNING,0);
	//calc total power
	double pwr_all_L,pwr_all_R,pwr_sig_L,pwr_sig_R,pwr_thd_L,pwr_thd_R;
	double snr_l,thd_l,sfdr_l,snr_r,thd_r,sfdr_r;
	for(int i=3;i<spect_waveform_L.size();i++)
	{
		pwr_all_L+=spect_waveform_L[i];
		pwr_all_R+=spect_waveform_R[i];
	}
	cout << "pwr_all_L" << pwr_all_L <<endl;
	cout << "pwr_all_R" << pwr_all_R <<endl;
	//cal signal power
	for(int i=RelCycles-2;i<RelCycles+2;i++)
	{
		pwr_sig_L+=spect_waveform_L[i];
		pwr_sig_R+=spect_waveform_R[i];
	}
	cout << "pwr_sig_L" << pwr_sig_L <<endl;
	cout << "pwr_sig_R" << pwr_sig_R <<endl;
	//cal thd power
	pwr_thd_L = spect_waveform_L[RelCycles*2]+spect_waveform_L[RelCycles*2-1]+spect_waveform_L[RelCycles*2+1]+
			    spect_waveform_L[RelCycles*3]+spect_waveform_L[RelCycles*3-1]+spect_waveform_L[RelCycles*3+1]+
			    spect_waveform_L[RelCycles*4]+spect_waveform_L[RelCycles*4-1]+spect_waveform_L[RelCycles*4+1]+
			    spect_waveform_L[RelCycles*5]+spect_waveform_L[RelCycles*5-1]+spect_waveform_L[RelCycles*5+1];
	pwr_thd_R = spect_waveform_R[RelCycles*2]+spect_waveform_R[RelCycles*2-1]+spect_waveform_R[RelCycles*2+1]+
			    spect_waveform_R[RelCycles*3]+spect_waveform_R[RelCycles*3-1]+spect_waveform_R[RelCycles*3+1]+
			    spect_waveform_R[RelCycles*4]+spect_waveform_R[RelCycles*4-1]+spect_waveform_R[RelCycles*4+1]+
			    spect_waveform_R[RelCycles*5]+spect_waveform_R[RelCycles*5-1]+spect_waveform_R[RelCycles*5+1];
	cout << "pwr_thd_L" << pwr_thd_L <<endl;
	cout << "pwr_thd_R" << pwr_thd_R <<endl;
	//calc snr
	snr_l = 10*(log10(pwr_sig_L/(pwr_all_L - pwr_sig_L - pwr_thd_L)));
	snr_r = 10*(log10(pwr_sig_R/(pwr_all_R - pwr_sig_R - pwr_thd_R)));
	DYNPARtype dac_thd_l,dac_thd_r;

	//DSP_THD(digtal_waveform_L,&dac_thd_l,RelCycles,5,3,DB,HANNING);
	//DSP_THD(digtal_waveform_R,&dac_thd_r,RelCycles,5,3,DB,HANNING);
	DSP_THD(digtal_waveform_L,&dac_thd_l,RelCycles,5,3,DB,0);
	DSP_THD(digtal_waveform_R,&dac_thd_r,RelCycles,5,3,DB,0);
	//snr_l = dac_thd_l.snr;
	//snr_r = dac_thd_r.snr;
	thd_l = dac_thd_l.thd;
	thd_r = dac_thd_r.thd;
	sfdr_l = dac_thd_l.sfdr;
	sfdr_r = dac_thd_r.sfdr;

	if(debug_mode)
	{
		PUT_DEBUG(awgpins, "orig_wave_L", digtal_waveform_L);
		PUT_DEBUG(awgpins, "spect_wave_L", spect_waveform_L);
		PUT_DEBUG(awgpins, "orig_wave_R", digtal_waveform_R);
		PUT_DEBUG(awgpins, "spect_wave_R", spect_waveform_R);
		cout << "snr_l :" << snr_l <<endl;
		cout << "snr_r :" << snr_r <<endl;
		cout << "thd_l :" << thd_l <<endl;
		cout << "thd_r :" << thd_r <<endl;
		cout << "sfdr_l :" << sfdr_l <<endl;
		cout << "sfdr_r :" << sfdr_r <<endl;
	}
	string sTsName;
	GET_TESTSUITE_NAME(sTsName);
//	TestSet.cont(true).TEST("", sTsName+"_snr_l", tmLimits, snr_l);
//	TestSet.cont(true).TEST("", sTsName+"_thd_l", tmLimits, thd_l);
//	TestSet.cont(true).TEST("", sTsName+"_sfdr_l", tmLimits,sfdr_l);
//	TestSet.cont(true).TEST("", sTsName+"_snr_r", tmLimits, snr_r);
//	TestSet.cont(true).TEST("", sTsName+"_thd_r", tmLimits, thd_r);
//	TestSet.cont(true).TEST("", sTsName+"_sfdr_r", tmLimits,sfdr_r);

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
REGISTER_TESTMETHOD("bm_analog_tml.Audio_ADC_Debug", Audio_ADC_Debug);
