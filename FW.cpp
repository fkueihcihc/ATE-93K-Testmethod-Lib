/*****************************************************************************
*
* Agilent Technologies Company Confidential
* (c) Copyright |2005|, Agilent Technologies, Inc., all rights reserved.
* No part of this source code may be duplicated or distributed in any form.
*
******************************************************************************/
/****************************************************************************/
/**
  \file	
  \brief 	93K Firmware Interface
  \author	Brian Buras
  \author	$Author: bburas $
  \version	$Revision: 1.3 $
  \remarks  This routine is used to safely and quickly communicate with the firmware
interface.

to sent a command to the firmware interface use the fwout command
similar to cout.
i.e.
       fwout << "DFGP? (" << pinName << ")" << endl;
		
the result is returned in the vector fwresult

For example the the following test method code

fwout << "DFPG? (IN_PINS_PULLUP)" << endl;
cout << "Input pins are = " << fwresult[0][1] << endl;
for (int i=0;i<fwresult.size();++i) {
      for (int j=0;j<fwresult[i].size();j++) {
            cout << "fwresult[" << i << "][" << j <<"] = \"" << fwresult[i][j] << "\"\n";
      }
}

Would result in the following output

Input pins are EXT_POR_L,TCK,TMS,TDI,JTAG_TRST_L,CODEC_RGDT_L,CODEC_SDI
fwresult[0][0] = "I"
fwresult[0][1] = "EXT_POR_L,TCK,TMS,TDI,JTAG_TRST_L,CODEC_RGDT_L,CODEC_SDI"
fwresult[0][2] = "IN_PINS_PULLUP"
fwresult[1][0] = "O"
fwresult[1][1] = "EXT_POR_L,TCK,TMS,TDI,JTAG_TRST_L,CODEC_RGDT_L,CODEC_SDI"
fwresult[1][2] = "IN_PINS_PULLUP"

Binary Data:
Binary data is formatted just like any other param
// print first 28 waveform indices
fwout << "VECD? PARA,VM,0,28,(CP)\n";
for(j=0;j<fwresult[i][5].size();j++) {
    cerr << "[" << fwresult[i][5][j]+0 << "]";
}

You can send binary data using the FWBinData wrapper:
// assume char *testdat2 points to something
char bindata[256];
// put stuff into bindata
fwout << "VECD PARA,VM,0,28,(CP)," << FWBinData(bindata,28) << endl;

Firmware Answer Emulation:
For offline work that requires real data, you can map certain fw
commands to a file. fwresult will return the contents of the file.

fwAnswerMap["VECD? PARA,VM,0,28,(@)\n"] = "/tmp/VECD.bin";
where "/tmp/VECD.bin" was created by using the following command:
$ /opt/hp93000/soc/fw/bin/hpt -e "VECD? PARA,VM,0,28,(@)" >  /tmp/VECD.bin

Parse Vector File Example:
loadSOCFile(fstream fin) {
    FWString fw;
    while(!fin.eof()) {
        fin >> fw;
        if(fw.command == "VECC") {
          cout << "found " << fw.length() << " bytes at " << fw.offset() << endl;
          fin.seekg(fw.offset());
          fin.getc(mybuffer,fw.length());
        }
    }
}

	\date	$Date: 2005/05/03 17:25:02 $
  \bug
  \xmlonly
	<type>support</type>
	<bundle>Firmware Interface</bundle>
	<module>Support Code</module>
  \endxmlonly

 */
/****************************************************************************/
#include <string>
#include <vector>
#include <map>

#if __GNUC__ >= 3
#include <sstream>
#include <fstream>
using namespace std;
#else
#include <strstream.h>
#include <fstream>
#define stringstream strstream
#define ostringstream ostrstream
#endif


#include "FW.h"

// place macro-abusing tpi interface in its own namespace to protect the rest of the code.
namespace tpi {
#include "/opt/hp93000/soc/pws/lib/tpi_c.h"
}


// Globals
// We should probably put this in a namespace.
// tpi_c and MAPI have all kinds of junk polluting the global namespace so why not.

vector<FWString> fwresult;
fwstream& fwout = getfwstream();
int fwloglevel =0;
map <string, string> fwAnswerMap;

fwstream& getfwstream() {
	static fwstream obj;
	return obj;
}

// not expecting a response
int fwexec(const char* fwcmd, int len) {
	vector<FWString> fwvec(0); // no pre-alloc
	return fwexec(fwcmd,len,fwvec);	
}

int fwexec(const string& fwcmd) {
	vector<FWString> fwvec(0); // no pre-alloc
	return fwexec(fwcmd.c_str(),fwcmd.size(),fwvec);
}

// FW_TASK does not properly return binary data. We use FwTask() here.
//int fwexec(const string& fwcmd, FWString &fwres, char *dest) {
//    return fwexec(fwcmd.c_str(),fwres,dest);
//}
int fwexec(const string& fwcmd, vector<FWString> &fwvec) {
    return fwexec(fwcmd.c_str(),fwcmd.size(),fwvec);
}

// multi-response
int fwexec(const char* fwcmd, int len, vector<FWString> &fwvec) {
	tpi::INT32 task_len = len;
	tpi::INT32 answ_len = answ_buf_size;
	tpi::INT32 retval = 0;
	const char *tmp = answ_buf;


	if(fwloglevel) {
		cerr.write(fwcmd,len);
	}

	// Add offline vector data emulation
	// We shouldn't see a performance hit -
	// For an empty map, find() will return immediately
	map <string, string>::iterator i_map = fwAnswerMap.find(fwcmd);
	if(i_map != fwAnswerMap.end()){
		//emulation mode - found answer to emulate
		if(fwloglevel) cout << "Offline Mode: Emulating answer for " << fwcmd << endl;
		fstream fwFile(i_map->second.c_str(), ios::in | ios::binary);
		answ_len = 0;
		while(fwFile.get(answ_buf[answ_len])){
			answ_len++;
		}
		fwFile.close();
		retval = 0;
	}
	else {
		// Why does HpFwTask take char* instead of const char*?
		tpi::HpFwTask(const_cast<char*>(fwcmd),&task_len,answ_buf,&answ_len,&retval);
	}
	answ_buf[answ_len]='\0';
	if(retval < -1) {
		ostringstream msg;
		msg << "ERROR: bad FW command (err code " << retval << "): " << fwcmd << ends;
		cerr << msg.str();
		return 1;
		//throw FWException(__FILE__,__LINE__,msg.str());
	}
	if(retval == 0) {
	}
	if(retval == -1) {
		cerr << "Warning recieved from soc fw interface for " << fwcmd << endl;
	}
	if(fwloglevel) {
		cerr.write(tmp,answ_len);
	}
	while(tmp < answ_buf + answ_len) {
		FWString fwres;
		tmp =  fwres.wrap(tmp);
		fwvec.push_back(fwres);
	}
	return 0;
}

int fwexec(const string& fwcmd, FWString &fwres, char *dest) {
	tpi::INT32 task_len = fwcmd.size();
	tpi::INT32 answ_len = answ_buf_size;
	tpi::INT32 retval = 0;
	
	// dangerous but trust me :)
	//fwlog << "FW:" << fwcmd;
	if(fwloglevel) {
		cerr << fwcmd << endl;
	}
	tpi::HpFwTask(const_cast<char*>(fwcmd.c_str()),&task_len,answ_buf,&answ_len,&retval);
	answ_buf[answ_len]='\0';
	if(retval == 0) {
		if(fwloglevel) {
			cerr << answ_buf << endl;
		}
		fwres.wrap(answ_buf,dest);
		return 0;
	}
	if(retval == -1) {
		cerr << "Warning recieved from soc fw interface for " << fwcmd << endl;
		fwres.wrap(answ_buf,dest);
		return 0;
	}
	if(retval < -1) {
		ostringstream msg;
		msg << "bad FW command (err code " << retval << "): " << fwcmd << ends;
		throw FWException(__FILE__,__LINE__,msg.str());
	}
	
	return 1;
}

//******************************************************************************
// FWString members
//******************************************************************************

FWString::FWString(const string result):index(0) {
	parseString(result);
}
//FWString::FWString(const char* result,unsigned int len):index(0) {
//    parseString(result);
//}

FWString::~FWString() {
}

// wrap a FW result string with FWString interface
const char *FWString::wrap(const char* result, char *dest) {
    return parseString(result,dest);
}

const char* FWString::getCommand() const {
    return command;
}

const string& FWString::operator[](int index) const {
	if(index < (int)params.size()) {
		return params[index];
	}
	else throw FWException(__FILE__,__LINE__,"Requested firmware param index out of range");
}

int FWString::getIntParam(int pindex=0) const {
	return atoi(params[pindex].c_str());
}

double FWString::getDoubleParam(int pindex=0) const {
	return atof(params[pindex].c_str());
}

FWString& FWString::operator=(const string& rhs) {
	parseString(rhs);
	return *this;
}

ostream& operator<< (ostream& str, const FWString& fw) {
	str << fw.command << " ";
	for(int i=0;i < (int)fw.params.size() ;i++) {
		str << fw.params[i] << ",";
	}
	return str;
}
istream& operator>> (istream& str, FWString& fw) { 
	string tmp;
	int in_quote=0;
	int in_parens=0;
	string param;
	unsigned char ch;
	
	str >> fw.command;
	fw.command[4] = '\0';
	
	fw.params.clear();
	
	while(!str.eof()) {
		ch = str.get();
		if(in_quote==1) {
			if(ch == '"') {
				in_quote = 0;
			}
			else {
				param += ch;
			}
		}
		else if(in_parens==1) {
			if(ch == ')') {
				in_parens=0;
			}
			else {
				param += ch;
			}
		}
		else if(ch == ',') {
			fw.params.push_back(param);
			param.resize(0);
		}
		else if(ch == '"') {
			in_quote=1;
		}
		else if(ch == '(') {
			in_parens=1;
		}
		else if(ch == '\n') {
			break;
		}
		else if(ch == '#') {  // if binary, we know its the last param. quickly copy the binary data
			char lenvalwid;
			
			lenvalwid = str.get() -48;
			fw.binlen=0;
			for(int i=0; i<lenvalwid; i++) {
				fw.binlen = fw.binlen*10;
				fw.binlen += str.get()-48;
			}
			fw.binpos = str.tellg();
			//str.seekg(str.tellg()+fw.binlen);
			str.seekg(ios::end);
		}
		else {
			param += ch;
		}
	}
	fw.params.push_back(param); // last parameter
	return str;
}

const char* FWString::parseString(const string& result, char *dest) {
	return parseString(result.c_str(),dest);
};

// reads FW result buffer up to 1st newline
const char* FWString::parseString(const char *result, char *dest) {
	int in_quote=0;
	int in_parens=0;
	const char *pos = result;
	bool done = false;
	string param;
	
	params.clear();
	
	strncpy(command, result, 4);
	command[4] = '\0';
	pos+=4;
	while((pos++)&&(!done)) {
		if(*pos == '\n') {  // multi-line FW response.
			params.push_back(param);
			done = true;
		}
		else if(in_quote==1) {
			if(*pos == '"') {
				in_quote = 0;
			}
			else {
				param += *pos;
			}
		}
		else if(in_parens==1) {
			if(*pos == ')') {
				in_parens=0;
			}
			else {
				param += *pos;
			}
		}
		else if(*pos == ',') {
			params.push_back(param);
			param.resize(0);
		}
		else if(*pos == '"') {
			in_quote=1;
		}
		else if(*pos == '(') {
			in_parens=1;
		}
		else if(*pos == '#') {  // if binary, we know its the last param. quickly copy the binary data
			pos++;
			char lenvalwid = *(pos++)-48;
			int blen=0;
			for(int i=0; i<lenvalwid; i++) {
				blen = blen*10;
				blen += *(pos++)-48;
			}
			if(dest) { // copy to specified destination
				copy(pos,(pos+blen),dest);
			}
			else {
				binlen = blen;
				params.push_back(string()); // add param string
				//copy(pos,(pos+blen),params.back().begin()); // copy into last param string

#if __GNUC__ >=3
				//copy into string container
				copy(pos,(pos+blen),back_inserter(params.back())); // copy into last param string
#else
				// hpux string - no push_back() AND
				// string::copy takes char* instead of const char * as INPUT
				// does anything $%#@ work in the hpux std library?
				params.back().resize(blen);
				for(int j=0;j<binlen;j++) { params.back()[j] = pos[j]; }
#endif

				binaryindex = params.size()-1; // remember which param is binary data
				pos += binlen;
				done = true;
			}
			//param.assign(result,pos,lenvalwid); // change this
		}
		else {
			param += *pos;
		}
	}
	return pos;
}

//************************************************************
// Stream buffer and manipulators
//************************************************************

// fwstreambuf manages our formatted firmware commands.
// command stream is executed when flushed ( << endl )
fwstreambuf::fwstreambuf():streambuf() {
	char *ptr = cmd_buf;
	setp(ptr, ptr + cmd_buf_size);   // set output buffer to statically alloced cmd buf
	setg(0, 0, 0); // no input buffer
}

fwstreambuf::~fwstreambuf()
{
}

// Since we can't execute a parial fw command, overflow is an error
int fwstreambuf::overflow(int c)
{
	ostringstream msg;
	msg << "FW stream buffer has overflowed. Did you forget to flush?" << endl;
	msg << "Use endl to flush and execute command." << endl;
	throw FWException(__FILE__,__LINE__,msg.str());
	/*
	put_buffer();
	if (c != EOF)
		if (pbase() == epptr())
			// run out of buffer?
			cerr << "fw stream buffer overflow!" << endl;
		else
			sputc(c);
	*/
	return 0;
}

// flush
int fwstreambuf::sync() {
	put_buffer();
	return 0;
}

// Here we take the current buffer contents and execute the firmware command
void fwstreambuf::put_buffer() {
	if (pbase() != pptr()) {
		int     len = (pptr() - pbase());
		char    *buffer = new char[len + 1];
		
		memcpy(buffer, pbase(), len);
		buffer[len] = 0; 
		fwresult.clear();
		fwexec(buffer, len, fwresult);		
		setp(pbase(), epptr());
		delete [] buffer;
	}
}

// format char* data into #spec firmware interface
// have to use sprintf here because I cannot get rougewave ostream (HPUX)
// width() and fill() to work the way I expect.
ostream& operator<<(ostream& s, const FWBinData& d) {
	char tmp[12];
	sprintf(tmp,"#9%.9d",d.size);
	s << tmp;
	s.write(d.data,d.size);
	return s;
}


/*
struct smanip {
	ios_base& (*f)(ios_base&,unsigned char*);
	unsigned char *bdata;
	smanip(ios_base& (*ff)(ios_base&,unsigned char*), unsigned char* d):f(ff),bdata(d) {}
};

ostream& operator<<(ostream& os,smanip& m) {
	return m.f(os,m.bdata);
}

ios_base& insert_binary(ios_base& s, unsigned char* data) {
	
	return s;
}

inline smanip binarydata(unsigned char* data) {
	return smanip(insert_binary,data);
}
*/
