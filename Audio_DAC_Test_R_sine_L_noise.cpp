#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;
using namespace V93kLimits;
#include "TMUtils.h"
#include "Common.h"
/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class Audio_DAC_Test_R_sine_L_noise: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
   string dgtpins,rly_on,rly_off;
   double sample_freq;
   int debug_mode,RelCycles,dgt_mode;
   int mNpoint;
   int mFs_Hz;
  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
	addParameter("DGT_Pins",       "string",    &dgtpins)
				.setDefault("")
				.setComment("specify the dgt pins");


	addParameter("Npoints","int",&mNpoint)
				.setDefault("")
				.setComment("the number of points sampled");

	addParameter("Fs_Hz","int",&mFs_Hz)
				.setDefault("");

	addParameter("RelCycles", "int",    &RelCycles)
				.setDefault("")
				.setComment("specify RelCycles");
	addParameter("DGT_Mode", "int",    &dgt_mode)
				.setDefault("")
				.setOptions("0:1")
				.setComment("specify dgt mode. 0:SINGLE, 1:DIFFERENTIAL");

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

	ARRAY_D dgtz_waveform_L, dgtz_waveform_R,spect_waveform_R,spect_waveform_L,d_Specrum(mNpoint/2);
	int LSL20HzIndex = 0,USL20kHzIndex = 0;
	ARRAY_COMPLEX FFTDataArray(mNpoint/2);
	THDtype  		THDArray;
	vector<string> dgtpins_list;
	double FundSignalPwr = 0.0,tmpPwr = 0.0,SpurSignalPwr = 0.0, HarmSignalPwr = 0.0,NoiseSignalPwr = 0.0;
    double Noise_dBV = 0.0;
    double Vout_RMS_R = 0.0;
	tokenize(dgtpins,dgtpins_list, ",");
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

	   PPMU_SETTING PPMU_SET_FVMI_3P3;
	   PPMU_RELAY  PPMU_RELAY_ON_3P3,PPMU_RELAY_OFF_3P3,PPMU_RELAY_AC_OFF_3P3;
	   PPMU_MEASURE PPMU_Mea_3P3;
	   PPMU_CLAMP  PPMU_CLAMP_ON_3P3,PPMU_CLAMP_OFF_3P3;
	   TASK_LIST TASK_3P3;
	   PPMU_SET_FVMI_3P3.pin("AUDIO_VCM").mode("VFIM").vForce(1 V).iRange(2 mA).min(-2 mA).max(2 mA);
	   PPMU_SET_FVMI_3P3.wait(5 ms);
	   PPMU_RELAY_ON_3P3.pin("AUDIO_VCM").status("PPMU_ON");

	   PPMU_RELAY_OFF_3P3.pin("AUDIO_VCM").status("ALL_OFF");
	   TASK_3P3.add(PPMU_SET_FVMI_3P3).add(PPMU_RELAY_ON_3P3).add(PPMU_RELAY_OFF_3P3);
	   TASK_3P3.execute();


		for(unsigned int i=0;i<dgtpins_list.size();i++)
		{
			if(dgt_mode == 0)
			{
				Routing.pin(dgtpins_list[i]).connect(TM::SINGLE);
			}
			else
			{
				Routing.pin(dgtpins_list[i]).connect(TM::DIFFERENTIAL);
			}
		}
		FLUSH();
		for(unsigned int i=0;i<dgtpins_list.size();i++)
		{
			DGT(dgtpins_list[i]).enable();
		}
		WAIT_TIME(5 ms);
		EXECUTE_TEST();
		for(unsigned int i=0;i<dgtpins_list.size();i++)
		{
			DGT(dgtpins_list[i]).disable();
			Routing.pin(dgtpins_list[i]).disconnect();
		}
	ON_FIRST_INVOCATION_END();
	int site_index=CURRENT_SITE_NUMBER()-1;
	for(unsigned int i=0;i<dgtpins_list.size();i++){
        if(i == 0){
		  dgtz_waveform_R = DGT(dgtpins_list[i]).getWaveform();
		  DSP_RMS(dgtz_waveform_R,&Vout_RMS_R);
		  DSP_THD(dgtz_waveform_R, &THDArray, RelCycles,3, DB, 0);// calculate THD & SND
		  string sTsName;
		  GET_TESTSUITE_NAME(sTsName);
		  if(debug_mode)
		  {
			  	char temp1[50];
				sprintf(temp1,"orig_waveR_site%d",site_index+1);
				PUT_DEBUG(dgtpins, "orig_wave1", dgtz_waveform_R);
				PUT_DEBUG(dgtpins, "spect_wave1", spect_waveform_R);
				cout << sTsName << " :Vout_RMS_R = " << Vout_RMS_R <<endl;
				cout<<"CALC SNR_R="<<THDArray.snr<<endl;
		  }
		TestSet.cont(true).TEST("", "Vout_RMS_R", tmLimits, Vout_RMS_R);
		TestSet.cont(true).TEST("", "SNR_R", tmLimits, THDArray.snr);
        }
        else{
  		  dgtz_waveform_L = DGT(dgtpins_list[i]).getWaveform();
  		  DSP_FFT(dgtz_waveform_L,FFTDataArray,RECT);
  		  LSL20HzIndex = ((20*mNpoint*2)/(2*mFs_Hz))-1;
  		  USL20kHzIndex = ((20000*mNpoint*2)/(2*mFs_Hz))-1;
            if(LSL20HzIndex == -1){
          	  LSL20HzIndex =1;
            }

            for(int i =LSL20HzIndex;i<USL20kHzIndex+1;i++){
          	  if(i == RelCycles){
          		  FundSignalPwr = FFTDataArray[i].real()*FFTDataArray[i].real()+FFTDataArray[i].imag()*FFTDataArray[i].imag();
          	  }
          	  else if(i%RelCycles == 0){
          		         tmpPwr = FFTDataArray[i].real()*FFTDataArray[i].real()+FFTDataArray[i].imag()*FFTDataArray[i].imag();
                           if(SpurSignalPwr < tmpPwr){
                          	 SpurSignalPwr = tmpPwr;
                           }

                           HarmSignalPwr+= tmpPwr;
          	  }
          	  else {

   		                 tmpPwr = FFTDataArray[i].real()*FFTDataArray[i].real()+FFTDataArray[i].imag()*FFTDataArray[i].imag();
                           if(i == 1||i == RelCycles -1|| i == RelCycles +1){
                          	 tmpPwr = 0.0;
                           }

                           if(SpurSignalPwr < tmpPwr){
                          	 SpurSignalPwr = tmpPwr;
                           }

                           NoiseSignalPwr+= tmpPwr;
          	  }
            }

            Noise_dBV = 20*log10(sqrt(NoiseSignalPwr)/1);
  			string sTsName;
  			GET_TESTSUITE_NAME(sTsName);
  			if(debug_mode)
  			{
  				char temp1[50];
				sprintf(temp1,"orig_waveR_site%d",site_index+1);
  				PUT_DEBUG(dgtpins, "orig_wave1", dgtz_waveform_L);
  				PUT_DEBUG(dgtpins, "spect_wave1", spect_waveform_L);
  				cout << sTsName << " Noise_dBV_L = " << Noise_dBV <<endl;
  			}
  			TestSet.cont(true).TEST("", "Noise_dBV_L", tmLimits, Noise_dBV);
          }

	}

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
REGISTER_TESTMETHOD("bm_analog_tml.Audio_DAC_Test_R_sine_L_noise", Audio_DAC_Test_R_sine_L_noise);
