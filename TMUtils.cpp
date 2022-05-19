/*****************************************************************************
*
* Verigy, Inc. Company Confidential
* (c) Copyright |2007|, Verigy, Inc., all rights reserved.
* No part of this source code may be duplicated or distributed in any form.
*
******************************************************************************/
/**
  \file
  \brief This is a collection of commonly needed functions for application development
  \remarks Need to insert examples of how to use the functions and classes in this module
*/
#include <vector>
#include <cstdio>
#include <execinfo.h>
#include <sstream>
#include <iostream>

#include <sys/utsname.h>
#include <unistd.h>

#include "TMRex.h"
#include "TMUtils.h"
#include "TMMsgStream.h"
using namespace std;

#include "libcicpi.h"

/**
A definition of the production executor process
*/
#define PROD_EXEC "/opt/hp93000/soc/prod_env/bin/HPSmarTest"

namespace TMFormat {

	/**
	   Convenence class for fomratting doubles
	   \code
	   TMFormat::DblFormat p3(3); // create dbl formatting object for 3 digits of precision
	   cerr << " value = " << p3(3.4599999) << endl;
	   cerr << " value = " << p3(2.9999999) << endl;
	   \endcode
	 */
	struct DblFormat {
		/// Precision of the value
		int prec;
		/// The double value
		double value;
		/// Format the result based on precision expected
		DblFormat(int p):prec(p) {}
		/// Operator for the data type
		DblFormat& operator()(double d) {
			value = d;
			return *this;
		}
	};
	
	/**
	  Stream operator for the DblFormat type
	*/
	ostream& operator<<(ostream& os, DblFormat& d) {
		os.setf(ios::fixed,ios::floatfield);
		os.precision(d.prec);
		return os << d.value;
	}

	// ex: cout << p8(1.12) << endl;
};


/**
*  A default exception handler implementation to provide more meaningful error message from TM code
*/
TMUtilsInit& tmUtilities = TMUtilsInit::initTMUtils();

/**
You can add additional error messages for functions in the TM API bu adding lines to this array:
*/
const char* defaultExceptionMessages[] = {"GET_USER_STRING","Check if string exists in testflow user variable list.",
										  "OTHER","Try something else ...",
										  "END"};

TMUtilsInit::TMUtilsInit() {
	int i=0;
	while(strncmp(defaultExceptionMessages[i],"END",3) != 0) {
		diagmap[defaultExceptionMessages[i]] = defaultExceptionMessages[i+1];
		i+=2;
	}
	//SET_EXCEPTION_HANDLER(TM::ALL,TMDefaultHandler,NULL);
	//int level;
	//GET_USER_FLAG("dbg_level",&level);
}
void TMUtilsInit::disableExceptionHandler() {
	SET_EXCEPTION_HANDLER(TM::ALL,TM::NONE);
}	
void TMUtilsInit::enableExceptionHandler() {
	SET_EXCEPTION_HANDLER(TM::ALL,TMDefaultHandler,NULL);
}

#if __GNUC__ >= 3
///////////////////////////////////////////////////////////////////////////////////////////
// types and defs for stack trace info
const int MAXTRACE=16; // maximum depth of stack trace
const int handler_level = 4; // number of exception handler calls - throw away

// store stack trace symbolic info (paths, demangled function names, line numbers)
struct SymbolInfo {
	string filename;
	string function;
	int lineno;  // not working at the moment
};

/*
  Get a stack trace at runtime - cool! This works only on Linux
  This is currently only defined locally to this .cpp file. Should it be converted to be globally accessible?
  \remarks Insert an example of usage here
*/

void getStackTrace(vector<SymbolInfo>& symbols) {
    char ** syms;
    static void * addrs[MAXTRACE];
    int num = backtrace(addrs,16);
    syms = backtrace_symbols(addrs, num);
	 SymbolInfo nextsym;
    for(int i=0;i<num;i++) {
		string mname; // the mangled function name
		//int offset;
        string line = syms[i];
		//cerr << "[" << line << "]" << endl;
        FXRex rex;    
        int beg[5],end[5];
        rex.parse("([^(]+)\\((\\w+)\\+(\\w+)",REX_CAPTURE);
        if(rex.match(line,beg,end,REX_FORWARD,5)) {
            nextsym.filename = line.substr(beg[1],end[1]-beg[0]);
            mname = line.substr(beg[2],end[2]-beg[2]);
			istringstream str(line.substr(beg[3],end[3]-beg[3]));
			str >> hex >> nextsym.lineno;
			
			// we need to demangle the function name
			vector<string> lv;
			execCmd(string("c++filt ") + mname,lv);
			if(lv.size()) {
				nextsym.function = lv[0];
			}
			
			// clean up some common verbose std lib types
			nextsym.function = global_replace(nextsym.function,"std::basic_string<char, std::char_traits<char>, std::allocator<char> >","std::string");
			symbols.push_back(nextsym);
        }
    }
    free(syms);
}
#else
/** 
No stack trace for HPUX
*/
void getStackTrace(vector<SymbolInfo>& symbols) {} 
#endif

/**
*  Default TM exception handler
*/
TM::EXIT_TYPE TMDefaultHandler(const EXCEPTION_INFO& einfo, void* userarg) {
	vector<SymbolInfo> syms;
	 getStackTrace(syms);
	 if(syms.size() < 4) return  TM::RETURN_TO_FLOW;
	 cerr << "------------- TM Exception " << einfo.getName() << " ----------------------" << endl;
	 cerr << "Description: " << einfo.getMessage() << endl;
	 cerr << tmUtilities.diagmap[einfo.getName()] << endl;
	 cerr << "Call trace:" << endl;
	 cerr << syms[handler_level].function << endl;
	 for( int i=handler_level+1;i<(int)syms.size(); i++) {
		 if(syms[i].function.find("::Invoke") != string::npos) { i = syms.size(); break;} // stop at TM invocation level
		 cerr << "     called from " << syms[i].function << endl;
	 }
	 cerr << "--------------------------------------------------------------------" << endl;;
    return TM::RETURN_TO_FLOW;
}

/**
* Static initialization of the class and single instance generation
*/
TMUtilsInit& TMUtilsInit::initTMUtils() {
	static TMUtilsInit object;
	return object;
}

/********************************
 TimeSTamp members
*********************************/
const char* TimeStamp::months[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
//  tm struct
//        int tm_sec;      /* seconds after the minute - [0,61] */
//           int tm_min;      /* minutes after the hour - [0,59] */
//           int tm_hour;     /* hours - [0,23] */
//           int tm_mday;     /* day of month - [1,31] */
//           int tm_mon;      /* month of year - [0,11] */
//           int tm_year;     /* years since 1900 */
//           int tm_wday;     /* days since Sunday - [0,6] */
//           int tm_yday;     /* days since January 1 - [0,365] */
//           int tm_isdst;    /* daylight savings time flag */
//

/**
Stream operator for the TimeStamp data type for output
*/
ostream& operator<<(ostream& o,const TimeStamp& ts) {
        return o;
}

void TimeStamp::getTime() {
        struct timeval tv;
        gettimeofday(&tv,&tz);
        millisecs = tv.tv_usec/1000;
        //PG 2005-10-30
        unix_time_sec = tv.tv_sec;
        localtime_r(&tv.tv_sec,&timestruct);
}

void TimeStamp::reset() {
        getTime();
}

string TimeStamp::toDateString() {
	if(timestruct.tm_year==0) { getTime(); }
	sprintf(formattedTime,"%.4d-%.2d-%.2d",
		1900 + timestruct.tm_year,timestruct.tm_mon+1,timestruct.tm_mday);
	return formattedTime;

}
string TimeStamp::toTimeString() {
	if(timestruct.tm_year==0) { getTime(); }
	/* modified by Kevin Zhu/Huatek
	 * sprintf(formattedTime,"%.2d:%.2d:%.2d.%.3d",
	 */
	sprintf(formattedTime,"%.2d:%.2d:%.2d.%.3ld",
		timestruct.tm_hour,timestruct.tm_min,timestruct.tm_sec,millisecs);
	return formattedTime;
}

string TimeStamp::toString() {
        // format time with unix time and local time
        // ex: 1130696328.60 2005-10-30 10:18:48.060
        if(timestruct.tm_year==0) { getTime(); }
	/* modified by Kevin Zhu/Huatek
         * sprintf(formattedTime,"%ld.%.3d %.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.3d",
	 */
	sprintf(formattedTime,"%ld.%.3ld %.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.3ld",
                 unix_time_sec, millisecs,
                1900 + timestruct.tm_year,timestruct.tm_mon+1,timestruct.tm_mday,
                timestruct.tm_hour,timestruct.tm_min,timestruct.tm_sec,millisecs);
        //PG 2005-10-30
        return formattedTime;

}
// Additional format option for easier logging
string TimeStamp::shortString() {
        if(timestruct.tm_year==0) { getTime(); }
	/* modified by Kevin Zhu/Huatek
         * sprintf(formattedTime,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.3d",
	 */
	 sprintf(formattedTime,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.3ld",
                1900 + timestruct.tm_year,timestruct.tm_mon+1,timestruct.tm_mday,
                timestruct.tm_hour,timestruct.tm_min,timestruct.tm_sec,millisecs);
        return formattedTime;

}

/**
*  Execute an external command and place output lines in vector
*/
int execCmd(const string & command, vector<string> & vector_of_lines ) {

   string line;
   FILE *mypipe;
   char buffer[1024];
	buffer[0] = '\0';
   vector_of_lines.clear();
   string pipe_command = command + " 2<&1";
   mypipe = popen(command.c_str(),"r");
   if ( mypipe == NULL ) {
      return -1;
   }
   while (!feof(mypipe)) {
     fgets(buffer,1024,mypipe);
     buffer[strlen(buffer)-1] = '\0'; // remove newline
     vector_of_lines.push_back(buffer);
   }
   int error_code = pclose(mypipe);
   //remove last element in the list. extra empty line not needed.
   vector_of_lines.pop_back();

   return error_code;
}

/**
* Replace every instance of match in source with replace
*/
string global_replace(const string& source, const string& match, const string& replace)
{
    string res = source;
    string::size_type pos;
	string::iterator s_itr,e_itr;
	while((pos = res.find(match)) != string::npos) {
		s_itr = res.begin()+pos;
		e_itr = s_itr + match.size();
		res.replace(s_itr,e_itr,replace);
	}
    return res;
}

/**
* Prompt the user to click a button in a UI
*/
long promptUser(const string& msg, const string& b2, const string& b3, const string& b4, const string& b5, const string& b6, const string& b7) {
	long response;
   	long status;
	status = MsgDisplayMsgGetResponse(
		const_cast<char*>(msg.c_str()),
		const_cast<char*>(b2.c_str()),
		const_cast<char*>(b3.c_str()),
		const_cast<char*>(b4.c_str()),
		const_cast<char*>(b5.c_str()),
		const_cast<char*>(b6.c_str()),
		const_cast<char*>(b7.c_str()),
		&response);
	
	return response;
}

/**
* Prompt the user for a string in a UI
*/
string promptUserString(const string& msg, const string& b2, const string& b3, const string& b4, const string& b5, const string& b6, const string& b7) {
	long response;
	long status;
	char buf[ CI_MAX_MESSGE_STR_LEN + 1 ];
	status = MsgDisplayMsgGetStringResponse(
		const_cast<char*>(msg.c_str()),
		const_cast<char*>(b2.c_str()),
		const_cast<char*>(b3.c_str()),
		const_cast<char*>(b4.c_str()),
		const_cast<char*>(b5.c_str()),
		const_cast<char*>(b6.c_str()),
		const_cast<char*>(b7.c_str()),
		&response,
		buf);
	return buf;
}

/**
Used for buffere manipulation based on OS settings
*/
#define DIGITS (sizeof(unsigned long) * CHAR_BIT)

/**
* Unsigned Long to String conversion
*/
string ultostr(unsigned long num, int base) {
	int digit; 
	int i=0; 
	int j=0; 
	int last_pos_buffer;
	char str[DIGITS];  
	char buffer[DIGITS]; 
	
	do {
		digit = (int)(num % base);
		num  /= base;
		
		if (digit > 9)
			digit += 'A' - 10;
		else
			digit += '0'; 
		
		/* digits written in this buffer will be reversed later */
		buffer[i++]=digit; 
		
	} while (num != 0);   
	
	/* copy buffer starting at last position i to returned string 
	   since we need to reverse the characters*/ 
	last_pos_buffer = i - 1;
	for(j=0;j<i;j++)
		str[j]=buffer[last_pos_buffer - j];
    
	str[j]='\0';
	
	return str;
 
}

/**
* Long to String conversion
*/
string ltostr(long num, int base) {
	int digit; 
	int i=0; 
	int j=0; 
	int last_pos_buffer;
	char str[DIGITS];  
	char buffer[DIGITS]; 
	
	do {
		digit = (int)(num % base);
		num  /= base;
		
		if (digit > 9)
			digit += 'A' - 10;
		else
			digit += '0'; 
		
		/* digits written in this buffer will be reversed later */
		buffer[i++]=digit; 
		
	} while (num != 0);   
	
	/* copy buffer starting at last position i to returned string 
	   since we need to reverse the characters*/ 
	last_pos_buffer = i - 1;
	for(j=0;j<i;j++)
		str[j]=buffer[last_pos_buffer - j];
    
	str[j]='\0';
	
	return str;
 
}

/**
* Return the fully qualified path of the current executing Device
*/
string deviceRelativePath(const string& filename) {
	char pgmDir[DM_MAX_DEVICE];
	tpi::GetDevPath(pgmDir);
	return pgmDir + string("/") + filename;
}

/**
* Tokenize full template specialization (string)
*/
// template <>
//void tokenize(const string& source, vector<string>& dest, const string& delimiter) {
//	dest.clear();
//	string::size_type lastPos = source.find_first_not_of(delimiter, 0);    //find first non delim char
//	string::size_type     pos = source.find_first_of(delimiter,lastPos);   //find the next delim after token
//
//	if ( string::npos == pos ) {
//		dest.push_back(source);
//	} else {
//		while (string::npos != pos || string::npos != lastPos)  {            //search for additional tokens
//			dest.push_back(source.substr(lastPos, pos - lastPos));     //found token, save in vector
//			lastPos = source.find_first_not_of(delimiter, pos);                 //find another non delim char
//			pos = source.find_first_of(delimiter,lastPos);                      //find the next delim after token
//		}   
//	}
//}

////////////////////////////////////////////////////
// stream inserters


/**
Stream extractor, complex inserter is already defined in "COMPLEX.h" - format is "(r,i)"
*/
istream& operator>>(istream& is, COMPLEX& val) {
	is .ignore(); // skip parens
	is >> val.real(); 
	is.ignore(); // skip comma
	is >> val.imag();
	is.ignore(); // skip parens
	return is;
}

//////////////////////////////////////////////////
////////////Dump system infomation
//////////////////////////////////////////////////

///Get smatest revision infor
const string GetSmartestRev()
{
    ostringstream ostr;
    
    char buffer[1024];
    string rev;
    vector<string> vector1;
    vector<string> vector2;
    
    ostr << "/usr/bin/strings "<< PROD_EXEC << " | grep \'Production s/w rev.\' 2<&1" <<ends;
    
    FILE *pipe = popen(ostr.str().c_str(), "r");
    if (pipe != NULL)
    {
        while ( !feof(pipe))
        {
            fgets(buffer,1024,pipe);
            rev = buffer;
            tokenize(rev, vector1,",");
            tokenize(vector1[0], vector2, " ");
            rev = vector2[vector2.size() - 1];
        }
    }
    pclose(pipe);
    return rev;
}

void getSysInfo()
{
	struct utsname u_name;
	uname(&u_name);
	const string strOSVersion(u_name.version);
	const string strSysName(u_name.sysname);
    const string strRelease(u_name.release);
	tminfo1 << "OS Version: " << strSysName << strRelease << " " << strOSVersion <<" "<< endl;
    char hostname[255];
	gethostname(hostname,255); // zero if success
	const string strHostName(hostname);
	tminfo1 <<"Host Name: " << strHostName <<endl;
	
    char* pFlag = "CI_OPTIMIZED_MODE";
	long lvalue = 0;
		
	int iRet = GetTestflowFlag(pFlag, &lvalue);
	if (0 == iRet)
    {
		const string strValue = lvalue > 0 ? "YES" : "NO"; 
		tminfo1 << "Test flow optimized: "<< strValue << endl;
    }
	else if (1 == iRet)
	{
		tmerr << "Wrong testflow flag name specified!" << endl;
	}
	else 
	{
		tmwarn << "Failed to get testflow optimized flag!" << endl;
	}
    const string strSmartestRev = GetSmartestRev();
    tminfo1 <<"Smartest Reversion: " << strSmartestRev<< endl;
	 loginfo1.flush();
    return ;
}

int parseListofInt(vector<INT>& listVector, STRING list){

   int pos, pos2;
   string delims = ";";
   string tmp, x_check;
   pos = list.find_first_not_of(delims, 0);
   int n_of_data = 0;
   while (pos >= 0)
   {
        pos2 = list.find_first_of(delims, pos);
        if (pos2 < 0) pos2 = list.length();
        tmp = list.substr(pos, pos2-pos);
        x_check = list.substr(pos+1, 1);
        int tmpInt;

        if ( x_check == "x" || x_check == "X" ) {
            tmpInt=strtol(tmp.c_str(), (char **)NULL, 16);
        } else {
        	tmpInt=strtol(tmp.c_str(), (char **)NULL, 10);
        }

        listVector.push_back(tmpInt);
        pos = list.find_first_not_of(delims, pos2);
        n_of_data++;
   }
   return (n_of_data);
}

