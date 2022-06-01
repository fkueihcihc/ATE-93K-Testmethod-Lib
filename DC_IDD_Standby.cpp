#include "testmethod.hpp"
#include "Common.h"
#include "dpsComCode.h"
//for test method API interfaces
#include "mapi.hpp"
#include <string>
using namespace std;
using namespace V93kLimits;

/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 *
 */

class IDD_Standby: public testmethod::TestMethod {

protected:
	//1. input parameter variables
	STRING dpsPinList;  // pin list in string format
	STRING  uvICC;

	double wait;        // wait time in ms before executing the test
	int sample_count;   // sample count to be averaged ( run the measurement sample_count times and average the result)
	int debug_mode;     // debug mode

	//2. variables shared by sites
	INT isOffline;       	// global offline mode flag
	STRING suiteName;       // test suite name

	//3. variables shared by sites, but changed in ON_FIRST_INVOCATION block
	STRING_VECTOR vPins; 	  // vectors for pin names
	STRING_VECTOR vUnits;  		 	  // store the units in test table
	vector<double> vUpperl; 		  // vectors for the upper limit of test items in test table
	vector<double> vLowerl; 		  // vectors for the lower limit of test items in test table
	int pin_count;    				  // number of pins to be measured


	dpsComCode dps_idd; 		 		  //dps for IDD

	//4. array or vectors shared by sites to store per site data

	/**
	 *Initialize the parameter interface to the testflow.
	 *This method is called just once after a testsuite is created.
	 */
	virtual void initialize()
	{
		addParameter("PinList",
				"PinString",
				&dpsPinList,
				testmethod::TM_PARAMETER_INPUT)
				.setDefault("VDD");
		addParameter("UserVarName",
				"string",
				&uvICC,
				testmethod::TM_PARAMETER_INPUT)
				.setDefault("ICC")
				.setComment("user variable name for record current");
		addParameter("Wait_ms", "double", &wait,testmethod::TM_PARAMETER_INPUT)
			.setDefault("2")
			.setComment("time in ms");
		addParameter("sample_count",  "int",  &sample_count)
			.setDefault("16")
			.setComment("number of measurements for average result; refer to TDC 55630");
		addParameter("debug_mode",  "int",  &debug_mode)
			.setDefault("0").setOptions("0:1")
			.setComment("0:no_need_ouput, 1:output_to_ui");

		//Add your initialization code here
		//Note: Test Method API should not be used in this method!
	}

	/**
	 *This test is invoked per site.
	 */

	virtual void run()
	{
		//Add your test code here.
		string testName;     // test item name

		ARRAY_D ValueRes;

		ON_FIRST_INVOCATION_BEGIN();
			// get global variables
			GET_SYSTEM_FLAG("offline",&isOffline);
			GET_TESTSUITE_NAME(suiteName);
			// parse pins and points input parameters
			vPins.clear();
			vPins = PinUtility.getDpsPinNamesFromPinList(dpsPinList);
			pin_count = vPins.size();
		ON_FIRST_INVOCATION_END();

		DPS_TASK dpsTask;        // DPS task

		ON_FIRST_INVOCATION_BEGIN();
		// 1. functional test to setup
//			DISCONNECT();
//			WAIT_TIME(5 ms);
			CONNECT();
//			WAIT_TIME(5 ms);
			FUNCTIONAL_TEST();

	   //2. set DPS task measurement range
			double iRange_min, iRange_max;  // measurement range
			double upperl, lowerl;          // limit in the test table

			dps_idd.read_All_DPS_Setting(debug_mode); // read DPS settings

			vUnits.resize(pin_count);
			vUpperl.resize(pin_count);
			vLowerl.resize(pin_count);

			for(unsigned int i = 0; i < vPins.size(); i++){
				try {
					testName = vPins[i]+"_IDD";
					const TMLimits::LimitInfo & limitref = tmLimits.getLimitRef(suiteName, testName);
					upperl = limitref.Usl * unitAsDouble(limitref.Units);  // calculate limit
					lowerl = limitref.Lsl * unitAsDouble(limitref.Units);
					vUnits[i] = limitref.Units;  // store the unit for later use
					vUpperl[i] = upperl;         // store limit for later use
					vLowerl[i] = lowerl;
				} catch (Error & errobj){
				  cerr << "Error: no limits found in test table in test: " << suiteName << ", test name: " << vPins[i] << endl;
				  throw Error("Error", "Error in IDDQ test");
				}

				iRange_max = dps_idd.calc_DPS_iMeasRange(vPins[i], lowerl, upperl);
				iRange_min = -iRange_max;
				dpsTask.pin(vPins[i]).min(iRange_min A).max(iRange_max A);
				if (debug_mode == 1){
					cout << "Current measurement range: " << vPins[i] << "  +/-"<< iRange_max << " A"<< endl;
				}
			}
	   // 3. do the measurement and reset Sequencer
				dpsTask.wait(wait ms).samples(sample_count).trigMode(TM::INTERNAL).execMode("PVAL");
				dpsTask.execute();

				Sequencer.reset();//reset Sequencer

		ON_FIRST_INVOCATION_END();

      //4. log result: parametric result and  set back test result to USER_DOUBLE
     // this part is outside of ON_FIRST_INVOCATION block

		ValueRes.resize(pin_count);
		for(int i=0;i<pin_count;i++)
		{
			ValueRes[i]=dpsTask.getValue(vPins[i]) / unitAsDouble(vUnits[i]);
			if(isOffline)
			{	ValueRes[i] = 2 mA;}
			if (debug_mode == 1){
					cout<<setw(25)<<vPins[i]<< setw(5)<<ValueRes[i]<< vUnits[i] <<endl;
			}
			// test name=VDD_IDD
			TESTSET().cont(TRUE).judgeAndLog_ParametricTest("",vPins[i]+"_IDD",tmLimits,ValueRes[i]);

			if("VDD" == vPins[i]) SET_USER_DOUBLE(uvICC,ValueRes[i]);//set back test result to USER_DOUBLE
		}


		return;
	}

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
REGISTER_TESTMETHOD("bm_dc_tml.IDD_Standby",IDD_Standby);


