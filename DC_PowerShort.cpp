#include "testmethod.hpp"
#include "Common.h"
#include "dpsComCode.h"
//for test method API interfaces
#include "mapi.hpp"
using namespace std;
using namespace V93kLimits;

/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 *V1p0:initial;
 *V1p1:Added Range as an option, and all pins are displayed on testtable;
 *
 */

class DC_PowerShort: public testmethod::TestMethod {

protected:
	STRING dpsPinList;
	STRING Range;
	double wait;

	/**
	 *Initialize the parameter interface to the testflow.
	 *This method is called just once after a testsuite is created.
	 */
	virtual void initialize()
	{
		addParameter("PinList",
				"PinString",
				&dpsPinList,
				testmethod::TM_PARAMETER_INPUT);
		addParameter("Range",
				"string",
				&Range,
				testmethod::TM_PARAMETER_INPUT)
				.setOptions("1A:100mA:25mA:2mA")
				.setDefault("100mA");
		addParameter("Wait_ms",
					"double",
					&wait,
				testmethod::TM_PARAMETER_INPUT)
				.setDefault("2")
				.setComment("ms");

		//Add your initialization code here
		//Note: Test Method API should not be used in this method!
	}


	/**
	 *This test is invoked per site.
	 */

	virtual void run()
	{
		//Add your test code here.
		static INT isDebugModeOn;
		static INT isOffline;
		static STRING tsName;

		GET_SYSTEM_FLAG("offline",&isOffline);
		GET_TESTSUITE_NAME(tsName);
		GET_TESTFLOW_FLAG("debug_mode",&isDebugModeOn);

		if (isDebugModeOn)
		{
			cout << endl << "Test Suite Name: " << tsName << endl;
		}
		static STRING_VECTOR vPins;
		static int Pins_size;
		DPS_TASK dpsTask;
		ARRAY_D TermRes;

		vPins = PinUtility.getDpsPinNamesFromPinList(dpsPinList);
		Pins_size = vPins.size();
		TermRes.resize(Pins_size);

		ON_FIRST_INVOCATION_BEGIN();

			DISCONNECT();

			CONNECT();

			WAIT_TIME(5 ms);

			FW_TASK("RLYC IDLE,OFF,(@)");// all digital pins disconnected

			if(Range == "1A")
			{	dpsTask.pin(dpsPinList).measurementMode(TM::MEASURE_CURRENT_UNGANGED).min(-1.0 A).max(1.0 A);}
			else if(Range == "100mA")
			{	dpsTask.pin(dpsPinList).measurementMode(TM::MEASURE_CURRENT_UNGANGED).min(-100 mA).max(100 mA);}
			else if(Range == "25mA")
			{	dpsTask.pin(dpsPinList).measurementMode(TM::MEASURE_CURRENT_UNGANGED).min(-25 mA).max(25 mA);}
			else if(Range == "2mA")
			{	dpsTask.pin(dpsPinList).measurementMode(TM::MEASURE_CURRENT_UNGANGED).min(-2 mA).max(2 mA);}

			dpsTask.wait(wait ms).samples(64).trigMode(TM::INTERNAL).execMode("PVAL");
			dpsTask.execute();

			FW_TASK("RLYC AC,OFF,(@)");// all digital pins disconnected

		ON_FIRST_INVOCATION_END();


			for(unsigned int i = 0; i < vPins.size(); i++)
			{

				TermRes[i] = dpsTask.getValue(vPins[i]) * 1e3;

				if (isOffline)
				{
						TermRes[i] = 10;
				}

				if (isDebugModeOn == 1)
				{
						cout <<setw(4)<<i<<std::left<<setw(25)<<vPins[i]<< setw(8)<<TermRes[i]<<"mA"<< endl;
				}

				TESTSET().cont(TRUE).judgeAndLog_ParametricTest(vPins[i],vPins[i],tmLimits,TermRes[i]);

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
REGISTER_TESTMETHOD("bm_dc_tml.DC_PowerShort", DC_PowerShort);

