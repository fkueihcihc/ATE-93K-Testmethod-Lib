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
class Video_DAC_Static_Test: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
   string dgtpins,rly_on,rly_off;
   double fullscale;
   int debug_mode,Offline_flag;

  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
	addParameter("DGT_Pins",       "string",    &dgtpins)
				.setDefault("")
				.setComment("specify the dgt pins");
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


/************************************ Generate Offline Data ************************************/
	GET_SYSTEM_FLAG("offline",&Offline_flag);
	  if(Offline_flag) {
		  dgtz_waveform.resize(1024);
		  for(int ii=0;ii<1024-1 ;ii++ ) {
			  if(ii<10){
				  dgtz_waveform[ii]=0;
			  }
			  else if(ii%10==0){
				  dgtz_waveform[ii+1]=dgtz_waveform[ii]+1;
			  }
			  else
				  dgtz_waveform[ii+1]=dgtz_waveform[ii];
		  }
		  cout<<"NOTE: It is offline mode !"<<endl;
	  }
/********************************** end **************************************/
	//create ramp
	DSP_MINMAX(dgtz_waveform,&dmin,&dmax,&imin,&imax);

	start = 0;
	while((dgtz_waveform[start]<dmin+0.01)&&(start<dgtz_waveform.size()))
	{
		start = start + 1;
	}
	end = dgtz_waveform.size() - 1;
	while((dgtz_waveform[end]>dmax-0.01)&&(end>0))
	{
		end = end -1;
	}
//	start = 50;end = 950;
	m = end - start + 1;
	ramp_waveform.resize(m);
	if(m>1)
	{
		int n = 0;

		for(int i=start;i<=end;i++)
		{
			ramp_waveform[n] = dgtz_waveform[i];
			n++;

		}
		//get inl, dnl

		DSP_LINEAR(ramp_waveform,&dac_lin,dac_dnla,dac_inla,MIN_RMS_LSB);
		//get zero offset
		double zero_sum = 0;
		double full_sum = 0;
		for(int i=0;i<=start;i++)
		{
			zero_sum+=dgtz_waveform[i];
		}
		offset = zero_sum/(start + 1)/fullscale;
		//get gain error
		for(int i=dgtz_waveform.size()-1;i>end;i--)
		{
			full_sum+=dgtz_waveform[i];
		}
		double full_offset = full_sum/(dgtz_waveform.size()-end + 1);
		gain_error = (full_offset - offset)/fullscale  - 1;
		inl = dac_lin.iamax;
		dnl = dac_lin.damax;
//		offset = offset * 100;
//		gain_error = gain_error * 100;

	}
	else
	{
//		offset = -999.99;
//		gain_error = -999.99;
		inl = -999.99;
		dnl = -999.99;
	}
	string sTsName;
		GET_TESTSUITE_NAME(sTsName);
	if(debug_mode)
	{
		PUT_DEBUG(dgtpins, "orig_wave", dgtz_waveform);
		PUT_DEBUG(dgtpins, "ramp_wave", ramp_waveform);
		if(m>1)
		{
			PUT_DEBUG(dgtpins, "dac_dnl", dac_dnla);
			PUT_DEBUG(dgtpins, "dac_inl", dac_inla);
		}
		cout <<sTsName <<" offset :" << offset <<endl;
		cout <<sTsName <<" gain_error :" << gain_error <<endl;
		cout <<sTsName <<" inl :" << inl <<endl;
		cout <<sTsName <<" dnl :" << dnl <<endl;

	}

//	TestSet.cont(true).TEST("", sTsName+"_offset", tmLimits, offset);
//	TestSet.cont(true).TEST("", sTsName+"_gain_error", tmLimits, gain_error);
	TestSet.cont(true).TEST("", sTsName+"_INL", tmLimits, inl);
	TestSet.cont(true).TEST("", sTsName+"_DNL", tmLimits, dnl);

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
REGISTER_TESTMETHOD("bm_analog_tml.Video_DAC_Static_Test", Video_DAC_Static_Test);
