#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;
/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class crc5_calculator: public testmethod::TestMethod {

protected:
/**
 *Initialize the parameter interface to the testflow.
 *This method is called just once after a testsuite is created.
 */

	string Input_data;

virtual void initialize()
{
	//Add your initialization code here
	//Note: Test Method API should not be used in this method!

    addParameter("Input_data(Hex)",       "string",    &Input_data)
    .setDefault("51 09 00 1c 00 52 1f 10")
      .setComment("the Input_data need seperated by space in byte");
}

/**
 *This test is invoked per site.
 */


virtual void run()
{

	    vector< string > ret;
	    Split(Input_data, " ", ret);

	    int size = ret.size();

	    uint8_t tmpchar[size];

	    for (size_t i = 0; i < ret.size(); ++i){

	    	long tmp=strtol(ret[i].c_str(), NULL, 16);
	    	tmpchar[i] = tmp;
	    }

	    printCrc5Log(tmpchar,size);


	return;
}


// CRC5

uint8_t CRC5(unsigned char *ptr, unsigned char len)
	{
	    uint8_t i, j, k;
	    uint8_t crc = 0x1f;

	    uint8_t crcin[5] = {1, 1, 1, 1, 1};
	    uint8_t crcout[5] = {1, 1, 1, 1, 1};
	    uint8_t din = 0;

	    j = 0x80;
	    k = 0;
	    for (i = 0; i < len; i++)
	    {
	        if (*ptr & j)
	        {
	            din = 1;
	        }
	        else
	        {
	            din = 0;
	        }
	        crcout[0] = crcin[4] ^ din;
	        crcout[1] = crcin[0];
	        crcout[2] = crcin[1] ^ crcin[4] ^ din;
	        crcout[3] = crcin[2];
	        crcout[4] = crcin[3];

	        j = j >> 1;
	        k++;
	        if (k == 8)
	        {
	            j = 0x80;
	            k = 0;
	            ptr++;
	        }
	        memcpy(crcin, crcout, 5);
	    }
	    crc = 0;
	    if(crcin[4])
	    {
	        crc |= 0x10;
	    }
	    if(crcin[3])
	    {
	        crc |= 0x08;
	    }
	    if(crcin[2])
	    {
	        crc |= 0x04;
	    }
	    if(crcin[1])
	    {
	        crc |= 0x02;
	    }
	    if(crcin[0])
	    {
	        crc |= 0x01;
	    }
	    return crc;
	}

// CRC5

void printCrc5Log(uint8_t buf[],int size){
    cout<<"input data (0x):";
    for(int i=0;i<size;i++){
	    int tmp=0;
	    tmp = buf[i];
	    tmp=tmp&0xff;
		int H4=tmp>>4&0xf;
		int L4=tmp&0xf;
		cout<<hex<<H4<<L4<<" ";
    }
    cout<<endl;
//    cout<<"bits:"<<dec<<8*size-5<<endl;
    uint8_t crc5_val0 = CRC5(buf, 8*size);
    cout<<"the crc5(0x):"<<hex<<((int(crc5_val0))>>4&0xf)
    		<<(int(crc5_val0)&0xf)<<endl;
}


void Split(const string &str, const string& seperator, vector< string >& Ret)
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

/**
 *This function will be invoked once the specified parameter's value is changed.
 *@param parameterIdentifier
 */
virtual void postParameterChange(const string& parameterIdentifier)
{
	//Add your code
	//Note: Test Method API should not be used in this method!
	return;
}

/**
 *This function will be invoked once the Select Test Method Dialog is opened.
 */
virtual const string getComment() const
{
	string comment = " please add your comment for this method.";
	return comment;
}
};
REGISTER_TESTMETHOD("bm_miscellaneous_tml.crc5_calculator", crc5_calculator);



