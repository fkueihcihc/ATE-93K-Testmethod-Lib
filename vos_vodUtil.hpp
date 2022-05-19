#ifndef VOSVODUTIL_H_
#define VOSVODUTIL_H_

#include "CommonUtil.hpp"
#include "PmuUtil.hpp"

/***************************************************************************
 *                    VOSVOD test class 
 ***************************************************************************
 */ 
class LVDS_VOS_VODTest
{
public:
/*
 *----------------------------------------------------------------------*
 *         test parameters container                                    *
 *----------------------------------------------------------------------*
 */
  struct LVDS_VOS_VODTestParam
  {
    /*original input parameters*/
    STRING pinlist;
    DOUBLE settlingTime;
    STRING measureMode;

    /*new generated parameter for convenience*/
    STRING_VECTOR expandedPins;       /*expanded and validated pins stored in Vector*/
    TM::DCTEST_MODE testMode;         /*current test mode*/
    STRING testsuiteName;             /*current testsuite name*/

    DOUBLE minValue;               
    DOUBLE maxValue;
    DOUBLE stepValue;
    
    bool hasMinValue;         /* if the min, max, step and resolution is valid*/
    bool hasMaxValue;
    bool hasStepValue;

    /*initialize stuff to some defaults.*/ 
    void init()
    {
      pinlist = "";
      expandedPins.clear();   
      minValue = 0.0;
      maxValue = 0.0;
      stepValue = 0.0;
      hasMinValue = false;
      hasMaxValue = false;
      hasStepValue = false;
    }

    /*default constructor for intializing parameters*/
    LVDS_VOS_VODTestParam()
    {
      init();
    }

  };
  
  /*
   *----------------------------------------------------------------------*
   *         test limit container                                    *
   *----------------------------------------------------------------------*
   */
  struct LVDS_VOS_VODTestLimit
  {
	LIMIT limit;                      /*LIMIT object for this test*/
    
    /*init limit*/
    void init()
    {
    	limit = TM::Fail;
    }
    
    /*default constructor */
    LVDS_VOS_VODTestLimit()
    {
      init();
    }
  };

  /*
  *----------------------------------------------------------------------*
  *         test results container                                       *
  *----------------------------------------------------------------------*
  */
  struct LVDS_VOS_VODTestResult
  {
    /*result container 
     *for voltage value, the unit is: V
     */
    MeasurementResultContainer measurementResult;

    /*initialize all stuffs to some defaults.*/
    void init()
    {
      measurementResult.init();       
    } 

    /*default constructor*/
    LVDS_VOS_VODTestResult()
    {
      init();
    }
  };

/*
 *----------------------------------------------------------------------*
 *         public interfaces of VOSVOD  test                               *
 *----------------------------------------------------------------------*
 */

/*
 *----------------------------------------------------------------------*
 * Routine: processParameter
 *
 * Purpose: parse input parameters and setup internal parameters
 *
 *----------------------------------------------------------------------*
 * Description:
 *   
 * Note:
 *----------------------------------------------------------------------*
 */
  static void processParameters(const STRING& pinlist, 
                                const DOUBLE settlingTime, 
                                const STRING& measureMode, 
                                const double& min,
                                const double& max,
                                const double& step,
                                LVDS_VOS_VODTestParam& param)
  {    
    /*Init param*/
    param.init();
    if ( settlingTime < 0.0 )
    {
      throw Error("DcTest::LVDS_VOS_VODTest()",
                  "settlingTime must be postive!");
    }
    param.settlingTime = settlingTime;
    param.measureMode = CommonUtil::trim(measureMode);
    /*
     ********************************************************* 
     * Since PMU_IFVM constructor does validate illegal 
     * pin names internally,and here ommit this check.
     * Meanwhile it check wrong type pins, 
     * if any, it throws an error. 
     *********************************************************
     */
    param.pinlist = CommonUtil::trim(pinlist);
    vector<string> pins = PinUtility.getDigitalPinNamesFromPinList(
                                                                 pinlist,
                                                                 TM::I_PIN|TM::O_PIN|TM::IO_PIN,
                                                                 FALSE, /* check missing pins */
                                                                 TRUE  /* check mismatched-type pins */
                                                                 );   
    param.expandedPins.resize(pins.size());
    copy(pins.begin(), pins.end(), param.expandedPins.begin());

    /*get the test suite name and test mode*/
    GET_TESTSUITE_NAME(param.testsuiteName);
    param.testMode = CommonUtil::getMode();
  }
  
  /*
 *----------------------------------------------------------------------*
 * Routine: processLimit
 *
 * Purpose: process limit
 *
 *----------------------------------------------------------------------*
 * Description:
 *   
 * Note:
 *----------------------------------------------------------------------*
 */
  static void processLimit(LVDS_VOS_VODTestLimit& testLimit1,LVDS_VOS_VODTestLimit& testLimit2,LVDS_VOS_VODTestLimit& testLimit3)
  {   
    /* init testLimit*/
    testLimit1.init();
    testLimit2.init();
    testLimit3.init();
    
    testLimit1.limit = CommonUtil::getLimit("passVoltageVoLimit_mV", "mV");    
    testLimit2.limit = CommonUtil::getLimit("passVoltageVosLimit_mV", "mV");    
    testLimit3.limit = CommonUtil::getLimit("passVoltageVodLimit_mV", "mV");    
  }

/*
 *----------------------------------------------------------------------*
 * Routine: doMeasurement
 *
 * Purpose: execute measurement by PPMU and store results
 *
 *----------------------------------------------------------------------*
 * Description:
 *   INPUT:  param       - test parameters
 *           testLimit   - test limit container
 *           result      - result container
 *
 *   OUTPUT: 
 *   RETURN: 
 *----------------------------------------------------------------------*
 */
  static void doMeasurement(const LVDS_VOS_VODTestParam& param,
          					const double& min,
          					const double& max,
          					const double& step,
                            const LVDS_VOS_VODTestLimit& testLimit1,
                            const LVDS_VOS_VODTestLimit& testLimit2,
                            const LVDS_VOS_VODTestLimit& testLimit3,
                            LVDS_VOS_VODTestResult& result,
                            double _results[4])
  {
	  	DOUBLE temp = 0;	    
	    DOUBLE dMeasValue = 0;
	    STRING_VECTOR::size_type j = 0;
	    
	    PPMU_MEASURE ppmuMeasure;
	    SPMU_TASK badcTask;
	    /*init _results*/
	    _results[0]=_results[2]= DBL_MAX; 
	    _results[1]=_results[3]= -DBL_MAX;
	    
	    pair<DOUBLE,DOUBLE> minmax;
//	    minmax.first  =  DBL_MAX;   //min val
//	    minmax.second = -DBL_MAX;   //max val
	    minmax.first  =  max;   //min val
	    minmax.second =  min;   //max val
	    
	    ON_FIRST_INVOCATION_BEGIN();
	    CONNECT();
	    if (param.measureMode == "PPMUpar")
	    {
    	  DOUBLE vt = min;   
		  temp = Primary.getLevelSpec().getSpecValue("V_term");  //vos [mV]

    	  for(; (vt < max)&& (minmax.first+minmax.second-2*vt>0); vt +=step)
    	  {
    		  Primary.getLevelSpec().change("V_term",vt);
    		  FLUSH();

    	  	  PmuUtil::ppmuParallelVoltageMeasurement(param.pinlist,
    	                                                  param.testMode,
    	                                                  1.0,         /* default value of forceCurrent */
    	                                                  testLimit1.limit,
    	                                                  TURN_OFF,       /* precharge */
    	                                                  0.0,         /* prechargeVoltage */
    	                                                  param.settlingTime,
    	                                                  "PARALLEL",  /* relaySwitchMode */
    	                                                  /*****************************
    	                                                   * set termination on to
    	                                                   * just switch PPMU_relay only 
    	                                                   * inside the sub-function.
    	                                                   *****************************
    	                                                   */
    	                                                  TURN_ON,        /* termination */ 
    	                                                  -0.1,              /* ppmuClamp Low/High */
    	                                                  0.1,              
    	                                                  ppmuMeasure
    	                                                  ); 
    	      PmuUtil::ppmuParallelVoltageGetResult(param.expandedPins,
    	                                            param.testMode,
    	                                            ppmuMeasure,                               
    	                                            result.measurementResult);
    	      for ( j = 0; j< param.expandedPins.size(); ++j )
    	      {
    	    	  dMeasValue = result.measurementResult.getPinsValue(param.expandedPins[j]);
//    	          dMeasValue = dMeasValue*1e3;                  //[mV]
    	          minmax.first = MIN(minmax.first,dMeasValue);  //min value
    	          minmax.second = MAX(minmax.second,dMeasValue);//max value*/
    	      }
    	        _results[0] = minmax.first; //min voltage [mV]
    	        _results[1] = minmax.second; //max voltage [mV]
    	   }
    	  
    	      _results[2] =(minmax.first+minmax.second)/2.0;  //vos [mV]
    	      _results[3] =minmax.second-minmax.first;  //vod [mV]
      }
      else
      {
        throw Error("LVDS_VOS_VODTest::doMeasurement","Unsupport measurement mode.");
      }
	  ON_FIRST_INVOCATION_END();
      if (param.measureMode == "PPMUpar")
      {
    	  PmuUtil::ppmuParallelVoltageGetResult(param.expandedPins,
                                            param.testMode,
                                            ppmuMeasure,                               
                                            result.measurementResult);
      }
      else
      {
    	  PmuUtil::spmuVoltageGetResult(param.expandedPins,
                                    param.testMode,
                                    badcTask,
                                    result.measurementResult);
      }
  }
/*
 *----------------------------------------------------------------------*
 * Routine: judgeAndDatalog
 *
 * Purpose: judge and put result into event datalog stream.
 *
 *----------------------------------------------------------------------*
 * Description:
 *   judge results of 'result' with pass limits from 'param'
 * 
 *   INPUT:  param       - test parameters
 *           result      - result container
 *           _results[4] - retrieved by built-in tf_result() in testflow.
 *   OUTPUT: 
 *   RETURN: 
 * Note:
 *----------------------------------------------------------------------*
 */
  static void judgeAndDatalog(const LVDS_VOS_VODTestParam& param,
                              const LVDS_VOS_VODTestLimit& testLimit1,
                              const LVDS_VOS_VODTestLimit& testLimit2,
                              const LVDS_VOS_VODTestLimit& testLimit3,
                              const LVDS_VOS_VODTestResult &result)
  {
    STRING_VECTOR::size_type j = 0;
    DOUBLE dMeasValue = 0;
    Boolean bPass = TRUE;
    
    switch ( param.testMode )
    {
    case TM::PVAL:
      for ( j = 0; j< param.expandedPins.size(); ++j )
      {
        dMeasValue = result.measurementResult.getPinsValue(param.expandedPins[j]);

        TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                          param.expandedPins[j],
                                          param.testsuiteName,
                                          testLimit1.limit,
                                          dMeasValue);
      }
      break;

    case TM::PPF:
      for ( j = 0; j < param.expandedPins.size(); ++j )
      {
        bPass = result.measurementResult.getPinPassFail(param.expandedPins[j]);
        TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                          param.expandedPins[j], 
                                          param.testsuiteName, 
                                          bPass?TM::Pass:TM::Fail, 
                                          0.0);
      }
      break;

    case TM::GPF:
      bPass = result.measurementResult.getGlobalPassFail();
      TESTSET().cont(TM::CONTINUE).judgeAndLog_ParametricTest(
                                        param.pinlist,param.testsuiteName,
                                        bPass?TM::Pass:TM::Fail, 
                                        0.0);
      break;

    default:
      throw Error("LVDS_VOS_VODTest::judgeAndDatalog",
                  "Unknown Test Mode");
    } /*end switch*/
  }

/*
 *----------------------------------------------------------------------*
 * Routine: reportToUI
 *
 * Purpose: output result to UIWindow 
 *
 *----------------------------------------------------------------------*
 * Description:
 *   display: 
 *       a) results from result,
 *       b) pass range from pass limits of param,
 *       c) pass or fail
 * 
 *   INPUT:  param              - test parameters
 *           result             - result container
 *           output             - "None" or "ReportUI" 
 *   OUTPUT: 
 *   RETURN:  
 * Note:
 *----------------------------------------------------------------------*
 */  
  static void reportToUI(const LVDS_VOS_VODTestParam& param,
                         const LVDS_VOS_VODTestLimit& testLimit1,
                         const LVDS_VOS_VODTestLimit& testLimit2,
                         const LVDS_VOS_VODTestLimit& testLimit3,
                         const LVDS_VOS_VODTestResult& result,
                         const STRING& output,
                         double _results[4])
  {
    CommonUtil::printTestSuiteName(output, "VOSVOD '", "'");

    STRING_VECTOR::size_type j = 0;
    DOUBLE dMeasValue = 0;
    Boolean bPass = TRUE;

    if ( output != "ReportUI" )
      return;

    switch ( param.testMode )
    {    
    case TM::PVAL:
      for ( j = 0; j< param.expandedPins.size(); ++j )
      {
        bPass = result.measurementResult.getPinPassFail(param.expandedPins[j]);
        dMeasValue = result.measurementResult.getPinsValue(param.expandedPins[j]);
/*
        CommonUtil::datalogToWindow(output,
                                    param.expandedPins[j], 
                                    dMeasValue,
                                    testLimit1.limit,"mV",bPass);*/
      }
    
/*      CommonUtil::datalogToWindow(output,
                                    "VO_MAX", 
                                    _results[1],
                                    testLimit1.limit,"mV",bPass);
      CommonUtil::datalogToWindow(output,
                                    "VO_MIN",
                                    _results[0],
                                    testLimit1.limit,"mV",bPass);
*/
      CommonUtil::datalogToWindow(output,
                                    "VOS", 
                                    _results[2],
                                    testLimit2.limit,"mV",bPass);
      CommonUtil::datalogToWindow(output,
                                    "VOD",
                                    _results[3],
                                    testLimit3.limit,"mV",bPass);
      break;

    case TM::PPF:
      for ( j = 0; (j < param.expandedPins.size())&&(bPass==true); ++j )
      {
        bPass = result.measurementResult.getPinPassFail(param.expandedPins[j]);
      }
      CommonUtil::datalogToWindow(output, "VOD", bPass);
      CommonUtil::datalogToWindow(output, "VOS", bPass);
      break;

    case TM::GPF:
      bPass = result.measurementResult.getGlobalPassFail();
      CommonUtil::datalogToWindow(output,param.testsuiteName, bPass);
      break;

    default:
      throw Error("LVDS_VOS_VODTest::reportToUI",
                  "Unknown Test Mode");

    }/* end switch*/
  }
private:
  LVDS_VOS_VODTest() {}//private constructor to prevent instantiation.
};
#endif /*VOSVOD_H_*/
