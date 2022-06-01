#include "TestMethod.h"
#include <vector>
#include <string>
#include <iterator>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "TestTableMgr.hpp"
using namespace std;
using namespace V93kLimits;

#define SITE_NUMBER 8


DOUBLE Frequency(ARRAY_I temp_data, double Test_Period,int fft_mod);
/*
 * func: split a string into vectors according to the splitter
 * param1: the string
 * param2: the ref to vector of string to hold the splitted string
 * param3: the splitter, like ",", ":", etc; space not supported
 */
void splitString2Vector(const string & str, vector<string> & strvec, const string splitter);
/*
 * func: if the test suite and test name are not available in the test table,
 *  	 then update the test table with the test suite name and test name as key; *
 *  Attention: the limit is set to 0 by default; the bin name and bin number is set to default value too;
 *   	  so please change the limit and bin name/number in the test table manually if needed
 * param1: test suite name; normally the 1st column of the test table
 * param2: test name; normally the 2nd column name of the test table
 * parma3: debug mode; 1=output debug information; 0=no output
 */
void updateTestTableIfNeeded(const TMLimits::LimitInfo & limit, int debugmode = 0);
void updateTestTableIfNeeded(const string &tsname, const string &testname, int debugmode = 0);

void checkBin_overonOff(const string &tsname, const vector<string> &tnames, int debugmode = 0);

/*
 * func: return the count of vector of the pattern
 * param: label name of the pattern
 */
int getVectorCount(const string &label);

/*
 * func: return the double value of the unit in string format
 * param: unit in string format
 * example: unitAsDouble("mA")
 */
double unitAsDouble(const string &unit);

/*
 * func: invert int/double/long data to string
 * param: int variable
 * example: int2string(10l)
 */
string to_string(int n);
string to_string(double n);
string to_string(long n);


vector<string> getUsedPortsPrimaryLabel(int debug_mode=0);   // returns the used ports in the running suite (the ports of the primary label
/*
 * func: auto-create serial digital capture variable
 * param: int samples,int bitsPerWord,string vCap,string cap_pin,string mylabel
 * example: GenSeriDigCapVar(1,32,"cap1","RO_RI","pm_chain")
 */
void GenSeriDigCapVar(int samples,int bitsPerWord,string vCap,string cap_pin,string mylabel);

/*
 * func: reverse raw 32-bit data,from MSB to LSB
 * param: register unsigned int
 * example: reverse(capData[i])
 */
unsigned int reverse(register unsigned int x);

/*
 * func: cut 32-bit data into 4 bytes
 * param: unsigned int
 * example: Word2Byte(tmpWord)
 */
ARRAY_I Word2Byte(unsigned int inData);

extern int efuse_data[SITE_NUMBER][87];
extern unsigned short crc2[SITE_NUMBER];
extern unsigned int chip_id32[SITE_NUMBER];
