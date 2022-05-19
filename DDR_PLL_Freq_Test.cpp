#include "testmethod.hpp"
#include "tml_def.h"
//for test method API interfaces
#include "mapi.hpp"
#include <libcicpi.h>
using namespace V93kLimits;
using namespace std;


/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class DDR_PLL_Freq_Test: public testmethod::TestMethod {

protected:
	string mVec_Var_1;
	string mVec_Var_2;
	string debug;
	string Relay_On;
	string Relay_Off;
	int FFT_MODE;
	double Sample_period_ns;

  virtual void initialize()
  {
	addParameter("mVec_Var_1", "string", &mVec_Var_1,testmethod::TM_PARAMETER_INPUT)
			.setDefault("")
			.setComment("The vector variable which captures device output signal");
	addParameter("mVec_Var_2", "string", &mVec_Var_2,testmethod::TM_PARAMETER_INPUT)
			.setDefault("")
			.setComment("The vector variable which captures device output signal");
	addParameter("Sample_Period_ns", "double", &Sample_period_ns).setComment(
			"the sample period of digital capture");
	addParameter("FFT_MODE", "int", &FFT_MODE).setDefault("1").setComment(
			"the FFT mode");
	addParameter("debug", "string", &debug).setDefault("NO")
				.setOptions("YES:NO")
				.setComment("the debug mode");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
	STRING TestsuitName;
	ARRAY_I capturearray_1;
	ARRAY_I capturearray_2;
	int capturesize_1;
	int capturesize_2;
	double Calculate_Freq_1;
	double Calculate_Freq_2;
	capturearray_1.resize(0);
	capturearray_2.resize(0);
	GET_TESTSUITE_NAME(TestsuitName);
	ON_FIRST_INVOCATION_BEGIN();
		CONNECT();
		if (Relay_On != "")
			Routing.util(Relay_On).on();
		if (Relay_Off != "")
			Routing.util(Relay_Off).off();
		DIGITAL_CAPTURE_TEST();
	ON_FIRST_INVOCATION_END();

	capturearray_1=VECTOR(mVec_Var_1).getVectors();
	capturearray_2=VECTOR(mVec_Var_2).getVectors();
	capturesize_1 = capturearray_1.size();
	capturesize_2 = capturearray_2.size();
	Calculate_Freq_1 = Frequency(capturearray_1,Sample_period_ns*1e-3,FFT_MODE);
	Calculate_Freq_2 = Frequency(capturearray_2,Sample_period_ns*1e-3,FFT_MODE);
	if (debug == "YES")
	{
		cout << TestsuitName << endl;
		PUT_DEBUG("", "pll_1", capturearray_1);
		PUT_DEBUG("", "pll_2", capturearray_2);
		cout <<"site:"<<CURRENT_SITE_NUMBER()<< " frequency dto[0] is : " << Calculate_Freq_1 << "  MHz" <<endl;
		cout <<"site:"<<CURRENT_SITE_NUMBER()<< " frequency dto[1] is : " << Calculate_Freq_2 << "  MHz" <<endl;
	}
     TESTSET().cont(TRUE).judgeAndLog_ParametricTest("DTO[0]",TestsuitName+"_DTO0",tmLimits,Calculate_Freq_1);
     TESTSET().cont(TRUE).judgeAndLog_ParametricTest("DTO[1]",TestsuitName+"_DTO1",tmLimits,Calculate_Freq_2);
//   TESTSET().cont(TRUE).judgeAndLog_ParametricTest("DTO[0]","PLL",LIMIT(TM::GT, 198, TM::LT, 218),Calculate_Freq_1);
//   TESTSET().cont(TRUE).judgeAndLog_ParametricTest("DTO[1]","PLL",LIMIT(TM::GT, 198, TM::LT, 218),Calculate_Freq_2);

    return;
  }

  DOUBLE Frequency(ARRAY_I temp_data, double Test_Period,int fft_mod)
  {
  	   DOUBLE      frequency_MHz;
  	   DOUBLE      gradient1;
  	   DOUBLE      offset1;
  	   ARRAY_I     data,temp_A1,temp_B1,temp_C1,temp_index1;
         int j1,j2;
         int size = temp_data.size();

         ARRAY_D spect ;
  	   int min_index , max_index ;
  	   double min , max , dR , interpol_index ;

     if(fft_mod ==0)
     {

  	   data.resize(size);
  	   temp_A1.resize(size);
  	   temp_B1.resize(size-1);

  	   DSP_EXTRACT(temp_data, data, 0, 0, size);
  	   DSP_EXTRACT(data, temp_A1, 0, 0, size-1);         // extract data
  	   DSP_EXTRACT(data, temp_B1, 1, 0, size -1);         // extract data

  	   temp_C1.resize(size-1);
  	   temp_index1.resize(size-1);


  	   j1=0;
  	   j2=0;

  	   for(int i=0; i<size-1; i++)
  	   {
  		   temp_C1[i] = temp_B1[i] - temp_A1[i];
  		   if (temp_C1[i]>0.5)
  		   {
  			   temp_index1[j1]=i  ;
  			   j1++;
  		   }
  	   }


  	   if(j1>1)
  	  	   {
  	  		   DSP_REG1(temp_index1, &gradient1, &offset1 , 0, j1);
  	  		 frequency_MHz =1000* 1/(Test_Period*gradient1);
  	  	   }
  	  	   else
  	  	   {
  	  		 frequency_MHz=-1;
  	  	   }
     }
     else
     {
  	   DSP_SPECTRUM ( temp_data , spect , VOLT , 1.0 , HANNING , 0 ) ;  // Tabei-Ueda algorithm needs power spectrum (no log!) with Hanning filter applied

  	   //cancel out the potential max bin at DC
  	   if (spect.size() > 2)
  	   {
  	     spect[0] = 0;
  	     spect[1] = 0;
  	   }
  	   else
  	   {
  	     cerr<<"WARN: spectrum data is too small."<<endl;
  	   }
  	   DSP_MINMAX  ( spect , &min , &max , &min_index , &max_index ) ;  // look for max bin

  	   // interpolating equations
  	   int max_index_plus_one = max_index + 1;
  	   max_index_plus_one = (max_index_plus_one < spect.size())?
  	                        max_index_plus_one : max_index_plus_one-1;

  	   if ( ( max_index > 0 ) && ( spect[ max_index - 1 ] > spect[ max_index_plus_one ] ) )
  	   {
  	     dR = spect[ max_index - 1 ] / spect[ max_index ] ;
  	     interpol_index = max_index + ( 1.0 - 2.0 * dR ) / ( 1.0 + dR ) ;
  	   }
  	   else
  	   {
  	     dR = spect[ max_index_plus_one ] / spect[ max_index ] ;
  	     interpol_index = max_index - ( 1.0 - 2.0 * dR ) / ( 1.0 + dR ) ;
  	   }

  	   frequency_MHz = interpol_index / ( Test_Period * temp_data.size() ) ;
     }
     return frequency_MHz;

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
REGISTER_TESTMETHOD("bm_ac_tml.DDR_PLL_Freq_Test", DDR_PLL_Freq_Test);
