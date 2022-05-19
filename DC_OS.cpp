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
 *
 * V1p0:initial.
 * V1p1:The default is SERIAL, and all pins are displayed on testtable;
 *
 */

class DC_OS: public testmethod::TestMethod {

protected:
	STRING mPinList;
	double uAForceC;
	STRING mParallel_Serial;
	STRING mDisconnect;
	STRING Gen_testTable;
	/**
	 *Initialize the parameter interface to the testflow.
	 *This method is called just once after a testsuite is created.
	 */
	virtual void initialize()
	{
		addParameter("PinList",
				"PinString",
				&mPinList,
				testmethod::TM_PARAMETER_INPUT);
		addParameter("uAForce",
					 "double",
					&uAForceC,
					testmethod::TM_PARAMETER_INPUT)
				.setOptions("-100:100")
				.setDefault("-100")
				.setComment("neg:-100,pos:100,in unit uA");
		addParameter("Parallel_Serial",
					"string",
					&mParallel_Serial,
					testmethod::TM_PARAMETER_INPUT)
				.setOptions("SERIAL")
				.setDefault("SERIAL");
		addParameter("DISCONNECT",
					"string",
					&mDisconnect,
					testmethod::TM_PARAMETER_INPUT)
				.setOptions("DISCONNECT:NO_DISCONNECT")
				.setDefault("NO_DISCONNECT");

		addParameter("Gen_testTable",
				"string",&Gen_testTable,
				testmethod::TM_PARAMETER_INPUT)
						   .setOptions("Yes:No")
		  	  	   	   	   .setDefault("No");
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

		if (isDebugModeOn == 1)
		{
			cout << endl << "Test Suite Name: " << tsName << endl;
		}
		static STRING_VECTOR vPins;
		static int Pins_size;
		ARRAY_D TermRes;

		vPins = PinUtility.getDigitalPinNamesFromPinList(mPinList,TM::ALL_DIGITAL,TRUE,TRUE);
		Pins_size = vPins.size();
		TermRes.resize(Pins_size);

		if("Yes"==Gen_testTable){
			ON_FIRST_INVOCATION_BEGIN();
				tmLimits.load();
				Generate_testTable(vPins);
				tmLimits.load();
			ON_FIRST_INVOCATION_END();
		}
		else if (mParallel_Serial == "SERIAL")
		{
			PPMU_SETTING SET[Pins_size],RESET[Pins_size],SET_to_ZERO,SET_to_AC_ON;
			PPMU_RELAY   Rly_PPMU_ON,Rly_AC_ON;
			PPMU_CLAMP CLAMP_ON,CLAMP_OFF;
			PPMU_MEASURE MEAS[Pins_size];
			TASK_LIST    TASK[Pins_size],TASK_Zero,TASK_AC_ON;

			ON_FIRST_INVOCATION_BEGIN();

				CONNECT();
				SET_to_ZERO.pin(mPinList).vForce(0 V).iRange(1000 uA).min(0 uA).max(1000 uA);
				Rly_PPMU_ON.pin(mPinList).status("PPMU_ON");
				Rly_PPMU_ON.wait(2 ms);
				CLAMP_ON.pin(mPinList).status("CLAMP_ON").high(3).low(-2);
				CLAMP_OFF.pin(mPinList).status("CLAMP_OFF");
				TASK_Zero.add(SET_to_ZERO).add(CLAMP_ON).add(Rly_PPMU_ON);
				TASK_Zero.execute();

				for (unsigned int j = 0; j < vPins.size(); j++)
				{
					SET[j].pin(vPins[j]).iForce(uAForceC uA).iRange(1000 uA).min(-1.5 V).max(1.5 V);
					SET[j].wait(0 ms);
					MEAS[j].pin(vPins[j]).execMode(TM::OVAL);
					RESET[j].pin(vPins[j]).vForce(0 V).iRange(1000 uA).min(0 uA).max(1000 uA);
					RESET[j].wait(0 ms);
					TASK[j].add(SET[j]).add(CLAMP_ON).add(MEAS[j]).add(CLAMP_OFF).add(RESET[j]);
					TASK[j].execute();
				}

			ON_FIRST_INVOCATION_END();

			for(unsigned int i = 0; i < vPins.size(); i++)
			{
				TermRes[i] = MEAS[i].getValue(vPins[i]);

				if (isOffline)
				{
					if (uAForceC > 0)
					{
						TermRes[i] = 0.5 V;
					}
					else
					{
						TermRes[i] = -0.5 V;
					}

				}

				if (isDebugModeOn == 1)
				{
					cout <<setw(4)<<i<<std::left<<setw(25)<<vPins[i]<< setw(8)<<TermRes[i]*1e3<<"mV"<< endl;
				}

				TESTSET().cont(TRUE).judgeAndLog_ParametricTest(vPins[i],vPins[i], tmLimits, TermRes[i]*1e3);
			}

			ON_FIRST_INVOCATION_BEGIN();

				SET_to_AC_ON.pin(mPinList).iForce(0 uA).iRange(1 mA).min(0 mV).max(2000 mV);
				Rly_AC_ON.pin(mPinList).status("AC_ON");
				TASK_AC_ON.add(SET_to_AC_ON).add(Rly_AC_ON).add(CLAMP_OFF);
				TASK_AC_ON.execute();

			ON_FIRST_INVOCATION_END();
		}

		ON_FIRST_INVOCATION_BEGIN();

			if (mDisconnect == "DISCONNECT")
			{
				DISCONNECT();
			}

		ON_FIRST_INVOCATION_END();

		return;

	}
/*****************************sub function*************************
*
* */
  void Generate_testTable(STRING_VECTOR &temp_pins_list)
  {

	char temp_test[50],temp_testname[50];
	string suite;
	GET_TESTSUITE_NAME(suite);
//
	TestTable* testTable = TestTable::getDefaultInstance();
	testTable->isCsvLoadNeeded();
	testTable->readCsvFile();
//
	int KeyValue[]={1};
	testTable->setLookupKeys(1,KeyValue);

	sprintf(temp_test,"%s",suite.c_str());


	for(size_t i=0;i<temp_pins_list.size();i++)
	{

		sprintf(temp_testname,"%s",temp_pins_list[i].c_str());
		if(testTable->getTestPos(temp_test)<0)
		{
			testTable->addTest(temp_test);
			testTable->setValue(temp_test,"Suite name",suite.c_str());
			testTable->setValue(temp_test,"Test name",temp_testname);
			testTable->writeCsvFile();
		}
	}

	TestTable::clearDefaultInstance();

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
REGISTER_TESTMETHOD("bm_dc_tml.DC_OS", DC_OS);

