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
 * V1p0: initial
 * V1p1: added setial test and multi-group test, modyfied by lvling 2017/1/25;
 *
 *
 */

class DC_IIHL: public testmethod::TestMethod {

protected:
	STRING mPinList;
	STRING mVForce;
	int stop_cycle;
	STRING mParallel_Serial;
	STRING Gen_testTable;
	STRING Check_func;
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
		addParameter("VForce_V",
					 "string",
					&mVForce,
					testmethod::TM_PARAMETER_INPUT)
				.setDefault("0")
				.setComment("in unit V");
		addParameter("stop_cycle",
					 "int",
					&stop_cycle,
					testmethod::TM_PARAMETER_INPUT)
				.setDefault("0");
		addParameter("Parallel_Serial",
							"string",
							&mParallel_Serial,
							testmethod::TM_PARAMETER_INPUT)
						.setOptions("PARALLEL:SERIAL")
						.setDefault("PARALLEL");
		addParameter("Gen_testTable","string",&Gen_testTable,testmethod::TM_PARAMETER_INPUT)
						   .setOptions("Yes:No")
		  	  	   	   	   .setDefault("No");
		addParameter("Check_func","string",&Check_func,testmethod::TM_PARAMETER_INPUT)
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

		static STRING_VECTOR vPins_vec,vForce_vec;
		static STRING_VECTOR tran_pins;
		static vector<STRING_VECTOR> vPins;
		static vector<double> dForce_vec;
		static int vPins_vec_size,vForce_vec_size;
		double value;

		ARRAY_D ValueRes;
		ARRAY_I sites;
		int site_num,site_index;
		site_num = GET_ENABLED_SITES(sites);
		Boolean Function_Res[site_num];

		GET_SYSTEM_FLAG("offline",&isOffline);
		GET_TESTSUITE_NAME(tsName);
		GET_TESTFLOW_FLAG("debug_mode",&isDebugModeOn);
		vPins.clear();
		dForce_vec.clear();
		splitStr(mPinList,';',vPins_vec);
		splitStr(mVForce,';',vForce_vec);
		vPins_vec_size = vPins_vec.size();
		vForce_vec_size = vForce_vec.size();

		if (isDebugModeOn == 1)
		{
			cout << endl << "Test Suite Name: " << tsName << endl;
		}
		for(int i=0;i<vPins_vec_size;i++)
		{
			if (isDebugModeOn == 1) cout<< "The Pins is "<<vPins_vec[i]<<endl;
			vPins.push_back(PinUtility.getDigitalPinNamesFromPinList(vPins_vec[i],TM::ALL_DIGITAL,TRUE,TRUE));

			if(i<vForce_vec_size)
			{
				value = atof(vForce_vec[i].c_str());
				dForce_vec.push_back(value);
			}
			else
			{
				cout<<"The input of mVForce is error!"<<endl;
			}

		}

		ON_FIRST_INVOCATION_BEGIN();

			CONNECT();

			if (stop_cycle !=0)
			{
				Sequencer.stopCycle(stop_cycle);
				FUNCTIONAL_TEST();
			}

			if("Yes"==Gen_testTable)
			{
				tmLimits.load();
				Generate_testTable(vPins,vPins_vec_size);
				tmLimits.load();
			}

		ON_FIRST_INVOCATION_END();

		site_index=CURRENT_SITE_NUMBER()-1;
		Function_Res[site_index]=GET_FUNCTIONAL_RESULT();//Function test

		if(mParallel_Serial == "PARALLEL")
		{
			PPMU_SETTING SET;
			PPMU_RELAY   RLY_PPMU_ON,RLY_AC_ON;
			PPMU_MEASURE MEAS;
			TASK_LIST    TASK,TASK_PPMUON,TASK_ACON;
			ON_FIRST_INVOCATION_BEGIN();

				for(int i=0;i<vPins_vec_size;i++)
				{
					{
						SET.pin(vPins_vec[i]).vForce(dForce_vec[i] V).iRange(50 uA).min(-100 uA).max(100 uA);

						RLY_PPMU_ON.pin(vPins_vec[i]).status("PPMU_ON");
						RLY_PPMU_ON.wait(1.3 ms);
						RLY_AC_ON.pin(vPins_vec[i]).status("AC_ON");
						RLY_AC_ON.wait(0.3 ms);
						MEAS.pin(vPins_vec[i]).execMode(TM::PVAL);
						TASK.add(RLY_PPMU_ON).add(SET).add(MEAS).add(RLY_AC_ON);
					}
				}

				TASK.execute();

			ON_FIRST_INVOCATION_END();

			for(int i=0;i<vPins_vec_size;i++)
			{
				ValueRes.resize(vPins[i].size());
				if (isDebugModeOn == 1) cout << endl << "The vPins_size is: " << vPins[i].size() << endl;
				if (isDebugModeOn == 1)
				{ cout << endl << "Current Group is: " << vPins_vec[i] << endl;}

				for(unsigned int j=0;j<vPins[i].size();j++)
				{
					ValueRes[j] = MEAS.getValue(vPins[i][j])*1e6;
					if (isOffline)
					{
						if (dForce_vec[i])
						{  ValueRes[j] = 5;}
						else
						{  ValueRes[j] = -5;}
					}
					if (isDebugModeOn == 1)
					{
						cout <<setw(4)<<j<<std::left<<setw(25)<<vPins[i][j]<< setw(8)<<ValueRes[j]<<"uA"<< endl;
					}
					TESTSET().cont(TRUE).judgeAndLog_ParametricTest(vPins[i][j], vPins[i][j], tmLimits, ValueRes[j]);
				}
			}

			Sequencer.reset();
		}
		else if(mParallel_Serial == "SERIAL")
		{
			int Serialsize;

			for(int i=0;i<vPins_vec_size;i++)
			{

					Serialsize = vPins[i].size();
					PPMU_SETTING SET[Serialsize],SWITCH[Serialsize];
					PPMU_MEASURE MEAS[Serialsize];
					PPMU_RELAY   RLY_PPMU_ON[Serialsize],RLY_AC_ON[Serialsize];
					TASK_LIST    TASK[Serialsize];

				ON_FIRST_INVOCATION_BEGIN();
				{
					for(unsigned int j=0;j<vPins[i].size();j++)
					{
						SET[j].pin(vPins[i][j]).vForce(dForce_vec[i] V).iRange(50 uA).min(-100 uA).max(100 uA).mode("VFIM");
						RLY_PPMU_ON[j].pin(vPins[i][j]).status("PPMU_ON");
						RLY_AC_ON[j].pin(vPins[i][j]).status("AC_ON");
						SWITCH[j].pin(vPins[i][j]).iForce(0 mA).iRange(20 mA).max(3000 mV).min(-1000 mV);

						MEAS[j].pin(vPins[i][j]).execMode(TM::OVAL);
						TASK[j].add(SET[j]).add(RLY_PPMU_ON[j]).add(MEAS[j]).add(SWITCH[j]).add(RLY_AC_ON[j]);
						TASK[j].execute();
					}
				}
				ON_FIRST_INVOCATION_END();

				ValueRes.resize(vPins[i].size());
				if (isDebugModeOn == 1) cout << endl << "The vPins_size is: " << vPins[i].size() << endl;
				if (isDebugModeOn == 1)
				{ cout << endl << "Current Group is: " << vPins_vec[i] << endl;}

				for(unsigned int  j=0;j<vPins[i].size();j++)
				{
					ValueRes[j] = MEAS[j].getValue(vPins[i][j])*1e6;
					if (isOffline)
					{
						if (dForce_vec[i])
						{  ValueRes[j] = 5;}
						else
						{  ValueRes[j] = -5;}
					}
					if (isDebugModeOn == 1)
					{
						cout <<setw(4)<<j<<std::left<<setw(25)<<vPins[i][j]<< setw(8)<<ValueRes[j]<<"uA"<< endl;
					}
					TESTSET().cont(TRUE).judgeAndLog_ParametricTest(vPins[i][j], vPins[i][j], tmLimits, ValueRes[j]);
				}

			}
				Sequencer.reset();
		}

		if (Function_Res[site_index])
		{  if (isDebugModeOn == 1) cout <<"Func result: Pass"<<endl;}
		else
		{  if (isDebugModeOn == 1) cout <<"Func result: Fail"<<endl;}
		if("Yes"==Check_func){
			TESTSET().cont(true).TEST("",tsName+"_FUNC",tmLimits,Function_Res[site_index]);
		}


		return;
	}

	/**
	 *This function will be invoked once the specified parameter's value is changed.
	 *@param parameterIdentifier
	 */
	 /*****************************sub function*************************
		   *
		   * */
	void Generate_testTable(vector<STRING_VECTOR> temp_pins_list,size_t temp_pins_list_size)
	{

		char temp_test[50],temp_testname[50];
		string suite;
		GET_TESTSUITE_NAME(suite);

		TestTable* testTable = TestTable::getDefaultInstance();
		testTable->isCsvLoadNeeded();
		testTable->readCsvFile();
		int KeyValue[]={1};
		testTable->setLookupKeys(1,KeyValue);

		sprintf(temp_test,"%s",suite.c_str());
		for(double i=0;i<temp_pins_list_size;i++)
		{
			for(unsigned int j=0;j<temp_pins_list[i].size();j++)
			{

				sprintf(temp_testname,"%s",temp_pins_list[i][j].c_str());

				if(testTable->getTestPos(temp_test)<0)
				{
					testTable->addTest(temp_test);
					testTable->setValue(temp_test,"Lsl","-10");
					testTable->setValue(temp_test,"Lsl_typ","GE");
					testTable->setValue(temp_test,"Usl_typ","LE");
					testTable->setValue(temp_test,"Usl","10");
					testTable->setValue(temp_test,"Units","uA");
					testTable->setValue(temp_test,"Suite name",suite.c_str());
					testTable->setValue(temp_test,"Test name",temp_testname);
					testTable->writeCsvFile();
				}
			}
		}

		TestTable::clearDefaultInstance();

	}
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

REGISTER_TESTMETHOD("bm_dc_tml.DC_IIHL", DC_IIHL);
