#include "testmethod.hpp"
#include "Common.h"
#include "dpsComCode.h"
//for test method API interfaces
#include "mapi.hpp"
//#include "DC_Utility_zte.hpp"
using namespace std;
using namespace V93kLimits;

/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 *
 * V1p0: inital;
 * V1p1:All pins are displayed on testtable;
 *
 *
 */


class DC_VOHL: public testmethod::TestMethod {

protected:
	STRING mPinList;
	STRING mACurrent;
	int stop_cycle;

	/**
	 *Initialize the parameter interface to the testflow.
	 *This method is called just once after a testsuite is created.
	 */
	virtual void initialize()
	{
		addParameter("PinList",
				"string",
				&mPinList,
				testmethod::TM_PARAMETER_INPUT);
		addParameter("mACurrent",
					 "string",
					&mACurrent,
					testmethod::TM_PARAMETER_INPUT)
				.setDefault("0")
				.setComment("in unit mV");
		addParameter("stop_cycle",
					 "int",
					&stop_cycle,
					testmethod::TM_PARAMETER_INPUT)
				.setDefault("0");


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

		static STRING_VECTOR vPins_vec,vForce_vec;
		static vector<STRING_VECTOR> vPins;
		static vector<double> dForce_vec;
		static int vPins_vec_size,vForce_vec_size;
		double value;

		PPMU_SETTING SET;
		PPMU_RELAY   RLY_PPMU_ON,RLY_AC_ON;
		PPMU_MEASURE MEAS;
		PPMU_CLAMP CLAMP_ON,CLAMP_OFF;
		TASK_LIST    TASK;
		ARRAY_D ValueRes;
		ARRAY_I sites;
		int site_num,site_index;
		site_num = GET_ENABLED_SITES(sites);
		Boolean Function_Res[site_num];

		ON_FIRST_INVOCATION_BEGIN();
		{
			GET_SYSTEM_FLAG("offline",&isOffline);
			GET_TESTSUITE_NAME(tsName);
			GET_TESTFLOW_FLAG("debug_mode",&isDebugModeOn);
			vPins.clear();
			dForce_vec.clear();
			splitStr(mPinList,';',vPins_vec);
			splitStr(mACurrent,';',vForce_vec);
			vPins_vec_size = vPins_vec.size();
			vForce_vec_size = vForce_vec.size();
			if (isDebugModeOn)
			{
				cout << endl << "Test Suite Name: " << tsName << endl;
			}
			if (stop_cycle !=0)
			{
				Sequencer.stopCycle(stop_cycle);
				FUNCTIONAL_TEST();
			}
			site_index=CURRENT_SITE_NUMBER()-1;
			Function_Res[site_index]=GET_FUNCTIONAL_RESULT();//Function test

			for(int i=0;i<vPins_vec_size;i++)
			{
				cout<< "The Pins is "<<vPins_vec[i]<<endl;
				vPins.push_back(PinUtility.getDigitalPinNamesFromPinList(vPins_vec[i],TM::ALL_DIGITAL,TRUE,TRUE));

				if(i<vForce_vec_size)
				{
					value = atof(vForce_vec[i].c_str());
					dForce_vec.push_back(value);
				}
				else
				{
					cout<<"The input of mACurrent is error!"<<endl;
				}
			}
			CONNECT();
		}
//		ON_FIRST_INVOCATION_END();
//
//		ON_FIRST_INVOCATION_BEGIN();
		{

			for(int i=0;i<vPins_vec_size;i++)
			{
				SET.pin(vPins_vec[i]).iForce(dForce_vec[i] mA).iRange(20 mA).min(0 V).max(3 V);
				RLY_PPMU_ON.pin(vPins_vec[i]).status("PPMU_ON");
				RLY_PPMU_ON.wait(1.3 ms);
				RLY_AC_ON.pin(vPins_vec[i]).status("AC_ON");
				RLY_AC_ON.wait(0.3 ms);
				MEAS.pin(vPins_vec[i]).execMode(TM::PVAL);
				CLAMP_ON.pin(vPins_vec[i]).status("CLAMP_ON").low(0 V).high(3 V);
				CLAMP_OFF.pin(vPins_vec[i]).status("CLAMP_OFF");
				TASK.add(SET).add(CLAMP_ON).add(RLY_PPMU_ON).add(CLAMP_OFF).add(MEAS).add(RLY_AC_ON);
//				TASK.setup();
			}
			TASK.execute();
			Sequencer.reset();
		}
		ON_FIRST_INVOCATION_END();

		for(unsigned int i = 0; i < vPins.size(); i++)
		{
			ValueRes.resize(vPins[i].size());
			if (isDebugModeOn)
			{ cout << endl << "Current Group is: " << vPins_vec[i] << endl;}

			for(unsigned int j=0;j<vPins[i].size();j++)
			{
				//ValueRes[j] = MEAS.getValue(vPins[i][j])*1e6;
				ValueRes[j] = MEAS.getValue(vPins[i][j]);
				if (isOffline)
				{
					if (dForce_vec[i] < 0)
					{
						if(i == 0)
						{	ValueRes[j] = 1.6;}
						else if(i == 1)
						{	ValueRes[j] = 2.5;}
						else
						{	ValueRes[j] = 3;}
					}
					else if (dForce_vec[i] > 0)
					{	 ValueRes[j] = 0.2;}
				}
				if (isDebugModeOn)
				{
//					cout <<setw(4)<<j<<std::left<<setw(25)<<vPins[i][j]<<setw(8)<<ValueRes[i]<<"V"<< endl;
					cout <<setw(4)<<j<<std::left<<setw(25)<<vPins[i][j]<<setw(8)<<ValueRes[j]<<"V"<< endl;
				}
				TESTSET().cont(TRUE).judgeAndLog_ParametricTest(vPins[i][j], vPins[i][j], tmLimits, ValueRes[j]);

			}
//			stringstream IntToString;
//			string num;
//			IntToString << i;
//			IntToString >> num;
//			TESTSET().cont(TRUE).judgeAndLog_ParametricTest(vPins[i], tsName+num, tmLimits, ValueRes);
		}


		if (Function_Res[site_index])
		{
			cout <<"Func result: Pass"<<endl;
		}
		else
		{
			cout <<"Func result: Fail"<<endl;
		}

		TESTSET().cont(true).TEST("",tsName+"_FUNC",tmLimits,Function_Res[site_index]);

		return;
	}

	/**
	 *This function will be invoked once the specified parameter's value is changed.
	 *@param parameterIdentifier
	 */

	void splitStr(const STRING& origStr, const CHAR sepCh, STRING_VECTOR& array, const Boolean bSupportBracket = FALSE)
	{

	    Boolean bIsWithinBracket;
	    STRING word;
	    const CHAR* str = origStr.c_str();

	    array.clear();

	    for (INT i = 0 ; *(str+i) != 0 ; ++i)
	    {
	        CHAR ch = str[i];
	        /* seperator CHAR '\n' */
	        if ( sepCh == '\n' )
	        {
	            if ( ch == sepCh )
	            {
	                array.push_back(word);
	                word = "";
	            }
	            else
	            {
	                word += ch;
	            }

	            continue;
	        }

	        /* for other seperator char, like ',', '"',' ", etc.*/
	        if( ch == sepCh )
	        {
	            if ( bIsWithinBracket && bSupportBracket )
	            {
	                word += ch;
	            }
	            else
	            {
	                array.push_back(word);
	                word = "";
	            }
	        }
	        else if ( ch == '(' )
	        {
	            bIsWithinBracket = TRUE;
	        }
	        else if ( ch == ')' )
	        {
	            bIsWithinBracket = TRUE;
	        }
	        else if ( ch == ' ' )
	        {
	            if ( i == 4)
	            {
	                /* this space is after the FW command name, eg. "DFPN " */
	                array.push_back(word);
	                word = "";
	            }
	        }
	        else
	        {
	            word += ch;
	        }
	    }
	    /* origStr is like "abc,def", we pick the rest one */
	    if ( word.size() )
	    {
	        array.push_back(word);
	    }
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
REGISTER_TESTMETHOD("bm_dc_tml.DC_VOHL", DC_VOHL);


