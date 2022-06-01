/*
 * Shmoo_PinMargin.cpp
 *
 *  Created on: May 17, 2022
 *      Author: pengbobo
 *      email:bobo.peng@bitmain.com
 */

#include "testmethod.hpp"
#include "mapi.hpp"
#include "tml_def.h"
#include "tpi_c.h"
#include <cstring>
#include <fstream>
#include <iomanip>

class shmoo_nonce: public testmethod::TestMethod {
protected:
	int Cap_samples;//capture samples;
	int expect_cnt;//expect ticket count;
	int org_freq;//original frequency
	string vCap;//capture variable
	string Tper;
	string Vfrc;
	double TScale_UL;
	double TScale_LL;
	double VScale_UL;
	double VScale_LL;
	int T_points;
	int V_points;
	int Vclamp_EN;

protected:
	/**
	 *Initialize the parameter interface to the testflow.
	 *This method is called just once after a testsuite is created.
	 *
	 *Note: TestMethod API should not be used in this method.
	 */
	virtual void initialize() {
	    addParameter("Cap_samples", "int", &Cap_samples)
	    .setDefault("1")
	      .setComment("the samples of digital capture");
	    addParameter("vCap",       "string",    &vCap)
	    .setDefault("bynonce_work_xcore")
	      .setComment("the Capture variable,must be unique");
	    addParameter("expect_cnt",  "int",    &expect_cnt)
	    .setDefault("7040")
	      .setComment("the ticket counter bits");
	    addParameter("org_freq_MHz",  "int",    &org_freq)
	    .setDefault("650")
	      .setComment("the original frequency");
		addParameter("Tper", "string", &Tper).setDefault("per_40").setComment(
				"Period Spec");
		addParameter("Vfrc", "string", &Vfrc).setDefault("vdd").setComment(
				"Level Spec ");
		addParameter("TScale_UL", "double", &TScale_UL).setDefault("1.2").setComment(
				"Up Limit of Period Scale");
		addParameter("TScale_LL", "double", &TScale_LL).setDefault("0.8").setComment(
				"Low Limit of Period Scale");
		addParameter("VScale_UL", "double", &VScale_UL).setDefault("1.2").setComment(
				"Up Limit of Voltage Scale");
		addParameter("VScale_LL", "double", &VScale_LL).setDefault("0.8").setComment(
				"Low Limit of Voltage Scale");
		addParameter("T_points", "int", &T_points).setDefault("11").setComment(
				"Period Shmoo points");
		addParameter("V_points", "int", &V_points).setDefault("11").setComment(
				"Voltage Shmoo points");
		addParameter("Vclamp_EN", "int", &Vclamp_EN).setDefault("1").setComment(
				"1 enable, 0 disable");
	}

	/**
	 *This test is invoked per site.
	 */
	virtual void run() {
		// add your code

		TIMING_SPEC Tspec = Primary.getTimingSpec();
		LEVEL_SPEC Vspec = Primary.getLevelSpec();
		double Cur_T_value = Tspec.getSpecValue(Tper);
		double Cur_V_value = Vspec.getSpecValue(Vfrc);
		int X, Y;
		int T_points_1, T_points_0;
		int V_points_1, V_points_0;
		double period, DPS_Value;
		double VScale_UL_Val, VScale_LL_Val;
		double T_UL_Val, T_LL_Val, Vmax;
		double Vstep, Tstep;
		STRING suite_name;
		char tmp_v[256], tmp_t[256];

		static ARRAY_I	aiCapData;
		aiCapData.resize(Cap_samples);
		aiCapData.init(0);

		static ARRAY_I		ProcessedData;
	 	ProcessedData.resize(Cap_samples);
	 	ProcessedData.init(0);

		SPMU_TASK dcscale;

		char device_path[DM_MAX_DEVICE];

		GetDevPath(device_path);

		strcat(device_path, "/shmoo/shm.txt"); //default file path

		cout << "the shmoo data path is " << device_path << endl;

		ofstream fout;

		fout.open(device_path, ios_base::out | ios_base::app);

		if (fout.good() != 1)

		{
			cout	<< "================== File Open Error,pls check the output directory permission! =================="
					<< endl;
			return;
		}

		GET_TESTSUITE_NAME(suite_name);

		cout << suite_name << endl;
		fout << suite_name << "\n";

		cout << "Cur_T_SPEC:" << Tper << "=" << Cur_T_value << "ns" << endl;
		fout << "Cur_T_SPEC:" << Tper << "=" << Cur_T_value << "ns" << endl;

		cout << "Current Freq:" << "=" << org_freq << "MHz" << endl;
		fout << "Current Freq:" << "=" << org_freq << "MHz" << endl;

		cout << "Cur_V_SPEC:" << Vfrc << "=" << Cur_V_value << "V" << endl;
		fout << "Cur_V_SPEC:" << Vfrc << "=" << Cur_V_value << "V" << endl;

		//calculate steps
		if (T_points == 1) {
			Tstep = 0;
		}
		else {
			Tstep = Cur_T_value * (TScale_UL - TScale_LL) / (T_points - 1);
		}
		if (V_points == 1) {
			Vstep = 0;
		}
		else {
			Vstep = Cur_V_value * (VScale_UL - VScale_LL) / (V_points - 1);
		}

		T_points_1 = INT(T_points * (TScale_UL - 1) / (TScale_UL - TScale_LL));
		T_points_0 = INT(T_points * (1 - TScale_LL) / (TScale_UL - TScale_LL));
		V_points_1 = INT(V_points * (VScale_UL - 1) / (VScale_UL - VScale_LL));
		V_points_0 = INT(V_points * (1 - VScale_LL) / (VScale_UL - VScale_LL));
		T_LL_Val = Cur_T_value - Tstep * T_points_0;
		T_UL_Val = Cur_T_value + Tstep * T_points_1;
		VScale_LL_Val = Cur_V_value - Vstep * V_points_0;
		VScale_UL_Val = Cur_V_value + Vstep * V_points_1;
		Vmax = VScale_UL_Val;
		cout << "T_LL_Val=" << T_LL_Val << "ns" << endl;
		cout << "T_UL_Val=" << T_UL_Val << "ns" << endl;
		cout << "F_UL_Val=" << org_freq*Cur_T_value/T_LL_Val << "MHz" << endl;
		cout << "F_LL_Val=" <<  org_freq*Cur_T_value/T_UL_Val << "MHz" << endl;
		cout << "VScale_LL_Val=" << VScale_LL_Val <<"V"<< endl;
		cout << "VScale_UL_Val=" << VScale_UL_Val <<"V"<< endl;

		fout << "T_LL_Val=" << T_LL_Val << "ns" << endl;
		fout << "T_UL_Val=" << T_UL_Val << "ns" << endl;
		fout << "F_LL_Val=" << org_freq*Cur_T_value/T_LL_Val << "MHz" << endl;
		fout << "F_UL_Val=" <<  org_freq*Cur_T_value/T_UL_Val << "MHz" << endl;
		fout << "VScale_LL_Val=" << VScale_LL_Val <<"V"<< endl;
		fout << "VScale_UL_Val=" << VScale_UL_Val <<"V"<< endl;

		if (Vmax > 5) {
			cout << "###########################################" << endl;
			cout << "   Warning!!!!!" << endl;
			cout << "   The Vmax=" << Vmax << " " << " , >5V " << endl;
			cout << "   Please make sure if it is safe!  " << endl;
			cout << "   If yes,pls modify the TestMethod specially!" << endl;
			cout << "###########################################" << endl;
			return;
		}

//		cout << Tper << "(nS)\t" << endl;
//		fout << Tper << "(nS)\t" << "\n";

		cout << "frequency" << "(MHz)\t" << endl;
		fout << "frequency" << "(MHZ)\t" << "\n";

		 CONNECT();

		DIGITAL_CAPTURE_TEST();

		if ((VScale_UL <= 1.5 && VScale_LL >= 0.5) || Vclamp_EN == 0) {
			for (Y = 0; Y < T_points; Y++) {

				 DISCONNECT();
				 WAIT_TIME(50 ms);
				 CONNECT();
				 WAIT_TIME(50 ms);

				period = T_UL_Val - Tstep * Y;
				Tspec.change(Tper, period);
				printf("%6.1f\t", org_freq*Cur_T_value/period);
				sprintf(tmp_t, "%6.1f", org_freq*Cur_T_value/period);

				fout << tmp_t << "\t";
				FLUSH(TM::APRM);

				for (X = 0; X < V_points; X++) {
					DPS_Value = VScale_LL_Val + Vstep * X;
					Vspec.change(Vfrc, DPS_Value);
					FLUSH(TM::APRM);

					DIGITAL_CAPTURE_TEST();

					aiCapData = VECTOR(vCap).getVectors();//get back the capture data
//					ProcessedData = CapDataProcess(aiCapData);//process raw data
//					DOUBLE pct_return = 0;
//				  for(int i=0;i<ProcessedData.size();i++){
//					  cout<<"ticket count:"<<ProcessedData[i]<<endl;
//					  pct_return =   double(ProcessedData[i])/expect_cnt *100;//calculate the percentage of samples/(expect nonce cnt)
//				  }
					long regVal = CaptDataProcess(aiCapData);

					DOUBLE pct_return =  double(regVal)/expect_cnt*100;//calculate the percentage of samples/(expect nonce cnt)
//					cout<<"regval:"<<regVal<<endl;
					printf("%5.2f%s\t", pct_return,"%");
					sprintf(tmp_v, "%5.2f%s", pct_return,"%");

					fout << tmp_v << "\t";
				}
				cout << endl;
				fout << "\n";
			}
			cout << " \t";
			fout << " \t";

			for (X = 0; X < V_points; X++)

			{
				DPS_Value = VScale_LL_Val + Vstep * X;

				printf("%5.3f\t", DPS_Value);
				sprintf(tmp_v, "%5.3f", DPS_Value);

				fout <<"  \t"<< tmp_v ;
			}

			cout << " :" << Vfrc << "(" << Cur_V_value << "V)" << endl;
			fout << " :" << Vfrc << "(" << Cur_V_value << "V)" << "\n" << "\n";
			fout.close();
		}

		else {
			cerr << "###############################" << endl;
			cerr << "Warning!!!!!" << endl;
			cerr << "The VScale_UL or VScale_LL is out of Range(0.5,1.5) "
					<< endl;
			cerr
					<< "Please check the level set,the Vfrc should be coefficent of Voltage"
					<< endl;
			cerr
					<< "If you make sure Vfrc is safety, pls change the Vclamp_EN=0,then continue"
					<< endl;
			cerr << "###############################" << endl;
		}

		Tspec.restore();
		FLUSH(TM::APRM);
		Vspec.restore();
		FLUSH(TM::APRM);

		return;
	}

	/**
	 *This function will be invoked once the specified parameter's value is changed.
	 *@param parameterIdentifier
	 *
	 *Note: TestMethod API should not be used in this method.
	 */

	long  CaptDataProcess(ARRAY_I  capData)
		{
		  long regVal= 0; //initial regVal
			for(int i = 0; i< capData.size();i++){
				if(i<=7) {regVal = regVal | capData[i]<<(24+i);} //bit0~7 shift as the bit 24~31 of regVal
				else if(i>7 && i<=15) {regVal = regVal | capData[i]<<(8+i);}//bit8~15 shift as the bit 16~23 of regVal
				else if(i>15 && i<=23) {regVal = regVal | capData[i]<<(i-8);}//bit16~23 shift as the bit 8~15 of regVal
				else  {regVal = regVal | capData[i]<<(i-24);}//bit24~31 shift as the bit 0~7 of regVal
			}

			return regVal;
		}
 	ARRAY_I CapDataProcess(ARRAY_I  capData){
  		ARRAY_I processedData;
  		processedData.resize(capData.size());
  		processedData.init(0);
  		for(int i=0;i<capData.size();i++){
//  			cout<<"raw data:"<<dec<<capData[i]<<endl;
  			unsigned int tmpWord = reverse(capData[i]);
//  			cout<<"reverse data:"<<hex<<tmpWord<<endl;
  			ARRAY_I tmpByte = Word2Byte(tmpWord);
//  			cout<<"cut byte:"<<tmpByte<<endl;
  			unsigned int ProcessedWord = (tmpByte[0]<<24)+(tmpByte[1]<<16)+(tmpByte[2]<<8)+tmpByte[3];
  			processedData[i]= ProcessedWord;
  		}
  		return processedData;
  	}

  	//reverse raw 32-bit data,from MSB to LSB
	unsigned int
	reverse(register unsigned int x)
	{
//		cout<<"x:0x"<<hex<<x<<endl;
	    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
	    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
	    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
	    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
//	    cout<<"processed x:0x"<<((x >> 16) | (x << 16))<<endl;
	    return((x >> 16) | (x << 16));

	}
	//cut 32-bit data into 4 bytes
  	ARRAY_I Word2Byte(unsigned int inData){

  		  ARRAY_I byte;
  		  byte.resize(4);
  		  byte.init(0);

  		  byte[0] =  0x000000ff & inData;
  		  byte[1] = (0x0000ff00 & inData)>>8;
  		  byte[2] = (0x00ff0000 & inData)>>16;
  		  byte[3] = (0xff000000 & inData)>>24;

  		  return byte;
	}

	virtual void postParameterChange(const string& parameterIdentifier) {
		//add your code
	}

};
REGISTER_TESTMETHOD("bm_ac_tml.Shmoo_PinMargin.shmoo_nonce", shmoo_nonce);

class shmoo_nonce_multisite: public testmethod::TestMethod {
protected:
	int Cap_samples;//capture samples;
	int expect_cnt;//expect ticket count;
	int org_freq;//original frequency
	string vCap;//capture variable
	string Tper;
	string Vfrc;
	double TScale_UL;
	double TScale_LL;
	double VScale_UL;
	double VScale_LL;
	int T_points;
	int V_points;
	int Vclamp_EN;

protected:
	/**
	 *Initialize the parameter interface to the testflow.
	 *This method is called just once after a testsuite is created.
	 *
	 *Note: TestMethod API should not be used in this method.
	 */
	virtual void initialize() {
	    addParameter("Cap_samples", "int", &Cap_samples)
	    .setDefault("32")
	      .setComment("the samples of digital capture");
	    addParameter("vCap",       "string",    &vCap)
	    .setDefault("bynonce_work_xcore")
	      .setComment("the Capture variable,must be unique");
	    addParameter("expect_cnt",  "int",    &expect_cnt)
	    .setDefault("7040")
	      .setComment("the ticket counter bits");
	    addParameter("org_freq_MHz",  "int",    &org_freq)
	    .setDefault("650")
	      .setComment("the original frequency");
		addParameter("Tper", "string", &Tper).setDefault("per_40").setComment(
				"Period Spec");
		addParameter("Vfrc", "string", &Vfrc).setDefault("vdd").setComment(
				"Level Spec ");
		addParameter("TScale_UL", "double", &TScale_UL).setDefault("1.2").setComment(
				"Up Limit of Period Scale");
		addParameter("TScale_LL", "double", &TScale_LL).setDefault("0.8").setComment(
				"Low Limit of Period Scale");
		addParameter("VScale_UL", "double", &VScale_UL).setDefault("1.2").setComment(
				"Up Limit of Voltage Scale");
		addParameter("VScale_LL", "double", &VScale_LL).setDefault("0.8").setComment(
				"Low Limit of Voltage Scale");
		addParameter("T_points", "int", &T_points).setDefault("11").setComment(
				"Period Shmoo points");
		addParameter("V_points", "int", &V_points).setDefault("11").setComment(
				"Voltage Shmoo points");
		addParameter("Vclamp_EN", "int", &Vclamp_EN).setDefault("1").setComment(
				"1 enable, 0 disable");
	}

	/**
	 *This test is invoked per site.
	 */
	virtual void run() {
		// add your code

		TIMING_SPEC Tspec = Primary.getTimingSpec();
		LEVEL_SPEC Vspec = Primary.getLevelSpec();
		double Cur_T_value = Tspec.getSpecValue(Tper);
		double Cur_V_value = Vspec.getSpecValue(Vfrc);
		int X, Y;
		int T_points_1, T_points_0;
		int V_points_1, V_points_0;
		double period, DPS_Value;
		double VScale_UL_Val, VScale_LL_Val;
		double T_UL_Val, T_LL_Val, Vmax;
		double Vstep, Tstep;
		STRING suite_name;
		char tmp_v[256], tmp_t[256];

		static ARRAY_I	aiCapData;
		aiCapData.resize(Cap_samples);
		aiCapData.init(0);

		SPMU_TASK dcscale;

		char device_path[DM_MAX_DEVICE];

		GetDevPath(device_path);

		strcat(device_path, "/shmoo/shm.txt"); //default file path

		ON_FIRST_INVOCATION_BEGIN();
			cout << "the shmoo data path is " << device_path << endl;
		ON_FIRST_INVOCATION_END();

		ofstream fout;

		fout.open(device_path, ios_base::out | ios_base::app);

		if (fout.good() != 1)

		{
			cout	<< "================== File Open Error,pls check the output directory permission! =================="
					<< endl;
			return;
		}
		ON_FIRST_INVOCATION_BEGIN();
			GET_TESTSUITE_NAME(suite_name);

			cout << suite_name << endl;
			fout << suite_name << "\n";

			cout << "Cur_T_SPEC:" << Tper << "=" << Cur_T_value << "ns" << endl;
			fout << "Cur_T_SPEC:" << Tper << "=" << Cur_T_value << "ns" << endl;
			cout << "Current Freq:" << "=" << org_freq << "MHz" << endl;
			fout << "Current Freq:" << "=" << org_freq << "MHz" << endl;
			cout << "Cur_V_SPEC:" << Vfrc << "=" << Cur_V_value << "V" << endl;
			fout << "Cur_V_SPEC:" << Vfrc << "=" << Cur_V_value << "V" << endl;
		ON_FIRST_INVOCATION_END();
		//calculate steps
		if (T_points == 1) {
			Tstep = 0;
		}
		else {
			Tstep = Cur_T_value * (TScale_UL - TScale_LL) / (T_points - 1);
		}
		if (V_points == 1) {
			Vstep = 0;
		}
		else {
			Vstep = Cur_V_value * (VScale_UL - VScale_LL) / (V_points - 1);
		}

		T_points_1 = INT(T_points * (TScale_UL - 1) / (TScale_UL - TScale_LL));
		T_points_0 = INT(T_points * (1 - TScale_LL) / (TScale_UL - TScale_LL));
		V_points_1 = INT(V_points * (VScale_UL - 1) / (VScale_UL - VScale_LL));
		V_points_0 = INT(V_points * (1 - VScale_LL) / (VScale_UL - VScale_LL));
		T_LL_Val = Cur_T_value - Tstep * T_points_0;
		T_UL_Val = Cur_T_value + Tstep * T_points_1;
		VScale_LL_Val = Cur_V_value - Vstep * V_points_0;
		VScale_UL_Val = Cur_V_value + Vstep * V_points_1;
		Vmax = VScale_UL_Val;

		ON_FIRST_INVOCATION_BEGIN();
		//print current information
		cout << "T_LL_Val=" << T_LL_Val << "ns" << endl;
		cout << "T_UL_Val=" << T_UL_Val << "ns" << endl;
		cout << "F_UL_Val=" << org_freq*Cur_T_value/T_LL_Val << "MHz" << endl;
		cout << "F_LL_Val=" <<  org_freq*Cur_T_value/T_UL_Val << "MHz" << endl;
		cout << "VScale_LL_Val=" << VScale_LL_Val <<"V"<< endl;
		cout << "VScale_UL_Val=" << VScale_UL_Val <<"V"<< endl;

		fout << "T_LL_Val=" << T_LL_Val << "ns" << endl;
		fout << "T_UL_Val=" << T_UL_Val << "ns" << endl;
		fout << "F_LL_Val=" << org_freq*Cur_T_value/T_LL_Val << "MHz" << endl;
		fout << "F_UL_Val=" <<  org_freq*Cur_T_value/T_UL_Val << "MHz" << endl;
		fout << "VScale_LL_Val=" << VScale_LL_Val <<"V"<< endl;
		fout << "VScale_UL_Val=" << VScale_UL_Val <<"V"<< endl;
		ON_FIRST_INVOCATION_END();

		if (Vmax > 5) {
			cout << "###########################################" << endl;
			cout << "   Warning!!!!!" << endl;
			cout << "   The Vmax=" << Vmax << " " << " , >5V " << endl;
			cout << "   Please make sure if it is safe!  " << endl;
			cout << "   If yes,pls modify the TestMethod specially!" << endl;
			cout << "###########################################" << endl;
			return;
		}

//		cout << Tper << "(nS)\t" << endl;
//		fout << Tper << "(nS)\t" << "\n";

		cout << "frequency" << "(MHz)\t" << endl;
		fout << "frequency" << "(MHZ)\t" << "\n";

		ON_FIRST_INVOCATION_BEGIN();
			CONNECT();
			DIGITAL_CAPTURE_TEST();
		ON_FIRST_INVOCATION_END();

		if ((VScale_UL <= 1.5 && VScale_LL >= 0.5) || Vclamp_EN == 0) {
			for (Y = 0; Y < T_points; Y++) {

				period = T_UL_Val - Tstep * Y;

				ON_FIRST_INVOCATION_BEGIN()
					 DISCONNECT();
					 WAIT_TIME(10 ms);
					 CONNECT();
					 WAIT_TIME(10 ms);
					Tspec.change(Tper, period);
					FLUSH(TM::APRM);
				ON_FIRST_INVOCATION_END();
				printf("%6.1f\t", org_freq*Cur_T_value/period);
				sprintf(tmp_t, "%6.1f", org_freq*Cur_T_value/period);
				fout << tmp_t << "\t";

				for (X = 0; X < V_points; X++) {

					DPS_Value = VScale_LL_Val + Vstep * X;

					ON_FIRST_INVOCATION_BEGIN()
						Vspec.change(Vfrc, DPS_Value);
						FLUSH(TM::APRM);
						DIGITAL_CAPTURE_TEST();
					ON_FIRST_INVOCATION_END();

					// get capture value for each site

					aiCapData = VECTOR(vCap).getVectors();//get back the capture data

					long regVal = CaptDataProcess(aiCapData);

					DOUBLE pct_return =  regVal/expect_cnt *100;//calculate the percentage of samples/(expect nonce cnt)
					//print shmoo data

					printf("%5.2f%s\t", pct_return,"%");
					sprintf(tmp_v, "%5.2f%s", pct_return,"%");
					fout << tmp_v << "\t";

				}

				cout << endl;
				fout << "\n";

			}

			cout << " \t";
			fout << " \t";

			for (X = 0; X < V_points; X++)

			{
				DPS_Value = VScale_LL_Val + Vstep * X;

				printf("%5.3f\t", DPS_Value);
				sprintf(tmp_v, "%5.3f", DPS_Value);
				fout <<"  \t"<< tmp_v ;
			}
			cout << " :" << Vfrc << "(" << Cur_V_value << "V)" << endl;
			fout << " :" << Vfrc << "(" << Cur_V_value << "V)" << "\n" << "\n";
			fout.close();
		}

		else {
			cerr << "###############################" << endl;
			cerr << "Warning!!!!!" << endl;
			cerr << "The VScale_UL or VScale_LL is out of Range(0.5,1.5) "
					<< endl;
			cerr
					<< "Please check the level set,the Vfrc should be coefficent of Voltage"
					<< endl;
			cerr
					<< "If you make sure Vfrc is safety, pls change the Vclamp_EN=0,then continue"
					<< endl;
			cerr << "###############################" << endl;
		}

		ON_FIRST_INVOCATION_BEGIN();
			Tspec.restore();
			FLUSH(TM::APRM);
			Vspec.restore();
			FLUSH(TM::APRM);
		ON_FIRST_INVOCATION_END();

		return;
	}


	long  CaptDataProcess(ARRAY_I  capData)
		{
		  long regVal= 0; //initial regVal
			for(int i = 0; i< capData.size();i++){
				if(i<=7) {regVal = regVal | capData[i]<<(24+i);} //bit0~7 shift as the bit 24~31 of regVal
				else if(i>7 && i<=15) {regVal = regVal | capData[i]<<(8+i);}//bit8~15 shift as the bit 16~23 of regVal
				else if(i>15 && i<=23) {regVal = regVal | capData[i]<<(i-8);}//bit16~23 shift as the bit 8~15 of regVal
				else  {regVal = regVal | capData[i]<<(i-24);}//bit24~31 shift as the bit 0~7 of regVal
			}

			return regVal;
		}

	/**
	 *This function will be invoked once the specified parameter's value is changed.
	 *@param parameterIdentifier
	 *
	 *Note: TestMethod API should not be used in this method.
	 */
	virtual void postParameterChange(const string& parameterIdentifier) {
		//add your code
	}

};
REGISTER_TESTMETHOD("bm_ac_tml.Shmoo_PinMargin.shmoo_nonce_multisite", shmoo_nonce_multisite);

class SHM_XVol_YPer: public testmethod::TestMethod
{
protected:
  string  Tper;
  string  VScale;
  double  TScale_UL;
  double  TScale_LL;
  double  VScale_UL;
  double  VScale_LL;
  int  T_points;
  int  V_points;
  int  Vclamp_EN;


protected:
/**
 *Initialize the parameter interface to the testflow.
 *This method is called just once after a testsuite is created.
 *
 *Note: TestMethod API should not be used in this method.
 */
virtual void initialize()
{
  addParameter("Tper",
               "string",
               &Tper)
    .setDefault("per_40")
    .setComment("Period Spec");
  addParameter("VScale",
               "string",
               &VScale)
    .setDefault("VScale")
    .setComment("Pls define VScale in Lvl Spec ");
  addParameter("TScale_UL",
               "double",
               &TScale_UL)
    .setDefault("1.5")
    .setComment("Up Limit of Period Scale");
  addParameter("TScale_LL",
               "double",
               &TScale_LL)
    .setDefault("0.5")
    .setComment("Low Limit of Period Scale");
  addParameter("VScale_UL",
               "double",
               &VScale_UL)
    .setDefault("1.5")
    .setComment("Up Limit of Voltage Scale");
  addParameter("VScale_LL",
               "double",
               &VScale_LL)
    .setDefault("0.5")
    .setComment("Low Limit of Voltage Scale");
  addParameter("T_points",
               "int",
               &T_points)
    .setDefault("11")
    .setComment("Period Shmoo points");
  addParameter("V_points",
               "int",
               &V_points)
    .setDefault("11")
    .setComment("Voltage Shmoo points");
  addParameter("Vclamp_EN",
               "int",
               &Vclamp_EN)
    .setDefault("1")
    .setComment("1 enable, 0 disable");

}

/**
 *This test is invoked per site.
 */
virtual void run()
{
	  // add your code

	    TIMING_SPEC  Tspec=Primary.getTimingSpec();
	    LEVEL_SPEC   Vspec=Primary.getLevelSpec();
	    double Cur_T_value=Tspec.getSpecValue(Tper);
	    double Cur_V_value=Vspec.getSpecValue(VScale);
	    int X,Y;
	    int T_points_1,T_points_0;
	    int V_points_1,V_points_0;
	    double period,DPS_Value;
	    double VScale_UL_Val,VScale_LL_Val;
	    double T_UL_Val,T_LL_Val,Vmax;
	    double Vstep,Tstep;
	    bool result;
	    STRING suite_name;
	    char  tmp_v[256],tmp_t[256];

	    SPMU_TASK dcscale;

		char device_path[DM_MAX_DEVICE];
		GetDevPath(device_path);
		strcat (device_path, "/shmoo/shm.txt");

		cout << "path is " << device_path << endl;

	    ofstream fout;

	    fout.open(device_path,ios_base::out|ios_base::app);

	    if(fout.good()!=1)

	     {
	    	cout<<"================== File Open Error,pls check the output directory permission! =================="<<endl;
	        return;
	     }

	    GET_TESTSUITE_NAME(suite_name);
	    cout<<suite_name<<endl;
	    fout<<suite_name<<"\n";

	    cout<<"Cur_T_SPEC:"<<Tper<<"="<<Cur_T_value<<"ns"<<endl;
	    fout<<"Cur_T_SPEC:"<<Tper<<"="<<Cur_T_value<<"ns"<<endl;

	     if(Cur_V_value==1)
	      {
	       cout<<"Cur_V_SPEC:"<<VScale<<"="<<Cur_V_value<<"*100%"<<endl;
	       fout<<"Cur_V_SPEC:"<<VScale<<"="<<Cur_V_value<<"*100%"<<endl;
	      }


	    else
	      {
	       cout<<"Cur_V_SPEC_value:"<<VScale<<"="<<Cur_V_value<<"v"<<endl;
	       fout<<"Cur_V_SPEC_value:"<<VScale<<"="<<Cur_V_value<<"v"<<endl;
	      }

	   if(T_points==1){
		   Tstep=0;
	   }

	   else
	   {

	    Tstep=Cur_T_value*(TScale_UL-TScale_LL)/(T_points-1);

	   }


	   if(V_points==1)
	    {
		   Vstep=0;
	    }

	   else
	   {
		   Vstep=Cur_V_value*(VScale_UL-VScale_LL)/(V_points-1);
	   }

	    T_points_1=INT(T_points*(TScale_UL-1)/(TScale_UL-TScale_LL));
	    T_points_0=INT(T_points*(1-TScale_LL)/(TScale_UL-TScale_LL));
	    V_points_1=INT(V_points*(VScale_UL-1)/(VScale_UL-VScale_LL));
	    V_points_0=INT(V_points*(1-VScale_LL)/(VScale_UL-VScale_LL));
	    T_LL_Val=Cur_T_value-Tstep*T_points_0;
	    T_UL_Val=Cur_T_value+Tstep*T_points_1;
	    VScale_LL_Val=Cur_V_value-Vstep*V_points_0;
	    VScale_UL_Val=Cur_V_value+Vstep*V_points_1;
	    Vmax=VScale_UL_Val;
	    cout<<"T_LL_Val="<<T_LL_Val<<"ns"<<endl;
	    cout<<"T_UL_Val="<<T_UL_Val<<"ns"<<endl;
	    cout<<"VScale_LL_Val="<<VScale_LL_Val<<endl;
	    cout<<"VScale_UL_Val="<<VScale_UL_Val<<endl;

	    fout<<"T_LL_Val="<<T_LL_Val<<"ns"<<endl;
	    fout<<"T_UL_Val="<<T_UL_Val<<"ns"<<endl;
	    fout<<"VScale_LL_Val="<<VScale_LL_Val<<endl;
	    fout<<"VScale_UL_Val="<<VScale_UL_Val<<endl;

	    if (Vmax>5)
	     {
	      cout<<"###########################################"<<endl;
	      cout<<"   Warning!!!!!"<<endl;
	      cout<<"   The Vmax="<<Vmax<<" "<<" , >5V "<<endl;
	      cout<<"   Please make sure if it is safe!  "<<endl;
	      cout<<"   If yes,pls modify the TestMethod specially!"<<endl;
	      cout<<"###########################################"<<endl;
	      return;
	     }

	    cout<<Tper<<"(nS)\t"<<endl;
	    fout<<Tper<<"(nS)\t"<<endl;
	     DISCONNECT();
	     CONNECT();

	     FUNCTIONAL_TEST();

	  if((VScale_UL<=1.5&&VScale_LL>=0.5)||Vclamp_EN==0)
	  {
	    for(Y=0;Y<T_points;Y++)
	    {
	        period=T_UL_Val-Tstep*Y;
	        Tspec.change(Tper,period);
	        printf("%5.2f\t",period);
	        sprintf(tmp_t,"%5.2f",period);
	        fout<< tmp_t<<"\t";
	        FLUSH(TM::APRM);

	        for(X=0;X<V_points;X++)
	        {
	        	DPS_Value=VScale_LL_Val+Vstep*X;
	            Vspec.change(VScale,DPS_Value);
	            FLUSH(TM::APRM);

	            FUNCTIONAL_TEST();

	            result=GET_FUNCTIONAL_RESULT();

	            if(result)
	             {
	               if(DPS_Value<=(Cur_V_value+0.000001)&&DPS_Value>=(Cur_V_value-0.000001)&&period<=(Cur_T_value+0.0000001)&&period>=(Cur_T_value-0.0000001))
	               {
					   cout<<setw(5)<<"*\t";
					   fout<<setw(5)<<"*\t";
	               }
	               else
	               {
					   cout<<setw(5)<<"P\t";
					   fout<<setw(5)<<"P\t";
	               }
	             }
	            else
	             {

	               if(DPS_Value<=(Cur_V_value+0.000001)&&DPS_Value>=(Cur_V_value-0.000001)&&period<=(Cur_T_value+0.0000001)&&period>=(Cur_T_value-0.0000001))
	               {
					   cout<<setw(5)<<"#\t";
					   fout<<setw(5)<<"#\t";
	               }
	               else
	               {
					   cout<<setw(5)<<".\t";
					   fout<<setw(5)<<".\t";

	               }
	             }
	        }
	        cout<<endl;
	        fout<<"\n";
	    }
	    cout<<"\t";
	    fout<<"\t\t";

	    for(X=0;X<V_points;X++)

	    {
	       DPS_Value=VScale_LL_Val+Vstep*X;
	       printf("%5.2f\t",DPS_Value);
	       sprintf(tmp_v,"%5.2f",DPS_Value);
	       fout<< tmp_v<<"\t";
	    }

	    cout<<" :"<<VScale<<"("<<Cur_V_value<<"*100%V)"<<endl;
	    fout<<" :"<<VScale<<"("<<Cur_V_value<<"*100%V)"<<"\n"<<"\n";
	    fout.close();
	  }

	 else
	 {  cerr<<"###############################"<<endl;
	    cerr<<"Warning!!!!!"<<endl;
	    cerr<<"The VScale_UL or VScale_LL is out of Range(0.5,1.5) "<<endl;
	    cerr<<"Please check the level set,the VScale should be coefficent of Voltage"<<endl;
	    cerr<<"If you make sure VScale is safety, pls change the Vclamp_EN=0,then continue"<<endl;
	    cerr<<"###############################"<<endl;
	 }

	    Tspec.restore();
	    FLUSH(TM::APRM);
	    Vspec.restore();
	    FLUSH(TM::APRM);

	    return;
}

/**
 *This function will be invoked once the specified parameter's value is changed.
 *@param parameterIdentifier
 *
 *Note: TestMethod API should not be used in this method.
 */
virtual void postParameterChange(const string& parameterIdentifier)
{
  //add your code
}

};
REGISTER_TESTMETHOD("bm_ac_tml.Shmoo_PinMargin.SHM_XVol_YPer",SHM_XVol_YPer);

class SHM_XVol_YPer_MS: public testmethod::TestMethod {
protected:
	string TPer;
	string VScale;
	double TScale_UL;
	double TScale_LL;
	double VScale_UL;
	double VScale_LL;
	int T_points;
	int V_points;
	int Vclamp_EN;
	string File_Path;
	string Functional_Result[16][1024];  //16 is the Max Site Number, 1024 means the biggest elment can store in the array


protected:
	/**
	 *Initialize the parameter interface to the testflow.
	 *This method is called just once after a testsuite is created.
	 *
	 *Note: TestMethod API should not be used in this method.
	 */


	virtual void initialize() {
		addParameter("TPer", "string", &TPer).setDefault("per_xxx").setComment(
				"Period Spec");
		addParameter("VScale", "string", &VScale).setDefault("VScale").setComment(
				"Pls define VScale in Lvl Spec ");
		addParameter("TScale_UL", "double", &TScale_UL).setDefault("1.5").setComment(
				"Up Limit of Period Scale");
		addParameter("TScale_LL", "double", &TScale_LL).setDefault("0.5").setComment(
				"Low Limit of Period Scale");
		addParameter("VScale_UL", "double", &VScale_UL).setDefault("1.2").setComment(
				"Up Limit of Voltage Scale");
		addParameter("VScale_LL", "double", &VScale_LL).setDefault("0.8").setComment(
				"Low Limit of Voltage Scale");
		addParameter("T_points", "int", &T_points).setDefault("10").setComment(
				"Period Shmoo points");
		addParameter("V_points", "int", &V_points).setDefault("10").setComment(
				"Voltage Shmoo points");
		addParameter("Vclamp_EN", "int", &Vclamp_EN).setDefault("1").setComment(
				"1 enable, 0 disable");
		addParameter("File_Path", "string", &File_Path).setDefault("/tmp/SHM.txt");
	}

	/**
	 *This test is invoked per site.
	 */
	virtual void run() {
		// add your code
		TIMING_SPEC Tspec = Primary.getTimingSpec();
		LEVEL_SPEC Vspec = Primary.getLevelSpec();
		double Cur_T_value = Tspec.getSpecValue(TPer);
		double Cur_V_value = Vspec.getSpecValue(VScale);
		int X, Y;
		int T_points_1, T_points_0;
		int V_points_1, V_points_0;
		double period, DPS_Value;
		double VScale_UL_Val, VScale_LL_Val;
		double T_UL_Val, T_LL_Val;
		double Vstep, Tstep;

		Tstep = Cur_T_value * (TScale_UL - TScale_LL) / (T_points - 1);
		Vstep = Cur_V_value * (VScale_UL - VScale_LL) / (V_points - 1);
		T_points_1 = INT(T_points * (TScale_UL - 1) / (TScale_UL - TScale_LL));
		T_points_0 = INT(T_points * (1 - TScale_LL) / (TScale_UL - TScale_LL));
		V_points_1 = INT(V_points * (VScale_UL - 1) / (VScale_UL - VScale_LL));
		V_points_0 = INT(V_points * (1 - VScale_LL) / (VScale_UL - VScale_LL));

		T_LL_Val = Cur_T_value - Tstep * T_points_0;
		T_UL_Val = Cur_T_value + Tstep * T_points_1;
		VScale_LL_Val = Cur_V_value - Vstep * V_points_0;
		VScale_UL_Val = Cur_V_value + Vstep * V_points_1;


		string period_mark[32];
		string voltage_mark[32];



		ON_FIRST_INVOCATION_BEGIN();

		CONNECT();
		FUNCTIONAL_TEST();

		if ((VScale_UL_Val <= 1.5 && VScale_LL_Val >= 0.5) || Vclamp_EN == 0) {
			for (Y = 0; Y < T_points; Y++) {
				period = T_UL_Val - Tstep * Y;
				Tspec.change(TPer, period);

				char tmp_t[256];
				sprintf(tmp_t, "%5.2f", period);
				period_mark[Y] = tmp_t;
				FLUSH(TM::APRM);

				for (X = 0; X < V_points; X++) {
					DPS_Value = VScale_LL_Val + Vstep * X;
					Vspec.change(VScale, DPS_Value);
					sprintf(tmp_t, "%5.2f", DPS_Value);
					voltage_mark[X] = tmp_t;


					FLUSH(TM::APRM);
					FUNCTIONAL_TEST();


					FOR_EACH_SITE_BEGIN();

					Boolean Func_Result = GET_FUNCTIONAL_RESULT();

					if(DPS_Value<=(Cur_V_value+0.000001)&&DPS_Value>=(Cur_V_value-0.000001)&&period<=(Cur_T_value+0.0000001)&&period>=(Cur_T_value-0.0000001))
					{
						Functional_Result[CURRENT_SITE_NUMBER() - 1][(X+1)*(Y+1) -1] = (Func_Result ? " *\t" : " #\t");
					}
					else
					{
						Functional_Result[CURRENT_SITE_NUMBER() - 1][(X+1)*(Y+1) -1] = (Func_Result ? " P\t" : " .\t");
					}

					FOR_EACH_SITE_END();
				}
			}
		}
		else {
			cerr << "###############################" << endl;
			cerr << "Warning!!!!!" << endl;
			cerr
			<< "The VScale_UL_Val or VScale_LL_Val is out of Range(0.5,1.5) "
			<< endl;
			cerr
			<< "Please check the level set,the VScale should be coefficent of Voltage"
			<< endl;
			cerr
			<< "If you make sure VScale is safe, pls change the Vclamp_EN=0,then continue"
			<< endl;
			cerr << "###############################" << endl;
			return ;

		}

		Tspec.restore();
		FLUSH(TM::APRM);
		Vspec.restore();
		FLUSH(TM::APRM);

		STRING suite_name;


		char device_path[DM_MAX_DEVICE];

		GetDevPath(device_path);

		strcat (device_path, "/shmoo/shm.txt");

		cout << "path is " << device_path << endl;
	    ofstream fout;

	    fout.open(device_path,ios_base::out|ios_base::app);


		GET_TESTSUITE_NAME(suite_name);
		cout<<suite_name<<"\n";
		fout<<suite_name<<"\n";
		cout
		<< "-------------------------------------------------------------"
		<< endl;

		cout << "Cur_T_SPEC_value:" << TPer << "=" << Cur_T_value << "ns"
				<< endl;
		cout << "Cur_V_SPEC_value:" << VScale << "=" << Cur_V_value << endl;
		fout << "Cur_T_SPEC_value:" << TPer << "=" << Cur_T_value << "ns"
				<< endl;
		fout << "Cur_V_SPEC_value:" << VScale << "=" << Cur_V_value << endl;
		fout << "T_LL_Val=" << T_LL_Val << "ns" << endl;
		fout << "T_UL_Val=" << T_UL_Val << "ns" << endl;
		fout << "VScale_LL_Val=" << VScale_LL_Val << endl;
		fout << "VScale_UL_Val=" << VScale_UL_Val << endl;

		cout << TPer << "(nS)\t" << endl;
		fout << TPer << "(nS)\t" << "\n";

		FOR_EACH_SITE_BEGIN();

		int iSite = CURRENT_SITE_NUMBER() - 1;

		string T_string;

		for (int index_T= 0; index_T < T_points+1 ; index_T++)
		{
			if(index_T <  T_points)
			{
				T_string = " "+period_mark[index_T]+"\t";


				for(int index_V = 0 ; index_V < V_points ; index_V++)
				{
					T_string = T_string + Functional_Result[iSite][(index_T+1)*(index_V+1) - 1];
				}

			}
			else
			{
				T_string= "\t";
				for(int index_V = 0 ; index_V < V_points ; index_V++)
				{
					T_string = T_string + voltage_mark[index_V]+"\t";
				}
				T_string = T_string +":" + VScale +"(1*100%v)" ;
			}

			cout << T_string << endl;
			fout << T_string << "\n" ;
		}

		FOR_EACH_SITE_END();

		fout << "\n";
		fout.close();

		ON_FIRST_INVOCATION_END();


		return;
	}

	/**
	 *This function will be invoked once the specified parameter's value is changed.
	 *@param parameterIdentifier
	 *
	 *Note: TestMethod API should not be used in this method.
	 */
	virtual void postParameterChange(const string& parameterIdentifier) {
		//add your code
	}

};
REGISTER_TESTMETHOD("bm_ac_tml.Shmoo_PinMargin.SHM_XVol_YPer_MS",SHM_XVol_YPer_MS);



class SHM_XVol_YPer_MP: public testmethod::TestMethod
{
protected:
  string  Tper;
  string  VScale;
  double  TScale_UL;
  double  TScale_LL;
  double  VScale_UL;
  double  VScale_LL;
  int  T_points;
  int  V_points;
  int  Vclamp_EN;


protected:
/**
 *Initialize the parameter interface to the testflow.
 *This method is called just once after a testsuite is created.
 *
 *Note: TestMethod API should not be used in this method.
 */
virtual void initialize()
{
  addParameter("Tper",
               "string",
               &Tper)
    .setDefault("per_xxx@pCLK")
    .setComment("CLK Period");
  addParameter("VScale",
               "string",
               &VScale)
    .setDefault("VScale")
    .setComment("Pls define VScale in Lvl Spec ");
  addParameter("TScale_UL",
               "double",
               &TScale_UL)
    .setDefault("1.5")
    .setComment("Up Limit of Period Scale");
  addParameter("TScale_LL",
               "double",
               &TScale_LL)
    .setDefault("0.5")
    .setComment("Low Limit of Period Scale");
  addParameter("VScale_UL",
               "double",
               &VScale_UL)
    .setDefault("1.2")
    .setComment("Up Limit of Voltage Scale");
  addParameter("VScale_LL",
               "double",
               &VScale_LL)
    .setDefault("0.8")
    .setComment("Low Limit of Voltage Scale");
  addParameter("T_points",
               "int",
               &T_points)
    .setDefault("11")
    .setComment("Period Shmoo points");
  addParameter("V_points",
               "int",
               &V_points)
    .setDefault("11")
    .setComment("Voltage Shmoo points");
  addParameter("Vclamp_EN",
               "int",
               &Vclamp_EN)
    .setDefault("1")
    .setComment("1 enable, 0 disable");
}

/**
 *This test is invoked per site.
 */
virtual void run()
{
  // add your code
    SPECIFICATION  Tspec=Primary.getSpecification();
    LEVEL_SPEC   Vspec=Primary.getLevelSpec();

    double Cur_T_value=Tspec.getSpecValue(Tper);
    double Cur_V_value=Vspec.getSpecValue(VScale);
    int X,Y;
    int T_points_1,T_points_0;
    int V_points_1,V_points_0;
    double period,DPS_Value;
    double VScale_UL_Val,VScale_LL_Val;
    double T_UL_Val,T_LL_Val,Vmax;
    double Vstep,Tstep;
    bool result;
    STRING suite_name;
    char  tmp_v[256],tmp_t[256];

	char device_path[DM_MAX_DEVICE];

	GetDevPath(device_path);

	strcat (device_path, "/shmoo/shm.txt");

	cout << "path is " << device_path << endl;

    ofstream fout;

    fout.open(device_path,ios_base::out|ios_base::app);

    if(fout.good()!=1)
     {
      cout<<"================== File Open Error,pls check the output directory permission! =================="<<endl;
       return;
     }


    GET_TESTSUITE_NAME(suite_name);
    cout<<suite_name<<endl;
    fout<<suite_name<<"\n";

    cout<<"Cur_T_SPEC:"<<Tper<<"="<<Cur_T_value<<"ns"<<endl;
    fout<<"Cur_T_SPEC:"<<Tper<<"="<<Cur_T_value<<"ns"<<endl;


     if(Cur_V_value==1)
      {
       cout<<"Cur_V_SPEC:"<<VScale<<"="<<Cur_V_value<<"*100%"<<endl;
       fout<<"Cur_V_SPEC:"<<VScale<<"="<<Cur_V_value<<"*100%"<<endl;
      }
    else
      {
       cout<<"Cur_V_SPEC_value:"<<VScale<<"="<<Cur_V_value<<"v"<<endl;
       fout<<"Cur_V_SPEC_value:"<<VScale<<"="<<Cur_V_value<<"v"<<endl;
      }


   if(T_points==1){
	   Tstep=0;
   }
   else
   {
	   Tstep=Cur_T_value*(TScale_UL-TScale_LL)/(T_points-1);
   }

   if(V_points==1){
	   Vstep=0;
   }
   else
   {
	   Vstep=Cur_V_value*(VScale_UL-VScale_LL)/(V_points-1);
   }

    T_points_1=INT(T_points*(TScale_UL-1)/(TScale_UL-TScale_LL));
    T_points_0=INT(T_points*(1-TScale_LL)/(TScale_UL-TScale_LL));
    V_points_1=INT(V_points*(VScale_UL-1)/(VScale_UL-VScale_LL));
    V_points_0=INT(V_points*(1-VScale_LL)/(VScale_UL-VScale_LL));

    T_LL_Val=Cur_T_value-Tstep*T_points_0;
    T_UL_Val=Cur_T_value+Tstep*T_points_1;
    VScale_LL_Val=Cur_V_value-Vstep*V_points_0;
    VScale_UL_Val=Cur_V_value+Vstep*V_points_1;
    Vmax=VScale_UL_Val;

    cout<<"T_LL_Val="<<T_LL_Val<<"ns"<<endl;
    cout<<"T_UL_Val="<<T_UL_Val<<"ns"<<endl;
    cout<<"VScale_LL_Val="<<VScale_LL_Val<<endl;
    cout<<"VScale_UL_Val="<<VScale_UL_Val<<endl;

    fout<<"T_LL_Val="<<T_LL_Val<<"ns"<<endl;
    fout<<"T_UL_Val="<<T_UL_Val<<"ns"<<endl;
    fout<<"VScale_LL_Val="<<VScale_LL_Val<<endl;
    fout<<"VScale_UL_Val="<<VScale_UL_Val<<endl;


    if (Cur_V_value!=1&&Vclamp_EN==1)
    {cout<<"###########################################"<<endl;
     cout<<"  Warning!!!!!"<<endl;
     cout<<"   Cur_V_value!=1"<<endl;
     cout<<"   Step1: Please make sure if there are some abnormal errors which not restore the spec in current or front testsuite!"<<endl;
     cout<<"          If yes, you should repair the error and download level&timing spec!"<<endl;
     cout<<"   Step2: if you make sure the VScale spec is absolutely Power value and there no Tester system error,"<<endl;
     cout<<"         Pls download the timing&spec,and  switch the Vclamp_EN=0,then continue and be careful!!"<<endl;
     cout<<"###########################################"<<endl;
     return;
    }

    if (Vmax>5)
     {

      cout<<"###########################################"<<endl;
      cout<<"   Warning!!!!!"<<endl;
      cout<<"   The Vmax="<<Vmax<<" "<<" , >5V "<<endl;
      cout<<"   Please make sure if it is safe!  "<<endl;
      cout<<"   If yes,pls modify the TestMethod specially!"<<endl;
      cout<<"###########################################"<<endl;
      return;
     }


    cout<<Tper<<"(nS)\t"<<endl;
    fout<<Tper<<"(nS)\t"<<"\n";

     CONNECT();
     FUNCTIONAL_TEST();

  if((VScale_UL<=1.5&&VScale_LL>=0.5)||Vclamp_EN==0)
  {
    for(Y=0;Y<T_points;Y++)
    {
        period=T_UL_Val-Tstep*Y;
        Tspec.change(Tper,period);

        printf("%6.1f\t",period);
        sprintf(tmp_t,"%6.1f",period);
        fout<< tmp_t<<"\t";
        FLUSH(TM::APRM);


        for(X=0;X<V_points;X++)
        { DPS_Value=VScale_LL_Val+Vstep*X;
            Vspec.change(VScale,DPS_Value);
            FLUSH(TM::APRM);
            FUNCTIONAL_TEST();

            result=GET_FUNCTIONAL_RESULT();
            if(result)
             {
               if(DPS_Value<=(Cur_V_value+0.000001)&&DPS_Value>=(Cur_V_value-0.000001)&&period<=(Cur_T_value+0.0000001)&&period>=(Cur_T_value-0.0000001))
               {
               cout<<setw(5)<<"*";
               fout<<"*\t";
               }
               else
               {
               cout<<setw(5)<<"P";
               fout<<"P\t";
               }
             }
            else
             {
               if(DPS_Value<=(Cur_V_value+0.000001)&&DPS_Value>=(Cur_V_value-0.000001)&&period<=(Cur_T_value+0.0000001)&&period>=(Cur_T_value-0.0000001))
               {
               cout<<setw(5)<<"#";
               fout<<"#\t";
               }
               else
               {
               cout<<setw(5)<<".";
               fout<<".\t";
               }
             }
        }
        cout<<endl;
        fout<<"\n";
    }

    cout<<"\t";
    fout<<"\t";
    for(X=0;X<V_points;X++)
    {
       DPS_Value=VScale_LL_Val+Vstep*X;
       printf("%5.2f",DPS_Value);
       sprintf(tmp_v,"%5.2f",DPS_Value);

       fout<< tmp_v<<"\t";
    }

    cout<<" :"<<VScale<<"("<<Cur_V_value<<"*%v)"<<endl;
    fout<<":"<<VScale<<"("<<Cur_V_value<<"*%v)"<<"\n"<<"\n";
    fout.close();
  }

 else
 {  cerr<<"###############################"<<endl;
    cerr<<"Warning!!!!!"<<endl;
    cerr<<"The VScale_UL or VScale_LL is out of Range(0.5,1.5) "<<endl;
    cerr<<"Please check the level set,the VScale should be coefficent of Voltage"<<endl;
     cerr<<"If you make sure VScale is safety, pls change the Vclamp_EN=0,then continue"<<endl;
    cerr<<"###############################"<<endl;
 }

    Tspec.restore();
    FLUSH(TM::APRM);
    Vspec.restore();
    FLUSH(TM::APRM);
    return;
}

/**
 *This function will be invoked once the specified parameter's value is changed.
 *@param parameterIdentifier
 *
 *Note: TestMethod API should not be used in this method.
 */
virtual void postParameterChange(const string& parameterIdentifier)
{
  //add your code
}

};
REGISTER_TESTMETHOD("bm_ac_tml.Shmoo_PinMargin.SHM_XVol_YPer_MP",SHM_XVol_YPer_MP);


/**
 *Testmethod class.
 *
 *For each testsuite using this testmethod, one object of this
 *class is created.
 */
class PM_Roff: public testmethod::TestMethod
{
protected:
  string  Tper;
  string  Roff;
  double  Roff_LL;
  double  Roff_UL;
  int  R_points;

protected:
/**
 *Initialize the parameter interface to the testflow.
 *This method is called just once after a testsuite is created.
 *
 *Note: TestMethod API should not be used in this method.
 */
virtual void initialize()
{
  addParameter("Tper",
               "string",
               &Tper)
    .setDefault("per_40")
    .setComment("Current Period Spec");
  addParameter("Roff",
               "string",
               &Roff)
    .setDefault("Roff")
    .setComment("Strobe Offset Spec ");
  addParameter("Roff_LL",
               "double",
               &Roff_LL)
    .setDefault("-20")
    .setComment("Roff Low Limit (ns)");
  addParameter("Roff_UL",
               "double",
               &Roff_UL)
    .setDefault("20")
    .setComment("Roff Up Limit (ns)");
  addParameter("R_points",
               "int",
               &R_points)
    .setDefault("21")
    .setComment("Strobe points");
}

/**
 *This test is invoked per site.
 */
virtual void run()
{
	TIMING_SPEC  RTim_spec=Primary.getTimingSpec();
	TIMING_SPEC  TTim_spec=Primary.getTimingSpec();

	double Cur_Roff_value=RTim_spec.getSpecValue(Roff);
	double Cur_T_value=TTim_spec.getSpecValue(Tper);

    int X;
    int R_points_1,R_points_0;
    double Rtmp=0;
    double Roff_UL_ADJ,Roff_LL_ADJ;
    double Rstep;
    bool result;
    STRING suite_name;
    char  tmp_r[256];

	char device_path[DM_MAX_DEVICE];

	GetDevPath(device_path);

	strcat (device_path, "/shmoo/pm.txt");

	cout << "path is " << device_path << endl;
    ofstream fout;
    fout.open(device_path,ios_base::out|ios_base::app);
    fout.precision(3);

      if(fout.good()!=1)
     {
      cout<<"================== File Open Error,pls check the output directory permission! =================="<<endl;
       return;
     }


    GET_TESTSUITE_NAME(suite_name);
    cout<<suite_name<<"\n";
    fout<<suite_name<<"\n";

    cout<<"Cur_T_SPEC:"<<Tper<<"="<<Cur_T_value<<"ns"<<endl;
    fout<<"Cur_T_SPEC:"<<Tper<<"="<<Cur_T_value<<"ns"<<endl;
    cout<<"Cur_Roff_SPEC:"<<Roff<<"="<<Cur_Roff_value<<"ns"<<endl;
    fout<<"Cur_Roff_SPEC:"<<Roff<<"="<<Cur_Roff_value<<"ns"<<endl;

    Rstep=(Roff_UL-Roff_LL)/(R_points-1);
    R_points_1=INT(R_points*(Roff_UL-Cur_Roff_value)/(Roff_UL-Roff_LL));
    R_points_0=INT(R_points*(Cur_Roff_value-Roff_LL)/(Roff_UL-Roff_LL));

    Roff_LL_ADJ=Cur_Roff_value-Rstep*R_points_0;
    Roff_UL_ADJ=Cur_Roff_value+Rstep*R_points_1;


    cout<<"Roff_LL_ADJ="<<Roff_LL_ADJ<<"ns"<<endl;
    cout<<"Roff_UL_ADJ="<<Roff_UL_ADJ<<"ns"<<endl;
    fout<<"Roff_LL_ADJ="<<Roff_LL_ADJ<<"ns"<<endl;
    fout<<"Roff_UL_ADJ="<<Roff_UL_ADJ<<"ns"<<endl;

     CONNECT();
     FUNCTIONAL_TEST();

        for(X=0;X<R_points;X++)
        { Rtmp=Roff_LL_ADJ+Rstep*X;
        	RTim_spec.change(Roff,Rtmp);
            FLUSH(TM::APRM);
            FUNCTIONAL_TEST();

            result=GET_FUNCTIONAL_RESULT();
            if(result)
             {
               if(Rtmp<=(Cur_Roff_value+0.0000001)&&Rtmp>=(Cur_Roff_value-0.0000001))
               {
               cout<<setw(5)<<"*";
               fout<<setw(5)<<"*";
               }
               else
               {
               cout<<setw(5)<<"P";
               fout<<setw(5)<<"P";
               }
             }
            else
             {
               if(Rtmp<=(Cur_Roff_value+0.0000001)&&Rtmp>=(Cur_Roff_value-0.0000001))
               {
               cout<<setw(5)<<"#";
               fout<<setw(5)<<"#";
               }
               else
               {
               cout<<setw(5)<<".";
               fout<<setw(5)<<".";
               }
             }
        }

    cout<<"\t";
    fout<<"\t";

    cout<<endl;
    fout<<"\n";

    for(X=0;X<R_points;X++)
    {
       Rtmp=Roff_LL_ADJ+Rstep*X;
       printf("%5.0f",Rtmp);
       sprintf(tmp_r,"%5.0f",Rtmp);
       fout<<tmp_r;
    }
    cout<<" :"<<Roff<<"(ns)"<<endl;
    fout<<" :"<<Roff<<"(ns)"<<"\n";
    fout.close();

    RTim_spec.restore();
    FLUSH(TM::APRM);
    return;
}

/**
 *This function will be invoked once the specified parameter's value is changed.
 *@param parameterIdentifier
 *
 *Note: TestMethod API should not be used in this method.
 */
virtual void postParameterChange(const string& parameterIdentifier)
{
  //add your code
}

};
REGISTER_TESTMETHOD("bm_ac_tml.Shmoo_PinMargin.PM_Roff",PM_Roff);

class PM_Roff_MP: public testmethod::TestMethod
{
protected:
  string  TPer;
  string  Roff;
  double  Roff_LL;
  double  Roff_UL;
  int  R_points;


protected:
/**
 *Initialize the parameter interface to the testflow.
 *This method is called just once after a testsuite is created.
 *
 *Note: TestMethod API should not be used in this method.
 */
virtual void initialize()
{
  addParameter("TPer",
               "string",
               &TPer)
    .setDefault("tc@pTX")
    .setComment("Current Period Spec");
  addParameter("Roff",
               "string",
               &Roff)
    .setDefault("t_off@pTX")
    .setComment("Strobe Offset Spec ");
  addParameter("Roff_LL",
               "double",
               &Roff_LL)
    .setDefault("-0.5")
    .setComment("Roff Low Limit (ns)");
  addParameter("Roff_UL",
               "double",
               &Roff_UL)
    .setDefault("0.5")
    .setComment("Roff Up Limit (ns)");
  addParameter("R_points",
               "int",
               &R_points)
    .setDefault("11")
    .setComment("Strobe points");
}

/**
 *This test is invoked per site.
 */
virtual void run()
{
   SPECIFICATION&  Rspec=Primary.getSpecification();
   SPECIFICATION&  Tspec=Primary.getSpecification();
   double Cur_Roff_value=Rspec.getSpecValue(Roff);
   double Cur_T_value=Tspec.getSpecValue(TPer);
    int X;
    int R_points_1,R_points_0;
    double Rtmp=0;
    double Roff_UL_ADJ,Roff_LL_ADJ;
    double Rstep;
    bool result;
    STRING suite_name;
    char  tmp_r[256];

    const string mylabel=Primary.getLabel();
	char device_path[DM_MAX_DEVICE];

	GetDevPath(device_path);
	strcat (device_path, "/shmoo/pm.txt");

	cout << "path is " << device_path << endl;

    ofstream fout;

    fout.open(device_path,ios_base::out|ios_base::app);
    fout.precision(3);

      if(fout.good()!=1)
     {
      cout<<"================== File Open Error,pls check the output directory permission! =================="<<endl;
       return;
     }


    cout<<"Pattern Name="<<mylabel<<endl;
    fout<<"Pattern Name="<<mylabel<<endl;

    cout<<"Cur_T_SPEC:"<<TPer<<"="<<Cur_T_value<<"ns"<<endl;
    fout<<"Cur_T_SPEC:"<<TPer<<"="<<Cur_T_value<<"ns"<<endl;
    cout<<"Cur_Roff_SPEC:"<<Roff<<"="<<Cur_Roff_value<<"ns"<<endl;
    fout<<"Cur_Roff_SPEC:"<<Roff<<"="<<Cur_Roff_value<<"ns"<<endl;

    Rstep=(Roff_UL-Roff_LL)/(R_points-1);
    R_points_1=INT(R_points*(Roff_UL-Cur_Roff_value)/(Roff_UL-Roff_LL));
    R_points_0=INT(R_points*(Cur_Roff_value-Roff_LL)/(Roff_UL-Roff_LL));

    Roff_LL_ADJ=Cur_Roff_value-Rstep*R_points_0;
    Roff_UL_ADJ=Cur_Roff_value+Rstep*R_points_1;


    cout<<"Roff_LL_ADJ="<<Roff_LL_ADJ<<"ns"<<endl;
    cout<<"Roff_UL_ADJ="<<Roff_UL_ADJ<<"ns"<<endl;
    fout<<"Roff_LL_ADJ="<<Roff_LL_ADJ<<"ns"<<endl;
    fout<<"Roff_UL_ADJ="<<Roff_UL_ADJ<<"ns"<<endl;

     CONNECT();
     FUNCTIONAL_TEST();

        for(X=0;X<R_points;X++)
        { Rtmp=Roff_LL_ADJ+Rstep*X;
            Rspec.change(Roff,Rtmp);
            FLUSH(TM::APRM);
            FUNCTIONAL_TEST();

            result=GET_FUNCTIONAL_RESULT();
            if(result)
             {
               if(Rtmp<=(Cur_Roff_value+0.0000001)&&Rtmp>=(Cur_Roff_value-0.0000001))
               {
               cout<<setw(5)<<"*";
               fout<<setw(5)<<"*";
               }
               else
               {
               cout<<setw(5)<<"P";
               fout<<setw(5)<<"P";
               }
             }
            else
             {
               if(Rtmp<=(Cur_Roff_value+0.0000001)&&Rtmp>=(Cur_Roff_value-0.0000001))
               {
               cout<<setw(5)<<"#";
               fout<<setw(5)<<"#";
               }
               else
               {
               cout<<setw(5)<<".";
               fout<<setw(5)<<".";
               }
             }
        }

    cout<<"\t";
    fout<<"\t";

    cout<<endl;
    fout<<"\n";

    for(X=0;X<R_points;X++)
    {
       Rtmp=Roff_LL_ADJ+Rstep*X;
       printf("%5.2f",Rtmp);
       sprintf(tmp_r,"%5.2f",Rtmp);

       fout<<tmp_r<<"\t";

    }
    cout<<" :"<<Roff<<"(ns)"<<endl;
    fout<<" :"<<Roff<<"(ns)"<<"\n";
    fout.close();

    Rspec.restore();
    FLUSH(TM::APRM);
    return;
}

/**
 *This function will be invoked once the specified parameter's value is changed.
 *@param parameterIdentifier
 *
 *Note: TestMethod API should not be used in this method.
 */
virtual void postParameterChange(const string& parameterIdentifier)
{
  //add your code
}

};
REGISTER_TESTMETHOD("bm_ac_tml.Shmoo_PinMargin.PM_Roff_MP",PM_Roff_MP);
