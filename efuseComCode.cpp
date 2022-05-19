#include "efuseComCode.h"

using namespace std;

using namespace V93kLimits;

EFUSE::EFUSE()
{
}

EFUSE::~EFUSE()
{
}

void EFUSE::setFTWriteInfo(vector<Register> FTWriteInfo)
{
	this->mFTWriteInfo = FTWriteInfo;
}

void EFUSE::setCPWriteInfo(vector<Register> CPWriteInfo)
{
	this->mCPWriteInfo = CPWriteInfo;
}

void EFUSE::setWordWriteInfo(vector<Register> WordWriteInfo)
{
	this->mWordWriteInfo = WordWriteInfo;
}

vector< vector< string > > EFUSE::getCsvData()
{
	return this->mCsvData;
}
vector< EI > EFUSE::getEfuseInfo()
{
	return this->mEfuseInfo;
}
vector<Register>  EFUSE::getFTWriteInfo()
{
	return this->mFTWriteInfo;
}

vector<Register>  EFUSE::getCPWriteInfo()
{
	return this->mCPWriteInfo;
}

vector<Register>  EFUSE::getWordWriteInfo()
{
	return this->mWordWriteInfo;
}

vector<WEV> EFUSE::getCPWordExpectValue()
{
	mCPWordExpectValue.clear();
	for(size_t i=0;i<mEfuseInfo.size();i++){
		if(mEfuseInfo[i].test_type == "CP"){
			double tmp_double;
			GET_USER_DOUBLE(mEfuseInfo[i].word_name,&tmp_double);
			WEV tmp_wev;
			tmp_wev.expect_val = long(tmp_double);
			tmp_wev.word_name = mEfuseInfo[i].word_name;
			mCPWordExpectValue.push_back(tmp_wev);
		}
	}
	return this->mCPWordExpectValue;
}


vector<WEV> EFUSE::getFTWordExpectValue()
{
	mFTWordExpectValue.clear();
	for(size_t i=0;i<mEfuseInfo.size();i++){
		if(mEfuseInfo[i].test_type == "FT"){
			double tmp_double;
			GET_USER_DOUBLE(mEfuseInfo[i].word_name,&tmp_double);
			WEV tmp_wev;
			tmp_wev.expect_val = long(tmp_double);
			tmp_wev.word_name = mEfuseInfo[i].word_name;
			mFTWordExpectValue.push_back(tmp_wev);
		}
	}
	return this->mFTWordExpectValue;
}

vector<int> EFUSE::getFTReadAddr()
{
	mFTReadAddr.clear();
	set<int> addrs;
	vector<WI> word_info;
	EfuseInfo_Format_Converted(word_info,"FT");

	for(size_t i=0;i<word_info.size();i++){
		for(int j=0;j<word_info[i].bits_cnt;j++){
			int tmp_addr=word_info[i].bit_info[j].read_addr;
			addrs.insert(tmp_addr);
		}
	}
	set<int>::iterator set_iter = addrs.begin();
	for(set_iter = addrs.begin() ; set_iter != addrs.end() ; ++set_iter){
		mFTReadAddr.push_back(*set_iter);
	}

	return this->mFTReadAddr;
}

vector<int> EFUSE::getCPReadAddr()
{
	mCPReadAddr.clear();
	set<int> addrs;
	vector<WI> word_info;
	EfuseInfo_Format_Converted(word_info,"CP");

	for(size_t i=0;i<word_info.size();i++){
		for(int j=0;j<word_info[i].bits_cnt;j++){
			int tmp_addr=word_info[i].bit_info[j].read_addr;
			addrs.insert(tmp_addr);
		}
	}
	set<int>::iterator set_iter = addrs.begin();
	for(set_iter = addrs.begin() ; set_iter != addrs.end() ; ++set_iter){
		mCPReadAddr.push_back(*set_iter);
	}
	return this->mCPReadAddr;
}

vector<int> EFUSE::getWordReadAddr(string word_name)
{
	mWordReadAddr.clear();
	set<int> addrs;

	WI word_info = EfuseInfo_Format_Converted(word_name);

	for(int i=0;i<word_info.bits_cnt;i++){
		int tmp_addr=word_info.bit_info[i].read_addr;
		addrs.insert(tmp_addr);
	}

	set<int>::iterator set_iter = addrs.begin();
	for(set_iter = addrs.begin() ; set_iter != addrs.end() ; ++set_iter){
		mWordReadAddr.push_back(*set_iter);
	}
	return this->mWordReadAddr;
}

vector<WRV> EFUSE::getFTWordReadValue(vector<long> DATAS)
{
	mFTWordReadValue.clear();
	vector<WI> word_info;
	EfuseInfo_Format_Converted(word_info,"FT");
	for(size_t i=0;i<word_info.size();i++){

		long read_val=0;
		WRV tmp_wrv;
		tmp_wrv.word_name=word_info[i].word_name;

		for(int j=0;j<word_info[i].bits_cnt;j++){
		  int addr=word_info[i].bit_info[j].read_addr;
		  int bit_index=word_info[i].bit_info[j].bit_index;

		  long tmp_val=0;
		  for(size_t k=0;k<mFTReadAddr.size();k++){
			  if(addr==mFTReadAddr[k]){
				  tmp_val=DATAS[k]>>bit_index &0x1;

				  break;
			  }
		  }

		  read_val=read_val+(tmp_val<<j);
		}

		tmp_wrv.read_val=read_val;

		mFTWordReadValue.push_back(tmp_wrv);
	}

	return this->mFTWordReadValue;
}

vector<WRV> EFUSE::getCPWordReadValue(vector<long> DATAS)
{
	mCPWordReadValue.clear();
	vector<WI> word_info;
	EfuseInfo_Format_Converted(word_info,"CP");
	for(size_t i=0;i<word_info.size();i++){

		long read_val=0;
		WRV tmp_wrv;
		tmp_wrv.word_name=word_info[i].word_name;

		for(int j=0;j<word_info[i].bits_cnt;j++){
		  int addr=word_info[i].bit_info[j].read_addr;
		  int bit_index=word_info[i].bit_info[j].bit_index;

		  long tmp_val=0;
		  for(size_t k=0;k<mCPReadAddr.size();k++){
			  if(addr==mCPReadAddr[k]){
				  tmp_val=DATAS[k]>>bit_index &0x1;

				  break;
			  }
		  }

		  read_val=read_val+(tmp_val<<j);
		}

		tmp_wrv.read_val=read_val;

		mCPWordReadValue.push_back(tmp_wrv);
	}
	return this->mCPWordReadValue;
}

long EFUSE::getWordReadValue(string word_name,vector<long> DATAS)
{
	WI word_info = EfuseInfo_Format_Converted(word_name);

	long read_val=0;

	for(int j=0;j<word_info.bits_cnt;j++){
	  int addr=word_info.bit_info[j].read_addr;
	  int bit_index=word_info.bit_info[j].bit_index;

	  long tmp_val=0;
	  for(size_t k=0;k<mWordReadAddr.size();k++){
		  if(addr==mWordReadAddr[k]){
			  tmp_val=DATAS[k]>>bit_index &0x1;

			  break;
		  }
	  }

	  read_val=read_val+(tmp_val<<j);
	}
	return read_val;
}

long EFUSE::getFTCRC8CalcValue()
{
	return this->mFTCRC8CalcValue;
}

long EFUSE::getCPCRC8CalcValue()
{
	return this->mCPCRC8CalcValue;
}
void EFUSE::calcCRC8Data(vector<int> DATAS,vector<unsigned char> &crc8data,int debug_mode)
{
	int data_size=DATAS.size();
	int byte_cnt=0;
	if(data_size%8==0){
		byte_cnt=int(data_size/8);
		for(int j=byte_cnt-1;j>=0;j--){
			int tmp=0;
			for(int k=0;k<8;k++){
				tmp=tmp|(DATAS[j*8+k]<<k);
			}

			tmp=tmp&0xff;
			if(debug_mode){
				int H4=tmp>>4&0xf;
				int L4=tmp&0xf;
				cout<<hex<<H4<<L4<<" ";
			}


			unsigned char tmp_char;
			tmp_char=(unsigned char)(tmp);
			crc8data.push_back(tmp_char);
		}
	}
	else{
		 int index_val=(data_size%8);
		 byte_cnt=int(data_size/8)+1;
		 for(int j=byte_cnt-1;j>=0;j--){
			int tmp=0;
			if(j<byte_cnt-1){
				for(int k=0;k<8;k++){
					tmp=tmp|(DATAS[j*8+k]<<k);
				}
			}
			else if(j==byte_cnt-1){
				for(int k=0;k<index_val;k++){
					tmp=tmp|(DATAS[j*8+k]<<k);
				}
			}

			tmp=tmp&0xff;

			if(debug_mode){
				int H4=tmp>>4&0xf;
				int L4=tmp&0xf;
				cout<<hex<<H4<<L4<<" ";
			}

			unsigned char tmp_char;
			tmp_char=(unsigned char)(tmp);
			crc8data.push_back(tmp_char);
		}
	}
}

unsigned short EFUSE::getCRC8(vector<unsigned char> p,unsigned short len)
{
	  unsigned char  polynominal,crc8,temp;
	  unsigned short crc8_value;
	  int i,j;
	  crc8=p[0];
	  crc8_value = crc8;
	  polynominal=0x31;
	  i=0;
	  j=1;
	  while(len--)
	  {
		   if((crc8&0x80)!=0)
		   {
			 crc8=crc8<<1;
			 temp=p[j]<<i;
			 temp=temp>>7;
			 crc8=crc8+temp;
			 crc8_value = crc8;
			 crc8=crc8^polynominal;
			 crc8_value = crc8;
		   }
		   else
		   {
			 crc8=crc8<<1;
			 temp=p[j]<<i;
			 temp=temp>>7;
			 crc8=crc8+temp;
			 crc8_value = crc8;
		   }
		   i=i+1;
		   if(i==8)
		   {
			 i=0;
			 j=j+1;
		   }
	  }
	  return crc8_value;
}

void EFUSE::ReadCsv()
{
	string strFileName;
    string Data;

	char device_path[DM_MAX_DEVICE];
	GetDevPath(device_path);
	string tmp_path;
	tmp_path=device_path;
	GET_USER_STRING("EfuseFileName",strFileName);
	strcat (device_path, strFileName.c_str());
    //

    FILE* pFile = fopen(device_path, "rb");
    if (pFile == 0)
    {
    	cout<<std::left<<"##########################################ERROR##################################################"<<endl;
    	cout<<std::left<<"#####93K Can't find the right Efuse File:"<<strFileName<<endl;
    	cout<<std::left<<"#####in the path:"<<tmp_path<<endl;
    	cout<<std::left<<"#####pls check if the filename or path is right!"<<endl;
    	cout<<std::left<<"##########################################ERROR##################################################"<<endl;
        exit(1);
    }

    fseek(pFile, 0, SEEK_END);
    long len = ftell(pFile);

    char *pBuffer = new char[len + 1];

    fseek(pFile, 0, SEEK_SET);
    fread(pBuffer, 1, len, pFile);
    fclose(pFile);

    pBuffer[len] = 0;
    Data.assign(pBuffer, len );

    delete[] pBuffer;

    //
    mCsvData.clear();
    mEfuseInfo.clear();
    //
    vector< string > ret;
    Split(Data, "\n", ret);

    for (size_t i = 0; i < ret.size(); ++i)
    {
    	if(ret[i].find("#")!=ret[i].npos){ //# as comment symbol
    		continue;
    	}
    	replace_all(ret[i],"\"","");//del "
    	replace_all(ret[i]," ","");//del " "
        vector< string > Fields,tmpVStr;
        EI tmpEI;
        Split(ret[i], ",", Fields);
        mCsvData.push_back(Fields);
        for(size_t j=0;j<Fields.size();j++){
        	if(Fields[j].length()>0)
        		tmpVStr.push_back(Fields[j]);
        }
        for(size_t k=0;k<tmpVStr.size();k++){
        	tmpEI.word_name = tmpVStr[0].c_str();
        	tmpEI.bits_cnt  = strtol(tmpVStr[1].c_str(),NULL,0);//atoi(tmpVStr[1].c_str());
        	tmpEI.start_addr = strtol(tmpVStr[2].c_str(),NULL,0);//atoi(tmpVStr[2].c_str());
        	tmpEI.start_bit = strtol(tmpVStr[3].c_str(),NULL,0);//atoi(tmpVStr[3].c_str());
        	tmpEI.bits_per_addr = strtol(tmpVStr[4].c_str(),NULL,0);//atoi(tmpVStr[4].c_str());
        	tmpEI.test_type = tmpVStr[5].c_str();
        	tmpEI.init_val = strtol(tmpVStr[6].c_str(),NULL,0);//atoi(tmpVStr[6].c_str());
        }
        mEfuseInfo.push_back(tmpEI);
    }
}

void EFUSE::Split(const string &str, const string& seperator, vector< string >& Ret)
{
    Ret.clear();

    size_t nStartPosFound = str.find(seperator, 0);
    size_t nFieldStart = 0;
    for (; nStartPosFound != -1; nStartPosFound = str.find(seperator, nStartPosFound))
    {
        string strSub = str.substr(nFieldStart, nStartPosFound - nFieldStart);
        nStartPosFound = nStartPosFound + seperator.size();
        nFieldStart = nStartPosFound;

        Ret.push_back(strSub);
    }

    //
    if (nFieldStart < str.size())
    {
        string strSub = str.substr(nFieldStart, str.size() - nFieldStart);
        Ret.push_back(strSub);
    }

}

void EFUSE::replace_all(string& str,const string& old_value, const string& new_value)
{
	while(true){
		string::size_type	pos(0);
		if((pos=str.find(old_value)) !=string::npos)
			str.replace(pos,old_value.length(),new_value);
		else break;
	}
}

void EFUSE::FTEfuseCodeCalc(int debug_mode)
{
	vector<Register> FTWriteInfo;
	FTWriteInfo.clear();
	vector<unsigned char> crc8data;
	crc8data.clear();
	if(debug_mode) cout<<"the crc8 data stream for FT(hex):";
	for(size_t i=0;i<mEfuseInfo.size();i++){
		if(mEfuseInfo[i].test_type == "FT"){
			if(mEfuseInfo[i].word_name !="crc_ate_ft"){
				string word_name = mEfuseInfo[i].word_name;
				double expect_val;
				GET_USER_DOUBLE(word_name,&expect_val);
				vector<int> tmpData;
				tmpData.clear();

				for(int j=0;j<mEfuseInfo[i].bits_cnt;j++){
					Register tmpWriteIno;
					tmpWriteIno.addr = mEfuseInfo[i].start_addr * mEfuseInfo[i].bits_per_addr + mEfuseInfo[i].start_bit + j;
					tmpWriteIno.data = long(expect_val)>>j & 0x1;

					tmpData.push_back(tmpWriteIno.data);
					FTWriteInfo.push_back(tmpWriteIno);
				}
				calcCRC8Data(tmpData,crc8data,debug_mode);
			}
		}
	}

	mFTCRC8CalcValue = getCRC8(crc8data,8*crc8data.size());
	if(debug_mode) cout<<"crc8 calc value for FT:0x"<<hex<<mFTCRC8CalcValue<<endl;
	SET_USER_DOUBLE("crc_ate_ft",mFTCRC8CalcValue);
	for(size_t i=0;i<mEfuseInfo.size();i++){
		if(mEfuseInfo[i].test_type == "FT"){
			if(mEfuseInfo[i].word_name =="crc_ate_ft"){
				string word_name = mEfuseInfo[i].word_name;

				double expect_val;
				GET_USER_DOUBLE(word_name,&expect_val);
				vector<int> tmpData;
				tmpData.clear();

				for(int j=0;j<mEfuseInfo[i].bits_cnt;j++){
					Register tmpWriteIno;
					tmpWriteIno.addr = mEfuseInfo[i].start_addr * mEfuseInfo[i].bits_per_addr + mEfuseInfo[i].start_bit + j;
					tmpWriteIno.data = long(expect_val)>>j & 0x1;

					FTWriteInfo.push_back(tmpWriteIno);
				}
			}
		}
	}


	setFTWriteInfo(FTWriteInfo);
}

void EFUSE::CPEfuseCodeCalc(int debug_mode)
{
	vector<Register> CPWriteInfo;
	CPWriteInfo.clear();
	vector<unsigned char> crc8data;
	crc8data.clear();
	if(debug_mode) cout<<"the crc8 data stream for CP(hex):";

	for(size_t i=0;i<mEfuseInfo.size();i++){
		if(mEfuseInfo[i].test_type == "CP"){
			if(mEfuseInfo[i].word_name !="crc_ate_cp"){
				string word_name = mEfuseInfo[i].word_name;

				double expect_val;
				GET_USER_DOUBLE(word_name,&expect_val);
				vector<int> tmpData;
				tmpData.clear();

				for(int j=0;j<mEfuseInfo[i].bits_cnt;j++){
					Register tmpWriteIno;
					tmpWriteIno.addr = mEfuseInfo[i].start_addr * mEfuseInfo[i].bits_per_addr + mEfuseInfo[i].start_bit + j;
					tmpWriteIno.data = long(expect_val)>>j & 0x1;

					tmpData.push_back(tmpWriteIno.data);
					CPWriteInfo.push_back(tmpWriteIno);
				}
				calcCRC8Data(tmpData,crc8data,debug_mode);
			}
		}
	}

	mCPCRC8CalcValue = getCRC8(crc8data,8*crc8data.size());
	if(debug_mode) cout<<"crc8 calc value for CP:0x"<<hex<<mCPCRC8CalcValue<<endl;
	SET_USER_DOUBLE("crc_ate_cp",mCPCRC8CalcValue);
	for(size_t i=0;i<mEfuseInfo.size();i++){
		if(mEfuseInfo[i].test_type == "CP"){
			if(mEfuseInfo[i].word_name =="crc_ate_cp"){
				string word_name = mEfuseInfo[i].word_name;

				double expect_val;
				GET_USER_DOUBLE(word_name,&expect_val);
				vector<int> tmpData;
				tmpData.clear();

				for(int j=0;j<mEfuseInfo[i].bits_cnt;j++){
					Register tmpWriteIno;
					tmpWriteIno.addr = mEfuseInfo[i].start_addr * mEfuseInfo[i].bits_per_addr + mEfuseInfo[i].start_bit + j;
					tmpWriteIno.data = long(expect_val)>>j & 0x1;

					CPWriteInfo.push_back(tmpWriteIno);
				}
			}
		}
	}
	setCPWriteInfo(CPWriteInfo);
}

void EFUSE::WordEfuseCodeCalc(string word_name)
{
	vector<Register> WordWriteInfo;
	WordWriteInfo.clear();

	for(size_t i=0;i<mEfuseInfo.size();i++){
		if(word_name == mEfuseInfo[i].word_name){
			double expect_val;
			GET_USER_DOUBLE(word_name,&expect_val);
			cout<<"the value for write word \""<<word_name<<"\" is "<<dec<<expect_val<<endl;
			vector<int> tmpData;
			tmpData.clear();

			for(int j=0;j<mEfuseInfo[i].bits_cnt;j++){
				Register tmpWriteIno;
				tmpWriteIno.addr = mEfuseInfo[i].start_addr * mEfuseInfo[i].bits_per_addr + mEfuseInfo[i].start_bit + j;
				tmpWriteIno.data = long(expect_val)>>j & 0x1;

				tmpData.push_back(tmpWriteIno.data);
				WordWriteInfo.push_back(tmpWriteIno);
			}
		}
	}
	setWordWriteInfo(WordWriteInfo);
}

void EFUSE::Generate_testTable(string test_name)
{
	tmLimits.load();

	char temp_test[50],temp_testname[50];
	string suite;
	GET_TESTSUITE_NAME(suite);
//
	TestTable* testTable = TestTable::getDefaultInstance();
	testTable->isCsvLoadNeeded();
	testTable->readCsvFile();
//
	int KeyValue[]={1};
	testTable->setLookupKeys(1,KeyValue);

	sprintf(temp_test,"%s",suite.c_str());

	sprintf(temp_testname,"%s",test_name.c_str());

	if(testTable->getTestPos(temp_test)<0)
	{
		testTable->addTest(temp_test);
		testTable->setValue(temp_test,"Lsl_typ","NA");
		testTable->setValue(temp_test,"Usl_typ","NA");
		testTable->setValue(temp_test,"Suite name",suite.c_str());
		testTable->setValue(temp_test,"Test name",temp_testname);

		testTable->writeCsvFile();
	}

	TestTable::clearDefaultInstance();
	tmLimits.load();

}

void EFUSE::Generate_testTable(string test_name,int Lsl,int Usl)
{
	tmLimits.load();

	char temp_test[50],temp_testname[50],temp_Lsl[50],temp_Usl[50];
	string suite;
	GET_TESTSUITE_NAME(suite);
//
	TestTable* testTable = TestTable::getDefaultInstance();
	testTable->isCsvLoadNeeded();
	testTable->readCsvFile();
//
	int KeyValue[]={1};
	testTable->setLookupKeys(1,KeyValue);

	sprintf(temp_test,"%s",suite.c_str());
	sprintf(temp_Lsl,"%d",Lsl);
	sprintf(temp_Usl,"%d",Usl);
	sprintf(temp_testname,"%s",test_name.c_str());

	if(testTable->getTestPos(temp_test)<0)
	{
		testTable->addTest(temp_test);
		testTable->setValue(temp_test,"Lsl",temp_Lsl);
		testTable->setValue(temp_test,"Lsl_typ","GE");
		testTable->setValue(temp_test,"Usl_typ","LE");
		testTable->setValue(temp_test,"Usl",temp_Usl);
		testTable->setValue(temp_test,"Units","");
		testTable->setValue(temp_test,"Suite name",suite.c_str());
		testTable->setValue(temp_test,"Test name",temp_testname);

		testTable->writeCsvFile();
	}

	TestTable::clearDefaultInstance();
	tmLimits.load();

}

void EFUSE::EfuseInfo_Format_Converted(vector<WI> &word_info,string test_type)
{
	word_info.clear();
	string READ_ADDRS_TYPE;

	double tmp_val;
	int TOTAL_ADDRS;
	int READ_ADDRS;
	GET_USER_STRING("READ_ADDRS_TYPE",READ_ADDRS_TYPE);
	GET_USER_DOUBLE("TOTAL_ADDRS",&tmp_val);
	TOTAL_ADDRS = int(tmp_val);
	GET_USER_DOUBLE("READ_ADDRS",&tmp_val);
	READ_ADDRS = int(tmp_val);

	for(size_t i=0;i<mEfuseInfo.size();i++)
	{
		if(test_type==mEfuseInfo[i].test_type){
			WordInfo tmp_word_info;

			tmp_word_info.word_name=mEfuseInfo[i].word_name;
			tmp_word_info.test_type=mEfuseInfo[i].test_type;
			tmp_word_info.bits_cnt=mEfuseInfo[i].bits_cnt;
			for(int j=0;j<mEfuseInfo[i].bits_cnt;j++){
				if("MSB"==READ_ADDRS_TYPE){
					int unmask_bit=0;
					for(int k=0;k<(TOTAL_ADDRS - READ_ADDRS);k++){
						unmask_bit=unmask_bit|1<<k;
					}
					tmp_word_info.bit_info[j].read_addr=(mEfuseInfo[i].start_addr*mEfuseInfo[i].bits_per_addr + mEfuseInfo[i].start_bit + j)>>(TOTAL_ADDRS - READ_ADDRS);
					tmp_word_info.bit_info[j].bit_index=(mEfuseInfo[i].start_addr*mEfuseInfo[i].bits_per_addr + mEfuseInfo[i].start_bit + j) & unmask_bit;
				}
				else if("LSB"==READ_ADDRS_TYPE){
					int unmask_bit=0;
					for(int k=0;k<READ_ADDRS;k++){
						unmask_bit=unmask_bit|1<<k;
					}
					tmp_word_info.bit_info[j].bit_index=(mEfuseInfo[i].start_addr*mEfuseInfo[i].bits_per_addr + mEfuseInfo[i].start_bit + j)>>READ_ADDRS;
					tmp_word_info.bit_info[j].read_addr=(mEfuseInfo[i].start_addr*mEfuseInfo[i].bits_per_addr + mEfuseInfo[i].start_bit + j) & unmask_bit;
				}
			}

			word_info.push_back(tmp_word_info);
		}
	}
}

WI EFUSE::EfuseInfo_Format_Converted(string word_name)
{
	WordInfo tmp_word_info;

	string READ_ADDRS_TYPE;

	double tmp_val;
	int TOTAL_ADDRS;
	int READ_ADDRS;
	GET_USER_STRING("READ_ADDRS_TYPE",READ_ADDRS_TYPE);
	GET_USER_DOUBLE("TOTAL_ADDRS",&tmp_val);
	TOTAL_ADDRS = int(tmp_val);
	GET_USER_DOUBLE("READ_ADDRS",&tmp_val);
	READ_ADDRS = int(tmp_val);

	for(size_t i=0;i<mEfuseInfo.size();i++)
	{
		if(word_name == mEfuseInfo[i].word_name){

			tmp_word_info.word_name=mEfuseInfo[i].word_name;
			tmp_word_info.test_type=mEfuseInfo[i].test_type;
			tmp_word_info.bits_cnt=mEfuseInfo[i].bits_cnt;
			for(int j=0;j<mEfuseInfo[i].bits_cnt;j++){
				if("MSB"==READ_ADDRS_TYPE){
					int unmask_bit=0;
					for(int k=0;k<(TOTAL_ADDRS - READ_ADDRS);k++){
						unmask_bit=unmask_bit|1<<k;
					}
					tmp_word_info.bit_info[j].read_addr=(mEfuseInfo[i].start_addr*mEfuseInfo[i].bits_per_addr + mEfuseInfo[i].start_bit + j)>>(TOTAL_ADDRS - READ_ADDRS);
					tmp_word_info.bit_info[j].bit_index=(mEfuseInfo[i].start_addr*mEfuseInfo[i].bits_per_addr + mEfuseInfo[i].start_bit + j) & unmask_bit;
				}
				else if("LSB"==READ_ADDRS_TYPE){
					int unmask_bit=0;
					for(int k=0;k<READ_ADDRS;k++){
						unmask_bit=unmask_bit|1<<k;
					}
					tmp_word_info.bit_info[j].bit_index=(mEfuseInfo[i].start_addr*mEfuseInfo[i].bits_per_addr + mEfuseInfo[i].start_bit + j)>>READ_ADDRS;
					tmp_word_info.bit_info[j].read_addr=(mEfuseInfo[i].start_addr*mEfuseInfo[i].bits_per_addr + mEfuseInfo[i].start_bit + j) & unmask_bit;
				}
			}
		}
	}
	return tmp_word_info;
}

int EFUSE::Match_check(vector<WRV> vwrv,vector<WEV> vwev)
{
	int flag_match=1;
	for(size_t i=0;i<vwrv.size();i++){
		for(size_t j=0;j<vwev.size();j++){
			if(vwrv[i].word_name == vwev[j].word_name){
				if(vwrv[i].read_val != vwev[j].expect_val){
					flag_match=0;
					return flag_match;
				}
				break;
			}
		}
	}
	return flag_match;
}
void EFUSE::FTReadCRC8Calc(int debug_mode)
{
	vector<WI> word_info;
	EfuseInfo_Format_Converted(word_info,"FT");
	vector<unsigned char> crc8data;
	crc8data.clear();
	if(debug_mode) cout<<"the crc8 data stream for FT(hex):";
	for(size_t i=0;i<word_info.size();i++){
		if(word_info[i].test_type == "FT"){
			if(word_info[i].word_name !="crc_ate_ft"){
				string word_name = word_info[i].word_name;
				int index = GetEfuseIndex(mFTWordReadValue,word_name);

				vector<int> tmpData;
				tmpData.clear();

				for(int j=0;j<word_info[i].bits_cnt;j++){
					int data = mFTWordReadValue[index].read_val>>j & 0x1;
					tmpData.push_back(data);
				}
				calcCRC8Data(tmpData,crc8data,debug_mode);
			}
		}
	}

	mFTCRC8CalcValue = getCRC8(crc8data,8*crc8data.size());
	if(debug_mode) cout<<"crc8 calc value for FT:0x"<<hex<<mFTCRC8CalcValue<<endl;
}

void EFUSE::CPReadCRC8Calc(int debug_mode)
{
	vector<WI> word_info;
	EfuseInfo_Format_Converted(word_info,"CP");
	vector<unsigned char> crc8data;
	crc8data.clear();
	if(debug_mode) cout<<"the crc8 data stream for CP(hex):";
	for(size_t i=0;i<word_info.size();i++){
		if(word_info[i].test_type == "CP"){
			if(word_info[i].word_name !="crc_ate_cp"){
				string word_name = word_info[i].word_name;
				int index = GetEfuseIndex(mCPWordReadValue,word_name);

				vector<int> tmpData;
				tmpData.clear();

				for(int j=0;j<word_info[i].bits_cnt;j++){
					int data = mCPWordReadValue[index].read_val>>j & 0x1;
					tmpData.push_back(data);
				}
				calcCRC8Data(tmpData,crc8data,debug_mode);
			}
		}
	}

	mCPCRC8CalcValue = getCRC8(crc8data,8*crc8data.size());
	if(debug_mode) cout<<"crc8 calc value for CP:0x"<<hex<<mCPCRC8CalcValue<<endl;
}

int EFUSE::GetEfuseIndex(vector<WRV> vwrv,string word_name)
{
	int index_val=-1;
	int flag=0;
	for(unsigned int i=0; (i<vwrv.size())&&(flag<1);i++){
		if(vwrv[i].word_name==word_name){
				index_val=i;
				flag=1;
		}
	}
	if(0==flag){
			cout<<"error! can't find the word_name:"+word_name<<endl;
	}

	return index_val;
}

vector<WI> EFUSE::getFTWordInfo()
{
  	vector<WI> word_info;
  	EfuseInfo_Format_Converted(word_info,"FT");
  	return word_info;
}

vector<WI> EFUSE::getCPWordInfo()
{
  	vector<WI> word_info;
  	EfuseInfo_Format_Converted(word_info,"CP");
  	return word_info;
}

void EFUSE::Init_userval()
{
	for(size_t i=0;i<mEfuseInfo.size();i++){

		string word_name = mEfuseInfo[i].word_name;
		double init_val = mEfuseInfo[i].init_val;
		SET_USER_DOUBLE(word_name,init_val);

	}
}
