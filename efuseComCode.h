#include "TestMethod.h"

#include <vector>
#include <string>
#include <iterator>
#include <iostream>
#include <set>
#include <algorithm>
#include <stdio.h>
#include "tml_def.h"
#include "tpi_c.h"


//
#define 	MAX_WORD_NUM    999  //Equal or great than word numbers

//***********************************************************************
//#define		TOTAL_ADDRS	    13 //total address lines,e.g.A12-A0 as total address lines
//#define 	READ_ADDRS    	10 //read address lines,e.g. A12-A3 as read address lines
//#define 	READ_ADDRS_TYPE	"MSB" //MSB or LSB,e.g. A12-A0 as address lines,if using 10 address lines,A9-A0 means LSB,A12-A3 means MSB
//***********************************************************************
typedef struct EfuseInfo{
	string word_name;
	int bits_cnt;
	int start_addr;
	int start_bit;
	int bits_per_addr;//1/2/4/8/16/32
	string test_type;//CP/FT
	long init_val;//initial value
} EI;

typedef struct BitReadInfo{
	int read_addr;
	int bit_index;
} BRI;

typedef struct WordInfo{
	string word_name;
	int bits_cnt;
	BRI bit_info[64];
	string test_type;//CP/FT
} WI;

typedef struct WordExpectValue{
	string word_name;
	long expect_val;
} WEV;

typedef struct WordReadValue{
	string word_name;
	long read_val;
} WRV;

typedef struct tagRegister {
	long addr;
	long data;
} Register;

class EFUSE
{

	private :

		vector< vector< string > > mCsvData;

		vector<Register> mFTWriteInfo;

		vector<Register> mCPWriteInfo;

		vector<Register> mWordWriteInfo;

		vector<int> mFTReadAddr;

		vector<int> mCPReadAddr;

		vector<int> mWordReadAddr;

		long mFTCRC8CalcValue;

		long mCPCRC8CalcValue;

		vector<WRV> mFTWordReadValue;

		vector<WRV> mCPWordReadValue;

		vector<WEV> mFTWordExpectValue;

		vector<WEV> mCPWordExpectValue;

		vector< EI > mEfuseInfo;

	public:

		EFUSE();
		~EFUSE();

	public:

		void setFTWriteInfo(vector<Register> FTWriteInfo);

		void setCPWriteInfo(vector<Register> CPWriteInfo);

		void setWordWriteInfo(vector<Register> WordWriteInfo);

	public:

		vector<Register> getFTWriteInfo();

		vector<Register> getCPWriteInfo();

		vector<Register> getWordWriteInfo();

		vector<WI> getFTWordInfo();

		vector<WI> getCPWordInfo();

		vector<int> getFTReadAddr();

		vector<int> getCPReadAddr();

		vector<int> getWordReadAddr(string word_name);

		vector<WRV> getFTWordReadValue(vector<long> DATAS);

		vector<WRV> getCPWordReadValue(vector<long> DATAS);

		long getWordReadValue(string word_name,vector<long> DATAS);

		vector<WEV> getFTWordExpectValue();

		vector<WEV> getCPWordExpectValue();

		long getFTCRC8CalcValue();

		long getCPCRC8CalcValue();

		vector< vector< string > > getCsvData();

		vector< EI > getEfuseInfo();

	protected:

		void calcCRC8Data(vector<int> DATAS,vector<unsigned char> &crc8data,int debug_mode);

		unsigned short getCRC8(vector<unsigned char> p,unsigned short len);

		void Split(const string &str, const string& seperator, vector< string >& Ret);

		void replace_all(string& str,const string& old_value, const string& new_value);

		void EfuseInfo_Format_Converted(vector<WI> &word_info,string test_type);

		WI EfuseInfo_Format_Converted(string word_name);

		int GetEfuseIndex(vector<WRV> vwrv,string word_name);

	public:

		void ReadCsv();

		void FTEfuseCodeCalc(int debug_mode);

		void CPEfuseCodeCalc(int debug_mode);

		void WordEfuseCodeCalc(string word_name);

		void FTReadCRC8Calc(int debug_mode);

		void CPReadCRC8Calc(int debug_mode);

		int Match_check(vector<WRV> vwrv,vector<WEV> vwev);

		void Generate_testTable(STRING test_name);

		void Generate_testTable(STRING test_name,int Lsl,int Usl);

		void Init_userval();

};

