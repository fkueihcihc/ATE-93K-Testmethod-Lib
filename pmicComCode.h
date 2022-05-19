//**************************************************************************************************
//*  Author:      Verigy - Shanghai, China  Sophia                      
//*  Description: PMU/PMIC common code                             
//*  SmarTest:    Recommend 6.5.4                                                  
//*  Functions:                                                                
//*                                  
//* IFVM	: 
//*           1. IFVM_PPMU(          pin, iForce, vout,optiona debug,optional delay time,optional sample_size)                               
//*           2. IFVM_PPMU_CLAMP(    pin, iForce, clamp_Low,clamp_High,vout,optiona debug,optional delay time,optional sample_size)                               
//*           3. IFVM_SPMU(spmu_task,pin, iForce, vout,optiona debug,optional delay time,optional sample_size)  
//* VFIM
//*           1. VFIM_PPMU(          pin, vForce, vout,optiona debug,optional delay time,optional sample_size)                               
//*           2. VFIM_SPMU(spmu_task,pin, vForce, vout,optiona debug,optional delay time,optional sample_size)                               
//* VF
//*           1. Vset_PPMU(          pin, vForce,optiona iRange): VF using PPMU 
//*           2. Vset_SPMU(spmu_task,pin, vForce,optiona iRange): VF using SPMU 
//* IF
//*           1. Iset_PPMU(          pin, iForce,optiona iRange): IF using PPMU 
//*           2. Iset_SPMU(spmu_task,pin, iForce,optiona iRange): IF using SPMU 
//* Hiz_Meas/VM
//*           1.  VM_BADC(spmu_task, pin): VM using BADC
//*           2.  VM_Hiz(pin, vForce,optiona iRange): VF using PPMU
//* Relay contril            
//*           1. DigPin_Relay(pin,relay_string) : control relays on PE
//*           2. disconnectSignalPin(pin)		: disconnect channel(relay) of DC SCALE card in signal mode
//*           3. Set_Relay_fromString      		: control relay of utilitys 
//*           4. Reset_SPMU(SPMU_TASK& SPMU_task,STRING pin_list);
//*           		: disconnect channel(relay) of DC SCALE card in signal mode
//*  Others:
//*            1. frequency(data)  
//*            2. getErrorNum(data)
//*            3. get_WaveDatas_fromString
//*            4. get_PIN_VI_fromString
//*            5. get_string_fromVector
//*            6. Get_Reg_AdrrData_from_String
//*            7. getVector_max
//*                                                                            
//*
//*
//*          PS1600_HV Driver/Meas
//* IFVM_HV
//*
//*
//*
//*
//*
//*
//*
//*
//**************************************************************************************************
//**************************************************************************************************
//* Input Parameter defination
//* 		> Pin(STRING)
//*					Description:	Pin name to be tested, It should be a single digital pin name defined in
//*												Pin configure or pin group name(only include 1 pin channel)
//*					Default Value: Pin = "LDO1";
//* 		> iForce/vForce (DOUBLE/STRING):
//*       			Description:(double) 	 used in forcing same voltage to one or more pins 
//*       			Description:(STRING) 	 used in forcing same voltage to one or more pins 
//*							 				Be carfule the PPMU current limit is 0-40mA, so if the iForce>40mA or iForce<-40mA,
//*											An error will be reported
//*					i.e: iForce = 1; (mA)
//*					i.e: iForce = "1,4,5"; (mA)
//* 		> SPMU_TASK: predefined SPMU_task 
//*					will used in all SPMU related test
//* Output Parameters(1) 
//*					vout(double): Measured Result in V
//*					vout(data)  : Measured Result using multi-site class(for user no need consider multi-site any more)
//*							      Using vout.get(pin,site_number,current_value) to extract the meased value
//* Optional Parameters (3)
//*					DEBUG: 1 to print result to ui, 0 no print out
//*								default value is 0
//*					Delay_time: Time of relay close&open for PPMU
//*								Time of settling time for SPMU
//*								default: 1 ms
//*					samples   : Sample size number of PPMU&SPMU measure
//*								default: 1 
//**************************************************************************************************

#ifndef PMIC_CODE_HEADER 
#define PMIC_CODE_HEADER

#include "TestMethod.h"

#include <vector>
#include <string>
#include <iterator>
#include <iostream>
//#include <sstream>
//#include <fstream>
#include <algorithm>


using namespace std;
#define SITE_NUMBER 8
/**
* Return the fully qualified path of the current executing Device
*/
//string deviceRelativePath(const STRING filename) {
//    char pgmDir[DM_MAX_DEVICE];
//    tpi::GetDevPath(pgmDir);
//    return pgmDir + string("/") + filename;
//}

/**
* Tokenize full template specialization (string)
*/

template <class T>
void tokenize(const STRING source, vector<T>& dest, const STRING delimiter) ;
//void tokenize(const STRING source, vector<T>& dest, const STRING delimiter) {
//    dest.clear();
//    string::size_type lastPos = source.find_first_not_of(delimiter, 0);    //find first non delim char
//    string::size_type     pos = source.find_first_of(delimiter,lastPos);   //find the next delim after token
//
//    istringstream istr;
//    T val;
//    while (string::npos != pos || string::npos != lastPos)  {            //search for additional tokens
//        istr.clear();
//        istr.str(source.substr(lastPos, pos - lastPos));
//        istr >> val;
//        dest.push_back(val);     //found token, save in vector
//        lastPos = source.find_first_not_of(delimiter, pos);                 //find another non delim char
//        pos = source.find_first_of(delimiter,lastPos);                      //find the next delim after token
//    }
//}
class data
{
public:
        map<string, map<int,map<double,double> > > m_data;
        void set(string pin_name,int site_number, double current_value, // OR VIN_VALUE
                                 double value);
        double get(string pin_name,int site_number, double current_value);

};


struct OTP_str_type
{

	string preOTPCtrl; // {(adrr1,data1);(adrr2,data2))
	ARRAY_D trimCode;
	string BlowReg;// (A1 D1-wait->D2)

};





//******************************************************
//* Name: IFVM_PPMU
//* description: using PPMU do IFVM
//* parameters:
//*
//*   DEBUG is an optional parameter,default value=0
//******************************************************
//******************************************************
//* fun_1 is used for one/multi_pin with same      current_forcing value
//* fun_2 is used for one/multi_pin with different current_forcing value
//******************************************************

void IFVM_PPMU		(STRING pin,      		double iForce, 				  ARRAY_D& Vout,int DEBUG=0, 	double delay_time=1.0);
void IFVM_PPMU		(STRING pin_list, 		double iForce,     			  data& Vout, 	int DEBUG=0, 	double delay_time=1.0);
void IFVM_PPMU		(STRING pin_list, 		STRING iForce_list,			  data& Vout, 	int DEBUG=0, 	double delay_time=1.0);
void IFVM_PPMU		(vector<STRING> sListPins,vector<double> sListCurrent,data& Vout,	int DEBUG=0,	double delay_time=1.0);

void IFVM_PPMU_CLAMP(STRING pin,			double iForce, 		double clamp_low,		double clamp_high,		ARRAY_D& Vout, 	int DEBUG=0, double delay_time=1.0);
void IFVM_PPMU_CLAMP(STRING pin_list, 		double iForce, 		double clamp_low,		double clamp_high,     	data& Vout, 	int DEBUG=0, double delay_time=1.0);
void IFVM_PPMU_CLAMP(STRING pin_list, 		STRING iForce_list, STRING clamp_low_list,	STRING clamp_high_list,	data& Vout, 	int DEBUG=0, double delay_time=1.0);
void IFVM_PPMU_CLAMP(vector<STRING> sListPins,vector<double> sListCurrent, vector<double> slist_clamp_low,vector<double> slist_clamp_high,data& Vout,int DEBUG=0,double delay_time=1.0);

void IFVM_PPMU_noRelay(STRING pin_list,double iForce,      data& Vout,int DEBUG=0);  //1
void IFVM_PPMU_noRelay(STRING pin_list,STRING iForce_list, data& Vout,int DEBUG=0);  //2



//******************************************************
//* Name: IFVM_SPMU
//* description: using SPMU do IFVM
//******************************************************
//* fun_1 is used for one/multi_pin with same      current_forcing value
//* fun_2 is used for one/multi_pin with different current_forcing value
void IFVM_SPMU(SPMU_TASK& SPMU_task,STRING pin,      double iForce, double vClamp,                ARRAY_D& Vout,int DEBUG=0, double delay_time=1.0,int samples=1);
void IFVM_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, double iForce, double vClamp,                   data& Vout,int DEBUG=0, double delay_time=1.0,int samples=1); //setting times
void IFVM_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, STRING iForce_list, STRING vClamp_list,         data& Vout,int DEBUG=0, double delay_time=1.0,int samples=1);

void IFVM_SPMU_PreCharge(SPMU_TASK& SPMU_task,STRING pin_list, double iForce, 		double vClamp, 		double PreCharge, 		data& Vout,int DEBUG=0, int pro_index =0, double delay_time=1.0,int samples=1);
void IFVM_SPMU_PreCharge(SPMU_TASK& SPMU_task,STRING pin_list, STRING iForce_list,  STRING vClamp_list, STRING PreCharge_list,	data& Vout,int DEBUG=0, double delay_time=1.0,int samples=1);

void IFVM_SPMU_noRelay(SPMU_TASK& SPMU_task,STRING pin_list, double iForce,       double vClamp, data& Vout,int DEBUG=0, double delay_time=1.0,int samples=1);

//******************************************************
//* Name: Vset_SPMU
//* Name: Iset_SPMU
//* description: using SPMU do Voltage source only 
//*   iRange is an optional parameter,unit is mA ,default value=100 mA
//******************************************************
void Vset_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, 			double vForce_value,        double iRang_Value=100.0);
void Vset_SPMU(SPMU_TASK& SPMU_task,vector<string> sListPins, 	vector<double> sListVolt,	double iRang_Value=100.0);
void Vset_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, 			STRING vForce_list, 		STRING iRang_list);
void Vset_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, STRING vForce_list, double iRang_Value);

void Vset_HPMU(SPMU_TASK& SPMU_task,STRING pin_list, 			double vForce_value,        double iRang_Value=100.0);
void Vset_SPMU_noRelay(SPMU_TASK& SPMU_task,STRING pin_list, double vForce_value);
void Iset_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, double iForce_value,double vClamp,double iForce_range=100.0);
//******************************************************
//* Name: Vset_PPMU
//* description: using PPMU do Voltage source only 
//*   iRange is an optional parameter,unit is mA ,default value=100 mA
//******************************************************
//void Vset_PPMU(STRING pin_list, double vForce_value);
void Vset_PPMU(STRING pin_list, double vForce_value,double iRang_Value= 20.0  );
void Vset_PPMU(STRING pin_list, STRING vForce_list, double iRang_Value= 20.0);
void Vset_PPMU(vector<string> sListPins, vector<double> sListVolt,double iRang_Value= 20.0);
void Vset_PPMU_noRelay(STRING pin_list, STRING vForce_list,double iRang_Value= 20.0);
void Iset_PPMU(STRING pin_list, double iForce_Value);
void Iset_PPMU(STRING pin_list, STRING iForce_List);
//added on Jul15-2013
void Iset_gangPPMU(STRING gangPin_list, DOUBLE iForce_Value,double active_value=0);//one force a value to gangPins
void Iset_gangPPMU(STRING gangPin_list, STRING vForce_list,double active_value=0);//different gangPins for different force value

//******************************************************
//* Name: IFVM_gangPPMU
//* description: using PPMU gang do IFVM
//* fun_1 is used for one/multi_pin with same      current_forcing value
//* fun_2 is used for one/multi_pin with different current_forcing value
//******************************************************
void IFVM_gangPPMU(STRING gangPin_force,     STRING gangPin_sense,     double iForce,      data Vout,int DEBUG=0,double active_value=0);
void IFVM_gangPPMU(STRING gangPin_force_list,STRING gangPin_sense_list,STRING iForce_list, data Vout,int DEBUG=0,double active_value=0);
void IFVM_gangPPMU_LDO(STRING gangPin_force,STRING gangPin_sense,DOUBLE iForce, ARRAY_D& Vout);
void IFVM_gangPPMU_LDO_VOUT(STRING gangPin_force,STRING gangPin_sense,DOUBLE iForce, ARRAY_D& Vout);

//******************************************************
//* Name: VM_PPMU,VM_BADC
//* description: using PPMU/BoardADC do Voltage meas only 
//*   DEBUG is an optional parameter,default value=0
//******************************************************
//void VM_BADC(SPMU_TASK& badc_task,STRING pin_list, data& Vout);
void VM_BADC(SPMU_TASK& badc_task,STRING pin_list,   data& Vout,int DEBUG=0, int progIndex=0,double delay_time=1.0,int samples=1);
void VM_PPMU(STRING pin_list,                        data& Vout,int DEBUG=0, int lev_index=0);
void Meas_PPMU(STRING pin_list,                        data& VIout,int DEBUG=0,int pro_index=0);
//void IM_PPMU(STRING pin_list,                        data& Iout,int DEBUG=0);

void VM_SPMU_Hiz(SPMU_TASK& SPMU_task,STRING pin_list,data& Vout,int DEBUG=0, double delay_time=1.0,int samples=10);
void Meas_SPMU(SPMU_TASK& SPMU_task,STRING pin_list,data& VIout,int DEBUG=0, int pro_index=0,double delay_time=1.0,int samples=10);


//******************************************************
//* Name: VFIM_PPMU/VFIM_SPMU
//* description: using PPMU/SPMU do IFVM
//******************************************************
void VFIM_PPMU(STRING pin_list,  double vForce_value, double iRang_Value,         data& Iout, int DEBUG=0, double delay_time=1.0,int samples=1);
void VFIM_PPMU(STRING pin_list,  STRING vForce_list,  STRING iRang_Value_list,    data& Iout, int DEBUG=0, double delay_time=1.0,int samples=1);
void VFIM_PPMU_noRelay(STRING pin_list, double vForce, double iRang_Value,        data& Iout, int DEBUG=0, double delay_time=1.0,int samples=1);

void VFIM_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, double vForce_value,                         data& Iout, int DEBUG=0, double delay_time=1.0,int samples=1);
void VFIM_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, double vForce_value,double iRang_Value ,     data& Iout, int DEBUG=0, double delay_time=1.0,int samples=1);
void VFIM_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, STRING vForce_list, STRING iRang_Value_list, data& Iout, int DEBUG=0, double delay_time=1.0,int samples=1);
void VFIM_SPMU_noRelay(SPMU_TASK& SPMU_task,STRING pin_list, double vForce_value,                 data& Iout, int DEBUG=0, double delay_time=1.0,int samples=1);


//*********************
//* PS1600_HV
void IFVM_HV(STRING pin_list, double iForce,     data& Vout, int DEBUG, double delay_time);
void VM_BADC_HV(HV_DC_TASK hvDCtask,STRING pin_list,data& Vout,int DEBUG=0, int progIndex=0,double delay_time=1.0,int samples=1);
//******************************************************
//* Name: DigPin_Relay
//* description: using to control relays in PE card AC,PPMU
//* i.e. DigPin_Relay("pin1","PPMU_ON")
//******************************************************
void DigPin_Relay(STRING pin_list,STRING status);

//******************************************************
//* Name: Reset SPMU
//* Name: disconnectSignalPin
//*
//*
//* description: using to disconnect relays DC_SCALE card in signal mode
//******************************************************
void Reset_SPMU			(SPMU_TASK& SPMU_task,STRING pin_list);
void disconnectSignalPin(const STRING& pin );

//******************************************************
//* Name: Set_Relay_fromString
//*      description: close/open relay based on input strings in one time
//Format relays_1:action;relays_2:action;relays_3:actions
//*		1. Using ":" to seperate relays and actions(ON/OFF) 
//*		2. Using ";" to seperate different actions(On/OFF)
//*			i.e. "k1,k2:ON; k3:OFF;K5:ON"
//******************************************************
void Set_Relay_fromString(STRING Relay_actions,int DEBUG=0);
void Set_Relay(STRING Relay_actions,int DEBUG=0);

//******************************************************
//* Using error map to caculate some AC test
//*
//* 1. GetErrorNO : Most basic func to reture the error map number
//*
//* 2.
//******************************************************

//******************************************************
//* Name: GetErrorNO
//* description: caculate the error count number
//******************************************************
int GetErrorNO(const STRING pin, int start_cycle, int no_of_cycles);
int GetErrorNO(const STRING pin);
//******************************************************
// * Name: SIM_AC_func
// *		description: timing measurement using error_map
// *		this is a simple and fast method for rising/falling and duty time measurement
// *		Which is suitable for MassProduction with low accaracy requirments(~1-2ns)
// *
// *		For pin  resolution is 
// * 		input parameters:
// * 			MeasPin: pins need to be measured
// * 			VPP:     Power/high_output level
// *			ratio_20,ratio_80: level ration(start and stop) for rising and falling test
// *			ratio_50: level ration(middle level) for duty test
// * 			Label_Rising,Label_Falling,Label_Duty:
// *						label name for rising,falling and duty test
// *			
// * 		output parameters:
// *				ac_time: test result
// *
//*************************************************************************
void SIM_AC_func(STRING MeasPin,double VPP, double ratio_20,double ratio_80,double ratio_50,STRING Label_Rising,STRING Label_falling,STRING Label_duty,data& ac_time,int Debug_Mode=1,int X_mode=8,double pattern_period= 10);



double Delay_Time_EMAP(const STRING pin1,const STRING pin2,double period = 20 );//20 ns

void set_Vol_Voh(const STRING pin,double Voh_value, double Vol_value);
void set_Vil_Vih(const STRING pin,double Vih_value, double Vil_value);
//added on Feb19
void set_Vihh(const STRING pin,double Vihh_value);

//***********************************************************************************************
//* Name: FM_Waveform_Generate
//* description: create FM modulated waveform
//* Input Parameters:
//*	  1. AWG points 	: 					INT         Nawg= 32768   Sampling Freq 	: DOUBLE      Fs = 32.768 MHz;  
//*							be careful of your sampling fequency, your awg point should coreponding the sampling frequency 
//*	  2. Dev Freq(i.e Fd = 75 KHz)		 	DOUBLE      Fd_cyc= 75;    
//*	  3. Modulation(i.e. Freq Fm = 1 KHz)	DOUBLE      Fm_cyc= 1;    
//* 	
//***********************************************************************************************

void FM_Waveform_Generate(int Nawg, double Fd_cyc, double Fm_cyc);

void PN_Calc(ARRAY_D spec, double main_tone, double RBW,STRING Freq_List, data& PPNout);

//******************************************************
//* Name: get_WaveDatas_fromString
//* description: extract and create data points vector from string
//* format: seperated by ";"
//* 	sigle point:3.5;3.6;
//* 	Ramp_up/down: start-stop:step i.e 3.5-4.0:0.2
//*		example: string ="3.8;3.2;3.2-3.4:0.5;4"
//*              ouput vector={3.8,3.2,3.2,3.25,3.3,3.35,3.4,4}
//*
//******************************************************
void get_WaveDatas_fromString(STRING Vwaveform,double Vstep,vector<double>& wave_data);
void get_WaveDatas_fromString(STRING Vwaveform,vector<DOUBLE>& wave_data);
void get_WaveDatas_fromString(STRING Vwaveform,vector<INT>& wave_data);

void get_PIN_VI_fromString(STRING Pin_Current,vector<STRING>& sListPins,vector<double>& sListCurrent);



//******************************************************
//* Name: getVector_max
//*		description: find the max value from a double vector
//******************************************************
double getVector_max(vector<double>& sListValue);

//******************************************************
//* Name: get_string_fromVector
//*		description: create a string based on all vector memeber seperated by comma
//******************************************************
string get_string_fromVector(vector<STRING>& sListStr);

//******************************************************
//* Name: Get_Reg_AdrrData_from_String
//*		description: create register adress vector and  data vector
//*
// * 		input parameters:
// * 			register_string
// * 		      format: "(0x1014,0x0303);(0x1004,0x0088);(0x0008,0x0088)"
// * 		output parameters:
// * 			addresses: "0x1014;0x1004;0x0008"
// * 			datas: "0x0303;0x0088;0x0088"
// * */
//*************************************************************************
void Get_Reg_AdrrData_from_String(string register_string, string& addresses, string& datas);


//******************************************************
//* Name: Get_data_from_data_file
//*****************
void Get_data_from_file(char* file_name,ARRAY_D& data_array,int data_size=65536);
void Get_data_from_file(char* file_name,ARRAY_I& data_array,int data_size=65536);

void get_WaveDatas_fromString(STRING Vwaveform,vector<DOUBLE>& wave_data);


// this is used to change one pattern one pin-one data per-site
//void Vec_Label_PinWrite(const STRING& labelName, const STRING& myPin,INT startVector, INT physWfIndex[] );
void Vec_Label_Write_OneVector(const STRING& labelName, const STRING& myPin,INT startVector, INT physWfIndex[] );
void Vec_Label_WriteData(const STRING& labelName, const STRING& myPin,INT startVector, INT data[],int bits_length=1, int phyIndex_d0=0,int phyIndex_d1=1 );

#endif

