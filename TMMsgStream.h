/*****************************************************************************
*
* Verigy, Inc. Company Confidential
* (c) Copyright |2007|, Verigy, Inc., all rights reserved.
* No part of this source code may be duplicated or distributed in any form.
*
******************************************************************************/
/*****************************************
   $RCSfile$
   $Author: bburas $
   $Revision: 240 $
   $Date: 2008-08-13 11:26:16 +0200 (Wed, 13 Aug 2008) $
******************************************/

#ifndef MSGSTREAM_H
#define MSGSTREAM_H

// Required by non smarTest files
//#include "tml_base.h"
//extern CComModule _Module;
//#include "tml_tcom.h"

#include <vector>
#include <map>

#include "TestMethod.h"
#include "libcicpi.h"
#include <string>

#if __GNUC__ >= 3
#include <iostream>
#include <sstream>
#include <fstream>
#include <streambuf>
using std::string;
using std::streampos;
using std::ostream;
using std::streambuf;

using std::vector;
using std::map;
typedef std::istringstream istrstream;

typedef ostream ostreamtype;
#else
#include <iostream.h>
#include <strstream.h>
#include <fstream.h>
#define stringstream strstream
typedef ostream_withassign ostreamtype;
#endif

///@cond
class TMLog;

//**************************************************************
// misc log stream utilities
string getFormattedTime(const string& fmt ="%h:%m:%s.%u" );

// track # of info streams with same file stream target
struct FileStreamElement {
	ofstream* fstr;
	int refcount;
};

//structures
 struct timeStamp
 {
	string testSuite;	
	struct timeval time;
	double elapsedTime;
	timeStamp(const string& o, struct timeval m, double e):testSuite(o),
	time(m),
	elapsedTime(e) {}
};
typedef vector<struct timeStamp> vecTimeStamp;

//**************************************************************
// streambuf for our log streams
//**************************************************************
class logstreambuf : public streambuf {
 public:
	logstreambuf(ostream*, int bufsize, const string& format);
	~logstreambuf();
	friend class TMMsgStream;
	void setFlags(long int f) { flags=f;}
	void stamp(); 
private:
	void put_buffer(void);
	void put_char(int);
	void output(const char*, int);
	string prefix;
	ostream* forwardstr;
	timeval last_tv; // track for deltas

/* Added by Wei Qian / Lavey, Aug-31-2007 */
	vecTimeStamp timeStamps;
	void storeStamp(const struct timeval& current, double elapsedTime);

protected:
	int overflow(int);
	int sync();
	long int flags;
	string formatstr; // log format
	string tsformatstr; // timestamp format
	const char *file;
	const char *methodname;
	int line;
	
};
///@endcond


extern logstreambuf *lb;

#if __GNUC__ >= 3
extern ostream logstr;
#else
extern ostream_withassign logstr;
#endif

int initDebugLog(ostream& f, int level, int flags=0);
int closeDebugLog();


/**
  \brief  A stream operator class like cout but with many added benefits and capabilities:
- Disable/enable streams with SmarTest flags (default mode) debug_mode and debug_analog
- Redirect streams from the UI window to a file, etc without modifying your code
- Flexibile formatting of output without need to modify your code. Include info such as source file, line number, time stamps and more

\remarks It is quite common to embed debug output messages in your code. But to control the output to be on in a debug mode requires additional lines of code and test flow variables. The following is an "old" way of doing this:

\code
if (VERBOSE > 10) {
	cout << "My debug message" << endl;
}
\endcode

Would become this as the "new" way:

\code
tminfo1 << "My debug message" << endl;
\endcode

<b>Getting Started:</b>

Make sure your Test Method project is properly configured:
- \subpage integration "Integration into an application"
- \subpage makefile "Makefile specific details"

You need to insert the following lines into your source code:
\code
#include "TMMsgStream.h"
\endcode

In addition to using the streams in your code, you need to setup the output formatting of the streams at the start of execution of the test flow. In the "device"/TestMethod/support_lib project dir you will find a class MsgStream that has a method format that will allow you to setup each stream from the test flow.

<b>Test flow flags</b>

By default the streams output is controlled by a combination of debug_analog and debug_mode:

\verbatim
Test flow flag	State	    Purpose
--------------	-------	    ------------------------------
debug_analog	enabled	    Streams can now be output
		disabled    All streams are turned off except tmerr

debug_mode	0	    Output tmwarn
		1	    Additionally output tminfo1
		2	    Additionally output tminfo2
		3	    Additionally output tminfo3
		4	    Additionally output tminfo4
\endverbatim

Optionally you can control the level of output from a Test Flow variable instead. If you define a variable with the name "info_level" in your flow then it will be read to determine which levels should be output. The value of the variable can be between 1-5.

<b>Short background on C++ streams</b>

The familiar cout is actually a global object provided by the C++ standard library that implements a stream interface. By default, the final target of all operations done on cout is the unix stdout file descriptor. In our case this goes to the UI report window. To be more specific the stream object is responsible mostly for formatting the data to a buffer, a stream buffer. It is the stream buffer that writes the collected characters to a target at the appropriate time (flushes the stream).

We want to create a stream that provides:
- Easy redirection to files or other destinations
- Ability to add formatting implicitly
- Turn off without affecting test execution time

<b>Basics of the class</b>

The TMMsgStream class provides a wrapper to add format settings, redirection and enable/disable options to the tminfo streams. You can use these features to adjust runtime messages to suit the appropriate stage in the test program development cycle.
- Create a default message format for easier collaboration and output processing.
- Provide verbose output for low level debugging.
- Redirect to file or buffer for performance profiling with some verbosity.
- Completely disable all message streams for full performance release.

The msg stream module provides 6 levels of messages corresponding to 6 global stream objects. The global streams are tminfo1-tminfo4, tmwarn and tmerr. The error stream tmerr cannot be disabled, it is meant to be seen by a production operator.

\verbatim
Type		Stream 		Usage
--------	------		------------------------------------------------------------
Errors		tmerr		Always enabled, output will generate an error if found
Warnings	tmwarn		Not enough to halt the test, but a message operator should see
Level 1		tminfo1		Basic debug information, eg Test Suite Name
Level 2		tminfo2		More Detailed information, Intermediate Output Information for example
Level 3		tminfo3		More Detailed information: Input settings for example
Level 4		tminfo4		Code developer detailed information: Function Call info, looping info,
				the end user will probably never use this level.
\endverbatim

<b>Stream formatting</b>

One of the advantages of using a stream buffer is the ability to format the stream without modifying the
C++ code outputing the message. With a single call to the streams .format() method, the output can be
customized to output additional information. The format can be controlled on a specific stream as well
providing the flexibility to be different between each stream. The format options are as follows:

General format codes
\verbatim
Format Code	Description
-----------	-----------------------------
%t		timestamp
%c		timestamp delta (in millisecs) from last timestamp
%i		current site number
%n		test number (from system flag ~Stest_number~T)
%s		test suite name
%f		source code file name
%l		source code line number
%m		the message itself
\endverbatim

Timestamp format codes
\verbatim
Format Code	Description
-----------	-----------------------------
%d		day of week (Mon, Tue ...)
%D		day of month (1-31)
%o		month (Jan, Feb ...)
%O		month (1-12)
%h or %H	hour (1-12)
%m or %M	minute (0-59)
%s or %S	seconds (0-59)
%u		microseconds
%i		milliseconds
\endverbatim

<b>Example usage</b>
\code
// include the msg streams
#include "TMMsgStream.h"

// use default format and default target (cerr)
tminfo1 << "started testmethod" << endl;
tmerr << "failed top open file" << endl;

// retarget to log file
loginfo1.setTarget("/tmp/prod1.txt");
tminfo1 << "now writing to file" << endl;

// set format to print timestamp (enclosed in []'s) then testsuite name, then message:
loginfo1.setFormat("[%t] %s %m");
tnminfo1 << "see new format" << endl;

// set timestamp format to show hour:minut:seconds
loginfo1.setTimestampFormat("%h:%m:%s");

// set timestamp format to show seconds.microseconds
loginfo1.setTimestampFormat("%s.%u");
\endcode
*/
class TMMsgStream {
public:
	/**
	  Constructor requires a stream operator, the number of the stream, and a default format
	*/
	TMMsgStream(ostream& forwardstr, int l, const string& format="%m");
	~TMMsgStream();

	/**
	  Flush all contents of the stream immediately
	*/
	void flush();

	/**
	Enables a stream
\code
loginfo3.enable();
\endcode
	*/
	void enable();

	/**
	Disables a stream from being output, supressing ALL stream processing overhead
\code
loginfo3.disable();
\endcode
	*/
	void disable();

	// remove this later - client should be responsible for supplying stream
	/**
	Set the output of the specific stream to a different output location, specifically a file
\code
loginfo1.setTarget("/tmp/myLog.txt","a");
\endcode

	\param pathname		The path of the file including the name of the file as well
	\param mode		Optional, write mode by default, "a" for append mode
	*/
	void setTarget(const string& pathname, const char* mode = "w");

	/**
	Set the output to an existing output stream
\code
loginfo1.setTarget(cerr);
\endcode

	\param ostr	An output stream such as cout, cerr or one that is user defined
	*/
	void setTarget(ostream& ostr);

	/**
	Define the format of the output stream to a custom setting of your choice
\code
logerr.setFormat("*** ERROR *** File: %f Line: %l Message: %m");
\endcode

	\param fmt	A format string. Reference the table in the remarks for available options
	*/
	void setFormat(const string& fmt);

	/**
	Define the format of the time stamp output to a specific type. This is used in conjunction with
	the %c format setting if defined in the .setFormat() method call.
\code
loginfo4.setTimestampFormat("Day: %d, Month: %o");
\endcode

	\param fmt	A format string. Reference the table in the remarks for available options
	*/
	void setTimestampFormat(const string& fmt);

	/**
	Returns the format string defined for the stream.
\code
string currentFormat = loginfo4.getFormat();

//use the same format on the other streams
loginfo1.setFormat(currentFormat);
\endcode
	*/
	string getFormat();

	/**
	Returns the timsetamp format string defined for the stream.
\code
string timeFormat = loginfo4.getTimestampFormat();

//use the same format on the other streams
loginfo1.setTimestampFormat(timeFormat);
\endcode
	*/
	string getTimestampFormat();

	/**
	  Gets the stream operator for this instance of the object
	*/
	inline ostream& getStreamRef(const char* file, int line, const char* methodname) {
		/*
		  string tsn;
		  INT ts_override_level;
		  GET_TESTSUITE_NAME(tsn);
		  GET_TESTSUITE_FLAG(tsn,"Test_level",&ts_override_level);
		  if(ts_override_level > level) { enable();}
		  else {
		  INT lvl;
		  GET_USER_FLAG("dbg_level",&lvl);
		  if(lvl < level) { disable();}
		  }
		*/
		lb->file = file;
		lb->line = line;
		lb->methodname = methodname;
		init();
		return *str;
	}

	/**
	Initializes the stream, specifically the time stamp delta creation so that it starts from zero
	at this point forward.
	*/
	void init();

	/**
	Brian, what is the difference between reset() and init() ?
	*/
	static void reset();

/**
	Reset the timestamp
\code
loginfo3.stamp();
\endcode
	*/
	void stamp();

	/**
	Summarize the time stamp delta to target output stream. It must be invoked
	after stamp(). 
\code
loginfo3.summary();
\endcode
	*/
    void summary();

private:
	TMMsgStream() {}
	void closetargetfile(); // close old target stream when changing target
	void set_target(ostream& ostr);
	void first_init();
	
	int flags;
	int level;
	bool bInitialized;
	logstreambuf *lb;
	ostreamtype* str;
	string filepath;
};

extern TMMsgStream loginfo4;
extern TMMsgStream loginfo3;
extern TMMsgStream loginfo2;
extern TMMsgStream loginfo1;
extern TMMsgStream logwarn;
extern TMMsgStream logerr;

#define tminfo4 loginfo4.getStreamRef(__FILE__,__LINE__,__PRETTY_FUNCTION__)
#define tminfo3 loginfo3.getStreamRef(__FILE__,__LINE__,__PRETTY_FUNCTION__)
#define tminfo2 loginfo2.getStreamRef(__FILE__,__LINE__,__PRETTY_FUNCTION__)
#define tminfo1 loginfo1.getStreamRef(__FILE__,__LINE__,__PRETTY_FUNCTION__)
#define tmwarn logwarn.getStreamRef(__FILE__,__LINE__,__PRETTY_FUNCTION__)
#define tmerr logerr.getStreamRef(__FILE__,__LINE__,__PRETTY_FUNCTION__)

void initTMStreams(STRING& info1_fmt, STRING& info2_fmt, STRING& info3_fmt, STRING& info4_fmt, STRING& warn_fmt, STRING& error_fmt, INT outputToFile, STRING& outputFileName );

#endif
