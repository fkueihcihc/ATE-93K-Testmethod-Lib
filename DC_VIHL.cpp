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
 */

class DC_VIHL: public testmethod::TestMethod {

protected:
	STRING mSetupList,mResultList;
	STRING method;
	int numofstep;
	testmethod::SpecVariable SpecVal;
	double min,max;

	/**
	 *Initialize the parameter interface to the testflow.
	 *This method is called just once after a testsuite is created.
	 */
	virtual void initialize()
	{
		addParameter("SpecValue",
				     "SpecVariable",
				     &SpecVal,
				     testmethod::TM_PARAMETER_INPUT);
		addParameter("SetupPinList",
				     "PinString",
				     &mSetupList,
				     testmethod::TM_PARAMETER_INPUT);
		addParameter("min",
				    "double",
				     &min,
					testmethod::TM_PARAMETER_INPUT)
				.setDefault("0")
				.setComment("in unit V");
		addParameter("max",
				    "double",
				     &max,
					testmethod::TM_PARAMETER_INPUT)
				.setDefault("0")
				.setComment("in unit V");
		addParameter("numofstep",
				     "int",
				     &numofstep,
					testmethod::TM_PARAMETER_INPUT)
				.setDefault("20");
		addParameter("method",
					"string",
					&method,
					testmethod::TM_PARAMETER_INPUT)
				.setOptions("Linear:LinBin:Binary")
				.setDefault("Linear");

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

		if(isOffline)
		{
			cout<<"=========================================="<<endl;
			cout<<"             This is offline              "<<endl;
			cout<<"=========================================="<<endl;
		}


		static STRING_VECTOR setupPins,resultPins;
		static int setPins_size;

		setupPins = PinUtility.getDigitalPinNamesFromPinList(mSetupList,TM::ALL_DIGITAL,TRUE,TRUE);
		setPins_size = setupPins.size();


		SPEC_SEARCH search_vol(SpecVal.mName,TM::LEV);
		if (method == "Linear")
		{
			search_vol.specPins(mSetupList).method(TM::Linear).unit("V").start(min).stop(max).numberOfSteps(numofstep).resolution(0.01);
		}
		else if(method == "LinBin")
		{
			search_vol.specPins(mSetupList).method(TM::LinBin).unit("V").start(min).stop(max).numberOfSteps(numofstep).resolution(0.01);
		}
		else if(method == "Binary")
		{
			search_vol.specPins(mSetupList).method(TM::Binary).unit("V").start(min).stop(max).numberOfSteps(numofstep).resolution(0.01);
		}

		search_vol.execute();

		DOUBLE Passvalue,Failvalue;

		switch(search_vol.getResultSpec())
		{
			case TM::TransitionPassFail:
			{
				Passvalue = search_vol.getPassVal();
				Failvalue = search_vol.getFailVal();
				if (isDebugModeOn)
				{
					cout <<setw(8)<<"v_VIL "<<std::left<<setw(5)<<"P = "<<setw(5)<< Passvalue <<"V"<< endl;
					cout <<setw(8)<<"v_VIL "<<std::left<<setw(5)<<"F = "<<setw(5)<< Failvalue <<"V"<< endl;
				}
				break;
			}
			case TM::TransitionFailPass:
			{
				Passvalue = search_vol.getPassVal();
				Failvalue = search_vol.getFailVal();
				if (isDebugModeOn)
				{
					cout <<setw(8)<<"v_VIH "<<std::left<<setw(5)<<"P = "<<setw(5)<< Passvalue <<"V"<< endl;
					cout <<setw(8)<<"v_VIH "<<std::left<<setw(5)<<"F = "<<setw(5)<< Failvalue <<"V"<< endl;

				}
				break;
			}

			case TM::AllPass:
			case TM::AllFail:
				cout << "No found within the search interval" << endl;
			break;
		}

			TESTSET().judgeAndLog_SpecSearch(search_vol);

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
REGISTER_TESTMETHOD("bm_dc_tml.DC_VIHL", DC_VIHL);


