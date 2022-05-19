
#include "TestMethod.h"
#include <vector>
#include <string>
#include <iterator>
#include <sstream>
#include <fstream>
#include "Common.h"
#include <algorithm>
#include "FW.h"
using namespace std;



int efuse_data[SITE_NUMBER][87];
unsigned short crc2[SITE_NUMBER];
unsigned int chip_id32[SITE_NUMBER];

DOUBLE Frequency(ARRAY_I temp_data, double Test_Period,int fft_mod)
{
	   DOUBLE      frequency_MHz;
	   DOUBLE      gradient1;
	   DOUBLE      offset1;
	   ARRAY_I     data,temp_A1,temp_B1,temp_C1,temp_index1;
       int j1,j2;
       int size = temp_data.size();

       ARRAY_D spect ;
	   int min_index , max_index ;
	   double min , max , dR , interpol_index ;

   if(fft_mod ==0)
   {

	   data.resize(size);
	   temp_A1.resize(size);
	   temp_B1.resize(size-1);

	   DSP_EXTRACT(temp_data, data, 0, 0, size);
	   DSP_EXTRACT(data, temp_A1, 0, 0, size-1);         // extract data
	   DSP_EXTRACT(data, temp_B1, 1, 0, size -1);         // extract data

	   temp_C1.resize(size-1);
	   temp_index1.resize(size-1);


	   j1=0;
	   j2=0;

	   for(int i=0; i<size-1; i++)
	   {
		   temp_C1[i] = temp_B1[i] - temp_A1[i];
		   if (temp_C1[i]>0.5)
		   {
			   temp_index1[j1]=i  ;
			   j1++;
		   }
	   }


	   if(j1>1)
	  	   {
	  		   DSP_REG1(temp_index1, &gradient1, &offset1 , 0, j1);
	  		 frequency_MHz =1000* 1/(Test_Period*gradient1);
	  	   }
	  	   else
	  	   {
	  		 frequency_MHz=-1;
	  	   }
   }
   else
   {
	   DSP_SPECTRUM ( temp_data , spect , VOLT , 1.0 , HANNING , 0 ) ;  // Tabei-Ueda algorithm needs power spectrum (no log!) with Hanning filter applied

	   //cancel out the potential max bin at DC
	   if (spect.size() > 2)
	   {
	     spect[0] = 0;
	     spect[1] = 0;
	   }
	   else
	   {
	     cerr<<"WARN: spectrum data is too small."<<endl;
	   }
	   DSP_MINMAX  ( spect , &min , &max , &min_index , &max_index ) ;  // look for max bin

	   // interpolating equations
	   int max_index_plus_one = max_index + 1;
	   max_index_plus_one = (max_index_plus_one < spect.size())?
	                        max_index_plus_one : max_index_plus_one-1;

	   if ( ( max_index > 0 ) && ( spect[ max_index - 1 ] > spect[ max_index_plus_one ] ) )
	   {
	     dR = spect[ max_index - 1 ] / spect[ max_index ] ;
	     interpol_index = max_index + ( 1.0 - 2.0 * dR ) / ( 1.0 + dR ) ;
	   }
	   else
	   {
	     dR = spect[ max_index_plus_one ] / spect[ max_index ] ;
	     interpol_index = max_index - ( 1.0 - 2.0 * dR ) / ( 1.0 + dR ) ;
	   }

	   frequency_MHz = interpol_index / ( Test_Period * temp_data.size() ) ;
   }
   return frequency_MHz;

}
string to_string(int n)
{
	ostringstream stream;
	stream<<n;
	return stream.str();
}


string to_string(double n)
{
	ostringstream stream;
	stream<<n;
	return stream.str();
}

string to_string(long n)
{
	ostringstream stream;
	stream<<n;
	return stream.str();
}

void splitString2Vector(const string & str, vector<string> & strvec, const string splitter)
{
	string::size_type pos1 = 0;
	string::size_type pos2 = 0;
	int splitter_len;
	splitter_len = splitter.length();

	do{
		while(pos1 != str.npos ){
			if (str[pos1] == ' ' || str[pos1] == '\t' || str[pos1] == '\n' )
				pos1++; // skip space, tab and new line characters
			else
				break;
		}
		pos2 = str.find(splitter, pos1);
		strvec.push_back(str.substr(pos1, pos2-pos1));
		pos1 = pos2 + splitter_len;
	}while(pos2 != str.npos);

}

void checkBin_overonOff(const string &tsname, const vector<string> &tnames, int debugmode)
{
	  int ts_pos, tn_pos, pos_arr[2]; // field postion for test suite and test name
	  int errstatus = 0;
//	  TestTable* testTable = TestTable::getDefaultInstance();  // it does not work for master file
	  TestTableMgr * ttablemgr = TestTableMgr::getDefaultInstance();
	  TestTable* testTable = ttablemgr->getTestTable((unsigned int)0);

//	  if(debugmode == 1) {
//		  for(int j = 0; j < testTable->getFieldCount(); j++) {
//			  cout << "field " << j << " is " << testTable->getFieldName(j) << endl;
//		  }
//	  }
	  ts_pos = testTable->getFieldPos("Suite name");
	  tn_pos = testTable->getFieldPos("Test name");
	  pos_arr[0] = ts_pos;
	  pos_arr[1] = tn_pos;

	  errstatus |= testTable->setLookupKeys(2, pos_arr);
	  errstatus = errstatus << 1;

	  string key;
	  string binoveron;

	  for(size_t i = 0; i < tnames.size(); i++) {
		  key = tsname + ":" + tnames[i];
		  binoveron = testTable->getValue(key.c_str(), "Bin_overon");
		  if(debugmode == 1) {
			  cout << "Bin_overon=" << binoveron << " in test " << tnames[i] << " in suite " << tsname << endl;
		  }
		  if( binoveron != "no") {
				cerr << "Error: Bin_overon is not set to no. Please check test table. suite name=" << tsname << " test name=" << tnames[i] << endl;
				throw Error("Error", "Error in Bin_overon setting");
		  }
	  }
}

void updateTestTableIfNeeded(const TMLimits::LimitInfo & limit, int debugmode)
{
	string tsname = limit.SuiteName;
	string testname = limit.TestName;
//	ON_FIRST_INVOCATION_BEGIN();
	  /*
	   * check if the test name is available in test table
	   */
	  try {
		  const TMLimits::LimitInfo & rlimit = tmLimits.getLimitRef(tsname, testname);
	  } catch (Error & errobj){
		  // Error Handler Program
		  int ts_pos, tn_pos, pos_arr[2]; // field postion for test suite and test name
		  int errstatus = 0;
		  //TestTable* testTable = TestTable::getDefaultInstance();
		  TestTableMgr * ttablemgr = TestTableMgr::getDefaultInstance();
		  ttablemgr->refreshActiveTables();
		  TestTable* testTable = ttablemgr->getTestTable((unsigned int)0);

		  if(debugmode == 1) {
			  for(int j = 0; j < testTable->getFieldCount(); j++) {
				  cout << "field " << j << " is " << testTable->getFieldName(j) << endl;
			  }
		  }
		  ts_pos = testTable->getFieldPos("Suite name");
		  tn_pos = testTable->getFieldPos("Test name");
		  pos_arr[0] = ts_pos;
		  pos_arr[1] = tn_pos;

		  errstatus |= testTable->setLookupKeys(2, pos_arr);
		  errstatus = errstatus << 1;

		  string key = tsname + ":" + testname;

		  //if(debugmode) cout << "updateTestTableIfNeeded: key is " << key << endl;
		  testTable->addTest(key.c_str(), BOTTOM_ROW);
		  errstatus |= testTable->setValue(key.c_str(), "Lsl_typ", limit.LslTyp.c_str()); errstatus = errstatus << 1;
		  errstatus |= testTable->setValue(key.c_str(), "Lsl", to_string(limit.Lsl).c_str());  errstatus = errstatus << 1;
 		  errstatus |= testTable->setValue(key.c_str(), "Usl_typ", limit.UslTyp.c_str());errstatus = errstatus << 1;
 		  // set -999 to invoke error by default in case that the limits are not modified afterwards.
		  errstatus |= testTable->setValue(key.c_str(), "Usl", to_string(limit.Usl).c_str()); errstatus = errstatus << 1;
		  errstatus |= testTable->setValue(key.c_str(), "Units", limit.Units.c_str()); errstatus = errstatus << 1;
 		  errstatus |= testTable->setValue(key.c_str(), "Test Number", to_string(limit.TestNumber).c_str());errstatus = errstatus << 1;
		  errstatus |= testTable->setValue(key.c_str(), "Bin_s_num", to_string(limit.BinsNum).c_str());errstatus = errstatus << 1;
 		  errstatus |= testTable->setValue(key.c_str(), "Bin_s_name", limit.BinsName.c_str());errstatus = errstatus << 1;
 		  errstatus |= testTable->setValue(key.c_str(), "Bin_h_num", to_string(limit.BinhNum).c_str());errstatus = errstatus << 1;
 		  errstatus |= testTable->setValue(key.c_str(), "Bin_h_name", limit.BinhName.c_str());errstatus = errstatus << 1;
 		  errstatus |= testTable->setValue(key.c_str(), "Bin_type", "bad");errstatus = errstatus << 1;
 		  errstatus |= testTable->setValue(key.c_str(), "Bin_reprobe", "no");errstatus = errstatus << 1;
		  errstatus |= testTable->setValue(key.c_str(), "Bin_overon", "no");errstatus = errstatus << 1;

		  errstatus |= testTable->writeCsvFile(); errstatus = errstatus << 1;
 		  if(debugmode) cout << "updateTestTableIfNeeded: err status = " << errstatus << endl;
		  if(errstatus != 0xFFFE){
			  cerr << "updateTestTableIfNeeded: Error: error occurred when operating test table" << endl;
				throw Error("Error", "Error while updating test table");
		  }
		  tmLimits.load();  // load the generated test table items to the memory
	  }
	  // the clearDefaultInstance() is not needed. 2020-08-13;
	  // otherwise, only one test can be added in test table, because the test table instance is cleared.
	  //TestTable::clearDefaultInstance();   // not sure if it is needed.
//	ON_FIRST_INVOCATION_END();
return;
}

void updateTestTableIfNeeded(const string &tsname, const string &testname, int debugmode)
{

	TMLimits::LimitInfo limit;
	limit.SuiteName = tsname;
	limit.TestName = testname;

    string bin_name = tsname + "_" + testname + "_Fail";
	limit.BinsName = bin_name;
	limit.BinhName = "OTHERWISE_BIN";
	updateTestTableIfNeeded(limit, debugmode);

return;
}



int getVectorCount(const string &label)
{
	fwout.clear();
	fwout << "TSTL \"" << label << "\",;" << endl;  // select the label to edit
	fwout << "NODV?" << endl;  // get the number of vectors in the label; NODV returns the total vector count
	// NODV 11 // return example
	return fwresult[0].getIntParam(0);

}

double unitAsDouble(const string &unit)
{
	if(unit == "fA") return 1.000000e-15;
	else if(unit == "pA") return 1.000000e-12;
	else if(unit == "nA") return 1.000000e-09;
	else if(unit == "uA") return 1.000000e-06;
	else if(unit == "mA") return 1.000000e-03;
	else if(unit == "A")  return 1.0;
	else if(unit == "KA") return 1.000000e+03;
	else if(unit == "MA") return 1.000000e+06;
	else if(unit == "GA") return 1.000000e+09;
	else if(unit == "fdB") return 1.000000e-15;
	else if(unit == "pdB") return 1.000000e-12;
	else if(unit == "ndB") return 1.000000e-09;
	else if(unit == "udB") return 1.000000e-06;
	else if(unit == "mdB") return 1.000000e-03;
	else if(unit == "dB")  return 1.0;
	else if(unit == "KdB") return 1.000000e+03;
	else if(unit == "MdB") return 1.000000e+06;
	else if(unit == "GdB") return 1.000000e+09;
	else if(unit == "fHz") return 1.000000e-15;
	else if(unit == "pHz") return 1.000000e-12;
	else if(unit == "nHz") return 1.000000e-09;
	else if(unit == "uHz") return 1.000000e-06;
	else if(unit == "mHz") return 1.000000e-03;
	else if(unit == "Hz")  return 1.0;
	else if(unit == "KHz") return 1.000000e+03;
	else if(unit == "MHz") return 1.000000e+06;
	else if(unit == "GHz") return 1.000000e+09;
	else if(unit == "fsec") return 1.000000e-15;
	else if(unit == "psec") return 1.000000e-12;
	else if(unit == "nsec") return 1.000000e-09;
	else if(unit == "usec") return 1.000000e-06;
	else if(unit == "msec") return 1.000000e-03;
	else if(unit == "sec")  return 1.0;
	else if(unit == "Ksec") return 1.000000e+03;
	else if(unit == "Msec") return 1.000000e+06;
	else if(unit == "Gsec") return 1.000000e+09;
	else if(unit == "fV") return 1.000000e-15;
	else if(unit == "pV") return 1.000000e-12;
	else if(unit == "nV") return 1.000000e-09;
	else if(unit == "uV") return 1.000000e-06;
	else if(unit == "mV") return 1.000000e-03;
	else if(unit == "V")  return 1.0;
	else if(unit == "KV") return 1.000000e+03;
	else if(unit == "MV") return 1.000000e+06;
	else if(unit == "GV") return 1.000000e+09;
	else if(unit == "fOhm") return 1.000000e-15;
	else if(unit == "pOhm") return 1.000000e-12;
	else if(unit == "nOhm") return 1.000000e-09;
	else if(unit == "uOhm") return 1.000000e-06;
	else if(unit == "mOhm") return 1.000000e-03;
	else if(unit == "Ohm") return 1.0;
	else if(unit == "KOhm") return 1.000000e+03;
	else if(unit == "MOhm") return 1.000000e+06;
	else if(unit == "GOhm") return 1.000000e+09;
	else if(unit == "fs") return 1.000000e-15;
	else if(unit == "ps") return 1.000000e-12;
	else if(unit == "ns") return 1.000000e-09;
	else if(unit == "us") return 1.000000e-06;
	else if(unit == "ms") return 1.000000e-03;
	else if(unit == "s")  return 1.0;
	else if(unit == "Ks") return 1.000000e+03;
	else if(unit == "Ms") return 1.000000e+06;
	else if(unit == "Gs") return 1.000000e+09;
	else if(unit == "kA") return 1.000000e+03;
	else if(unit == "kdB") return 1.000000e+03;
	else if(unit == "kHz") return 1.000000e+03;
	else if(unit == "ksec") return 1.000000e+03;
	else if(unit == "kV") return 1.000000e+03;
	else if(unit == "kOhm") return 1.000000e+03;
	else if(unit == "ks") return 1.000000e+03;
	else if(unit == "fohm") return 1.000000e-15;
	else if(unit == "pohm") return 1.000000e-12;
	else if(unit == "nohm") return 1.000000e-09;
	else if(unit == "uohm") return 1.000000e-06;
	else if(unit == "mohm") return 1.000000e-03;
	else if(unit == "ohm") return 1.0;
	else if(unit == "Kohm") return 1.000000e+03;
	else if(unit == "Mohm") return 1.000000e+06;
	else if(unit == "Gohm") return 1.000000e+09;
	else if(unit == "dBm") return 1.0;
	else if(unit == "pts") return 1.0;
	else if(unit == "cnt")  return 1.0;
	else if(unit == "Vp")  return 1.0;
	else if(unit == "Vpk")  return 1.0;
	else if(unit == "Sps")  return 1.0;
	else if(unit == "bps")  return 1.0;
	else if(unit == "UI")  return 1.0;
	else if(unit == "IRE")  return 1.0;
	else if(unit == "rpm")  return 1.0;
	else if(unit == "W")  return 1.0;
	else if(unit == "mW") return 1.000000e-03;
	else if(unit == "uW") return 1.000000e-06;
	else if(unit == "pF") return 1.000000e-12;
	else if(unit == "pVs") return 1.000000e-12;
	else if(unit == "Vpp")  return 1.0;
	else if(unit == "Vrms")  return 1.0;
	else if(unit == "ppm") return 1.000000e-06;
	else if(unit == "pct")  return 1.0;
	else if(unit == "LSB")  return 1.0;
	else if(unit == "deg")  return 1.0;
	else if(unit == "rad")  return 1.0;
	else if(unit == "dBV")  return 1.0;
	else if(unit == "dBm0")  return 1.0;
	else if(unit == "dBc")  return 1.0;
	else if(unit == "dBr")  return 1.0;
	else if(unit == "dBrn")  return 1.0;
	else if(unit == "CODE")  return 1.0;
	else if(unit == "degC")  return 1.0;
	else if(unit == "degk")  return 1.0;
	else if(unit == "degF")  return 1.0;
	else if(unit == "uS") return 1.000000e-06;
	else if(unit == "mS") return 1.000000e-03;
	else if(unit == "S")  return 1.0;
	else if(unit == "pJ") return 1.000000e-12;
	else if(unit == "nJ") return 1.000000e-09;
	else if(unit == "uJ") return 1.000000e-06;
	else if(unit == "mJ") return 1.000000e-03;
	else if(unit == "J")  return 1.0;
	else if(unit == "Bps")  return 1.0;
	else if(unit == "KBps") return 1.000000e+03;
	else if(unit == "MBps") return 1.000000e+06;
	else if(unit == "GBps") return 1.000000e+09;
	else
	{
		cerr << "Error: in unitAsDouble(), unsupported unit: " << unit  << endl;
		throw Error("Error", "Error in converting string unit to double type");
		return -1e23;
	}
}

vector<string> getUsedPortsPrimaryLabel(int debug_mode)
{
	vector<string> vPorts;
	vPorts.clear();
    if( Primary.isTimingPortBased() || Primary.isLabelPortBased() )
    {
		string port;
		string pattern = Primary.getLabel();
		if(debug_mode == 1) cout << "pattern is " << pattern << endl;
		fwout.clear();
		fwresult.clear();
		fwout << "SQLB? \"" << pattern.c_str() <<  "\",ALL,(@@)" << endl;  // query ports
		// the fw command output
//			Task>  SQLB? "MPB_gephy_ldo_test_setup_0x327c0200",ALL,(@@)
//			SQLB "MPB_gephy_ldo_test_setup_0x327c0200",MPBU,123,125,,(pGEPHY_APB_PORT)
//			SQLB "MPB_gephy_ldo_test_setup_0x327c0200",MPBU,907,914,,(pGEPHY_MDC_PORT)
		for(unsigned int m = 0; m < fwresult.size(); m++)
		{
			if(debug_mode == 1) cout << "port is " << fwresult[m][5] << endl;
			port = fwresult[m][5];
			vPorts.push_back(port);
		}
    }
    else
    {
    	vPorts.push_back("@");
    }

    return vPorts;
}
