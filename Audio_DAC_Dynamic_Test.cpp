#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
#include "TMUtils.h"
#include "Common.h"
using namespace std;
using namespace V93kLimits;
/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class Audio_DAC_Dynamic_Test: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */

   string dgtpins;
   int debug_mode,RelCycles,Offline_flag;

  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
	addParameter("DGT_Pins",
			"PinString",
			&dgtpins,
			testmethod::TM_PARAMETER_INPUT)
  .setDefault("")
  .setComment("specify the dgt pins");
  addParameter("debug_mode", "int",    &debug_mode)
        .setDefault("")
        .setOptions("0:1")
        .setComment("specify debug mode");
  addParameter("RelCycles", "int",    &RelCycles)
        .setDefault("")
        .setComment("specify RelCycles");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
   //Add your test code here.

  ARRAY_D dgtz_waveform_L,dgtz_waveform_R, spect_waveform_L,spect_waveform_R;
  string sTsName;

  vector<string> dgtpins_list;
  tokenize(dgtpins, dgtpins_list, ",");
  GET_TESTSUITE_NAME(sTsName);


  ON_FIRST_INVOCATION_BEGIN();


    CONNECT();
    Routing.pin(dgtpins).connect(TM::SINGLE);
    FLUSH();
    WAIT_TIME(5 ms);
	for(unsigned int i=0;i<dgtpins_list.size();i++)
	{
		DGT(dgtpins_list[i]).enable();
	}

	EXECUTE_TEST();

	for(unsigned int i=0;i<dgtpins_list.size();i++)
	{
		DGT(dgtpins_list[i]).disable();
		Routing.pin(dgtpins_list[i]).disconnect();
	}
  ON_FIRST_INVOCATION_END();




    for(unsigned int i =0;i<dgtpins_list.size();i++)
    {
    	if(dgtpins_list[i]=="AUDIO_OUTL"){

    	    dgtz_waveform_L = DGT(dgtpins_list[i]).getWaveform();

    	    /***************************************** Generate Offline Data **********************************/
    	    GET_SYSTEM_FLAG("offline", &Offline_flag);
    	    if(Offline_flag)
    	    {
    	    	dgtz_waveform_L.init(0);
    	    	dgtz_waveform_R.init(0);
    	    	dgtz_waveform_L.resize(512);
    	    	dgtz_waveform_R.resize(512);
    	    	for (int ii=0;ii<512-1;ii++)
    	    	{
    	    		double var=2*M_PI*ii/512*13;
    	    		dgtz_waveform_L[ii]=512*sin(var);
    	           dgtz_waveform_R[ii]=512*sin(var);
    	      }
    	      cout<<"NOTE: It is offline mode!"<<endl;
    	    }
    	    /**************************************************   End  ******************************************/


    		DSP_SPECTRUM(dgtz_waveform_L,spect_waveform_L,DB,1.414,RECT,0);

    		if(debug_mode)
    		{
    			PUT_DEBUG(dgtpins, "orig_wave_L", dgtz_waveform_L);
    			PUT_DEBUG(dgtpins, "spect_wave_L", spect_waveform_L);

    		}

    		DYNPARtype dac_thd;
    		DSP_THD(dgtz_waveform_L,&dac_thd,RelCycles,5,3,DB,0);


    		if(debug_mode)
    		{
    			cout << sTsName<<" dSNR_L :" << dac_thd.snr <<endl;
    			cout << sTsName<<" dTHD_L :" << dac_thd.thd <<endl;
    		}

    		TestSet.cont(true).TEST("", sTsName+"_dSNR_L", tmLimits, dac_thd.snr);
    		TestSet.cont(true).TEST("", sTsName+"_dTHD_L", tmLimits, dac_thd.thd);
    	}
    	else {
    		dgtz_waveform_R = DGT(dgtpins_list[i]).getWaveform();

    		 /***************************************** Generate Offline Data **********************************/
    		    	    GET_SYSTEM_FLAG("offline", &Offline_flag);
    		    	    if(Offline_flag)
    		    	    {
    		    	    	dgtz_waveform_L.init(0);
    		    	    	dgtz_waveform_R.init(0);
    		    	    	dgtz_waveform_L.resize(512);
    		    	    	dgtz_waveform_R.resize(512);
    		    	    	for (int ii=0;ii<512-1;ii++)
    		    	    	{
    		    	    		double var=2*M_PI*ii/512*13;
    		    	    		dgtz_waveform_L[ii]=512*sin(var);
    		    	           dgtz_waveform_R[ii]=512*sin(var);
    		    	      }
    		    	      cout<<"NOTE: It is offline mode!"<<endl;
    		    	    }
    		    	    /**************************************************   End  ******************************************/

    		DSP_SPECTRUM(dgtz_waveform_R,spect_waveform_R,DB,1.414,RECT,0);

    		if(debug_mode)
    		{
    			PUT_DEBUG(dgtpins, "orig_wave_R", dgtz_waveform_R);
    			PUT_DEBUG(dgtpins, "spect_wave_R", spect_waveform_R);

    		}

    		DYNPARtype dac_thd;
    		DSP_THD(dgtz_waveform_R,&dac_thd,RelCycles,5,3,DB,0);


    		if(debug_mode)
    		{
    			cout << sTsName<<" dSNR_R :" << dac_thd.snr <<endl;
    			cout << sTsName<<" dTHD_R :" << dac_thd.thd <<endl;
    		}

    		TestSet.cont(true).TEST("", sTsName+"_dSNR_R", tmLimits, dac_thd.snr);
    		TestSet.cont(true).TEST("", sTsName+"_dTHD_R", tmLimits, dac_thd.thd);
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
REGISTER_TESTMETHOD("bm_analog_tml.Audio_DAC_Dynamic_Test", Audio_DAC_Dynamic_Test);
