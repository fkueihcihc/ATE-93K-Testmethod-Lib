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
class Video_DAC_Dynamic_Test: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */

   string dgtpins,rly_on,rly_off;
   int debug_mode,RelCycles,Offline_flag;

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
   //Add your test code here.

  ARRAY_D dgtz_waveform, spect_waveform;
  string sTsName;
  GET_TESTSUITE_NAME(sTsName);


  ON_FIRST_INVOCATION_BEGIN();


    CONNECT();
    Routing.pin(dgtpins).connect(TM::SINGLE);
    FLUSH();
    DGT(dgtpins).enable();

    EXECUTE_TEST();

    DGT(dgtpins).disable();
    Routing.pin(dgtpins).disconnect();
  ON_FIRST_INVOCATION_END();

    dgtz_waveform = DGT(dgtpins).getWaveform();

  /***************************************** Generate Offline Data **********************************/
   GET_SYSTEM_FLAG("offline", &Offline_flag);
   if(Offline_flag)
   {
     dgtz_waveform.resize(4096);
     for(int ii=0;ii<4096;ii++)
     {
       double var=2*M_PI*ii/4096*13;
       dgtz_waveform[ii]=4096*sin(var);
     }
     cout<<"NOTE: It is offline mode!"<<endl;
   }
   /**************************************************   End  ******************************************/
  DSP_SPECTRUM(dgtz_waveform,spect_waveform,DB,1.414,RECT,0);

  if(debug_mode)
  {
    PUT_DEBUG(dgtpins, "orig_wave", dgtz_waveform);
    PUT_DEBUG(dgtpins, "spect_wave", spect_waveform);

  }

  DYNPARtype dac_thd;
  DSP_THD(dgtz_waveform,&dac_thd,RelCycles,5,3,DB,0);


  if(debug_mode)
  {
    cout << sTsName<<" dSFDR :" << dac_thd.sfdr <<endl;
  }

  TestSet.cont(true).TEST("", sTsName+"_dSFDR", tmLimits, dac_thd.sfdr);
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
REGISTER_TESTMETHOD("bm_analog_tml.Video_DAC_Dynamic_Test", Video_DAC_Dynamic_Test);
