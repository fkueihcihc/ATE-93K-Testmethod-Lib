/*****************************************
  File: FW.h

   93K Firmware Interface
   Copyright (c) Verigy,
   Applications Developmentent Center, Semiconductor test
 
   $RCSfile: FW.h,v $
   $Revision: 1.3 $
   $Date: 2005/05/03 17:25:02 $
******************************************/
#include <string>
#include <vector>
#include <map>

#if __GNUC__ >= 3
#include <sstream>
#include <fstream>
using std::vector;
using std::string;
using std::map;
using std::stringstream;
using std::streampos;
using std::istream;
using std::ostream;
using std::streambuf;
#else
#include <strstream.h>
#include <fstream>
#define stringstream strstream
#endif

#include <iostream>
using std::cerr;

#ifndef FW_H
#define FW_H

//using namespace std;

//const unsigned int answ_buf_size = 0x100000;     // 1 megabyte 
//static char answ_buf[answ_buf_size];
//const unsigned int cmd_buf_size = 0x100000;     // 1 megabyte 
//static char cmd_buf[cmd_buf_size];

/** Firmware string container class
    
       Construct an object from a firwmware task result string 
       or from a file of soc firmware commands.
       
       type-safe access to fields through template function getParam<TYPE>
       or get string fields through operator []
       
 */

class BaseException {
	//--- protected constructor to prevent instance of this class ---
protected:
	BaseException(const char* type, const char* file, int line, const string& msg) {
		cerr << "\n\nA " << type << " Exception has been thrown\n"
			  << "\tin file " << file << " on line " << line
			  << "\n\n" << msg << "\n\n";
	}
};

class Exception : public BaseException {
public:
	Exception(const char* file, int line, const string& msg) :
		BaseException("Exception",file,line,msg) {}
};

class FWException : public BaseException {
public:
	FWException(const char* file, int line, const string& msg) :
		BaseException("FWExec Exception",file,line,msg) {}
};

class GPIBException : public BaseException {
public:
	GPIBException(const char* file, int line, const string& msg) :
		BaseException("GPIB Exception",file,line,msg) {}
};

class FWString {
 public:

    // construct object for file stream operations >>
    FWString():index(0),binlen(0),binaryindex(0) {
      command[0] = '\0';
    }

    ~FWString();
	 
    /// Construct from FW command string (reply).
    // Future: change vector elems to string* to avoid copying
    FWString(const string result);
	 
    /// wrap a FW result string with FWString interface
    const char *wrap(const char* result, char *dest = 0);

    const char* getCommand() const;

    /// return pointer to bin data
    streampos offset() const{
		 return binpos;
    }
	 
    inline unsigned int blength() const{
		 return binlen;
    }
	 
    inline const char* bdata() const{
		 return params[binaryindex].c_str();
    }

    int count() { return params.size();}

    /** Return firmware result parameter specified by index
		  @param index parameter index starting from 0
		  @return parameter as string
		  Will throw an exception if out of range.
    */
    const string& operator[](int index) const;
	 
    FWString& operator=(const string& src);
	 
    /** Return param value as type
		  double d = fwres.getParam<double>(1);
		  @param index parameter index starting from 0
		  return oarameter as specified type
    */
    template<typename T> T getParam(int pindex) const {
		 stringstream istr;
		 T data = T();
		 istr << params[pindex];
		 istr >> data;      // oh thank you strstream
		 return data;
    }
	 
    // aCC v1.23 doesn't support member template specialization
    // an efficient accessor for int and double params
    int getIntParam(int) const;
    double getDoubleParam(int) const;
	 
    friend istream& operator>>(istream& str, FWString& fw); 
    friend ostream& operator<<(ostream& str, const FWString& fw); 
	 
	 
private:
    vector<string> params;
    streampos binpos;  // track position of binary data in input stream;
    int index;
    const char* parseString(const string& result, char *dest=0);
    const char* parseString(const char *, char *dest=0);
    char command[5];
    mutable unsigned int binlen;      // destructive copy.
    int binaryindex; // param index for bin data (if any)
};

/** Replacement for streambuf
    Send our formatted stream to MCD
*/
class fwstreambuf: public streambuf {
 public:
    fwstreambuf();
    ~fwstreambuf();
 private:
    void      put_buffer(void);
    
 protected:
    int       overflow(int);
    int       sync();
};

int fwexec(const char* fwcmd, int len, vector<FWString> &fwvec);

/** Execute a firmware query
    Execute the given command and place formatted result into the 2nd parameter.
    example:
  
    FWResult fwres;
    fwexec("FTST?",fwres);
    if(fwres[0] == "P") { // test passed}

    @param fwcmd firmware query string
    @param fwres firmware result string
    @param bindest optional pointer to pre-allocated binary data destination
*/
int fwexec(const string& fwcmd, FWString &fwres, char *bindest);
/** Execute a multiple result firmware query
    Execute the given command and place formatted results into the 2nd parameter.
    example:
  
    vector<FWResult> fwres;
    fwexec("DFPN?",fwres);
    for(vector<FWResult>::iterator itr = fwres.begin();itr!=fwres.end();itr++) {
      cout " pin " << (*itr)[2] << " at channelNo " << (*itr)[0] << endl;
    }

    @param fwcmd firmware query string
    @param fwres firmware result string
    @param bindest optional pointer to pre-allocated binary data destination
*/
int fwexec(const string& fwcmd, vector<FWString> &fwvec);
int fwexec(const string& fwcmd);
/// Execute a command without a result
//int fwexecbin(const char* fwcmd,int len);
//int fwexec(const char* fwcmd,int len = -1);
//int fwexec(const string& fwcmd);

class fwstream : public ostream {
public:
private:
    fwstream():ostream(fwbuf = new fwstreambuf()) {
    }
    ~fwstream() { 
        delete fwbuf; }
    friend fwstream& getfwstream();

    fwstreambuf *fwbuf;
};

// wrapper for binary user data
struct FWBinData {
	const char *data;
	int size;
	FWBinData(const char*d, int s):data(d),size(s) {}
	//FWBinData(const vector<int> v):data(d),size(s) {}
};

ostream& operator<<(ostream& s, const FWBinData& d);

fwstream& getfwstream();

extern vector<FWString> fwresult;
extern fwstream& fwout;
extern int fwloglevel;
extern map <string, string> fwAnswerMap;

#endif
