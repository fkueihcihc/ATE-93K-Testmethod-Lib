/*
 * TimDiagFormatGen.cpp
 *
 *	this method is for auto-generating timing diagram format
 */

#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
#include "tml_def.h"
#include "tpi_c.h"
#include <fstream>

using namespace std;

/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */

class TimDiagFormatGen: public testmethod::TestMethod {

protected:

	/**
	 *Initialize the parameter interface to the testflow.
	 *This method is called just once after a testsuite is created.
	 */
	string mPinList;
	string mFormat_name;
	string mModeContext;

	virtual void initialize() {
		//Add your initialization code here
		//Note: Test Method API should not be used in this method!
		addParameter("PinList", "PinString", &mPinList,
				testmethod::TM_PARAMETER_INPUT);
		addParameter("Format_name", "string", &mFormat_name).setDefault("xxx").setComment(
				"the timing diagram format name");
		addParameter("MODECONTEXT", "string", &mModeContext).setDefault(
				"DEFAULT").setComment(
				"the MODECONTEXT name such as DEFAULT/FASTDIFF");
	}
	/**
	 *This test is invoked per site.
	 */
	virtual void run() {
		//Add your test code here.
		static STRING_VECTOR vPins;
		vPins = PinUtility.getDigitalPinNamesFromPinList(mPinList,
				TM::ALL_DIGITAL, TRUE, TRUE);
		char device_path[DM_MAX_DEVICE];

		GetDevPath(device_path);

		strcat(device_path, "/format/");

		//create file
		ofstream fout;
		string FormatFile = device_path;
		FormatFile = FormatFile + "RT" + mFormat_name;
		fout.open(FormatFile.c_str(), ios_base::out); //create new file and write in overwrite mode
		if (fout.good() != 1) {
			cout
					<< "================== File Open Error,pls check the output directory permission! =================="
					<< endl;
			cout << "the path is :" << FormatFile << endl;
			return;
		}
		fout << "format definition file" << endl; //write the file head
		fout << "$alias_set=" << mModeContext << endl;
		for (size_t i = 0; i < vPins.size(); i++) {
			string mPin = vPins[i];
			fout << mPin << "," << "o" << "," << "0" << endl;
			fout << mPin << "," << "i" << "," << "0" << endl;
		}
		fout.close();
		//register format
		string format_RT_file = device_path;
		format_RT_file = format_RT_file + "format_RT";
		fout.open(format_RT_file.c_str(), ios_base::out | ios_base::app); //create new file and write in appending mode
		fout << mFormat_name << endl;
		fout.close();
		return;
	}
	/*****************************sub function*************************
	 *
	 * */
	/**
	 *This function will be invoked once the specified parameter's value is changed.
	 *@param parameterIdentifier
	 */
	virtual void postParameterChange(const string& parameterIdentifier) {
		//Add your code
		//Note: Test Method API should not be used in this method!
		return;
	}
	/**
	 *This function will be invoked once the Select Test Method Dialog is opened.
	 */
	virtual const string getComment() const {
		string comment = " please add your comment for this method.";
		return comment;
	}
};
REGISTER_TESTMETHOD("bm_miscellaneous_tml.TimDiagFormatGen", TimDiagFormatGen);
