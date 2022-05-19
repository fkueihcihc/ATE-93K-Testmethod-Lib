/*****************************************
   Copyright (c) Agilent Technologies,
   Applications Development Center, 2005

   $RCSfile$
   $Author: bburas $
   $Revision: 240 $
   $Date: 2008-08-13 11:26:16 +0200 (Wed, 13 Aug 2008) $
******************************************/

/***************************************************

****************************************************/
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sstream>
#include <fstream>
#include <iomanip>


using namespace std;

#include "TMMsgStream.h"
extern long DisableErrorMessages(void);
extern long EnableErrorMessages(void);

// map of file stream targets
static map<string,FileStreamElement> fstrmap;

static const string TMMSG_REVISION = "3.02";

/*****************************************************************************
 You can define new streams or change existing streams here
 intput parameters:
   1: target stream (this can be a file stream or anything w/stream interface
   2: minimum debug level to activate the stream (0 = always on)
   3: optional format string define as such:

      %t : timestamp in form of hh:mm:ss.mmm   (hour,min,sec,millisecs)
      %d : datestamp in form of YYYY-MM-DD
      %n : test number (retrieved from system flag "test_number")
      %s : test suite name
      %f : source code file name from which message originated
      %l : source code line from which message originated
      %c : timestamp delta from last msg in us.
      %p : name of class and method
      %i : current site number
      %m : the log message itself

    Example:
      Print the timestamp inside brackets, then the testsuite name then a colon and the message
      "[%t] %s : %m"

    Default:
      If you don't specify anything the default is "%m" which just prints the message

  You can also modify the logs from your TM code using the TMMsgStream class wrappers (see below).
   Effects last for as long as the flow is open
    Eample:
      loginfo1.setTarget(new ofstream("/path/to/my/log.txt")); // change target of tminfo1

      logerr.setFormat("*************** ERROR! in %f at line %l **************\n %m ");

      loginfo2.disable(); // force stream off

******************************************************************************/

// wrappers for our log streams - set project defaults here
TMMsgStream loginfo4(cerr, 5, "%s [%i]       %m [%c]" );
TMMsgStream loginfo3(cerr, 4, "%s [%i]     %m [%c]");
TMMsgStream loginfo2(cerr, 3, "%s [%i]   %m [%c]");
TMMsgStream loginfo1(cerr, 2, "%s [%i] %m [%c]");
TMMsgStream logwarn(cerr,  1, "Warning : %s, %f %l: %m");
TMMsgStream logerr(cerr,   0, "Error : %s, %f %l: %m");


/*****************************************
  Put timestamp stuff here for now
******************************************/

// get secs and usecs since epoch
inline struct timeval getrawtime() {
    /* deleted by Kevin Zhu/Huatek 
	 * char res[256];
	 */
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv,&tz);
	return tv;
}
const char* months[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
string formatStamp(struct timeval& tv, const string& fmt="%h:%m:%s.%u") {
	ostringstream ftime;
	struct tm timestruct;
	struct timezone tz;

	gettimeofday(&tv,&tz);
	
	localtime_r(&tv.tv_sec,&timestruct);

	int mode=0;
	/* modified by Kevin Zhu/Huatek
	 * for(int i=0;i<fmt.length();i++) {
	 */
	for(int i=0;i<(int)fmt.length();i++) {
		if(mode==0) {
			if(fmt[i] == '%') {
				mode = 1;
			}
			else {
				ftime << fmt[i];
			}
		}
		else if(mode==1) {
			switch (fmt[i]) {
				case '%': ftime << '%' ; break;
				case 'd': ftime << timestruct.tm_wday; break;
				case 'D': ftime << timestruct.tm_mday; break;
				case 'o': ftime << timestruct.tm_mon; break;
				case 'O': ftime << months[timestruct.tm_mon]; break;
				case 'h': ftime << setfill('0') << setw(2) << timestruct.tm_hour; break;
				case 'H': ftime << setfill('0') << setw(2) << timestruct.tm_hour; break;
				case 'm': ftime << setfill('0') << setw(2) << timestruct.tm_min; break;
				case 'M': ftime << setfill('0') << setw(2) << timestruct.tm_min; break;
				case 's': ftime << setfill('0') << setw(2) << timestruct.tm_sec; break;
				case 'S': ftime << setfill('0') << setw(2) << timestruct.tm_sec; break;
				case 'u': ftime << setfill('0') << setw(6) << tv.tv_usec; break;
				case 'i': ftime << setfill('0') << setw(3) << int(tv.tv_usec/1000); break;
				default:
					cerr << "Warning: Bad timestamp format string - don't know param %" << fmt[i] << " " << endl;
			}
			mode = 0;
					
		}
		
	}
	return ftime.str();
}

string getFormattedTime(const string& fmt) {
	struct timeval tv;
	tv = getrawtime();
	return formatStamp(tv,fmt);
}

inline long compute_delta(struct timeval& last, struct timeval& current) {
	 if (last.tv_usec > current.tv_usec) {
      		current.tv_usec += 1000000;
       		current.tv_sec--;
  	 }	  
	return	(current.tv_sec - last.tv_sec) * 1000000 + (current.tv_usec - last.tv_usec);
}


///@cond

//*********************************************************************
// custom stream buffer implementation

logstreambuf::logstreambuf(ostream* f, int bufsize, const string& fmt) : streambuf(),forwardstr(f),formatstr(fmt) {
	if(epptr() - pbase()) { delete[] pbase(); }
	if(bufsize) {
		char *ptr = new char[bufsize];
		setp(ptr, ptr + bufsize);   // set output buffer to statically alloced cmd buf
	}
	else {
		setp(0,0); // unbuffered
	}
	setg(0, 0, 0); // no input buffer
	tsformatstr = "%h:%m:%s.%u"; // default
}

logstreambuf::~logstreambuf() {
	sync();
	delete[] pbase();
}

int logstreambuf::overflow(int c) {
	put_buffer();
	if (c != EOF) {
		if (pbase() == epptr())
			put_char(c);
		else
			sputc(c);
	}        
	return 0;
}

// flush
int logstreambuf::sync() {
	put_buffer();
	return 0;
}

// set the last timestamp to current
void logstreambuf::stamp() {
	last_tv =  getrawtime();
	return ;
}

void logstreambuf::storeStamp(const struct timeval& current, double elapsedTime)
// snap shot the current time
{
	// extract the test name  from the system
    string testSuite;
    GET_TESTSUITE_NAME(testSuite);

	timeStamps.push_back( timeStamp::timeStamp(testSuite, current, elapsedTime));
	
	return;
}

// parse user format string and write buffer to target
void logstreambuf::output(const char *data, int len) {
	if(len==0) return;
	string tsn;
	INT tn;
	int mode=0;
	int insertln=0;
	double delta;
	struct timeval cur_tv;
	/* modified by Kevin Zhu/Huatek 
	 * for(int i=0;i<formatstr.length();i++) {
	 */
	for(int i=0;i<(int)formatstr.length();i++) {
		if(mode==0) {
			if(formatstr[i] == '%') {
				mode = 1;
			}
			else {
				(*forwardstr) << formatstr[i];
			}
		}
		else if(mode==1) {
			switch (formatstr[i]) {
				case '%': (*forwardstr) << '%' ;break;
				case 't': (*forwardstr) << getFormattedTime(tsformatstr); break;
				case 'i': (*forwardstr) << CURRENT_SITE_NUMBER(); break;
				case 'r':
					cur_tv = getrawtime();
					delta = (double)compute_delta(last_tv, cur_tv)/1000.0;
					(*forwardstr) << fixed << setprecision(2) << delta; 
					storeStamp(cur_tv, delta);
					last_tv = cur_tv;
					break;
				case 'c':
					cur_tv = getrawtime();
					delta = (double)compute_delta(last_tv, cur_tv)/1000.0;
					(*forwardstr) << fixed << setprecision(2) << delta; 
					storeStamp(cur_tv, delta);
					last_tv = cur_tv;
					break;
				case 'n':
					GET_SYSTEM_FLAG("test_number",&tn);
					(*forwardstr) << tn;
					break;
				case 's':
					GET_TESTSUITE_NAME(tsn);
					(*forwardstr) << tsn;
					break;
				case 'p':
					(*forwardstr) << methodname;
					break;
				case 'f':
					(*forwardstr) << file;
					break;
				case 'l':
					(*forwardstr) << line;
					break;
				case 'm':
					if(data[len-1] == '\n') { // don't place the newline until the end
						forwardstr->write(data,len-1);
						insertln=1;
					}
					else {
						forwardstr->write(data,len);
					}
					break;
				default:
					cerr << "Warning: Bad TMMsgStream format string - don't know param %" << formatstr[i] << " " << endl;
			}
			mode = 0;
					
		}
		
	}
	if(insertln) (*forwardstr) << "\n";
};


void logstreambuf::put_char(int chr) {
	char x[2];
	x[0] = chr;
	output(x,1);
}

void logstreambuf::put_buffer() {
	if (pbase() != pptr()) {
		int len = (pptr() - pbase());
		output(pbase(),len);
		setp(pbase(), epptr());
	}
	
}

///@endcond

///////////////////////////////////////////////////////
// TMMsgStream class methods
///////////////////////////////////////////////////////

TMMsgStream::TMMsgStream(ostream& forwardstr, int l, const string& format):level(l) {
	lb = new logstreambuf(&forwardstr, 4096, format);
	str = new ostreamtype(lb);
	bInitialized = false;
//	init();
}

TMMsgStream::~TMMsgStream() {
	if( bInitialized )
		closetargetfile();
}

void TMMsgStream::first_init() {
	if( !bInitialized )
	{
		bInitialized = true;
		init();
	}
}

void TMMsgStream::init() {
	enable();
	INT lvl;
	INT dbga;
	try {
		DisableErrorMessages();
		GET_USER_FLAG("info_level",&lvl);
		EnableErrorMessages();
		if(level > (lvl+1)) { disable();}
	} catch(Error& err) {
		//	cerr << "user flag \"info_level\" not found - reverting to debug_analog and debug_mode" << endl;
		GET_TESTFLOW_FLAG("debug_mode",&lvl);
		GET_TESTFLOW_FLAG("debug_analog",&dbga);
		if(dbga) {
			if(level > (lvl+1)) { disable();}
		}
		else {
			if(level > 0) {disable();}
		}
	}
}

/*void TMMsgStream::stamp() {
	lb->stamp();
}*/

void TMMsgStream::flush() {
	first_init();
	lb->forwardstr->flush();
}

void TMMsgStream::enable() {
	first_init();
	str->clear();
}

void TMMsgStream::disable() {
	first_init();
	str->clear(ios::failbit);
	//	cerr << "level " << level << " is disabled" << endl;
}

void TMMsgStream::setTarget(const string& pathname, const char* mode) {
	first_init();
	
	closetargetfile();
	// new file target
	if(fstrmap.find(pathname) == fstrmap.end()) { 
		ofstream *f;
		if (mode[0] == 'w') {
			f =  new ofstream(pathname.c_str());
		}
		else if (mode[0] == 'a') {
			f =  new ofstream(pathname.c_str(),ios::app);
		}
		else {
			cerr << "TMMsgSTream ERROR: invalid target file mode: " << mode << endl;
			return;
		}
		if(!f) {
			cerr << "TMMsgStream ERROR: TMMsgStream unable to open file " << pathname << " for writing" << endl;
			return;
		}
		FileStreamElement el;
		el.fstr = f;
		el.refcount = 1;
		fstrmap[pathname] = el;
	}
	else { // alread exists in map
		fstrmap[pathname].refcount++;
	}
	filepath = pathname;
	set_target(*(fstrmap[pathname].fstr));

}

void TMMsgStream::closetargetfile() {
	first_init();

	// remove old file target is exists
	if(filepath.size()) {
		FileStreamElement& el = fstrmap[filepath];
		el.refcount--;
		if(el.refcount == 0) {
			el.fstr->flush();
			delete el.fstr;
			fstrmap.erase(filepath);
		}
		filepath="";
	}
}

void TMMsgStream::setTarget(ostream& ostr) {
	first_init();

	closetargetfile();
	set_target(ostr);
}

void TMMsgStream::set_target(ostream& ostr) {
	first_init();

	lb->forwardstr = &ostr;
}

void TMMsgStream::setFormat(const string& fmt) {
	first_init();

	lb->formatstr = fmt;
}

void TMMsgStream::setTimestampFormat(const string& fmt) {
	first_init();

	lb->tsformatstr = fmt;
}

string TMMsgStream::getFormat() {
	first_init();

	return lb->formatstr;
}

string TMMsgStream::getTimestampFormat() {
	first_init();

	return lb->tsformatstr;
}

void TMMsgStream::reset() {

	loginfo4.init();
	loginfo3.init();
	loginfo2.init();
	loginfo1.init();
	logwarn.init();
	logerr.init();
}

void TMMsgStream::stamp() {
	first_init();

	//clear the history value
	lb->timeStamps.clear();
	
	// snap shot the current time
	lb->stamp();
}

void  TMMsgStream::summary()
{
	first_init();

	ostream* ostr = lb->forwardstr;
	*ostr << "TestSuite, level, elapsedTime"<<endl;
	
	for(vecTimeStamp::iterator iter = lb->timeStamps.begin(); iter != lb->timeStamps.end(); iter++)
	{
		struct timeStamp tmStamp = *iter;
		(*ostr) << tmStamp.testSuite  << ", ";
		(*ostr) << level-1 << ", ";
		(*ostr) << tmStamp.elapsedTime << " ms"<<endl;
	}
	return;
}


void initTMStreams(STRING& info1_fmt, STRING& info2_fmt, STRING& info3_fmt, STRING& info4_fmt, STRING& warn_fmt, STRING& error_fmt, INT outputToFile, STRING& outputFileName )
{
    loginfo4.setFormat(info4_fmt);
    loginfo3.setFormat(info3_fmt);
    loginfo2.setFormat(info2_fmt);
    loginfo1.setFormat(info1_fmt);
    logwarn.setFormat(warn_fmt);
    logerr.setFormat(error_fmt);

    if (outputToFile) {
        loginfo1.setTarget(outputFileName,"a");
        loginfo2.setTarget(outputFileName);
        loginfo3.setTarget(outputFileName);
        loginfo4.setTarget(outputFileName);   
    }
    
	loginfo1.stamp();
	loginfo2.stamp();
	loginfo3.stamp();
	loginfo4.stamp();

}

