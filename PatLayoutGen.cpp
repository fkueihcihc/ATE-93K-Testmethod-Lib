/*
 * PatLayoutGen.cpp
 *
 *	this method is for auto-generating pattern layout
 */

#include "testmethod.hpp"

//for test method API interfaces

#include "tml_def.h"

#include "tpi_c.h"

#include "sys/stat.h"

#include "sys/types.h"

#include <fstream>

using namespace std;

/**

 * Test method class.

 * For each testsuite using this test method, one object of this

 * class is created.

 */

class PatLayoutGen: public testmethod::TestMethod {

protected:

	/**

	 *Initialize the parameter interface to the testflow.

	 *This method is called just once after a testsuite is created.

	 */

	string mInputCsv;

	string mLayout_name;

	string mPatMasterFile;

	vector<string> v_signal;

	vector<string> v_type;

	virtual void initialize()

	{

		//Add your initialization code here

		//Note: Test Method API should not be used in this method!

		addParameter("InputCsv", "string", &mInputCsv).setDefault(
				"/PatLayout/xxx.csv").setComment(

		"the input signal information file");

		addParameter("Layout_name", "string", &mLayout_name).setDefault("xxx").setComment(

		"the pattern layout name");

		addParameter("PatMasterFile", "string", &mPatMasterFile).setDefault(
				"xxx.pmf").setComment(

		"the pattern master file used");

	}

	/**

	 *This test is invoked per site.

	 */

	virtual void run()

	{

		//Add your test code here.

		ReadCsv(mInputCsv);

		Gen_layout(mLayout_name, mPatMasterFile);

		return;

	}

	/*****************************sub function*************************

	 *

	 * */

	void Gen_layout(string& sLayout_name, string& sPatMasterFile)

	{
		char device_path[DM_MAX_DEVICE];
		GetDevPath(device_path);
		sPatMasterFile = "/.settings/vectors/" + sPatMasterFile + "/.layout/"
				+ sLayout_name;

		strcat(device_path, sPatMasterFile.c_str());

		//create directory if not exist

		int isCreate = mkdir(device_path,
				S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);

		if (!isCreate)

			printf("create path:%s\n", device_path);

		else

			printf("needn't create path! the directory exists : %s \n",
					device_path);

		//create file displays.properties

		string tmp_path1 = device_path;

		tmp_path1 = tmp_path1 + "/displays.properties";

		ofstream fout;

		fout.open(tmp_path1.c_str(), ios_base::out); //create new file and write in overwrite mode

		if (fout.good() != 1)

		{

			cout
					<< "================== File Open Error,pls check the output directory permission! =================="
					<< endl;

			cout << "the path is :" << tmp_path1 << endl;

			return;

		}

		const char *text1 =

				"<\?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"\?>\n"

						"<!DOCTYPE properties SYSTEM \"http://java.sun.com/dtd/properties.dtd\">\n"

						"<properties>\n"

						"<entry key=\"hideXModeAreas\">true</entry>\n"

						"<entry key=\"horizontal\">false</entry>\n"

						"<entry key=\"hideProtocols\">true</entry>\n"

						"<entry key=\"hideHeaderComments\">false</entry>\n"

						"<entry key=\"hideHeaderInstruction\">false</entry>\n"

						"<entry key=\"hideVectors\">false</entry>\n"

						"</properties>";

		fout << text1 << endl;

		fout.close();

		//create file displays.xml

		string tmp_path2 = device_path;

		tmp_path2 = tmp_path2 + "/displays.xml";

		fout.open(tmp_path2.c_str(), ios_base::out); //create new file and write in overwrite mode

		if (fout.good() != 1)

		{

			cout
					<< "================== File Open Error,pls check the output directory permission! =================="
					<< endl;

			cout << "the path is :" << tmp_path2 << endl;

			return;

		}

		const char *text_head =

				"<\?xml version =\'1.0\' encoding =\'UTF-8\'\?> \n"

						"<displays xmlns:xsi =\'http://www.w3.org/2001/XMLSchema-instance\'>";

		const char *text_end = "</displays>";

		fout << text_head << endl;

		for (size_t i = 0; i < v_signal.size(); i++) {

			string sLine = "<signal name =\'" + v_signal[i] + " (" + v_type[i]
					+ ")" + "\' index =\'false\'/>";

			fout << sLine << endl;

		}

		fout << text_end << endl;

		fout.close();

	}

	void ReadCsv(STRING strFileName)

	{

		string Data;

		char device_path[DM_MAX_DEVICE];

		GetDevPath(device_path);

		string tmp_path;

		strcat(device_path, strFileName.c_str());

		tmp_path = device_path;

		FILE* pFile = fopen(device_path, "rb");

		if (pFile == 0)

		{

			cout << std::left
					<< "##########################################ERROR##################################################"
					<< endl;

			cout << std::left << "#####93K Can't find the right input File:"
					<< strFileName << endl;

			cout << std::left << "#####in the path:" << tmp_path << endl;

			cout << std::left
					<< "#####pls check if the filename or path is right!"
					<< endl;

			cout << std::left
					<< "##########################################ERROR##################################################"
					<< endl;

			exit(1);

		}

		fseek(pFile, 0, SEEK_END);

		long len = ftell(pFile);

		char *pBuffer = new char[len + 1];

		fseek(pFile, 0, SEEK_SET);

		fread(pBuffer, 1, len, pFile);

		fclose(pFile);

		pBuffer[len] = 0;

		Data.assign(pBuffer, len);

		delete[] pBuffer;

		vector<string> ret;

		Split(Data, "\n", ret);

		v_signal.clear();

		v_type.clear();

		for (size_t i = 0; i < ret.size(); ++i)

		{

			if (ret[i].find("#") != ret[i].npos) { //# as comment symbol

				continue;

			}

			replace_all(ret[i], "\"", ""); //del "

			replace_all(ret[i], " ", ""); //del " "

			vector<string> Fields;

			vector<string> tmpVStr(2);

			Fields.clear();

			tmpVStr.clear();

			Split(ret[i], ",", Fields);

			if (Fields.size() > 1) {

				v_signal.push_back(Fields[0]);

				v_type.push_back(Fields[1]);

			}
		}
	}

	void Split(const string &str, const string& seperator,
			vector<string>& Ret) {
		Ret.clear();
		size_t nStartPosFound = str.find(seperator, 0);

		size_t nFieldStart = 0;

		for (; nStartPosFound != -1;
				nStartPosFound = str.find(seperator, nStartPosFound))

				{

			string strSub = str.substr(nFieldStart,
					nStartPosFound - nFieldStart);

			nStartPosFound = nStartPosFound + seperator.size();

			nFieldStart = nStartPosFound;

			Ret.push_back(strSub);

		}

		if (nFieldStart < str.size())

		{

			string strSub = str.substr(nFieldStart, str.size() - nFieldStart);

			Ret.push_back(strSub);

		}

	}

	void replace_all(string& str, const string& old_value,
			const string& new_value)

			{

		while (true) {

			string::size_type pos(0);

			if ((pos = str.find(old_value)) != string::npos)

				str.replace(pos, old_value.length(), new_value);

			else
				break;

		}

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
REGISTER_TESTMETHOD("bm_miscellaneous_tml.PatLayoutGen", PatLayoutGen);
