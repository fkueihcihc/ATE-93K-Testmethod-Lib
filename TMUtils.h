/*****************************************************************************
*
* Verigy, Inc. Company Confidential
* (c) Copyright |2007|, Verigy, Inc., all rights reserved.
* No part of this source code may be duplicated or distributed in any form.
*
******************************************************************************/
/**
  \file
  TMUtils is a collection of common functions needed in application development.
  \remarks   Please consult the documentation below for specific information on each of the functions
	     available. In general each of these is a stand alone function that can be called within
	     the application code.

<b>Getting Started:</b>

Make sure your Test Method project is properly configured:
- \subpage integration "Integration into an application"
- \subpage makefile "Makefile specific details"

You need to insert the following lines into your source code:
\code
#include "TMUtils.h"
\endcode
After this, reference the specific documentation on the function and use in your application code.
*/

#include "TestMethod.h"
#include <string>
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <iterator>
#include <iostream>
#include <fstream>

#ifndef TM_UTILS_I
#define TM_UTILS_I

using std::vector;
using std::string;
using std::istringstream;
/**
Internal us name definition
*/
typedef suseconds_t usectype;

namespace tpi {
#include "/opt/hp93000/soc/pws/lib/tpi_c.h"
}

//******************************************************************************
// Random collection of utility functinos
//******************************************************************************
/**
 * Dump the system information to the log fle
 * The information contains OS version, SW version, Workstation Model, Test flow
 * Optimized, HostName, Number of executions,and so on
 */
void getSysInfo();

/** 
 * Make a system call and place each line from stdout/stderr into vector.
 * Example:
\code
vector<string> files;
execCmd("ls -l /tmp",files);
for(int i=0;i<files.size();i++) { cout << files[i] << endl;}
\endcode
 */
int execCmd(const string & command, vector<string> & vector_of_lines );

/* replace every occurance of match with replace
 * Example:
\code
string tmp = "/opt/hp9300 /etc/hp93000 /var/hp93000";
global_replace(tmp,"hp","verigy");
// tmp now equals "/opt/verigy9300 /etc/verigy93000 /var/verigy93000"
\endcode
 */
string global_replace(const string& source, const string& match, const string& replace);

/**
 * Break input string into tokens by delimiter. This will attempt to extract tokens of type T.
 * Push results into vector of T.
 * Example 1:
\code
string input = "unoi dos tres quatorze";
vector<string> words;
tokenize(input,words);
\endcode
 * Example 2:
\code
string csv_line = "1.2, 3.007, 29.55, 9.99";
vector<double> vals;
tokenize(csv_line,vals,",");
\endcode
*/
 template <class T>
 void tokenize(const string& source, vector<T>& dest, const string& delimiter = " ");

// legacy functions (from hpux)
string ltostr(long num, int base);
string ultostr(unsigned long num, int base);
	 
/**
* A function that will throw a simple UI waiting for user input to click on the box. This
* is useful for stopping execution of TM code at a specific point to allow the person to
* perform an action and then click the button when the flow can be executed further
*
* 1st parameter is message
* other params are optional. Up to 6 button labels in addition to default "OK"
*/
long promptUser(const string& msg, const string& b2="", const string& b3="", const string& b4="", const string& b5="", const string& b6="", const string& b7="");

/**
* A function that will throw a UI for the user to enter some information. This
* is useful for having the user enter a file name, for example.
*
* same as above but returns user input string
*/
string promptUserString(const string& msg, const string& b2="", const string& b3="", const string& b4="", const string& b5="", const string& b6="", const string& b7="");


// Declare default handler for TM exceptions
TM::EXIT_TYPE TMDefaultHandler(const EXCEPTION_INFO& einfo, void* userarg);

/**
  \brief Class for static init of TMUtil resources
  \remarks  Explanation and examples of the class usage here
  
  Example:
\code
tmUtilities.enableExceptionHandler();

.... your code ....

tmUtilities.disableExceptionHandler();
\endcode
  
*/
class TMUtilsInit {
public:
	/**
	  Singleton accessor method to ensure we get one and only one copy of the object
	*/
	static TMUtilsInit& initTMUtils();
	/**
	  Brian what is this?
	*/
	map<string,string> diagmap;
	/**
	  Turn off the exception handler
	*/
	void disableExceptionHandler();
	/**
	  Turn on the exception handler
	*/
	void enableExceptionHandler();
private:
	TMUtilsInit();
	~TMUtilsInit() {}
};

#if __GNUC__ >= 3
typedef   std::stringstream  STD_SSTREAM;
#else
/**
Wrappers for string streams - provides std::stringstream interface for hpux
example:
\code
STD_SSTREAM strm;
strm << "formatted text " << ends;
cout << strm.str();
\endcode
*/
class SafeStrStream : public  strstream {
public:
	SafeStrStream() {}
	~SafeStrStream() {
	}
	/** Bad things happen when an empty literal is used:
	    strstream << "";
	    adding a terminator seems to fix it.
	*/
	string str() {
		*this << ends;
		string stmp = strstream::str();
		rdbuf()->freeze(false);
		return stmp;
	}
};  // SafeStrStream
/**
Internal stream definition name
*/
typedef   SafeStrStream    STD_SSTREAM;
#endif

/**
* Class for creating a formatted timestamp string
* example:
\code
cerr << TimeStamp().toString() << endl;
\endcode
*/
class TimeStamp {
public:
	/**
	  Create a time stamp from a timeval struct
	*/
	TimeStamp(struct timeval tv):millisecs(0) {localtime_r(&tv.tv_sec,&timestruct);}
	/**
	  Create a time stamp from a time struct
	*/
	TimeStamp(time_t time):millisecs(0) {localtime_r(&time,&timestruct);}
	/**
	  Create a time stamp with the getTime() function call
	*/
	TimeStamp():millisecs(0) {getTime();}
	/**
	* Resets the class so that all stamps from this point forward starts at zero
	*/
	void reset();
	friend ostream& operator<<(ostream&,const TimeStamp&);
	/**
	* Converts the object to a string for output into a standard stream such as cout
	*/
	string toString();
	/**
	* Converts the object to a date formatted string
	*/
	string toDateString();
	/**
	* Converts the object to a time formatted string
	*/
	string toTimeString();
	/**
	* Converts the object to a compacted string
	*/
	string shortString();
	/**
	* Stores the names of the months
	*/
	static const char* months[12];
private:
	char formattedTime[256];
	void getTime();
	struct tm timestruct;
	usectype millisecs;
	struct timezone tz;
	//PG 2005-10-30
	unsigned long unix_time_sec;
};

/**
 stream inserters
*/
ostream& operator<<(ostream&,const TimeStamp&);

/**
 stream xtractors
*/
istream& operator>>(istream& is, COMPLEX& val);

extern TMUtilsInit& tmUtilities;

/**
* A simple function to get the base path of the current executing device directory
* and then append the desired filename to it so that it ends up as a fully qualified
* path + filename.
*/
string deviceRelativePath(const string& filename);

/**
* C++ template for generically writing SmarTest data types such as ARRAY_D to
* a file.
*
* Works for ARRAY_I,ARRAY_D, ARRAY_COMPLEX, vector<T>, and
* any type that has a ::size method, operator[] and element ostream<<.
*
* Does not work for C arrays. You shouldn't use them anyway.
* Example:
\code
ARRAY_D data;
writeArrayToFile(data,deviceRelativePath("rxdata/dat1.txt"))
...
ARRAY_D inarray(SIZE);
readArrayFromFile(inarray,deviceRelativePath("rxdata/dat1.txt"));
\endcode
*/
template <class T>
int writeArrayToFile(const T& a, const string& path) {
	ofstream fout;
	fout.open(path.c_str(),ios::out);

	if(!fout) {
		cerr << " Unable to write to data target storage file : " << path << endl;
		return -1;
	}
	// Copy the array to file
	for(int i=0; i<a.size(); i++) {
		fout  << a[i] << " ";
	}

	return 0;
}

/**
* C++ template for generically reading SmarTest data types such as ARRAY_D from
* a file.
*
* Works for ARRAY_I,ARRAY_D, ARRAY_COMPLEX, vector<T>, and
* any type that has a ::size method, operator[] and element ostream<<.
*
* Does not work for C arrays. You shouldn't use them anyway.
* Example:
\code
ARRAY_D data;
writeArrayToFile(data,deviceRelativePath("rxdata/dat1.txt"))
...
ARRAY_D inarray(SIZE);
readArrayFromFile(inarray,deviceRelativePath("rxdata/dat1.txt"));
\endcode
*/
template <class T>
int readArrayFromFile(T& a, const string& path) {
	if (a.size() >= 1) {
		ifstream fin;
		fin.open(path.c_str(),ios::in);
		
		if(!fin) {
			cerr << " Online data target storage file : " << path << "not found" << endl;
			return -1;
		}
		
		for(int i=0; (i<a.size()) && (fin) ; i++) {
			fin  >> a[i];
		}
	}
	return 0;

}

template <class T>
void tokenize(const string& source, vector<T>& dest, const string& delimiter) {
	dest.clear();
	string::size_type lastPos = source.find_first_not_of(delimiter, 0);    //find first non delim char
	string::size_type     pos = source.find_first_of(delimiter,lastPos);   //find the next delim after token
	
	istringstream istr;
	T val;	
	
	if ( string::npos == pos ) {
		istr.clear();
		istr.str(source);
		istr >> val;
		dest.push_back(val);		
	} else {
		while (string::npos != pos || string::npos != lastPos)  {            //search for additional tokens
			istr.clear();
			istr.str(source.substr(lastPos, pos - lastPos));
			istr >> val;
			dest.push_back(val);     //found token, save in vector
			lastPos = source.find_first_not_of(delimiter, pos);                 //find another non delim char
			pos = source.find_first_of(delimiter,lastPos);                      //find the next delim after token
		}     
	}
}

// full specialization for string
//template <>
//void tokenize(const string& source, vector<string>& dest, const string& delimiter);

#endif

int parseListofInt(vector<INT>& listVector, STRING list);
