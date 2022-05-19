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
class Audio_DAC_NoisePower_test: public testmethod::TestMethod {

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

	addParameter("debug_mode", "int",    &debug_mode)
				.setDefault("")
				.setOptions("0:1")
				.setComment("specify debug mode");
	addParameter("RelCycles", "int",    &RelCycles)
				.setDefault("")
				.setComment("specify RelCycles");
	addParameter("DGT_Mode", "int",    &dgt_mode)
				.setDefault("")
				.setOptions("0:1")
				.setComment("specify dgt mode. 0:SINGLE, 1:DIFFERENTIAL");
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

	ARRAY_D dgtz_waveform_L, dgtz_waveform_R,spect_waveform_L,spect_waveform_R,d_Specrum(mNpoint/2);
	int LSL20HzIndex = 0,USL20kHzIndex = 0;
	ARRAY_COMPLEX FFTDataArray_R(mNpoint/2),FFTDataArray_L(mNpoint/2);
	vector<string> dgtpins_list;
	double FundSignalPwr = 0.0,tmpPwr = 0.0,SpurSignalPwr = 0.0, HarmSignalPwr = 0.0,NoiseSignalPwr = 0.0;
    double Noise_dBV_R = 0.0,Noise_dBV_L = 0.0;
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

//	   PPMU_SETTING PPMU_SET_FVMI_3P3;
//	   PPMU_RELAY  PPMU_RELAY_ON_3P3,PPMU_RELAY_OFF_3P3,PPMU_RELAY_AC_OFF_3P3;
//	   PPMU_MEASURE PPMU_Mea_3P3;
//	   PPMU_CLAMP  PPMU_CLAMP_ON_3P3,PPMU_CLAMP_OFF_3P3;
//	   TASK_LIST TASK_3P3;
//	   PPMU_SET_FVMI_3P3.pin("AUDIO_VCM").mode("VFIM").vForce(1 V).iRange(2 mA).min(-2 mA).max(2 mA);
//	   PPMU_SET_FVMI_3P3.wait(5 ms);
//	   PPMU_RELAY_ON_3P3.pin("AUDIO_VCM").status("PPMU_ON");
//
//	   PPMU_RELAY_OFF_3P3.pin("AUDIO_VCM").status("ALL_OFF");
//	   TASK_3P3.add(PPMU_SET_FVMI_3P3).add(PPMU_RELAY_ON_3P3).add(PPMU_RELAY_OFF_3P3);
//	   TASK_3P3.execute();


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

		for(unsigned int i=0;i<dgtpins_list.size();i++)
		{
			DGT(dgtpins_list[i]).enable();
		}
		FLUSH(TM::APRM);
		WAIT_TIME(5 ms);
		EXECUTE_TEST();
		for(unsigned int i=0;i<dgtpins_list.size();i++)
		{
			DGT(dgtpins_list[i]).disable();
			Routing.pin(dgtpins_list[i]).disconnect();
		}
	ON_FIRST_INVOCATION_END();

	for(unsigned int j=0;j<dgtpins_list.size();j++){
        if(j == 0){
		  dgtz_waveform_R = DGT(dgtpins_list[j]).getWaveform();
		  DSP_FFT(dgtz_waveform_R,FFTDataArray_R,RECT);
		  LSL20HzIndex = ((20*mNpoint*2)/(2*mFs_Hz))-1;
		  USL20kHzIndex = ((20000*mNpoint*2)/(2*mFs_Hz))-1;
          if(LSL20HzIndex == -1){
        	  LSL20HzIndex =1;
          }

          for(int i =LSL20HzIndex;i<USL20kHzIndex+1;i++){
        	  if(i == RelCycles){
        		  FundSignalPwr = FFTDataArray_R[i].real()*FFTDataArray_R[i].real()+FFTDataArray_R[i].imag()*FFTDataArray_R[i].imag();
        	  }
        	  else if(i%RelCycles == 0){
        		         tmpPwr = FFTDataArray_R[i].real()*FFTDataArray_R[i].real()+FFTDataArray_R[i].imag()*FFTDataArray_R[i].imag();
                         if(SpurSignalPwr < tmpPwr){
                        	 SpurSignalPwr = tmpPwr;
                         }

                         HarmSignalPwr+= tmpPwr;
        	  }
        	  else {

 		                 tmpPwr = FFTDataArray_R[i].real()*FFTDataArray_R[i].real()+FFTDataArray_R[i].imag()*FFTDataArray_R[i].imag();
                         if(i == 1||i == RelCycles -1|| i == RelCycles +1){
                        	 tmpPwr = 0.0;
                         }

                         if(SpurSignalPwr < tmpPwr){
                        	 SpurSignalPwr = tmpPwr;
                         }

                         NoiseSignalPwr+= tmpPwr;
        	  }
          }

          Noise_dBV_R = 20*log10(sqrt(NoiseSignalPwr)/1);

			string sTsName;
			GET_TESTSUITE_NAME(sTsName);
			if(debug_mode)
			{
				PUT_DEBUG(dgtpins_list[j], "orig_wave1", dgtz_waveform_R);
				PUT_DEBUG(dgtpins_list[j], "spect_wave1", spect_waveform_R);
				cout << sTsName << " Noise_dBV_R = " << Noise_dBV_R <<endl;
			}
			TestSet.cont(true).TEST("", "Noise_dBV_R", tmLimits, Noise_dBV_R);
        }
        else{
  		  dgtz_waveform_L = DGT(dgtpins_list[j]).getWaveform();
  		  DSP_FFT(dgtz_waveform_L,FFTDataArray_L,RECT);
  		  LSL20HzIndex = ((20*mNpoint*2)/(2*mFs_Hz))-1;
  		  USL20kHzIndex = ((20000*mNpoint*2)/(2*mFs_Hz))-1;
            if(LSL20HzIndex == -1){
          	  LSL20HzIndex =1;
            }

            for(int i =LSL20HzIndex;i<USL20kHzIndex+1;i++){
          	  if(i == RelCycles){
          		  FundSignalPwr = FFTDataArray_L[i].real()*FFTDataArray_L[i].real()+FFTDataArray_L[i].imag()*FFTDataArray_L[i].imag();
          	  }
          	  else if(i%RelCycles == 0){
          		         tmpPwr = FFTDataArray_L[i].real()*FFTDataArray_L[i].real()+FFTDataArray_L[i].imag()*FFTDataArray_L[i].imag();
                           if(SpurSignalPwr < tmpPwr){
                          	 SpurSignalPwr = tmpPwr;
                           }

                           HarmSignalPwr+= tmpPwr;
          	  }
          	  else {

   		                 tmpPwr = FFTDataArray_L[i].real()*FFTDataArray_L[i].real()+FFTDataArray_L[i].imag()*FFTDataArray_L[i].imag();
                           if(i == 1||i == RelCycles -1|| i == RelCycles +1){
                          	 tmpPwr = 0.0;
                           }

                           if(SpurSignalPwr < tmpPwr){
                          	 SpurSignalPwr = tmpPwr;
                           }

                           NoiseSignalPwr+= tmpPwr;
          	  }
            }

                Noise_dBV_L = 20*log10(sqrt(NoiseSignalPwr)/1);
  			    string sTsName;
  			    GET_TESTSUITE_NAME(sTsName);
				if(debug_mode)
				{
					PUT_DEBUG(dgtpins_list[j], "orig_wave2", dgtz_waveform_L);
					PUT_DEBUG(dgtpins_list[j], "spect_wave2", spect_waveform_L);
  				cout << sTsName << " Noise_dBV_L = " << Noise_dBV_L <<endl;
  			}
  			TestSet.cont(true).TEST("", "Noise_dBV_L", tmLimits, Noise_dBV_L);
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
REGISTER_TESTMETHOD("bm_analog_tml.Audio_DAC_NoisePower_test", Audio_DAC_NoisePower_test);
