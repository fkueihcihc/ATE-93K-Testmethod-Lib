#include "testmethod.hpp"
#include "pmicComCode.h"
//for test method API interfaces

#include "d2sFramework.h"
#include "mapi.hpp"
#include "d2sPmu.h"
using namespace std;
using namespace V93kLimits;
/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class FREQ_38M4_TEST: public testmethod::TestMethod {

protected:

	  double	  mCaptureFreq_MHz;

	  int	  mSampleSize_pts;

	  int	  mWaitTime_ms;

	  double    mPpmuiRange_uA;

	  double    mVolWifibt;

	  double    mVohWifibt;

	  int 		Debug_Mode;

	  int 		D2SMode;



protected:

  /**

   *Initialize the parameter interface to the testflow.

   *This method is called just once after a testsuite is created.

   *

   *Note: Test Method API should not be used in this method.

   */

  virtual void initialize()

  {

    //Add your initialization code here

	    addParameter("captureFreq_MHz",

	                 "double",

	                 &mCaptureFreq_MHz,

	                 testmethod::TM_PARAMETER_INPUT);



	    addParameter("sampleSize_pts",

	                 "int",

	                 &mSampleSize_pts,

	                 testmethod::TM_PARAMETER_INPUT);



	    addParameter("waitTime_ms",

	                 "int",

	                 &mWaitTime_ms,

	                 testmethod::TM_PARAMETER_INPUT);



	    addParameter("Debug_Mode", "int", &Debug_Mode)

	              .setDefault("1")

	              .setOptions("0:1")

	              .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

		addParameter("D2SMode", "int", &D2SMode)

				   .setDefault("1")

				   .setOptions("1:2")

				   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");

	    addParameter("Vol_WIFIBT",

	                 "double",

	                 &mVolWifibt,

	                 testmethod::TM_PARAMETER_INPUT);

	    addParameter("Voh_WIFIBT",

	                 "double",

	                 &mVohWifibt,

	                 testmethod::TM_PARAMETER_INPUT);

  }



  /**

   *This test is invoked per site.

   */

  virtual void run()

  {

	    //Define d2s var

		d2sFrameWorkModeType::Enum d2s_mode;

		d2sPmu &myD2S = d2sPmu::Instance();

		d2sFramework fr;



		ARRAY_I 	SIM1_CLKOUT_CapData0(mSampleSize_pts),SIM1_CLKOUT_CapData1(mSampleSize_pts),SIM1_CLKOUT_CapData2(mSampleSize_pts);

		ARRAY_I 	SIM1_CLKOUT_CapData3(mSampleSize_pts),SIM1_CLKOUT_CapData4(mSampleSize_pts),SIM1_CLKOUT_CapData5(mSampleSize_pts),SIM1_CLKOUT_CapData6(mSampleSize_pts);

		ARRAY_I 	SIM1_RSTOUT_CapData(mSampleSize_pts);

		ARRAY_I 	SIM1_DATAOUT_CapData(mSampleSize_pts);

		ARRAY_I 	SIM1_DATAIN_CapData(mSampleSize_pts);





		DOUBLE  	SIM1_CLKOUT_Freq_kHz0 = 0.0,SIM1_RSTOUT_Freq_kHz0 = 0.0,SIM1_DATAOUT_Freq_kHz0 = 0.0,SIM1_DATAIN_Freq_kHz0 = 0.0;

		DOUBLE  	SIM1_CLKOUT_Freq_kHz1 = 0.0,SIM1_RSTOUT_Freq_kHz1 = 0.0,SIM1_DATAOUT_Freq_kHz1 = 0.0,SIM1_DATAIN_Freq_kHz1 = 0.0;

		DOUBLE  	SIM1_CLKOUT_Freq_kHz2 = 0.0,SIM1_RSTOUT_Freq_kHz2 = 0.0,SIM1_DATAOUT_Freq_kHz2 = 0.0,SIM1_DATAIN_Freq_kHz2 = 0.0;

		DOUBLE  	SIM1_CLKOUT_Freq_kHz3 = 0.0,SIM1_RSTOUT_Freq_kHz3 = 0.0,SIM1_DATAOUT_Freq_kHz3 = 0.0,SIM1_DATAIN_Freq_kHz3 = 0.0;

		DOUBLE  	SIM1_CLKOUT_Freq_kHz4 = 0.0,SIM1_RSTOUT_Freq_kHz4 = 0.0,SIM1_DATAOUT_Freq_kHz4 = 0.0,SIM1_DATAIN_Freq_kHz4 = 0.0;

		DOUBLE  	SIM1_CLKOUT_Freq_kHz5 = 0.0,SIM1_RSTOUT_Freq_kHz5 = 0.0,SIM1_DATAOUT_Freq_kHz5 = 0.0,SIM1_DATAIN_Freq_kHz5 = 0.0;



		DOUBLE  	SIM1_CLKOUT_TransNum0 = 0.0,SIM1_RSTOUT_TransNum0 = 0.0,SIM1_DATAOUT_TransNum0 = 0.0,SIM1_DATAIN_TransNum0 = 0.0;

		DOUBLE  	SIM1_CLKOUT_TransNum1 = 0.0,SIM1_RSTOUT_TransNum1 = 0.0,SIM1_DATAOUT_TransNum1 = 0.0,SIM1_DATAIN_TransNum1 = 0.0;

		DOUBLE  	SIM1_CLKOUT_TransNum2 = 0.0,SIM1_RSTOUT_TransNum2 = 0.0,SIM1_DATAOUT_TransNum2 = 0.0,SIM1_DATAIN_TransNum2 = 0.0;

		DOUBLE  	SIM1_CLKOUT_TransNum3 = 0.0,SIM1_RSTOUT_TransNum3 = 0.0,SIM1_DATAOUT_TransNum3 = 0.0,SIM1_DATAIN_TransNum3 = 0.0;

		DOUBLE  	SIM1_CLKOUT_TransNum4 = 0.0,SIM1_RSTOUT_TransNum4 = 0.0,SIM1_DATAOUT_TransNum4 = 0.0,SIM1_DATAIN_TransNum4 = 0.0;

		DOUBLE  	SIM1_CLKOUT_TransNum5 = 0.0,SIM1_RSTOUT_TransNum5 = 0.0,SIM1_DATAOUT_TransNum5 = 0.0,SIM1_DATAIN_TransNum5 = 0.0;



		INT  		SIM1_CLKOUT_FirstTransIndex0 = -1,SIM1_RSTOUT_FirstTransIndex0 = -1,SIM1_DATAOUT_FirstTransIndex0 = -1,SIM1_DATAIN_FirstTransIndex0 = -1;

		INT  		SIM1_CLKOUT_FirstTransIndex1 = -1,SIM1_RSTOUT_FirstTransIndex1 = -1,SIM1_DATAOUT_FirstTransIndex1 = -1,SIM1_DATAIN_FirstTransIndex1 = -1;

		INT  		SIM1_CLKOUT_FirstTransIndex2 = -1,SIM1_RSTOUT_FirstTransIndex2 = -1,SIM1_DATAOUT_FirstTransIndex2 = -1,SIM1_DATAIN_FirstTransIndex2 = -1;

		INT  		SIM1_CLKOUT_FirstTransIndex3 = -1,SIM1_RSTOUT_FirstTransIndex3 = -1,SIM1_DATAOUT_FirstTransIndex3 = -1,SIM1_DATAIN_FirstTransIndex3 = -1;

		INT  		SIM1_CLKOUT_FirstTransIndex4 = -1,SIM1_RSTOUT_FirstTransIndex4 = -1,SIM1_DATAOUT_FirstTransIndex4 = -1,SIM1_DATAIN_FirstTransIndex4 = -1;

		INT  		SIM1_CLKOUT_FirstTransIndex5 = -1,SIM1_RSTOUT_FirstTransIndex5 = -1,SIM1_DATAOUT_FirstTransIndex5 = -1,SIM1_DATAIN_FirstTransIndex5 = -1;



		INT  		SIM1_CLKOUT_LastTransIndex0 = -1,SIM1_RSTOUT_LastTransIndex0 = -1,SIM1_DATAOUT_LastTransIndex0 = -1,SIM1_DATAIN_LastTransIndex0 = -1;

		INT  		SIM1_CLKOUT_LastTransIndex1 = -1,SIM1_RSTOUT_LastTransIndex1 = -1,SIM1_DATAOUT_LastTransIndex1 = -1,SIM1_DATAIN_LastTransIndex1 = -1;

		INT  		SIM1_CLKOUT_LastTransIndex2 = -1,SIM1_RSTOUT_LastTransIndex2 = -1,SIM1_DATAOUT_LastTransIndex2 = -1,SIM1_DATAIN_LastTransIndex2 = -1;

		INT  		SIM1_CLKOUT_LastTransIndex3 = -1,SIM1_RSTOUT_LastTransIndex3 = -1,SIM1_DATAOUT_LastTransIndex3 = -1,SIM1_DATAIN_LastTransIndex3 = -1;

		INT  		SIM1_CLKOUT_LastTransIndex4 = -1,SIM1_RSTOUT_LastTransIndex4 = -1,SIM1_DATAOUT_LastTransIndex4 = -1,SIM1_DATAIN_LastTransIndex4 = -1;

		INT  		SIM1_CLKOUT_LastTransIndex5 = -1,SIM1_RSTOUT_LastTransIndex5 = -1,SIM1_DATAOUT_LastTransIndex5 = -1,SIM1_DATAIN_LastTransIndex5 = -1;



		SIM1_CLKOUT_CapData0.init(0);

		SIM1_CLKOUT_CapData1.init(0);

		SIM1_CLKOUT_CapData2.init(0);

		SIM1_CLKOUT_CapData3.init(0);

		SIM1_CLKOUT_CapData4.init(0);

		SIM1_CLKOUT_CapData5.init(0);

		SIM1_RSTOUT_CapData.init(0);

		SIM1_DATAOUT_CapData.init(0);

		SIM1_DATAIN_CapData.init(0);



		TIMING_SPEC spec1(2,1);
		TIMING_SPEC spec2(2,4);

		ON_FIRST_INVOCATION_BEGIN();

				Primary.getLevelSpec().change("VOL_WIFIBT",mVolWifibt V);
				FLUSH(TM::APRM);
				Primary.getLevelSpec().change("VOH_WIFIBT",mVohWifibt V);
				FLUSH(TM::APRM);

				Primary.timing(spec2);
				FLUSH(TM::APRM);

				Vset_PPMU("XO_19M2_SEL",0.0,0.5);

				Routing.util("K5").on();	//switch to 38M4 crystal
				FLUSH(TM::APRM);
				Routing.util("K6").off();	//switch to 38M4 crystal
				FLUSH(TM::APRM);
				//DigPin_Relay("SYS_CLK_19M2,ABB_CLK_19M2","ALL_ON");
				FW_TASK("RLYC AC,OFF,(SYS_CLK_19M2,ABB_CLK_19M2)");
				FLUSH(TM::APRM);
				SMC_WAIT_TIME(500 ms);

					switch (D2SMode ){
						case 0:
						case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
						case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
					}

					fr.registerTransactionPort(myD2S);

				Primary.label("FREQ_19M2_MEAS");
				FLUSH();
				DIGITAL_CAPTURE_TEST ();
				WAIT_TEST_DONE ();

		//	    Routing.util("K5,K6").off();
		//	    FLUSH(TM::APRM);

		//	    DigPin_Relay("SYS_CLK_19M2,ABB_CLK_19M2","ALL_OFF");
			    ON_FIRST_INVOCATION_END();

		//	    //get_digital waveform data

			    SIM1_CLKOUT_CapData0 = VECTOR("ABB_CLK_19M2_DIGCAP").getVectors();
			    SIM1_CLKOUT_CapData1 = VECTOR("SYS_CLK_19M2_DIGCAP").getVectors();

			    ON_FIRST_INVOCATION_BEGIN();

				FW_TASK("RLYC AC,OFF,(RF0_CLK_19M2,RF1_CLK_19M2,WIFIBT_CLK_19M2,CODEC_CLK_19M2)");
				SMC_WAIT_TIME(500 ms);

				Primary.timing(spec2);
				FLUSH(TM::APRM);

				Primary.label("FREQ_19M2_MEAS_2");
				FLUSH();
				DIGITAL_CAPTURE_TEST ();
				WAIT_TEST_DONE ();

			    Routing.util("K5,K6").off();
			    FLUSH(TM::APRM);
			    Vset_PPMU("XO_19M2_SEL",1.8,0.5);

			    FW_TASK("RLYC IDLE,OFF,(SYS_CLK_19M2,ABB_CLK_19M2,RF0_CLK_19M2,RF1_CLK_19M2,WIFIBT_CLK_19M2,CODEC_CLK_19M2)");

			    ON_FIRST_INVOCATION_END();

				//	    //get_digital waveform data

				SIM1_CLKOUT_CapData2 = VECTOR("CODEC_CLK_19M2_DIGCAP").getVectors();
				SIM1_CLKOUT_CapData3 = VECTOR("RF0_CLK_19M2_DIGCAP").getVectors();
				SIM1_CLKOUT_CapData4 = VECTOR("RF1_CLK_19M2_DIGCAP").getVectors();
				SIM1_CLKOUT_CapData5 = VECTOR("WIFI_CLK_19M2_DIGCAP").getVectors();

				ON_FIRST_INVOCATION_BEGIN();

				Primary.timing(spec1);
				FLUSH(TM::APRM);

				fr.d2s_LABEL_BEGIN("FREQ_MEAS_REG_RECOVER",d2s_mode);
					myD2S.write(0x0FE,0x00);	//set to sine waveform
					myD2S.write(0x10C,0x02);	//set to sine waveform
				fr.d2s_LABEL_END();

				ON_FIRST_INVOCATION_END();

				//put_digital waveform data_debug@SIM1_CLKOUT

				for (int i = 1;i<mSampleSize_pts;i++)
				{  //search positive transition
					if (SIM1_CLKOUT_CapData0 [i] > SIM1_CLKOUT_CapData0[i-1])
						{    //evaluate first_trans_index and last_trans_index
							if (SIM1_CLKOUT_FirstTransIndex0 == -1)
								SIM1_CLKOUT_FirstTransIndex0 = i ;
							else
								SIM1_CLKOUT_LastTransIndex0 = i;
							SIM1_CLKOUT_TransNum0++;
					   }
				}

				//put_digital waveform data_debug@SIM1_CLKOUT
				for (int i = 1;i<mSampleSize_pts;i++)
				{  //search positive transition
					if (SIM1_CLKOUT_CapData1 [i] > SIM1_CLKOUT_CapData1[i-1])
						{    //evaluate first_trans_index and last_trans_index
							if (SIM1_CLKOUT_FirstTransIndex1 == -1)
								SIM1_CLKOUT_FirstTransIndex1 = i ;
							else
								SIM1_CLKOUT_LastTransIndex1 = i;
							SIM1_CLKOUT_TransNum1++;
					   }
				}

				//put_digital waveform data_debug@SIM1_CLKOUT

				for (int i = 1;i<mSampleSize_pts;i++)
				{  //search positive transition
					if (SIM1_CLKOUT_CapData2 [i] > SIM1_CLKOUT_CapData2[i-1])
						{    //evaluate first_trans_index and last_trans_index
							if (SIM1_CLKOUT_FirstTransIndex2 == -1)
								SIM1_CLKOUT_FirstTransIndex2 = i ;
							else
								SIM1_CLKOUT_LastTransIndex2 = i;
							SIM1_CLKOUT_TransNum2++;
					   }
				}

				//put_digital waveform data_debug@SIM1_CLKOUT

				for (int i = 1;i<mSampleSize_pts;i++)
				{  //search positive transition
					if (SIM1_CLKOUT_CapData3 [i] > SIM1_CLKOUT_CapData3[i-1])
						{    //evaluate first_trans_index and last_trans_index
							if (SIM1_CLKOUT_FirstTransIndex3 == -1)
								SIM1_CLKOUT_FirstTransIndex3 = i ;
							else
								SIM1_CLKOUT_LastTransIndex3 = i;
								SIM1_CLKOUT_TransNum3++;
					   }
				}

				//put_digital waveform data_debug@SIM1_CLKOUT

				for (int i = 1;i<mSampleSize_pts;i++)
				{  //search positive transition
					if (SIM1_CLKOUT_CapData4 [i] > SIM1_CLKOUT_CapData4[i-1])
						{    //evaluate first_trans_index and last_trans_index
							if (SIM1_CLKOUT_FirstTransIndex4 == -1)
								SIM1_CLKOUT_FirstTransIndex4= i ;
							else
								SIM1_CLKOUT_LastTransIndex4 = i;
								SIM1_CLKOUT_TransNum4++;
					   }

				}

				//put_digital waveform data_debug@SIM1_CLKOUT

				for (int i = 1;i<mSampleSize_pts;i++)
				{  //search positive transition
					if (SIM1_CLKOUT_CapData5 [i] > SIM1_CLKOUT_CapData5[i-1])
						{    //evaluate first_trans_index and last_trans_index
							if (SIM1_CLKOUT_FirstTransIndex5 == -1)
								SIM1_CLKOUT_FirstTransIndex5= i ;
							else
								SIM1_CLKOUT_LastTransIndex5 = i;
								SIM1_CLKOUT_TransNum5++;
					   }
				}

			        // find transitions and discard one

				if (SIM1_CLKOUT_TransNum0 > 0) SIM1_CLKOUT_TransNum0--;
				if (SIM1_CLKOUT_TransNum1 > 0) SIM1_CLKOUT_TransNum1--;
				if (SIM1_CLKOUT_TransNum2 > 0) SIM1_CLKOUT_TransNum2--;
				if (SIM1_CLKOUT_TransNum3 > 0) SIM1_CLKOUT_TransNum3--;
				if (SIM1_CLKOUT_TransNum4 > 0) SIM1_CLKOUT_TransNum4--;
				if (SIM1_CLKOUT_TransNum5 > 0) SIM1_CLKOUT_TransNum5--;

			        //define format for frequency count

					SIM1_CLKOUT_Freq_kHz0=(SIM1_CLKOUT_TransNum0*mCaptureFreq_MHz)/(SIM1_CLKOUT_LastTransIndex0-SIM1_CLKOUT_FirstTransIndex0);
					SIM1_CLKOUT_Freq_kHz1=(SIM1_CLKOUT_TransNum1*mCaptureFreq_MHz)/(SIM1_CLKOUT_LastTransIndex1-SIM1_CLKOUT_FirstTransIndex1);
					SIM1_CLKOUT_Freq_kHz2=(SIM1_CLKOUT_TransNum2*mCaptureFreq_MHz)/(SIM1_CLKOUT_LastTransIndex2-SIM1_CLKOUT_FirstTransIndex2);
					SIM1_CLKOUT_Freq_kHz3=(SIM1_CLKOUT_TransNum3*mCaptureFreq_MHz)/(SIM1_CLKOUT_LastTransIndex3-SIM1_CLKOUT_FirstTransIndex3);
					SIM1_CLKOUT_Freq_kHz4=(SIM1_CLKOUT_TransNum4*mCaptureFreq_MHz)/(SIM1_CLKOUT_LastTransIndex4-SIM1_CLKOUT_FirstTransIndex4);
					SIM1_CLKOUT_Freq_kHz5=(SIM1_CLKOUT_TransNum5*mCaptureFreq_MHz)/(SIM1_CLKOUT_LastTransIndex5-SIM1_CLKOUT_FirstTransIndex5);

		    if (Debug_Mode)

	        {  // print frequency name and value to the ui report
	           cout<<"ABB_CLK_TransNum0        		= "<<setprecision(4)<<SIM1_CLKOUT_TransNum0<<endl;
	           cout<<"ABB_CLK_LastTransIndex0  		= "<<setprecision(4)<<SIM1_CLKOUT_LastTransIndex0<<endl;
	           cout<<"ABB_CLK_FirstTransIndex0 		= "<<setprecision(4)<<SIM1_CLKOUT_FirstTransIndex0<<endl;
	           cout<<"Frequency @ABB_CLK    		= "<<setprecision(4)<<SIM1_CLKOUT_Freq_kHz0<<"MHz"<<endl<<endl;
	           cout<<"SYS_CLK_TransNum1        		= "<<setprecision(4)<<SIM1_CLKOUT_TransNum1<<endl;
	           cout<<"SYS_CLK_LastTransIndex1  		= "<<setprecision(4)<<SIM1_CLKOUT_LastTransIndex1<<endl;
	           cout<<"SYS_CLK_FirstTransIndex1 		= "<<setprecision(4)<<SIM1_CLKOUT_FirstTransIndex1<<endl;
	           cout<<"Frequency @SYS_CLK     		= "<<setprecision(4)<<SIM1_CLKOUT_Freq_kHz1<<"MHz"<<endl<<endl;
	           cout<<"CODEC_CLK_TransNum1        	= "<<setprecision(4)<<SIM1_CLKOUT_TransNum2<<endl;
	           cout<<"CODEC_CLK_LastTransIndex1  	= "<<setprecision(4)<<SIM1_CLKOUT_LastTransIndex2<<endl;
	           cout<<"CODEC_CLK_FirstTransIndex1 	= "<<setprecision(4)<<SIM1_CLKOUT_FirstTransIndex2<<endl;
	           cout<<"Frequency @CODEC_CLK     		= "<<setprecision(4)<<SIM1_CLKOUT_Freq_kHz2<<"MHz"<<endl<<endl;
	           cout<<"RF0_CLK_TransNum1        		= "<<setprecision(4)<<SIM1_CLKOUT_TransNum3<<endl;
	           cout<<"RF0_CLK_LastTransIndex1  		= "<<setprecision(4)<<SIM1_CLKOUT_LastTransIndex3<<endl;
	           cout<<"RF0_CLK_FirstTransIndex1 		= "<<setprecision(4)<<SIM1_CLKOUT_FirstTransIndex3<<endl;
	           cout<<"Frequency @RF0_CLK     		= "<<setprecision(4)<<SIM1_CLKOUT_Freq_kHz3<<"MHz"<<endl<<endl;
	           cout<<"RF1_CLK_TransNum1        		= "<<setprecision(4)<<SIM1_CLKOUT_TransNum4<<endl;
	           cout<<"RF1_CLK_LastTransIndex1  		= "<<setprecision(4)<<SIM1_CLKOUT_LastTransIndex4<<endl;
	           cout<<"RF1_CLK_FirstTransIndex1 		= "<<setprecision(4)<<SIM1_CLKOUT_FirstTransIndex4<<endl;
	           cout<<"Frequency @RF1_CLK     		= "<<setprecision(4)<<SIM1_CLKOUT_Freq_kHz4<<"MHz"<<endl<<endl;
	           cout<<"WIFIBT_CLK_TransNum1        	= "<<setprecision(4)<<SIM1_CLKOUT_TransNum5<<endl;
	           cout<<"WIFIBT_CLK_LastTransIndex1  	= "<<setprecision(4)<<SIM1_CLKOUT_LastTransIndex5<<endl;
	           cout<<"WIFIBT_CLK_FirstTransIndex1 	= "<<setprecision(4)<<SIM1_CLKOUT_FirstTransIndex5<<endl;
	           cout<<"Frequency @WIFIBT_CLK     	= "<<setprecision(4)<<SIM1_CLKOUT_Freq_kHz5<<"MHz"<<endl<<endl;
	        }

//

		    TestSet.cont(true).TEST("ABB_CLK","ABB_CLK_FREQ_MHz",tmLimits.getLimitObj("ABB_CLK_psssFreq_MHz"),SIM1_CLKOUT_Freq_kHz0);
		    TestSet.cont(true).TEST("SYS_CLK","SYS_CLK_FREQ_MHz",tmLimits.getLimitObj("SYS_CLK_psssFreq_MHz"),SIM1_CLKOUT_Freq_kHz1);
		    TestSet.cont(true).TEST("CODEC_CLK","CODEC_CLK_FREQ_MHz",tmLimits.getLimitObj("CODEC_CLK_psssFreq_MHz"),SIM1_CLKOUT_Freq_kHz2);
		    TestSet.cont(true).TEST("RF0_CLK","RF0_CLK_FREQ_MHz",tmLimits.getLimitObj("RF0_CLK_psssFreq_MHz"),SIM1_CLKOUT_Freq_kHz3);
		    TestSet.cont(true).TEST("RF1_CLK","RF1_CLK_FREQ_MHz",tmLimits.getLimitObj("RF1_CLK_psssFreq_MHz"),SIM1_CLKOUT_Freq_kHz4);
		    TestSet.cont(true).TEST("WIFIBT_CLK","WIFIBT_CLK_FREQ_MHz",tmLimits.getLimitObj("WIFIBT_CLK_psssFreq_MHz"),SIM1_CLKOUT_Freq_kHz5);

    return;

  }



  /**

   *This function will be invoked once the specified parameter's value is changed.

   *@param parameterIdentifier

   *

   *Note: Test Method API should not be used in this method.

   */

  virtual void postParameterChange(const string& parameterIdentifier)

  {

    //Add your code

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

REGISTER_TESTMETHOD("bm_ac_tml.FREQ_38M4_TEST", FREQ_38M4_TEST);
