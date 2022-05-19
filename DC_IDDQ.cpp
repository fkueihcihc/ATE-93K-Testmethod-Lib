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
 * V1p0:
 *
 */

class DC_IDDQ: public testmethod::TestMethod {

protected:
	STRING dpsPinList;
	STRING TestPoint;
	STRING Port;

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
		addParameter("TestCycle",
				"string",
				&TestPoint,
				testmethod::TM_PARAMETER_INPUT);
		addParameter("Port","string",&Port,testmethod::TM_PARAMETER_INPUT)
				  	  		.setOptions("@:pDFT_TRANS")
				  	  		.setDefault("@");

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

		static STRING_VECTOR dpsPins,vPoints;
		static int dpsPins_size,vPoints_size;
		int value;

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
			dpsPins.clear();
			vPoints.clear();
			splitStr(TestPoint,',',vPoints);
			vPoints_size = vPoints.size();


			if (isDebugModeOn == 1)
			{	cout << endl << "Test Suite Name: " << tsName << endl;
				cout<<"vPoints_size="<<vPoints_size<<endl;
			}

			dpsPins = PinUtility.getDpsPinNamesFromPinList(dpsPinList);
			dpsPins_size = dpsPins.size();
			CONNECT();
		}
		ON_FIRST_INVOCATION_END();

		DPS_TASK dpsTask[vPoints_size];
		for(int i=0;i<vPoints_size;i++)
		{
			value = atof(vPoints[i].c_str());

			ON_FIRST_INVOCATION_BEGIN();
			Sequencer.stopCycle(value,Port);
			FUNCTIONAL_TEST();
			ON_FIRST_INVOCATION_END();

			if (isDebugModeOn == 1)
			{	cout<<setw(10)<<"on Cycle "<<std::left<<setw(10)<<vPoints[i]<<endl;}

			if(1)
			{
				site_index=CURRENT_SITE_NUMBER()-1;
				Function_Res[site_index]=GET_FUNCTIONAL_RESULT();//Function test
				TESTSET().cont(true).TEST("",tsName+"_FUNC",tmLimits,Function_Res[site_index]);
				if(!Function_Res)
				{	if (isDebugModeOn == 1) cout<<"Site "<< CURRENT_SITE_NUMBER() <<" is Fail!"<<endl;}
			}

			ON_FIRST_INVOCATION_BEGIN();
			dpsTask[i].pin(dpsPinList).min(0).max(100 mA);
			dpsTask[i].pin(dpsPinList).measurementMode(TM::MEASURE_CURRENT_UNGANGED);
			dpsTask[i].wait(3 ms).samples(64).trigMode(TM::INTERNAL).execMode("PVAL");
			dpsTask[i].execute();
			ON_FIRST_INVOCATION_END();

			int IDDQ_SUM = 0;

			ValueRes.resize(dpsPins_size);
			for(int j=0;j<dpsPins_size;j++)
			{
				ValueRes[j]=dpsTask[i].getValue(dpsPins[j])*1e3;
				if(isOffline)
				{	ValueRes[j] = 2 mA;}
				if (isDebugModeOn == 1)
				{
						cout<<setw(25)<<dpsPins[j]<<std::left<<setw(5)<<ValueRes[j]<<" mA"<<endl;
				}

				TESTSET().cont(TRUE).TEST(dpsPins[j],dpsPins[j],tmLimits,ValueRes[j]);

				if("DVDD_CORE_0P9"==dpsPins[j] || "DVDD_A53_0P9"==dpsPins[j]){
					IDDQ_SUM+=ValueRes[j];
//					cout<<"IDDQ_SUM: "<<IDDQ_SUM<<" mA"<<endl;
				}
			}

			ON_FIRST_INVOCATION_BEGIN();
			Sequencer.reset();
			ON_FIRST_INVOCATION_END();

			TESTSET().cont(TRUE).TEST("","IDDQ_SUM",tmLimits,IDDQ_SUM);
			SET_USER_DOUBLE("IDDQ_SUM",IDDQ_SUM);

		}

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
REGISTER_TESTMETHOD("bm_dc_tml.DC_IDDQ", DC_IDDQ);


