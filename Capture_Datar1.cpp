#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;
using namespace V93kLimits;

/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class Capture_Datar1: public testmethod::TestMethod {

protected:
/**
 *Initialize the parameter interface to the testflow.
 *This method is called just once after a testsuite is created.
 */
string  	iDebug;
string  	sVecVar;//
int  		Sample_size,Data_size;//
//	  string  	Reconnect;
virtual void initialize()
{
	//Add your initialization code here
	//Note: Test Method API should not be used in this method!

	addParameter("Capture_Signal",       "string",    &sVecVar)
		.setDefault("v_RO")
		.setComment("The list of test variable, separated by comma");

	addParameter("Sample_Size",     "int",    &Sample_size)
		.setDefault("32")
		.setComment("The sample size of digital capture");

	addParameter("Data_size",     "int",    &Data_size)
		.setDefault("8")
		.setComment("The sample size of data");

	addParameter("Debug_Mode",     "string",    &iDebug)
		.setDefault("OFF")
		.setOptions("OFF:ON")
		.setComment("Select debug mode");

}

/**
 *This test is invoked per site.
 */
virtual void run()
{
	//Add your test code here.

	ARRAY_I 		aiCapData, aReSampData, aResultData;
	static int	iOffline;
	int			ReSample_Data_size;
	int			factor;
	int			index;
LIMIT lmt_bin1;
double low_limit,high_limit;
low_limit=7577;
high_limit=7616;
lmt_bin1.low(TM::GE,low_limit);
lmt_bin1.high(TM::LE,high_limit);


	ON_FIRST_INVOCATION_BEGIN();

		GET_SYSTEM_FLAG("offline",&iOffline);
		//DISCONNECT();
		CONNECT();
		WAIT_TIME(10 ms);
		DIGITAL_CAPTURE_TEST();

	ON_FIRST_INVOCATION_END();

	ReSample_Data_size = Sample_size/Data_size;
	aiCapData.resize(Sample_size);
	aReSampData.resize(ReSample_Data_size);
	aResultData.resize(ReSample_Data_size/4);
	aiCapData.init(0);
	aReSampData.init(0);
	aResultData.init(0);
	//	  site_number = CURRENT_SITE_NUMBER()-1;

	if(!iOffline)
		aiCapData = VECTOR(sVecVar).getVectors();
	else
	{
		for(int i = 0; i < Sample_size; i++)
		{
			if(i%4 == 0)
				aiCapData[i] = 0;
			else
				aiCapData[i] = 1;
		}
	}

	factor = 1;
	index = 0;
	for(int i = 0; i < Sample_size; i++)
	{
//		cout<<aiCapData[i]<<endl;
		if(i != 0 && i%Data_size == 0)
		{
			index++;
			factor = 1;
		}
		aReSampData[index]+=(aiCapData[i]*factor);
		factor*=2;
	}

	for(int i = 0; i < ReSample_Data_size/4; i++)
	aResultData[i] = aReSampData[i*4]*256*65536+aReSampData[i*4+1]*65536+aReSampData[i*4+2]*256+aReSampData[i*4+3];
//	aResultData[0]=(aiCapData[7]<<15)|(aiCapData[6]<<14)|(aiCapData[5]<<13)|(aiCapData[4]<<12)|(aiCapData[3]<<11)|(aiCapData[2]<<10)|(aiCapData[1]<<9)|(aiCapData[0]<<8)|
//			       (aiCapData[15]<<7)|(aiCapData[14]<<6)|(aiCapData[13]<<5)|(aiCapData[12]<<4)|(aiCapData[11]<<3)|(aiCapData[10]<<2)|(aiCapData[9]<<1)|(aiCapData[8]<<0);

	if(iDebug == "ON")
	{
		cout<<"\nOn site "<<CURRENT_SITE_NUMBER();
		//cout<<",captured data stream is ";
		//for(int i = 0; i <Sample_size; i++)
		//	cout<<dec<<aiCapData[i];
		//cout<<endl;
		//cout<<"\nCaptured raw Data is: "<<endl;
		//for(int i = (ReSample_Data_size-1); i >= 0; i--)
		//	cout<<"Raw data "<<i<<" is "<<dec<<aReSampData[i]<<" = 0x"<<hex<<aReSampData[i]<<endl;
		//cout<<"\nResult data output is :"<<endl;
		for(int i = (ReSample_Data_size/4-1); i >= 0; i--)
			cout<<"rate data "<<i<<" is 0x"<< hex<<aResultData[i]<<" = "<<dec<<aResultData[i]<<endl;
	}
	unsigned int var=0;
	var= aResultData[0];
//	SET_USER_DOUBLE("ccno", aResultData[0]);
//	GET_USER_DOUBLE("ccno",&var);
	cout<<"site "<<CURRENT_SITE_NUMBER()-1<< " rate data "<<"is 0x"<< hex<<aResultData[0]<<" = "<<dec<<var<<endl;

	TESTSET().cont(true).judgeAndLog_ParametricTest("@", "nonce_no", lmt_bin1, var);

	/*for(int i = 0; i < ReSample_Data_size/2; i++)
	{
		stringstream ss;
	  	string temp_string;
	  	ss<<i;
	  	ss>>temp_string;
	  	TESTSET().cont(true).judgeAndLog_ParametricTest("@", "Process_Monitor_"+temp_string, lmt_bin1, aResultData[i]);
	}*/
	return;
}
//14*2*336=9408
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
REGISTER_TESTMETHOD("Capture_Datar1", Capture_Datar1);
