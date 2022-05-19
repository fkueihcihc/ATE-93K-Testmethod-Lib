//**************************************************************************************************
//*  Author:      Verigy - Shanghai, China  Sophia                      
//*  Description: PMU/PMIC common code                             
//*  SmarTest:    Recommend 6.5.4 or later                                                 
//*  Functions:                                                                
//*                                  
//* IFVM	: 
//*           1. IFVM_PPMU(          pin, iForce,         vout,optiona debug,optional delay time,optional sample_size)                               
//*           2. IFVM_SPMU(spmu_task,pin, iForce, vClamp, vout,optiona debug,optional delay time,optional sample_size)
//*           3. IFVM_SPMU_noRelay
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


#include "TestMethod.h"

#include <vector>
#include <string>
#include <iterator>
#include <sstream>
#include <fstream>

#include "FW.h"
#include "pmicComCode.h"



using namespace std;

//int DEBUG=0;

//**************************************************************************************************
//*  Function_Name:      IFVM_PPMU(pin, iForce, vout)                                  
//**************************************************************************************************
//**************************************************************************************************
//* Description: Use PPMU to force a current to a pin then measure it output voltage
//*							 Be carful of following things:
//*														1. This is IFVM test for single pin
//*														2. Active_load is not considered, so only PPMU_relay is closed when 
//*														   executing the IFVM actions
//* Input Parameter defination
//* 		Pin(STRING)
//*					Description:	Pin name to be tested, It should be a single digital pin name defined in
//*												Pin configure or pin group name(only include 1 pin channel)
//*					Default Value: Pin = "LDO1";
//* 		iForce(DOUBLE)
//*       	Description: 	Current sinking to the device-pin in mA 
//*							 				Be carfule the PPMU current limit is 0-40mA, so if the iForce>40mA or iForce<-40mA,
//*											An error will be reported
//*					Default Value: iForce = 1; (mA)
//* Output Parameters 
//*					vout(DOUBLE): Measured Result in V                        
//**************************************************************************************************

//**************************************************************************************************
//*  Function_Name:      IFVM_PPMU(Pin_list, iForce, vout[])                                  
//**************************************************************************************************
//**************************************************************************************************
//* Description: Use PPMU to force a same current to a "group" of pins then measure the output voltages
//*							 About this function:
//*														1. This is IFVM test for multi-pin ;
//*															 all pins are forced same current and measure voltage in parallel 
//*														2. Active_load is not considered, so only PPMU_relay is closed when 
//*														   executing the IFVM actions
//* Input Parameter defination
//* 		Pin_list(STRING)
//*					Description:	Pins to be tested, It can be a single pin name, a pin group name, 
//*												or a pin and pin group combination which seperated by coma
//*					Default Value: Pin = "LDO1,LDO2";
//*												
//* 		iForce(DOUBLE)
//*       Description: 	Current sinking to the device-pin in mA 
//*							 				Be carfule the PPMU current limit is 0-40mA, so if the iForce>40mA or iForce<-40mA,
//*											An error will be reported
//*				Default Value: iForce = 1; (mA)
//*
//* Output Parameters 
//*				vout(DOUBLE_ARRAY): All measured values for all pins in V                        
//**************************************************************************************************

//**************************************************************************************************
//*  Function_Name:      IFVM_PPMU(Pin_list, iForce_list, vout[])                                  
//**************************************************************************************************
//**************************************************************************************************
//* Description: Use PPMU to force different currents to different pins then measure the output voltages
//*							 About this function:
//*														1. This is multi-pin IFVM test;
//*															 different pins are forced different current and measure voltage in parallel 
//*														2. Active_load is not considered, so only PPMU_relay is closed when 
//*														   executing the IFVM actions
//* Input Parameter defination
//* 		Pin_list(STRING)
//*					Description:	Pins to be tested, It can be a single pin name, a pin group name, 
//*									for MTK         			or a pin and pin group combination which seperated by coma
//*					Default Value: Pin = "LDO1,LDO2";
//*												
//* 		iForce(DOUBLE)
//*       Description: 	Current sinking to the device-pin in mA 
//*							 				Be carfule the PPMU current limit is 0-40mA, so if the iForce>40mA or iForce<-40mA,
//*											An error will be reported
//*				Default Value: iForce = 1; (mA)
//*
//* Output Parameters 
//*				vout(DOUBLE_ARRAY): All measured values for all pins in V                        
//**************************************************************************************************
//**************************************************************************************************

/**
* Tokenize full template specialization (string)
*/


template <class T>
void tokenize(const STRING source, vector<T>& dest, const STRING delimiter) {
    dest.clear();
    string::size_type lastPos = source.find_first_not_of(delimiter, 0);    //find first non delim char
    string::size_type     pos = source.find_first_of(delimiter,lastPos);   //find the next delim after token

    istringstream istr;
    T val;
    while (string::npos != pos || string::npos != lastPos)  {            //search for additional tokens
        istr.clear();
        istr.str(source.substr(lastPos, pos - lastPos));
        istr >> val;
        dest.push_back(val);     //found token, save in vector
        lastPos = source.find_first_not_of(delimiter, pos);                 //find another non delim char
        pos = source.find_first_of(delimiter,lastPos);                      //find the next delim after token
    }
}

void data::set(string pin_name,int site_number, double current_value, double value)
{
        m_data[pin_name][site_number][current_value] = value;
}


double data::get(string pin_name,int site_number, double current_value)
{
    return m_data[pin_name][site_number][current_value] ;
}


//**************************************************************************************************
//* 1. Force single current to single pin using PPMU
//**************************************************************************************************
void IFVM_PPMU(STRING pin, DOUBLE iForce, ARRAY_D& Vout,int DEBUG, double delay_time)
{

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set,ppmu_off;
    PPMU_RELAY    relay_open, relay_close;
    PPMU_MEASURE  meas_set;
    TASK_LIST     task1;
//    DOUBLE relayClose_wait = 1;     //msec
//    DOUBLE relayOpen_wait = 1;     //msec
    int site_index;
    //************************************************************************************************
    //* IFVM executing...
    //************************************************************************************************
        ppmu_set.pin(pin).iForce(iForce mA).iRange(iForce mA).min(0 ).max(4 V);
        meas_set.pin(pin).execMode(TM::OVAL);
        ppmu_off.pin(pin).iForce(0.0).iRange(iForce mA).min(0 ).max(4 V);
        
        relay_close. pin(pin).status("PPMU_ON");
        relay_close. wait(delay_time mS);
        
        relay_open . pin(pin).status("AC_ON"); 
        relay_open. wait(delay_time mS);
        
        //task1.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
        task1.add(ppmu_set).add(relay_close).add(meas_set).add(relay_open).add(ppmu_off);
        task1.execute();
        
        //************************************************************************************************
        //* Extract the value per-pin per-site
        //************************************************************************************************
        FOR_EACH_SITE_BEGIN();
          site_index=CURRENT_SITE_NUMBER()-1;
          Vout[site_index] = meas_set.getValue(pin);
              
          if(DEBUG)
          {
        	  cout<<"site = "<<site_index+1 <<"  "<<pin<<"_measured = "<<Vout[site_index]<<endl;
          }
        FOR_EACH_SITE_END();

}


//add clamp
void IFVM_PPMU_CLAMP(STRING pin, DOUBLE iForce, double clamp_low,double clamp_high, ARRAY_D& Vout,int DEBUG, double delay_time)
{

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set,ppmu_off;
    PPMU_RELAY    relay_open, relay_close;

    PPMU_MEASURE  meas_set;
    TASK_LIST     task1;
//    DOUBLE relayClose_wait = 1;     //msec
//    DOUBLE relayOpen_wait = 1;     //msec
    int site_index;
    //************************************************************************************************
    //* IFVM executing...
    //************************************************************************************************
        ppmu_set.pin(pin).iForce(iForce mA).iRange(iForce mA).min(0 ).max(4 V);
        meas_set.pin(pin).execMode(TM::OVAL);
        ppmu_off.pin(pin).iForce(0.0).iRange(iForce mA).min(0 ).max(4 V);
        
        relay_close. pin(pin).status("PPMU_ON");
        relay_close. wait(delay_time mS);
        
        relay_open . pin(pin).status("AC_ON"); 
        relay_open. wait(delay_time mS);
        
        PPMU_CLAMP    clamp_on, clamp_off;
        clamp_on.pin(pin).status("CLAMP_ON").low(clamp_low).high(clamp_high);
        clamp_off.pin(pin).status("CLAMP_OFF");

        
        //task1.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
        task1.add(ppmu_set).add(clamp_on).add(relay_close).add(clamp_off).add(meas_set).add(relay_open).add(ppmu_off);
        task1.execute();
        
        //************************************************************************************************
        //* Extract the value per-pin per-site
        //************************************************************************************************
        FOR_EACH_SITE_BEGIN();
          site_index=CURRENT_SITE_NUMBER()-1;
          Vout[site_index] = meas_set.getValue(pin);
              
          if(DEBUG)
          {
        	  cout<<"site = "<<site_index+1 <<"  "<<pin<<"_measured = "<<Vout[site_index]<<endl;
          }
        FOR_EACH_SITE_END();

}
//**************************************************************************************************
//* 2.Force same current to multi-pin using PPMU
//**************************************************************************************************

//**************************************************************************************************
//* 3. Force different current to different-pins using PPMU
//**************************************************************************************************

//**************************************************************************************************
//* 6. Force single current to single pin using HPPMU/DPS32/VI32(SIG mode)
//**************************************************************************************************
void IFVM_SPMU(SPMU_TASK& SPMU_task,STRING pin, DOUBLE iForce, DOUBLE vClamp,ARRAY_D& Vout,int DEBUG, double delay_time,int samples)
{
    int site_index;
    //ON_FIRST_INVOCATION_BEGIN();
        // IFVM executing...
        SPMU_task.pin(pin).settling(delay_time mS).relay("NT").iForce(iForce mA).clamp(vClamp V).max(5 V).min(0 V).mode("IFVM").samples(samples);

        SPMU_task.execMode("PVAL").preAction("NONE");
        SPMU_task.execute();
        
        //************************************************************************************************
        //* Extract the value per-pin per-site
        //************************************************************************************************
        
        FOR_EACH_SITE_BEGIN();
           site_index=CURRENT_SITE_NUMBER()-1;
        
            Vout[site_index] = SPMU_task.getValue(pin);
            if(DEBUG)
            {
                cout<<"site = "<<site_index+1 <<"  "<<pin<<"_measured = "<<Vout[site_index]<<endl;
            }
        FOR_EACH_SITE_END();
}

//**************************************************************************************************
//* 7.Force same current to multi-pin using SPMU
//* Force same current to multi-pin using HPPMU/DPS32/VI32(SIG mode)
//**************************************************************************************************

//**************************************************************************************************
//* 8. Force different current to different-pins using SPMU
//**************************************************************************************************

//************************************************************************************************
//force same current to different pins
//consider multi_site
//data is stored in the class(data)
//************************************************************************************************
void IFVM_PPMU(STRING pin_list, double iForce,     data& Vout, int DEBUG, double delay_time)
{
    

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set,ppmu_off;
    PPMU_RELAY    relay_open, relay_close;
    PPMU_MEASURE  meas_set;
    TASK_LIST     task1;
    DOUBLE relayClose_wait = delay_time;     //msec
    DOUBLE relayOpen_wait = delay_time;      //msec
    int pin_index,site_index;
    
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    STRING_VECTOR pinNameList;
    int iNumPins;

    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    pinNameList = PinUtility.getDigitalPinNamesFromPinList(pin_list, TM::IO_PIN, TRUE, TRUE);
    iNumPins=pinNameList.size();
     
    //************************************************************************************************
    //* IFVM executing...
    //************************************************************************************************
   // ON_FIRST_INVOCATION_BEGIN();
           
        ppmu_set.pin(pin_list).iForce(iForce mA).iRange(iForce mA).min(0 ).max(4 V);
        meas_set.pin(pin_list).execMode(TM::OVAL);
        ppmu_off.pin(pin_list).iForce(0.0).iRange(iForce mA).min(0 ).max(4 V);
        
        relay_close. pin(pin_list).status("PPMU_ON");
        relay_close. wait(relayClose_wait mS);
        
        //"PPMU_OFF" will keep AC relay open, using "AC_ON" to switch PPMU off and keep AC relay on
        relay_open . pin(pin_list).status("AC_ON"); 
        relay_open. wait(relayOpen_wait mS);
        
        //task1.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
        task1.add(ppmu_set).add(relay_close).add(meas_set).add(relay_open).add(ppmu_off);
        task1.execute();
    
        //************************************************************************************************
        //* Extract the value per-pin per-site
        //************************************************************************************************
              
        FOR_EACH_SITE_BEGIN();
          site_index=CURRENT_SITE_NUMBER()-1;
            
          for(pin_index=0;pin_index<iNumPins;pin_index++)
          {
              Vout.set(pinNameList[pin_index], site_index, iForce, meas_set.getValue(pinNameList[pin_index]));
            
                            
              if(DEBUG)
              {
                cout<<"site = "<<site_index+1 <<"  "<<pinNameList[pin_index]<<"_"<< iForce<<"mA_measured = "<<Vout.get(pinNameList[pin_index], site_index, iForce)<<endl;
              }
          }
     FOR_EACH_SITE_END();
    
}

//add clamp
void IFVM_PPMU_CLAMP(STRING pin_list, double iForce,  double clamp_low,double clamp_high,   data& Vout, int DEBUG, double delay_time)
{
    

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set,ppmu_off;
    PPMU_RELAY    relay_open, relay_close;
    PPMU_MEASURE  meas_set;
    TASK_LIST     task1;
    DOUBLE relayClose_wait = delay_time;     //msec
    DOUBLE relayOpen_wait = delay_time;      //msec
    int pin_index,site_index;
    
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    STRING_VECTOR pinNameList;
    int iNumPins;

    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    pinNameList = PinUtility.getDigitalPinNamesFromPinList(pin_list, TM::IO_PIN, TRUE, TRUE);
    iNumPins=pinNameList.size();
     
    //************************************************************************************************
    //* IFVM executing...
    //************************************************************************************************
   // ON_FIRST_INVOCATION_BEGIN();
           
        ppmu_set.pin(pin_list).iForce(iForce mA).iRange(iForce mA).min(0 ).max(4 V);
        meas_set.pin(pin_list).execMode(TM::OVAL);
        ppmu_off.pin(pin_list).iForce(0.0).iRange(iForce mA).min(0 ).max(4 V);
        
        relay_close. pin(pin_list).status("PPMU_ON");
        relay_close. wait(relayClose_wait mS);
        
        //"PPMU_OFF" will keep AC relay open, using "AC_ON" to switch PPMU off and keep AC relay on
        relay_open . pin(pin_list).status("AC_ON"); 
        relay_open. wait(relayOpen_wait mS);
        
//        //task1.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
//        task1.add(ppmu_set).add(relay_close).add(meas_set).add(relay_open).add(ppmu_off);
//        task1.execute();
//    
        PPMU_CLAMP    clamp_on, clamp_off;
        clamp_on.pin(pin_list).status("CLAMP_ON").low(clamp_low).high(clamp_high);
        clamp_off.pin(pin_list).status("CLAMP_OFF");

        
        //task1.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
        task1.add(ppmu_set).add(clamp_on).add(relay_close).add(clamp_off).add(meas_set).add(relay_open).add(ppmu_off);
        task1.execute();
        //************************************************************************************************
        //* Extract the value per-pin per-site
        //************************************************************************************************
              
        FOR_EACH_SITE_BEGIN();
          site_index=CURRENT_SITE_NUMBER()-1;
            
          for(pin_index=0;pin_index<iNumPins;pin_index++)
          {
              Vout.set(pinNameList[pin_index], site_index, iForce, meas_set.getValue(pinNameList[pin_index]));
            
                            
              if(DEBUG)
              {
                cout<<"site = "<<site_index+1 <<"  "<<pinNameList[pin_index]<<"_"<< iForce<<"mA_measured = "<<Vout.get(pinNameList[pin_index], site_index, iForce)<<endl;
              }
          }
     FOR_EACH_SITE_END();
    
}
void IFVM_PPMU_noRelay(STRING pin_list, double iForce,     data& Vout, int DEBUG)
{
    
	//************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set,ppmu_off;
    PPMU_MEASURE  meas_set;
    TASK_LIST     task1;
//    DOUBLE relayClose_wait = 1;     //msec
 //   DOUBLE relayOpen_wait = 1;      //msec
    int pin_index,site_index;
    
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    STRING_VECTOR pinNameList;
    int iNumPins;

    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    pinNameList = PinUtility.getDigitalPinNamesFromPinList(pin_list, TM::IO_PIN, TRUE, TRUE);
    iNumPins=pinNameList.size();
     
    //************************************************************************************************
    //* IFVM executing...
    //************************************************************************************************
   // ON_FIRST_INVOCATION_BEGIN();
           
        ppmu_set.pin(pin_list).iForce(iForce mA).iRange(iForce mA).min(0 ).max(4 V);
        meas_set.pin(pin_list).execMode(TM::OVAL);
//        ppmu_off.pin(pin_list).iForce(0.0).iRange(iForce mA).min(0 ).max(4 V);
//        relay_close. pin(pin_list).status("PPMU_ON");
//        relay_close. wait(relayClose_wait mS);
//        
//        //"PPMU_OFF" will keep AC relay open, using "AC_ON" to switch PPMU off and keep AC relay on
//        relay_open . pin(pin_list).status("AC_ON"); 
//        relay_open. wait(relayOpen_wait mS);
        
        task1.add(ppmu_set).add(meas_set);
        task1.execute();
    
        //************************************************************************************************
        //* Extract the value per-pin per-site
        //************************************************************************************************
              
        FOR_EACH_SITE_BEGIN();
          site_index=CURRENT_SITE_NUMBER()-1;
            
          for(pin_index=0;pin_index<iNumPins;pin_index++)
          {
              Vout.set(pinNameList[pin_index], site_index, iForce, meas_set.getValue(pinNameList[pin_index]));
            
                            
              if(DEBUG)
              {
                cout<<"site = "<<site_index+1 <<"  "<<pinNameList[pin_index]<<"_"<< iForce<<"mA_measured = "<<Vout.get(pinNameList[pin_index], site_index, iForce)<<endl;
              }
          }
     FOR_EACH_SITE_END();
    
}
//************************************************************************************************
//force different current to different pins
//consider multi_site
//data is stored in the class(data)
//************************************************************************************************
void IFVM_PPMU(STRING pin_list,STRING iForce_list, data& Vout, int DEBUG, double delay_time)
{
    
    
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    static vector<string> sListPins;
    static vector<DOUBLE> sListCurrent;
    int iNumGPs,iNumCurr,iCounter,iNumPins;
    int pin_index,site_index;
        STRING_VECTOR pinNameList;

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set,ppmu_off;
    PPMU_RELAY    relay_open, relay_close;
    PPMU_MEASURE  meas_set;
    TASK_LIST     task1;

    DOUBLE relayClose_wait = delay_time;     //msec
    DOUBLE relayOpen_wait = delay_time;      //msec
    
    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    tokenize(pin_list,sListPins,",");       iNumGPs=sListPins.size();
    tokenize(iForce_list,sListCurrent,","); iNumCurr=sListCurrent.size();
    if(iNumGPs!=iNumCurr) cout<<"number error!! "<<endl;
    
        
    if(DEBUG)
    {   
        cout<<pin_list <<endl<<"pinGP_number = "<<iNumGPs<<endl;
        for(iCounter=0; iCounter<iNumGPs;iCounter++)
        {
            cout<<sListPins[iCounter]<<endl;
        }
        cout<<iForce_list <<endl<<"current_number = "<<iNumCurr<<endl;
        for(iCounter=0; iCounter<iNumCurr;iCounter++)
        {
            cout<<sListCurrent[iCounter]<<endl;
        }
    }

        //Vout.resize(iNumPins);
    //************************************************************************************************
    //* IFVM executing...
    //************************************************************************************************
   // ON_FIRST_INVOCATION_BEGIN();
    
    for(pin_index=0;pin_index< iNumGPs;pin_index++)
    {
        ppmu_set.pin(sListPins[pin_index]).iForce(sListCurrent[pin_index] mA).iRange(sListCurrent[pin_index] mA).min(0 V).max(4 V);
    	
    }
    meas_set.pin(pin_list).execMode(TM::OVAL);
    ppmu_off.pin(pin_list).iForce(0.0).iRange(20 mA).min(0 ).max(4 V);
    
    relay_close. pin(pin_list).status("PPMU_ON");
    relay_close. wait(relayClose_wait mS);
    
    //"PPMU_OFF" will keep AC relay open, using "AC_ON" to switch PPMU off and keep AC relay on
    relay_open . pin(pin_list).status("AC_ON"); 
    relay_open. wait(relayOpen_wait mS);
    
    //task1.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
    task1.add(ppmu_set).add(relay_close).add(meas_set).add(relay_open).add(ppmu_off);
    task1.execute();
    

    //************************************************************************************************
    //* Extract the value per-pin per-site
    //************************************************************************************************
    
    FOR_EACH_SITE_BEGIN();
          site_index=CURRENT_SITE_NUMBER()-1;
            
          for(int current_index=0;current_index<iNumCurr;current_index++)
          {
              pinNameList = PinUtility.getDigitalPinNamesFromPinList(sListPins[current_index], TM::IO_PIN, TRUE, TRUE);
              iNumPins=pinNameList.size();
              for(int pin_index=0;pin_index<iNumPins;pin_index++)
              {

            	  Vout.set(pinNameList[pin_index], site_index, sListCurrent[current_index], meas_set.getValue(pinNameList[pin_index]));
            	  if(DEBUG)
            	  {
            		  cout<<"site = "<<site_index+1 <<"  "<<pinNameList[pin_index]<<"_"<< sListCurrent[current_index]<<"mA_measured = "<<setprecision(3) << setw(7)<<Vout.get(pinNameList[pin_index],site_index, sListCurrent[current_index])<<endl;
            	  }
              }
          }
     FOR_EACH_SITE_END();

    
}

//add clamp
void IFVM_PPMU_CLAMP(STRING pin_list,STRING iForce_list, STRING clamp_Low_list,STRING clamp_High_list,data& Vout, int DEBUG, double delay_time)
{
    
    
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    static vector<string> sListPins;
    static vector<DOUBLE> sListCurrent;  
    static vector<DOUBLE> sListClamp_Low;
    static vector<DOUBLE> sListClamp_High;
    int iNumGPs,iNumCurr,iCounter,iNumPins,iNumclamp_L,iNumclamp_H;
    int pin_index,site_index;
        STRING_VECTOR pinNameList;

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set,ppmu_off;
    PPMU_RELAY    relay_open, relay_close;
    PPMU_MEASURE  meas_set;
    TASK_LIST     task1;
    PPMU_CLAMP    clamp_on, clamp_off;
    DOUBLE relayClose_wait = delay_time;     //msec
    DOUBLE relayOpen_wait = delay_time;      //msec
    
    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    tokenize(pin_list,sListPins,",");       		iNumGPs=sListPins.size();
    tokenize(iForce_list,sListCurrent,","); 		iNumCurr=sListCurrent.size(); 
    tokenize(clamp_Low_list,sListClamp_Low,","); 	iNumclamp_L=sListClamp_Low.size();
    tokenize(clamp_High_list,sListClamp_High,","); 	iNumclamp_H=sListClamp_High.size();
    if(iNumGPs!=iNumCurr) cout<<"number error!! "<<endl;
    
        
    if(DEBUG)
    {   
        cout<<pin_list <<endl<<"pinGP_number = "<<iNumGPs<<endl;
        for(iCounter=0; iCounter<iNumGPs;iCounter++)
        {
            cout<<sListPins[iCounter]<<endl;
        }
        cout<<iForce_list <<endl<<"current_number = "<<iNumCurr<<endl;
        for(iCounter=0; iCounter<iNumCurr;iCounter++)
        {
            cout<<sListCurrent[iCounter]<<endl;
        }
    }

        //Vout.resize(iNumPins);
    //************************************************************************************************
    //* IFVM executing...
    //************************************************************************************************
   // ON_FIRST_INVOCATION_BEGIN();
    
    for(pin_index=0;pin_index< iNumGPs;pin_index++)
    {
        ppmu_set.pin(sListPins[pin_index]).iForce(sListCurrent[pin_index] mA).iRange(sListCurrent[pin_index] mA).min(0 V).max(4 V);
        clamp_on.pin(sListPins[pin_index]).status("CLAMP_ON").low(sListClamp_Low[pin_index]).high(sListClamp_High[pin_index]);
    }
    meas_set.pin(pin_list).execMode(TM::OVAL);
    ppmu_off.pin(pin_list).iForce(0.0).iRange(20 mA).min(0 ).max(4 V);
    clamp_on.pin(sListPins[pin_index]).status("CLAMP_ON").low(sListClamp_Low[pin_index]).high(sListClamp_High[pin_index]);
    relay_close. pin(pin_list).status("PPMU_ON");
    relay_close. wait(relayClose_wait mS);
    
    //"PPMU_OFF" will keep AC relay open, using "AC_ON" to switch PPMU off and keep AC relay on
    relay_open . pin(pin_list).status("AC_ON"); 
    relay_open. wait(relayOpen_wait mS);
    clamp_off.pin(pin_list).status("CLAMP_OFF");
//    //task1.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
//    task1.add(ppmu_set).add(relay_close).add(meas_set).add(relay_open).add(ppmu_off);
//    task1.execute();
    
    //task1.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
    task1.add(ppmu_set).add(clamp_on).add(relay_close).add(clamp_off).add(meas_set).add(relay_open).add(ppmu_off);
    task1.execute();

    //************************************************************************************************
    //* Extract the value per-pin per-site
    //************************************************************************************************
    
    FOR_EACH_SITE_BEGIN();
          site_index=CURRENT_SITE_NUMBER()-1;
            
          for(int current_index=0;current_index<iNumCurr;current_index++)
          {
              pinNameList = PinUtility.getDigitalPinNamesFromPinList(sListPins[current_index], TM::IO_PIN, TRUE, TRUE);
              iNumPins=pinNameList.size();
              for(int pin_index=0;pin_index<iNumPins;pin_index++)
              {

            	  Vout.set(pinNameList[pin_index], site_index, sListCurrent[current_index], meas_set.getValue(pinNameList[pin_index]));
            	  if(DEBUG)
            	  {
            		  cout<<"site = "<<site_index+1 <<"  "<<pinNameList[pin_index]<<"_"<< sListCurrent[current_index]<<"mA_measured = "<<setprecision(3) << setw(7)<<Vout.get(pinNameList[pin_index],site_index, sListCurrent[current_index])<<endl;
            	  }
              }
          }
     FOR_EACH_SITE_END();

    
}
void IFVM_PPMU(vector<STRING> sListPins,vector<DOUBLE> sListCurrent,data& Vout,  int DEBUG, double delay_time)
{
    
    
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
   
    int iNumGPs,iNumCurr,iNumPins;
    int pin_index,site_index;
        STRING_VECTOR pinNameList;

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set,ppmu_off;
    PPMU_RELAY    relay_open, relay_close;
    PPMU_MEASURE  meas_set;
    TASK_LIST     task1;

    DOUBLE relayClose_wait = delay_time;     //msec
    DOUBLE relayOpen_wait = delay_time;      //msec
    
    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    iNumGPs=sListPins.size();
    iNumCurr=sListCurrent.size();
    if(iNumGPs!=iNumCurr) cout<<"number error!! "<<endl;
    
        
//    if(0)
//    {   
//        cout<<pin_list <<endl<<"pinGP_number = "<<iNumGPs<<endl;
//        for(iCounter=0; iCounter<iNumGPs;iCounter++)
//        {
//            cout<<sListPins[iCounter]<<endl;
//        }
//        cout<<iForce_list <<endl<<"current_number = "<<iNumCurr<<endl;
//        for(iCounter=0; iCounter<iNumCurr;iCounter++)
//        {
//            cout<<sListCurrent[iCounter]<<endl;
//        }
//    }

        //Vout.resize(iNumPins);
    //************************************************************************************************
    //* IFVM executing...
    //************************************************************************************************
   // ON_FIRST_INVOCATION_BEGIN();
    
    for(pin_index=0;pin_index< iNumGPs;pin_index++)
    {
        ppmu_set.pin(sListPins[pin_index]).iForce(sListCurrent[pin_index] mA).iRange(sListCurrent[pin_index] mA).min(0 V).max(4 V);
    	
    
	    meas_set.pin(sListPins[pin_index]).execMode(TM::OVAL);
	    ppmu_off.pin(sListPins[pin_index]).iForce(0.0).iRange(20 mA).min(0 ).max(4 V);
        relay_close. pin(sListPins[pin_index]).status("PPMU_ON");
        relay_open . pin(sListPins[pin_index]).status("AC_ON");
    }
    relay_close. wait(relayClose_wait mS);
    
    //"PPMU_OFF" will keep AC relay open, using "AC_ON" to switch PPMU off and keep AC relay on
    
    relay_open. wait(relayOpen_wait mS);
    
    //task1.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
    task1.add(ppmu_set).add(relay_close).add(meas_set).add(relay_open).add(ppmu_off);
    task1.execute();
    

    //************************************************************************************************
    //* Extract the value per-pin per-site
    //************************************************************************************************
    
    FOR_EACH_SITE_BEGIN();
          site_index=CURRENT_SITE_NUMBER()-1;
            
          for(int current_index=0;current_index<iNumCurr;current_index++)
          {
              pinNameList = PinUtility.getDigitalPinNamesFromPinList(sListPins[current_index], TM::IO_PIN, TRUE, TRUE);
              iNumPins=pinNameList.size();
              for(int pin_index=0;pin_index<iNumPins;pin_index++)
              {

            	  Vout.set(pinNameList[pin_index], site_index, sListCurrent[current_index], meas_set.getValue(pinNameList[pin_index]));
            	  if(DEBUG)
            	  {
            		  cout<<"site = "<<site_index+1 <<"  "<<pinNameList[pin_index]<<"_"<< sListCurrent[current_index]<<"mA_measured = "<<setprecision(3) << setw(7)<<Vout.get(pinNameList[pin_index],site_index, sListCurrent[current_index])<<endl;
            	  }
              }
          }
     FOR_EACH_SITE_END();

    
}


//add clamp
void IFVM_PPMU_CLAMP(vector<STRING> sListPins,vector<DOUBLE> sListCurrent,vector<double> slist_clamp_low,vector<double> slist_clamp_high,data& Vout,  int DEBUG, double delay_time)
{
    
    
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
   
    int iNumGPs,iNumCurr,iNumPins;
    int pin_index,site_index;
        STRING_VECTOR pinNameList;

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set,ppmu_off;
    PPMU_RELAY    relay_open, relay_close;
    PPMU_MEASURE  meas_set;
    TASK_LIST     task1;
    PPMU_CLAMP    clamp_on, clamp_off;
    
    DOUBLE relayClose_wait = delay_time;     //msec
    DOUBLE relayOpen_wait = delay_time;      //msec
    
    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    iNumGPs=sListPins.size();
    iNumCurr=sListCurrent.size();
    if(iNumGPs!=iNumCurr) cout<<"number error!! "<<endl;
    

    //************************************************************************************************
    //* IFVM executing...
    //************************************************************************************************
   // ON_FIRST_INVOCATION_BEGIN();
    
    for(pin_index=0;pin_index< iNumGPs;pin_index++)
    {
        ppmu_set.pin(sListPins[pin_index]).iForce(sListCurrent[pin_index] mA).iRange(sListCurrent[pin_index] mA).min(0 V).max(4 V);
    	
    
	    meas_set.pin(sListPins[pin_index]).execMode(TM::OVAL);
	    ppmu_off.pin(sListPins[pin_index]).iForce(0.0).iRange(20 mA).min(0 ).max(4 V);
        relay_close. pin(sListPins[pin_index]).status("PPMU_ON");
        relay_open . pin(sListPins[pin_index]).status("AC_ON");
        clamp_on.pin(sListPins[pin_index]).status("CLAMP_ON").low(slist_clamp_low[pin_index]).high(slist_clamp_high[pin_index]);
        clamp_off.pin(sListPins[pin_index]).status("CLAMP_OFF");
    }
    relay_close. wait(relayClose_wait mS);
    
    //"PPMU_OFF" will keep AC relay open, using "AC_ON" to switch PPMU off and keep AC relay on
    
    relay_open. wait(relayOpen_wait mS);
    
//    //task1.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
//    task1.add(ppmu_set).add(relay_close).add(meas_set).add(relay_open).add(ppmu_off);
//    task1.execute();
//    


     
     //task1.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
     task1.add(ppmu_set).add(clamp_on).add(relay_close).add(clamp_off).add(meas_set).add(relay_open).add(ppmu_off);
     task1.execute();

    //************************************************************************************************
    //* Extract the value per-pin per-site
    //************************************************************************************************
    
    FOR_EACH_SITE_BEGIN();
          site_index=CURRENT_SITE_NUMBER()-1;
            
          for(int current_index=0;current_index<iNumCurr;current_index++)
          {
              pinNameList = PinUtility.getDigitalPinNamesFromPinList(sListPins[current_index], TM::IO_PIN, TRUE, TRUE);
              iNumPins=pinNameList.size();
              for(int pin_index=0;pin_index<iNumPins;pin_index++)
              {

            	  Vout.set(pinNameList[pin_index], site_index, sListCurrent[current_index], meas_set.getValue(pinNameList[pin_index]));
            	  if(DEBUG)
            	  {
            		  cout<<"site = "<<site_index+1 <<"  "<<pinNameList[pin_index]<<"_"<< sListCurrent[current_index]<<"mA_measured = "<<setprecision(3) << setw(7)<<Vout.get(pinNameList[pin_index],site_index, sListCurrent[current_index])<<endl;
            	  }
              }
          }
     FOR_EACH_SITE_END();

    
}
//Created Sophia Nov24 2010
// Updated on Feb16,2012 add an optional parameter
void IFVM_PPMU_noRelay(STRING pin_list,STRING iForce_list,data& Vout, int DEBUG)
{

    
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    static vector<string> sListPins;
    static vector<DOUBLE> sListCurrent;
    int iNumGPs,iNumCurr,iCounter,iNumPins;
    int pin_index,site_index;
        STRING_VECTOR pinNameList;

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set;
    PPMU_MEASURE  meas_set;
    TASK_LIST     task1;

    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    tokenize(pin_list,sListPins,",");       iNumGPs=sListPins.size();
    tokenize(iForce_list,sListCurrent,","); iNumCurr=sListCurrent.size();
    if(iNumGPs!=iNumCurr) cout<<"number error!! "<<endl;
    
        
    if(DEBUG)
    {   
        cout<<pin_list <<endl<<"pinGP_number = "<<iNumGPs<<endl;
        for(iCounter=0; iCounter<iNumGPs;iCounter++)
        {
            cout<<sListPins[iCounter]<<endl;
        }
        cout<<iForce_list <<endl<<"current_number = "<<iNumCurr<<endl;
        for(iCounter=0; iCounter<iNumCurr;iCounter++)
        {
            cout<<sListCurrent[iCounter]<<endl;
        }
    }

    //************************************************************************************************
    //* IFVM executing...
    //************************************************************************************************
    for(pin_index=0;pin_index< iNumGPs;pin_index++)
    {
        ppmu_set.pin(sListPins[pin_index]).iForce(sListCurrent[pin_index] mA).iRange(sListCurrent[pin_index] mA).min(0 V).max(4 V);
    	
    }

    meas_set.pin(pin_list).execMode(TM::OVAL);
    task1.add(ppmu_set).add(meas_set);
    task1.execute();

    //************************************************************************************************
    //* Extract the value per-pin per-site
    //************************************************************************************************
    
    FOR_EACH_SITE_BEGIN();
          site_index=CURRENT_SITE_NUMBER()-1;
            
          for(int current_index=0;current_index<iNumCurr;current_index++)
          {
              pinNameList = PinUtility.getDigitalPinNamesFromPinList(sListPins[current_index], TM::IO_PIN, TRUE, TRUE);
              iNumPins=pinNameList.size();
              for(int pin_index=0;pin_index<iNumPins;pin_index++)
              {

            	  Vout.set(pinNameList[pin_index], site_index, sListCurrent[current_index], meas_set.getValue(pinNameList[pin_index]));
            	  
            	  if(DEBUG)
            	  {
            		  cout<<"site = "<<site_index+1 <<"  "<<pinNameList[pin_index]<<"_"<< sListCurrent[current_index]<<"mA_measured = "<<setprecision(3) << setw(7)<<Vout.get(pinNameList[pin_index],site_index, sListCurrent[current_index])<<endl;
            	  }
              }
          }
     FOR_EACH_SITE_END();

}
//added by sophia Oct12-2011
void IFVM_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, DOUBLE iForce, DOUBLE vClamp,data& Vout,int DEBUG, double delay_time,int samples)
{
	    //************************************************************************************************
	    //* Parameter related variable defination
	    //************************************************************************************************
	    STRING_VECTOR sListPins;
	    int iNumPins;
	    int pin_index,site_index;
	   // ON_FIRST_INVOCATION_BEGIN();
	    //************************************************************************************************
	    //* Split string-pins/string-iForce to array; Also get related number
	    //************************************************************************************************
	    tokenize(pin_list,sListPins,",");       iNumPins=sListPins.size();

	    // IFVM executing...
	    SPMU_task.pin(pin_list)
	    	.settling(delay_time mS) 
	    	.relay("NT")
	    	.iForce(iForce mA)
	    	.clamp(vClamp V)
	    	.max(3 V)
	    	.min(0 V)
	    	.mode("IFVM")
	    	.samples(samples);

	    SPMU_task.execMode("PVAL").preAction("NONE").execute();
	    
	    //************************************************************************************************
	    //* Extract the value per-pin per-site
	    //************************************************************************************************
	        
	   FOR_EACH_SITE_BEGIN();
	      site_index=CURRENT_SITE_NUMBER()-1;
	      for(pin_index=0;pin_index<iNumPins;pin_index++)
	      {
	          Vout.set(sListPins[pin_index], site_index,iForce, SPMU_task.getValue(sListPins[pin_index]));
	          if(DEBUG)
	          {
	            cout<<"site = "<<site_index+1 <<"  "<<sListPins[pin_index]<<"_"<<iForce<<"mA_measured = "     <<setprecision(3) << setw(7)<<Vout.get(sListPins[pin_index], site_index, iForce)<<endl;
	          
	          }
	      }
	    FOR_EACH_SITE_END();
}


//added by sophia on Dec21
//added by sophia Oct12-2011
//void IFVM_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, DOUBLE iForce, DOUBLE vClamp,data& Vout, int DEBUG,double delay_time,int samples)
//{
//	    //************************************************************************************************
//	    //* Parameter related variable defination
//	    //************************************************************************************************
//	    STRING_VECTOR sListPins;
//	    int iNumPins;
//	    int pin_index,site_index;
//	   // ON_FIRST_INVOCATION_BEGIN();
//	    //************************************************************************************************
//	    //* Split string-pins/string-iForce to array; Also get related number
//	    //************************************************************************************************
//	    tokenize(pin_list,sListPins,",");       iNumPins=sListPins.size();
//
//	    // IFVM executing...
//	    SPMU_task.pin(pin_list)
//	    	.settling(delay_time mS) 
//	    	.relay("NT")
//	    	.iForce(iForce mA)
//	    	.clamp(vClamp V)
//	    	.max(3 V)
//	    	.min(0 V)
//	    	.mode("IFVM")
//	    	.samples(samples);
//
//	    SPMU_task.execMode("PVAL").preAction("NONE").execute();
//	    
//	    //************************************************************************************************
//	    //* Extract the value per-pin per-site
//	    //************************************************************************************************
//	        
//	   FOR_EACH_SITE_BEGIN();
//	      site_index=CURRENT_SITE_NUMBER()-1;
//	      for(pin_index=0;pin_index<iNumPins;pin_index++)
//	      {
//	          Vout.set(sListPins[pin_index], site_index,iForce, SPMU_task.getValue(sListPins[pin_index]));
//	          if(DEBUG)
//	          {
//	            cout<<"site = "<< site_index+1 <<"  "<<sListPins[pin_index]<<"_"<<iForce<<"mA_measured = "     <<setprecision(3) << setw(7)<<Vout.get(sListPins[pin_index], site_index, iForce)<<endl;
//	          
//	          }
//	      }
//	    FOR_EACH_SITE_END();
//}



//void IFVM_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, DOUBLE iForce, DOUBLE vClamp,data& Vout,int DEBUG, double delay_time,int samples)
//{
//       //************************************************************************************************
//    //* Parameter related variable defination
//    //************************************************************************************************
//    STRING_VECTOR sListPins;
//    int iNumPins;
//    int pin_index,site_index;
//   // ON_FIRST_INVOCATION_BEGIN();
//    //************************************************************************************************
//    //* Split string-pins/string-iForce to array; Also get related number
//    //************************************************************************************************
//    tokenize(pin_list,sListPins,",");       iNumPins=sListPins.size();
//    //cout<<"iNumPins = "<<iNumPins<<endl;
//    //Vout.resize(iNumPins);
//
//    // IFVM executing...
//    SPMU_task.pin(pin_list)
//    	.settling(delay_time mS)
//    	.relay("NT")
//    	.iForce(iForce mA)
//    	.clamp(vClamp V)
//    	.max(3 V)
//    	.min(0 V)
//    	.mode("IFVM")
//    	.samples(samples);
//
//    SPMU_task.execMode("PVAL").preAction("NONE").execute();
//    
//    //************************************************************************************************
//    //* Extract the value per-pin per-site
//    //************************************************************************************************
//        
//   FOR_EACH_SITE_BEGIN();
//      site_index=CURRENT_SITE_NUMBER()-1;
//      for(pin_index=0;pin_index<iNumPins;pin_index++)
//      {
//          Vout.set(sListPins[pin_index], site_index,iForce, SPMU_task.getValue(sListPins[pin_index]));
//          if(0)
//          {
//            cout<<"site = "<<site_index+1 <<"  "<<sListPins[pin_index]<<"_"<<iForce<<"mA_measured = "     <<setprecision(3) << setw(7)<<Vout.get(sListPins[pin_index], site_index, iForce)<<endl;
//          
//          }
//      }
//    FOR_EACH_SITE_END();
//    
//}
////add a new parameter to control DEBUG (output to ui or not) on Nov24-2011 

//void IFVM_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, DOUBLE iForce, DOUBLE vClamp,data& Vout,int DEBUG, double delay_time,int samples)
//{
//
//    //************************************************************************************************
//    //* Parameter related variable defination
//    //************************************************************************************************
//    STRING_VECTOR sListPins;
//    int iNumPins;
//    int pin_index,site_index;
//   // ON_FIRST_INVOCATION_BEGIN();
//    //************************************************************************************************
//    //* Split string-pins/string-iForce to array; Also get related number
//    //************************************************************************************************
//    tokenize(pin_list,sListPins,",");       iNumPins=sListPins.size();
//
//    // IFVM executing...
//    SPMU_task.pin(pin_list)
//    	.settling(delay_time mS)
//    	.relay("NT")
//    	.iForce(iForce mA)
//    	.clamp(vClamp V)
//    	.max(3 V)
//    	.min(0 V)
//    	.mode("IFVM")
//    	.samples(samples);
//
//    SPMU_task.execMode("PVAL").preAction("NONE").execute();
//    
//    //************************************************************************************************
//    //* Extract the value per-pin per-site
//    //************************************************************************************************
//        
//   FOR_EACH_SITE_BEGIN();
//      site_index=CURRENT_SITE_NUMBER()-1;
//      for(pin_index=0;pin_index<iNumPins;pin_index++)
//      {
//          Vout.set(sListPins[pin_index], site_index,iForce, SPMU_task.getValue(sListPins[pin_index]));
//          if(DEBUG)
//          {
//            cout<<"site = "<<site_index+1 <<"  "<<sListPins[pin_index]<<"_"<<iForce<<"mA_measured = "     <<setprecision(3) << setw(7)<<Vout.get(sListPins[pin_index], site_index, iForce)<<" V"<<endl;
//          
//          }
//      }
//    FOR_EACH_SITE_END();
//    
//}

//void IFVM_SPMU_noRelay(SPMU_TASK& SPMU_task,STRING pin_list, DOUBLE iForce, DOUBLE vClamp, int samples,data& Vout)
//{
//	int DEBUG=0;
//    DOUBLE delay_time = 1;     //msec
//    //************************************************************************************************
//    //* Parameter related variable defination
//    //************************************************************************************************
//    STRING_VECTOR sListPins;
//    int iNumPins;
//    int pin_index,site_index;
//   // ON_FIRST_INVOCATION_BEGIN();
//    //************************************************************************************************
//    //* Split string-pins/string-iForce to array; Also get related number
//    //************************************************************************************************
//    tokenize(pin_list,sListPins,",");       iNumPins=sListPins.size();
//    //cout<<"iNumPins = "<<iNumPins<<endl;
//    //Vout.resize(iNumPins);
//
//    // IFVM executing...
//	//  spmu_ILIMIT_task.pin("dc_SWBUCK1")
//    SPMU_task.pin(pin_list)
//			  		//.iForceRange(500 mA) 
//			  		.iForce(iForce mA)				//this should be align with the last setting		
//			  		.clamp(vClamp V)
//			  		.min(3 V)
//			    	.max(3.6 V)
//			    	//.VMureRange(8 V)
//			    	.relay("NO");		
//	  
//    SPMU_task.preAction("NONE").execMode("PVAL").execute();
//    
////	  SPMU_task.pin(pin_list).iForce(iForce mA);
////	  SPMU_task.execute();
//    //************************************************************************************************
//    //* Extract the value per-pin per-site
//    //************************************************************************************************
//        
//   FOR_EACH_SITE_BEGIN();
//      site_index=CURRENT_SITE_NUMBER()-1;
//      for(pin_index=0;pin_index<iNumPins;pin_index++)
//      {
//          Vout.set(sListPins[pin_index], site_index,iForce, SPMU_task.getValue(sListPins[pin_index]));
//          if(DEBUG)
//          {
//            cout<<"site = "<<site_index+1 <<"  "<<sListPins[pin_index]<<"_"<<iForce<<"mA_measured = "     <<setprecision(3) << setw(7)<<Vout.get(sListPins[pin_index], site_index, iForce)<<endl;
//          
//          }
//      }
//    FOR_EACH_SITE_END();
//    
//}

//add DEBUG
//void IFVM_SPMU_noRelay(SPMU_TASK& SPMU_task,STRING pin_list, DOUBLE iForce, DOUBLE vClamp, data& Vout,int DEBUG, double delay_time,int samples)
//{
//	
//    //DOUBLE delay_time = 1;     //msec
//    //************************************************************************************************
//    //* Parameter related variable defination
//    //************************************************************************************************
//    STRING_VECTOR sListPins;
//    int iNumPins;
//    int pin_index,site_index;
//   // ON_FIRST_INVOCATION_BEGIN();
//    //************************************************************************************************
//    //* Split string-pins/string-iForce to array; Also get related number
//    //************************************************************************************************
//    tokenize(pin_list,sListPins,",");       iNumPins=sListPins.size();
//    //cout<<"iNumPins = "<<iNumPins<<endl;
//    //Vout.resize(iNumPins);
//
//    // IFVM executing...
//	//  spmu_ILIMIT_task.pin("dc_SWBUCK1")
//    SPMU_task.pin(pin_list)
//			  		//.iForceRange(500 mA) 
//			  		.iForce(iForce mA)				//this should be align with the last setting		
//			  		.clamp(vClamp V)
//			  		.min(3 V)
//			    	.max(3.6 V)
//			    	//.vMeasureRange(8 V)
//			    	.relay("NO");		
//	  
//    SPMU_task.preAction("NONE").execMode("PVAL").execute();
//    
////	  SPMU_task.pin(pin_list).iForce(iForce mA);
////	  SPMU_task.execute();
//    //************************************************************************************************
//    //* Extract the value per-pin per-site
//    //************************************************************************************************
//        
//   FOR_EACH_SITE_BEGIN();
//      site_index=CURRENT_SITE_NUMBER()-1;
//      for(pin_index=0;pin_index<iNumPins;pin_index++)
//      {
//          Vout.set(sListPins[pin_index], site_index,iForce, SPMU_task.getValue(sListPins[pin_index]));
//          if(DEBUG)
//          {
//            cout<<"site = "<<site_index+1<<"  "<<sListPins[pin_index]<<"_"<<iForce<<"mA_measured = "     <<setprecision(3) << setw(7)<<Vout.get(sListPins[pin_index], site_index, iForce)<<endl;
//          
//          }
//      }
//    FOR_EACH_SITE_END();
//    
//}
void IFVM_SPMU_PreCharge(SPMU_TASK& SPMU_task,STRING pin_list, DOUBLE iForce, DOUBLE vClamp, DOUBLE PreCharge,data& Vout,int DEBUG, int pro_index,double delay_time,int samples)
{
    //SPMU_TASK SPMU_task;
 //   DOUBLE delay_time = 1;     //msec
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************

    int iNumPins;
    int pin_index;
    int site_index;
    double DelayTime=delay_time, IForce=iForce, VClamp=vClamp, PRECharge=PreCharge;


    // IFVM executing...
    SPMU_task.pin(pin_list)
    	.settling(DelayTime)
    	.relay("NT")
    	.iForce(IForce mA)
    	.iForceRange(abs(IForce) mA)
    	.preCharge(PRECharge).clamp(VClamp V)
    	.max(2 V).min(0 V)
    	.mode("IFVM")
    	.samples(samples);

    SPMU_task.execMode("PVAL").preAction("NONE").execute(); 
    
    //************************************************************************************************
    //* Split string-pins/string-iForce to array; Also get related number
    //************************************************************************************************
 
	vector<string> sListPins; 
	
	if ( string::npos == pin_list.find_first_of(",",0) ) {
		sListPins.push_back(pin_list);
	} else {
		tokenize(pin_list,sListPins,",");   
	}
    iNumPins=sListPins.size();
    //cout<<"iNumPins = "<<iNumPins<<endl;
        
    //************************************************************************************************
    //* Extract the value per-pin per-site
    //************************************************************************************************

	FOR_EACH_SITE_BEGIN();
      site_index=CURRENT_SITE_NUMBER()-1;

      for(pin_index=0;pin_index<iNumPins;pin_index++)
      {
          Vout.set(sListPins[pin_index], site_index,pro_index, SPMU_task.getValue(sListPins[pin_index]));
          if(DEBUG)
          { 
        	  cout<<"site = "<<site_index+1 <<"  "<<sListPins[pin_index]<<"_"<<iForce<<"mA_measured = "     <<setprecision(3) << setw(7)<<Vout.get(sListPins[pin_index], site_index, pro_index)<<endl;
        	  cout << delay_time << " " << iForce << " " << vClamp << " " << PreCharge << endl;
           	  cout << DelayTime << " " << IForce << " " << VClamp << " " << PRECharge << endl;
              
          }
      }
    FOR_EACH_SITE_END();
    
}
//*************************************
//added by sophia
//Jun 15th 2012
//*************************************

void IFVM_SPMU_PreCharge(SPMU_TASK& SPMU_task,STRING pin_list, STRING iForce_list, STRING vClamp_list, STRING PreCharge_list, data& Vout,int DEBUG, double delay_time,int samples)
{

    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    static vector<string> sListPins;
    static vector<DOUBLE> sListCurrent,sListVClamp,sListVpreCharge;
    int iNumPins,iNumCurr,iNumVclamp;
    int pin_index,site_index;
    //************************************************************************************************
    //* Split string-pins/string-iForce to array; Also get related number
    //************************************************************************************************
    tokenize(pin_list,sListPins,",");           iNumPins=sListPins.size();
    tokenize(iForce_list,sListCurrent,",");     iNumCurr=sListCurrent.size();
    tokenize(vClamp_list,sListVClamp,",");      iNumVclamp=sListVClamp.size();
    tokenize(PreCharge_list,sListVpreCharge,",");      iNumVclamp=sListVClamp.size();
    
    if((iNumPins!=iNumCurr)||(iNumPins!=iNumVclamp)) cout<<"number error!! "<<endl;
    //cout<<"iNumPins = "<<iNumPins<<endl;
    //Vout.resize(iNumPins);

    // IFVM executing...
    for(pin_index=0;pin_index<iNumPins;pin_index++)
    {
        SPMU_task.pin(sListPins[pin_index])
        			.settling(delay_time mS)
        			.relay("NT")
        			.iForce(sListCurrent[pin_index] mA)
        			.preCharge(sListVpreCharge[pin_index] V)
        			.clamp(sListVClamp[pin_index] V)
        			.max(3 V)
        			.min(0 V)
        			.mode("IFVM")
        			.samples(samples);
    }

    SPMU_task.execMode("PVAL").preAction("NONE").execute();
    
    //************************************************************************************************
    //* Extract the value per-pin per-site
    //************************************************************************************************


   FOR_EACH_SITE_BEGIN();
      site_index=CURRENT_SITE_NUMBER()-1;
      for(pin_index=0;pin_index<iNumPins;pin_index++)
      {
          Vout.set(sListPins[pin_index], site_index,sListCurrent[pin_index], SPMU_task.getValue(sListPins[pin_index]));
          if(DEBUG)
          {
            cout<<"site = "<<site_index+1 <<"  "<<sListPins[pin_index]<<"_"<<sListCurrent[pin_index]<<"mA_measured = "     <<Vout.get(sListPins[pin_index], site_index, sListCurrent[pin_index])<<endl;
          
          }
      }
    FOR_EACH_SITE_END();
    
    
}
void IFVM_SPMU(SPMU_TASK& SPMU_task,STRING pin_list,STRING iForce_list, STRING vClamp_list,data& Vout,int DEBUG, double delay_time,int samples)
{


    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    static vector<string> sListPins;
    static vector<DOUBLE> sListCurrent,sListVClamp;
    int iNumPins,iNumCurr,iNumVclamp;
    int pin_index,site_index;
    //************************************************************************************************
    //* Split string-pins/string-iForce to array; Also get related number
    //************************************************************************************************
    tokenize(pin_list,sListPins,",");           iNumPins=sListPins.size();
    tokenize(iForce_list,sListCurrent,",");     iNumCurr=sListCurrent.size();
    tokenize(vClamp_list,sListVClamp,",");      iNumVclamp=sListVClamp.size();
    
    if((iNumPins!=iNumCurr)||(iNumPins!=iNumVclamp)) cout<<"number error!! "<<endl;
    //cout<<"iNumPins = "<<iNumPins<<endl;
    //Vout.resize(iNumPins);

    // IFVM executing...
    for(pin_index=0;pin_index<iNumPins;pin_index++)
    {
        SPMU_task.pin(sListPins[pin_index])
        			.settling(delay_time mS)
        			.relay("NT")
        			.iForce(sListCurrent[pin_index] mA)
        			.clamp(sListVClamp[pin_index] V)
        			.max(3 V)
        			.min(0 V)
        			.mode("IFVM")
        			.samples(samples);
    }

    SPMU_task.execMode("PVAL").preAction("NONE").execute();
    
    //************************************************************************************************
    //* Extract the value per-pin per-site
    //************************************************************************************************


   FOR_EACH_SITE_BEGIN();
      site_index=CURRENT_SITE_NUMBER()-1;
      for(pin_index=0;pin_index<iNumPins;pin_index++)
      {
          Vout.set(sListPins[pin_index], site_index,sListCurrent[pin_index], SPMU_task.getValue(sListPins[pin_index]));
          if(DEBUG)
          {
            cout<<"site = "<<site_index+1 <<"  "<<sListPins[pin_index]<<"_"<<sListCurrent[pin_index]<<"mA_measured = "     <<Vout.get(sListPins[pin_index], site_index, sListCurrent[pin_index])<<endl;
          
          }
      }
    FOR_EACH_SITE_END();
    
}




///Iout meas VFIM 
//void VFIM_PPMU(STRING pin_list, double vForce_value, double iRang_Value,    data& Vout,int DEBUG)
//{
//    
//
//    //************************************************************************************************
//    //* ppmu_xxx/taskX defination
//    //************************************************************************************************
//    PPMU_SETTING  ppmu_set,ppmu_off;
//    PPMU_RELAY    relay_open, relay_close;
//    PPMU_MEASURE  meas_set;
//    TASK_LIST     task_ppmu;
//    DOUBLE relayClose_wait = 1;     //msec
//    DOUBLE relayOpen_wait = 1;      //msec
//    int pin_index,site_index;
//    
//    //************************************************************************************************
//    //* Parameter related variable defination
//    //************************************************************************************************
//    STRING_VECTOR pinNameList;
//    int iNumPins;
//
//    //************************************************************************************************
//    //* Split string-pins/string-iForce to array
//    //* Also get related number
//    //************************************************************************************************
//    pinNameList = PinUtility.getDigitalPinNamesFromPinList(pin_list, TM::IO_PIN, TRUE, TRUE);
//    iNumPins=pinNameList.size();
//     
//    //************************************************************************************************
//    //* IFVM executing...
//    //************************************************************************************************
//   // ON_FIRST_INVOCATION_BEGIN();
//           
//        ppmu_set.pin(pin_list).vForce(vForce_value V).iRange(iRang_Value mA).min(0 ).max(iRang_Value mA);
//        meas_set.pin(pin_list).execMode(TM::OVAL);
//        ppmu_off.pin(pin_list).vForce(0.0 V).iRange(iRang_Value mA).min(0 ).max(iRang_Value mA);
//        
//        relay_close. pin(pin_list).status("PPMU_ON");
//        relay_close. wait(relayClose_wait mS);
//        
//        //"PPMU_OFF" will keep AC relay open, using "AC_ON" to switch PPMU off and keep AC relay on
//        relay_open . pin(pin_list).status("AC_ON"); 
//        relay_open. wait(relayOpen_wait mS);
//        
//        task_ppmu.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
//        task_ppmu.execute();
//    
//        //************************************************************************************************
//        //* Extract the value per-pin per-site
//        //************************************************************************************************
//
//        FOR_EACH_SITE_BEGIN();
//          site_index=CURRENT_SITE_NUMBER()-1;
//            
//          for(pin_index=0;pin_index<iNumPins;pin_index++)
//          {
//              Vout.set(pinNameList[pin_index], site_index, vForce_value, meas_set.getValue(pinNameList[pin_index]));
//              if(DEBUG)
//              {
//                cout<<"site = "<<site_index+1 <<"  "<<pinNameList[pin_index]<<"_"<< vForce_value <<"V_measured = "<<1e6*Vout.get(pinNameList[pin_index], site_index, vForce_value)<<" uA"<<endl;
//              }
//          }
//     FOR_EACH_SITE_END();
//    
//}
//add DEBUG
///Iout meas VFIM 
void VFIM_PPMU(STRING pin_list, double vForce_value, double iRang_Value,  data& Vout,int DEBUG, double delay_time,int samples)
{

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set,ppmu_off;
    PPMU_RELAY    relay_open, relay_close;
    PPMU_MEASURE  meas_set;
    TASK_LIST     task_ppmu;

    int pin_index,site_index;
    
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    STRING_VECTOR pinNameList;
    int iNumPins;

    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    pinNameList = PinUtility.getDigitalPinNamesFromPinList(pin_list, TM::IO_PIN, TRUE, TRUE);
    iNumPins=pinNameList.size();
     
    //************************************************************************************************
    //* IFVM executing...
    //************************************************************************************************
   // ON_FIRST_INVOCATION_BEGIN();
           
        ppmu_set.pin(pin_list).vForce(vForce_value V).iRange(iRang_Value mA).min(0 ).max(iRang_Value mA);
        meas_set.pin(pin_list).execMode(TM::OVAL).numberOfSamples(samples);
        ppmu_off.pin(pin_list).vForce(0.0 V).iRange(iRang_Value mA).min(0 ).max(iRang_Value mA);
        
        relay_close. pin(pin_list).status("PPMU_ON");
        relay_close. wait(delay_time mS);
        
        //"PPMU_OFF" will keep AC relay open, using "AC_ON" to switch PPMU off and keep AC relay on
        relay_open . pin(pin_list).status("AC_ON"); 
        relay_open. wait(delay_time mS);
        
        task_ppmu.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
        task_ppmu.execute();
    
        //************************************************************************************************
        //* Extract the value per-pin per-site
        //************************************************************************************************

        FOR_EACH_SITE_BEGIN();
          site_index=CURRENT_SITE_NUMBER()-1;
            
          for(pin_index=0;pin_index<iNumPins;pin_index++)
          {
              Vout.set(pinNameList[pin_index], site_index, vForce_value, meas_set.getValue(pinNameList[pin_index]));
              if(DEBUG)
              {
                cout<<"site = "<<site_index+1 <<"  "<<pinNameList[pin_index]<<"_"<< vForce_value <<"V_measured = "<<1e3*Vout.get(pinNameList[pin_index], site_index, vForce_value)<<" mA"<<endl;
              }
          }
     FOR_EACH_SITE_END();
    
}
//no relay control
void VFIM_PPMU_noRelay(STRING pin_list, double vForce_value, double iRang_Value,    data& Vout,int DEBUG, double delay_time,int samples)
{
    

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set;
    PPMU_MEASURE  meas_set;
    TASK_LIST     task_ppmu;
    int pin_index,site_index;
    
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    STRING_VECTOR pinNameList;
    int iNumPins;

    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    pinNameList = PinUtility.getDigitalPinNamesFromPinList(pin_list, TM::IO_PIN, TRUE, TRUE);
    iNumPins=pinNameList.size();
     
    //************************************************************************************************
    //* IFVM executing...
    //************************************************************************************************
   // ON_FIRST_INVOCATION_BEGIN();
           
        ppmu_set.pin(pin_list).vForce(vForce_value V).iRange(iRang_Value).min(0 ).max(iRang_Value mA);
        meas_set.pin(pin_list).execMode(TM::OVAL).numberOfSamples(samples);
        task_ppmu.add(ppmu_set).add(meas_set);
                
        task_ppmu.execute();
    
        //************************************************************************************************
        //* Extract the value per-pin per-site
        //************************************************************************************************

        FOR_EACH_SITE_BEGIN();
          site_index=CURRENT_SITE_NUMBER()-1;
            
          for(pin_index=0;pin_index<iNumPins;pin_index++)
          {
              Vout.set(pinNameList[pin_index], site_index, vForce_value, meas_set.getValue(pinNameList[pin_index]));
              if(DEBUG)
              {
                cout<<"site = "<<site_index+1 <<"  "<<pinNameList[pin_index]<<"_"<< vForce_value <<"V_measured = "<<1e6*Vout.get(pinNameList[pin_index], site_index, vForce_value)<<" uA"<<endl;
              }
          }
     FOR_EACH_SITE_END();
    
}
////VFIM_PPMU_noRelay
void VFIM_PPMU(STRING pin_list,  STRING vForce_list, STRING iRang_Value_list,    data& Iout,int DEBUG, double delay_time,int samples)
{

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set,ppmu_off;
    PPMU_RELAY    relay_open, relay_close;
    PPMU_MEASURE  meas_set;
    TASK_LIST     task_ppmu;

    int pin_index,site_index;
    static vector<string> sListPins;
    static vector<double> sListVol,sListRang;

    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    STRING_VECTOR pinNameList;
    int iNumPins,iNumVols,iNumRngs,iNumAllPins;

    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************

    
    tokenize(pin_list,sListPins,";");        	iNumPins=sListPins.size();
    tokenize(vForce_list,sListVol,",");      	iNumVols=sListVol.size();
    tokenize(iRang_Value_list,sListRang,",");   iNumRngs=sListRang.size();
    
    //************************************************************************************************
    //* IFVM executing...
    //************************************************************************************************
    for(pin_index=0;pin_index<iNumPins;pin_index++)
    {        
    	//cout<<"pin:"<< sListPins[pin_index] <<" VF ="<<sListVol[pin_index]<<endl;
        ppmu_set.pin(sListPins[pin_index]).vForce(sListVol[pin_index] V).iRange(sListRang[pin_index] mA).min(0 ).max(sListRang[pin_index] mA);
        meas_set.pin(sListPins[pin_index]).execMode(TM::OVAL).numberOfSamples(samples);
        //ppmu_off.pin(sListPins[pin_index]).vForce(0.0 V).iRange(sListRang[pin_index] mA).min(0 ).max(sListRang[pin_index] mA);;

        relay_close. pin(sListPins[pin_index]).status("PPMU_ON");	relay_close. wait(delay_time mS);
        relay_open . pin(sListPins[pin_index]).status("AC_ON");     relay_open. wait(delay_time mS);
    }

    task_ppmu.add(relay_close).add(ppmu_set).add(meas_set).add(relay_open);
    task_ppmu.execute();
    
    //************************************************************************************************
    //* Extract the value per-pin per-site
    //************************************************************************************************

    FOR_EACH_SITE_BEGIN();
    	  site_index=CURRENT_SITE_NUMBER()-1;
          for(int current_index=0;current_index<iNumPins;current_index++)
          {
              pinNameList = PinUtility.getDigitalPinNamesFromPinList(sListPins[current_index], TM::IO_PIN, TRUE, TRUE);
              iNumAllPins=pinNameList.size();
              for(int pin_index=0;pin_index<iNumAllPins;pin_index++)
              {
        	    	Iout.set(pinNameList[pin_index], site_index, sListVol[current_index], meas_set.getValue(pinNameList[pin_index]));
        	    	if(DEBUG)  cout<<"site = "<<site_index+1 <<"  "<<pinNameList[pin_index]<<"_"<<sListVol[current_index] <<"V_measured = "<<1e3*Iout.get(pinNameList[pin_index], site_index, sListVol[current_index])<<" mA"<<endl;
        	    	
        	    }
              
          }
     FOR_EACH_SITE_END();

}

void VFIM_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, double vForce_value, data& Iout,int DEBUG, double delay_time,int samples)
{
	
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    STRING_VECTOR sListPins;
    int iNumPins;
    int pin_index,site_index;
   
   // ON_FIRST_INVOCATION_BEGIN();
    //************************************************************************************************
    //* Split string-pins/string-iForce to array; Also get related number
    //************************************************************************************************
    tokenize(pin_list,sListPins,",");       iNumPins=sListPins.size();
    //cout<<"iNumPins = "<<iNumPins<<endl;
    //Vout.resize(iNumPins);

    // IFVM executing...
    SPMU_task.pin(pin_list)
    	.settling(delay_time)
    	.relay("NT")
    	.vForce(vForce_value V)
    	.clamp(1 mA)
    	.max(100 mA)
    	.min(0 mA)
    	.mode("VFIM")
    	.samples(samples);

    SPMU_task.execMode("PVAL").preAction("NONE").execute();
    
    //************************************************************************************************
    //* Extract the value per-pin per-site
    //************************************************************************************************
        
   FOR_EACH_SITE_BEGIN();
      site_index=CURRENT_SITE_NUMBER()-1;
      for(pin_index=0;pin_index<iNumPins;pin_index++)
      {
    	  Iout.set(sListPins[pin_index], site_index,vForce_value, SPMU_task.getValue(sListPins[pin_index]));
          if(DEBUG)
          {
            cout<<"site = "<<site_index+1 <<"  "<<sListPins[pin_index]<<"_"<<vForce_value<<"V_measured = "     <<setprecision(3) << setw(7)<<Iout.get(sListPins[pin_index], site_index, vForce_value)*1e6<<"uA"<<endl;
          
          }
      }
    FOR_EACH_SITE_END();
    
}

//
//void VFIM_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, double vForce_value,double iRang_Value, data& Iout)
//{
//	   //************************************************************************************************
//	    //* Parameter related variable defination
//	    //************************************************************************************************
//	    STRING_VECTOR sListPins;
//	    int iNumPins;
//	    int pin_index,site_index;
//	    int samples=10;
//	   // ON_FIRST_INVOCATION_BEGIN();
//	    //************************************************************************************************
//	    //* Split string-pins/string-iForce to array; Also get related number
//	    //************************************************************************************************
//	    tokenize(pin_list,sListPins,",");       iNumPins=sListPins.size();
//	    //cout<<"iNumPins = "<<iNumPins<<endl;
//	    //Vout.resize(iNumPins);
//
//	    // IFVM executing...
//	    SPMU_task.pin(pin_list)
//	    	.relay("NT")
//	    	.iForceRange(iRang_Value mA)
//	    	.vForce(vForce_value V)
//	    	.clamp(iRang_Value mA)
//	    	.max(iRang_Value mA)
//	    	.min(0 mA)
//	    	.mode("VFIM")
//	    	.samples(samples);
//
//	    SPMU_task.execMode("PVAL").preAction("NONE").execute();
//	    
//	    //************************************************************************************************
//	    //* Extract the value per-pin per-site
//	    //************************************************************************************************
//	        
//	   FOR_EACH_SITE_BEGIN();
//	      site_index=CURRENT_SITE_NUMBER()-1;
//	      for(pin_index=0;pin_index<iNumPins;pin_index++)
//	      {
//	    	  Iout.set(sListPins[pin_index], site_index,vForce_value, SPMU_task.getValue(sListPins[pin_index]));
//	          if(0)
//	          {
//	            cout<<"site = "<<site_index+1 <<"  "<<sListPins[pin_index]<<"_"<<vForce_value<<"V_measured = "     <<setprecision(3) << setw(7)<<Iout.get(sListPins[pin_index], site_index, vForce_value)*1e3<<"mA"<<endl;
//	          
//	          }
//	      }
//	    FOR_EACH_SITE_END();
//	
//}
//
////Add on Nov 24th 2011, add parameter DEBUG
//void VFIM_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, double vForce_value,double iRang_Value,int DEBUG ,data& Iout)
//{
//	   //************************************************************************************************
//	    //* Parameter related variable defination
//	    //************************************************************************************************
//	    STRING_VECTOR sListPins;
//	    int iNumPins;
//	    int pin_index,site_index;
//	    int samples=10;
//	   // ON_FIRST_INVOCATION_BEGIN();
//	    //************************************************************************************************
//	    //* Split string-pins/string-iForce to array; Also get related number
//	    //************************************************************************************************
//	    tokenize(pin_list,sListPins,",");       iNumPins=sListPins.size();
//	    //cout<<"iNumPins = "<<iNumPins<<endl;
//	    //Vout.resize(iNumPins);
//
//	    // IFVM executing...
//	    SPMU_task.pin(pin_list)
//	    	.relay("NT")
//	    	.iForceRange(iRang_Value mA)
//	    	.vForce(vForce_value V)
//	    	.clamp(iRang_Value mA)
//	    	.max(iRang_Value mA)
//	    	.min(0 mA)
//	    	.mode("VFIM")
//	    	.samples(samples);
//
//	    SPMU_task.execMode("PVAL").preAction("NONE").execute();
//	    
//	    //************************************************************************************************
//	    //* Extract the value per-pin per-site
//	    //************************************************************************************************
//	        
//	   FOR_EACH_SITE_BEGIN();
//	      site_index=CURRENT_SITE_NUMBER()-1;
//	      for(pin_index=0;pin_index<iNumPins;pin_index++)
//	      {
//	    	  Iout.set(sListPins[pin_index], site_index,vForce_value, SPMU_task.getValue(sListPins[pin_index]));
//	          if(DEBUG)
//	          {
//	            cout<<"site = "<<site_index+1 <<"  "<<sListPins[pin_index]<<"_"<<vForce_value<<"V_measured = "     <<setprecision(3) << setw(7)<<Iout.get(sListPins[pin_index], site_index, vForce_value)*1e3<<"mA"<<endl;
//	          
//	          }
//	      }
//	    FOR_EACH_SITE_END();
//	
//}


//Add on Nov 24th 2011, add parameter DEBUG

void VFIM_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, double vForce_value,double iRang_Value,data& Iout,int DEBUG, double delay_time,int samples)
{
	   //************************************************************************************************
	    //* Parameter related variable defination
	    //************************************************************************************************
	    STRING_VECTOR sListPins;
	    int iNumPins;
	    int pin_index,site_index;
	 
	   // ON_FIRST_INVOCATION_BEGIN();
	    //************************************************************************************************
	    //* Split string-pins/string-iForce to array; Also get related number
	    //************************************************************************************************
	    tokenize(pin_list,sListPins,",");       iNumPins=sListPins.size();
	    //cout<<"iNumPins = "<<iNumPins<<endl;
	    //Vout.resize(iNumPins);

	    // IFVM executing...
	    SPMU_task.pin(pin_list)
	    	.settling(delay_time ms)
	    	.relay("NT")
	    	.iForceRange(iRang_Value mA)
	    	.vForce(vForce_value V)
	    	.clamp(iRang_Value mA)
	    	.max(iRang_Value mA)
	    	.min(0 mA)
	    	.mode("VFIM")
	    	.samples(samples);

	    SPMU_task.execMode("PVAL").preAction("NONE").execute();
	    
	    //************************************************************************************************
	    //* Extract the value per-pin per-site
	    //************************************************************************************************
	        
	   FOR_EACH_SITE_BEGIN();
	      site_index=CURRENT_SITE_NUMBER()-1;
	      for(pin_index=0;pin_index<iNumPins;pin_index++)
	      {
	    	  Iout.set(sListPins[pin_index], site_index,vForce_value, SPMU_task.getValue(sListPins[pin_index]));
	          if(DEBUG)
	          {
	            cout<<"site = "<<site_index+1<<"  "<<sListPins[pin_index]<<"_"<<vForce_value<<"V_measured = "     <<setprecision(3) << setw(7)<<Iout.get(sListPins[pin_index], site_index, vForce_value)*1e3<<"mA"<<endl;
	          
	          }
	      }
	    FOR_EACH_SITE_END();
	
}


void VFIM_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, STRING vForce_list,STRING iRang_Value_list, data& Iout,int DEBUG, double delay_time,int samples)
{
	   //************************************************************************************************
	    //* Parameter related variable defination
	    //************************************************************************************************
	    int pin_index,site_index;
	    
	   // ON_FIRST_INVOCATION_BEGIN();
	    //************************************************************************************************
	    //* Split string-pins/string-iForce to array; Also get related number
	    //************************************************************************************************
	    //************************************************************************************************
	    //* Parameter related variable defination
	    //************************************************************************************************
	   // STRING_VECTOR pinNameList;
	    int iNumPins,iNumVols,iNumRngs,iNumAllPins;
	    static vector<string> sListPins,pinNameList;
	    static vector<double> sListVol,sListRang;
	    //************************************************************************************************
	    //* Split string-pins/string-iForce to array
	    //* Also get related number
	    //************************************************************************************************

	    
	    tokenize(pin_list,sListPins,";");        	iNumPins=sListPins.size();
	    tokenize(vForce_list,sListVol,",");      	iNumVols=sListVol.size();
	    tokenize(iRang_Value_list,sListRang,",");   iNumRngs=sListRang.size();
	    
	    
	    //************************************************************************************************
	     //* IFVM executing...
	     //************************************************************************************************
	     for(pin_index=0;pin_index<iNumPins;pin_index++)
	     {        
//	     	//cout<<"pin:"<< sListPins[pin_index] <<" VF ="<<sListVol[pin_index]<<endl;
//	         ppmu_set.pin(sListPins[pin_index]).vForce(sListVol[pin_index] V).iRange(sListRang[pin_index] mA).min(0 ).max(sListRang[pin_index] mA);
//	         meas_set.pin(sListPins[pin_index]).execMode(TM::OVAL);
//	         //ppmu_off.pin(sListPins[pin_index]).vForce(0.0 V).iRange(sListRang[pin_index] mA).min(0 ).max(sListRang[pin_index] mA);;
//
//	         relay_close. pin(sListPins[pin_index]).status("PPMU_ON");	relay_close. wait(relayClose_wait mS);
//	         relay_open . pin(sListPins[pin_index]).status("AC_ON");     relay_open. wait(relayOpen_wait mS);

	 	    // IFVM executing...
	 	    SPMU_task.pin(sListPins[pin_index])
	 	    	.relay("NT")
	 	    	.settling(delay_time ms)
	 	    	.vForce(sListVol[pin_index] V)
	 	    	.clamp(sListRang[pin_index] mA)
	 	    	.max(sListRang[pin_index] mA)
	 	    	.min(0 mA)
	 	    	.mode("VFIM")
	 	    	.samples(samples);
	     }

	    SPMU_task.execMode("PVAL").preAction("NONE").execute();
	    
	    //************************************************************************************************
	    //* Extract the value per-pin per-site
	    //************************************************************************************************
	        
	   FOR_EACH_SITE_BEGIN();
	      site_index=CURRENT_SITE_NUMBER()-1;
	               for(int current_index=0;current_index<iNumPins;current_index++)
	               {
	                   //pinNameList = PinUtility.getDigitalPinNamesFromPinList(sListPins[current_index], TM::IO_PIN, TRUE, TRUE);
	                   tokenize(sListPins[current_index],pinNameList,","); 
	                   	    
	                   iNumAllPins=pinNameList.size();
	                   for(int pin_index=0;pin_index<iNumAllPins;pin_index++)
	                   {
	             	    	Iout.set(pinNameList[pin_index], site_index, sListVol[current_index],  SPMU_task.getValue(pinNameList[pin_index]));
	             	    	if(DEBUG)  cout<<"site = "<<site_index+1 <<"  "<<pinNameList[pin_index]<<"_"<<sListVol[current_index] <<"V_measured = "<<1e3*Iout.get(pinNameList[pin_index], site_index, sListVol[current_index])<<" mA"<<endl;
	             	    	
	             	    }
	                   
	               }
	    FOR_EACH_SITE_END();
	
}


//void Vset_PPMU(STRING pin_list, double vForce_value,double iRang_Value)
//{
//    
//    //************************************************************************************************
//    //* ppmu_xxx/taskX defination
//    //************************************************************************************************
//    PPMU_SETTING  ppmu_set,ppmu_off;
//    PPMU_RELAY    relay_open, relay_close;
//    TASK_LIST     task_ppmu;
//    DOUBLE relayClose_wait = 1;     //msec
//   
//    //************************************************************************************************
//    //* Parameter related variable defination
//    //************************************************************************************************
//    STRING_VECTOR pinNameList;
//    int iNumPins;
//
//    //************************************************************************************************
//    //* Split string-pins/string-iForce to array
//    //* Also get related number
//    //************************************************************************************************
//    pinNameList = PinUtility.getDigitalPinNamesFromPinList(pin_list, TM::IO_PIN, TRUE, TRUE);
//    iNumPins=pinNameList.size();
//     
//    //************************************************************************************************
//    //* VF executing...
//    //************************************************************************************************
//    ppmu_set.pin(pin_list).vForce(vForce_value V).iRange(iRang_Value mA).min(0 ).max(iRang_Value mA);
//    relay_close. pin(pin_list).status("PPMU_ON");
//    relay_close. wait(relayClose_wait mS);
//
//    task_ppmu.add(relay_close).add(ppmu_set);
//    task_ppmu.execute();
//}
//


void Vset_PPMU(STRING pin_list, double vForce_value,double iRang_Value)
{
    
    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set,ppmu_off;
    PPMU_RELAY    relay_open, relay_close;
    PPMU_CLAMP    clamp_on, clamp_off;
    TASK_LIST     task_ppmu;
    DOUBLE relayClose_wait = 1;     //msec
   

    //************************************************************************************************
    //* VF executing...
    //************************************************************************************************
    ppmu_set.pin(pin_list).vForce(vForce_value V).iRange(iRang_Value mA).min(0 ).max(iRang_Value mA);
    relay_close. pin(pin_list).status("PPMU_ON");
    relay_close. wait(relayClose_wait mS);

    clamp_on.pin(pin_list).status("CLAMP_ON").low(-1).high(5);
    clamp_off.pin(pin_list).status("CLAMP_OFF");

    task_ppmu.add(ppmu_set).add(clamp_on).add(relay_close).add(clamp_off);//.add(meas_set).add(relay_open).add(ppmu_off);
    task_ppmu.execute();

    task_ppmu.add(relay_close).add(ppmu_set);
    task_ppmu.execute();
}

void Vset_PPMU(STRING pin_list, STRING vForce_list,double iRang_Value)
{
	 
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    static vector<string> sListPins;
    static vector<DOUBLE> sListVolt;
    int iNumGPs,iNumVolt;
    int pin_index;
        STRING_VECTOR pinNameList;

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set;
    PPMU_RELAY    relay_close;
    PPMU_CLAMP    clamp_on, clamp_off;
    TASK_LIST     task1;

    DOUBLE relayClose_wait = 2;     //msec
    //DOUBLE relayOpen_wait = 1;      //msec
    
    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    tokenize(pin_list,sListPins,",");       iNumGPs=sListPins.size();
    tokenize(vForce_list,sListVolt,","); iNumVolt=sListVolt.size();
    if(iNumGPs!=iNumVolt) cout<<"number error!! "<<endl;
    
    for(pin_index=0;pin_index<iNumGPs;pin_index++)
     {        
     	 ppmu_set.pin(sListPins[pin_index]).vForce(sListVolt[pin_index] V).iRange(iRang_Value mA).min(0.00001 mA ).max(iRang_Value mA);
         relay_close. pin(sListPins[pin_index]).status("PPMU_ON");	
         relay_close. wait(relayClose_wait mS);

         //add clamp to avoid hot switch
         clamp_on.pin(sListPins[pin_index]).status("CLAMP_ON").low(-1).high(5);
         clamp_off.pin(sListPins[pin_index]).status("CLAMP_OFF");
     }

    task1.add(ppmu_set).add(clamp_on).add(relay_close).add(clamp_off);
    task1.execute();

//     task1.add(relay_close).add(ppmu_set);
//     task1.execute();
}

///


//******************************************
//Drive/Set VOLTAGE USING PPMU
//INPUT PIN AND VOLT ARE VECTORS
//******************************************
void Vset_PPMU(vector<string> sListPins, vector<DOUBLE> sListVolt,double iRang_Value)
{
	 
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************

    int iNumGPs,iNumVolt;
    int pin_index;
    STRING_VECTOR pinNameList;

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set;
    PPMU_RELAY    relay_close;
    PPMU_CLAMP    clamp_on, clamp_off;
    TASK_LIST     task1;

    //************************************************************************************************
    //* SET DEFAUT RELAY CLOSE TIME
    //************************************************************************************************
    DOUBLE relayClose_wait = 1;     //msec
    
    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    iNumGPs=sListPins.size();
    iNumVolt=sListVolt.size();
    if(iNumGPs!=iNumVolt) cout<<"number error!! "<<endl;
    
    for(pin_index=0;pin_index<iNumGPs;pin_index++)
     {        
     	 ppmu_set.pin(sListPins[pin_index]).vForce(sListVolt[pin_index] V).iRange(iRang_Value mA).min(0 ).max(iRang_Value mA);
         relay_close. pin(sListPins[pin_index]).status("PPMU_ON");	
         relay_close. wait(relayClose_wait mS);

         //add clamp to avoid hot switch
         clamp_on.pin(sListPins[pin_index]).status("CLAMP_ON").low(-1).high(5);
         clamp_off.pin(sListPins[pin_index]).status("CLAMP_OFF");
     }

    task1.add(ppmu_set).add(clamp_on).add(relay_close).add(clamp_off);
    task1.execute();


//     task1.add(relay_close).add(ppmu_set);
//     task1.execute();
}

void Iset_PPMU(STRING pin_list, DOUBLE iForce_Value)
{
	 
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    static vector<string> sListPins;
    static vector<DOUBLE> sListVolt;
    int iNumGPs;//,iNumVolt,iNumPins;
    int pin_index;
        STRING_VECTOR pinNameList;

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set;
    PPMU_RELAY    relay_close;
    PPMU_CLAMP    clamp_on, clamp_off;
    TASK_LIST     task1;

    DOUBLE relayClose_wait = 1;     //msec
    //DOUBLE relayOpen_wait = 1;      //msec
    
    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    tokenize(pin_list,sListPins,",");       iNumGPs=sListPins.size();
    
    
    for(pin_index=0;pin_index<iNumGPs;pin_index++)
     {       
    	//.iForce(iForce mA).iRange(iForce mA).min(0 ).max(4 V);
     	 ppmu_set.pin(sListPins[pin_index]).iForce(iForce_Value mA).iRange(iForce_Value mA).min(0 ).max(5 V);
         relay_close. pin(sListPins[pin_index]).status("PPMU_ON");	
         relay_close. wait(relayClose_wait mS);


         clamp_on.pin(sListPins[pin_index]).status("CLAMP_ON").low(-1).high(5);
         clamp_off.pin(sListPins[pin_index]).status("CLAMP_OFF");
     }

    task1.add(ppmu_set).add(clamp_on).add(relay_close).add(clamp_off);
    task1.execute();


//     task1.add(relay_close).add(ppmu_set);
//     task1.execute();
}
///added on Jul15-2013
void Iset_PPMU(STRING pin_list, STRING iForce_list)
{

    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    static vector<string> sListPins;
    static vector<DOUBLE> sListCurr;
    int iNumGPs,iNumCurr;//,iNumPins;
    int pin_index;
        STRING_VECTOR pinNameList;

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set;
    PPMU_RELAY    relay_close;
    PPMU_CLAMP    clamp_on, clamp_off;
    TASK_LIST     task1;

    DOUBLE relayClose_wait = 1;     //msec
    //DOUBLE relayOpen_wait = 1;      //msec

    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    tokenize(pin_list,sListPins,",");       iNumGPs=sListPins.size();

    tokenize(iForce_list,sListCurr,","); iNumCurr=sListCurr.size();

    for(pin_index=0;pin_index<iNumGPs;pin_index++)
     {
    	//.iForce(iForce mA).iRange(iForce mA).min(0 ).max(4 V);
     	 ppmu_set.pin(sListPins[pin_index]).iForce(sListCurr[pin_index] mA).iRange(20 mA).min(0 ).max(5 V);
         relay_close. pin(sListPins[pin_index]).status("PPMU_ON");
         relay_close. wait(relayClose_wait mS);


         clamp_on.pin(sListPins[pin_index]).status("CLAMP_ON").low(-1).high(5);
         clamp_off.pin(sListPins[pin_index]).status("CLAMP_OFF");

     }

    task1.add(ppmu_set).add(clamp_on).add(relay_close).add(clamp_off);
    task1.execute();


//     task1.add(relay_close).add(ppmu_set);
//     task1.execute();
}
void Iset_gangPPMU(STRING gangPin_list, DOUBLE iForce_Value,double active_value)
{

	  //************************************************************************************************
	    //* Parameter related variable defination
	    //************************************************************************************************
	    static vector<string> sList_gang,sListPins;
	    int iNumPins,iNumGang;


	    //************************************************************************************************
	    //* ppmu_xxx/taskX defination
	    //************************************************************************************************
	    PPMU_SETTING  ppmu_set;
	    PPMU_RELAY    relay_close;
	    PPMU_CLAMP    clamp_on, clamp_off;
	    TASK_LIST     task1;

	    DOUBLE relayClose_wait = 1;     //msec
	    //************************************************************************************************
	    //* Split string-pins/string-iForce to array
	    //* Also get related number
	    //************************************************************************************************
	    tokenize(gangPin_list,sList_gang,",");  iNumGang=sList_gang.size();



	    for(int pin_index=0; pin_index<iNumGang;pin_index++)
   	    {


			sListPins = PinUtility.getDigitalPinNamesFromPinList(sList_gang[pin_index],TM::ALL_DIGITAL);
			iNumPins=sList_gang.size();
			ppmu_set.pin(sList_gang[pin_index]).iForce((iForce_Value/iNumPins-active_value) mA).iRange(20 mA).min(0 ).max(5 V);

//	        clamp_on.pin(sListPins[pin_index]).status("CLAMP_ON").low(-1).high(5);
//	        clamp_off.pin(sListPins[pin_index]).status("CLAMP_OFF");
   	    }

	    if(abs(active_value)>0.01)
	   	    	relay_close. pin(gangPin_list).status("ACPM_ON");
	   	    else
	   	    	relay_close. pin(gangPin_list).status("PPMU_ON");
	    relay_close. wait(relayClose_wait mS);



//        task1.add(ppmu_set).add(clamp_on).add(relay_close).add(clamp_off);
//        task1.execute();

	    task1.add(relay_close).add(ppmu_set);
	    task1.execute();

}
//differnt gangPins for different force values
void Iset_gangPPMU(STRING gangPin_list, STRING gang_iForceValues,double active_value)
{

	  //************************************************************************************************
	    //* Parameter related variable defination
	    //************************************************************************************************
	    static vector<string> sList_gang,sListPins;
	    static vector<double> sListCurr;
	    int iNumGang,iNumCurr,iNumPins;

	    double iforce_value;
	    //************************************************************************************************
	    //* ppmu_xxx/taskX defination
	    //************************************************************************************************
	    PPMU_SETTING  ppmu_set;
	    PPMU_RELAY    relay_close;
	    PPMU_CLAMP    clamp_on, clamp_off;
	    TASK_LIST     task1;

	    DOUBLE relayClose_wait = 1;     //msec
	    //************************************************************************************************
	    //* Split string-pins/string-iForce to array
	    //* Also get related number
	    //************************************************************************************************

	    tokenize(gangPin_list,sList_gang,",");  iNumGang=sList_gang.size();
	    tokenize(gang_iForceValues,sListCurr,","); iNumCurr=sListCurr.size();

	    if((iNumGang!=iNumCurr)) cout<<"gangPins number should same with currentForce numbers!! "<<endl;


	    for(int pin_index=0; pin_index<iNumGang;pin_index++)
	    {
	    	sListPins = PinUtility.getDigitalPinNamesFromPinList(sList_gang[pin_index],TM::ALL_DIGITAL);
	    	iNumPins=sListPins.size();
	    	iforce_value=sListCurr[pin_index]/iNumPins-active_value;

	    	ppmu_set.pin(sList_gang[pin_index]).iForce(iforce_value mA).iRange(20 mA).min(0 ).max(5 V);

//	        clamp_on.pin(sListPins[pin_index]).status("CLAMP_ON").low(-1).high(5);
//	        clamp_off.pin(sListPins[pin_index]).status("CLAMP_OFF");
	    }

	    if(abs(active_value)>0.01)
	    	relay_close. pin(gangPin_list).status("ACPM_ON");
	    else
	    	relay_close. pin(gangPin_list).status("PPMU_ON");
	    relay_close. wait(relayClose_wait mS);

	    task1.add(relay_close).add(ppmu_set);
	    task1.execute();



//        task1.add(ppmu_set).add(clamp_on).add(relay_close).add(clamp_off);
//        task1.execute();
}

void IFVM_gangPPMU(STRING gangPin_force_list,STRING gangPin_sense_list, DOUBLE iForce_Value,data Vout,int DEBUG,double active_value)
{

	  //************************************************************************************************
	    //* Parameter related variable defination
	    //************************************************************************************************
	    static vector<string> sList_gang,sListPins,sListPins_s;
	    int iNumPins,iNumGang,iNumSensePin;


	    //************************************************************************************************
	    //* ppmu_xxx/taskX defination
	    //************************************************************************************************
	    PPMU_SETTING  ppmu_set,ppmu_off;
	    PPMU_RELAY    relay_close,relay_open;
	    TASK_LIST     task1;
	    PPMU_MEASURE meas_set;
	    DOUBLE relayClose_wait = 1;     //msec
	    //************************************************************************************************
	    //* Split string-pins/string-iForce to array
	    //* Also get related number
	    //************************************************************************************************
	    tokenize(gangPin_force_list,sList_gang,",");  iNumGang=sList_gang.size();

	    for(int pin_index=0; pin_index<iNumGang;pin_index++)
   	    {


			sListPins = PinUtility.getDigitalPinNamesFromPinList(sList_gang[pin_index],TM::ALL_DIGITAL);
			iNumPins=sListPins.size();
			ppmu_set.pin(sList_gang[pin_index]).iForce((iForce_Value/iNumPins-active_value) mA).iRange(20 mA).min(0 ).max(5 V);
   	    }


	    if(abs(active_value)>0.01)
	   	    	relay_close. pin(gangPin_force_list).status("ACPM_ON");
	   	    else
	   	    	relay_close. pin(gangPin_force_list).status("PPMU_ON");

	    meas_set.pin(gangPin_sense_list).execMode(TM::OVAL);
	    ppmu_set.pin(gangPin_sense_list).iForce(0.0 mA).iRange(20 mA).min(0 ).max(5 V);
	    ppmu_off.pin(gangPin_force_list).iForce(0.0).iRange(20 mA).min(0 ).max(4 V);

	    relay_close.pin(gangPin_sense_list).status("PPMU_ON");
	    relay_close. wait(relayClose_wait mS);


	    //"PPMU_OFF" will keep AC relay open, using "AC_ON" to switch PPMU off and keep AC relay on
	    relay_open . pin(gangPin_force_list).status("ALL_OFF");
	    relay_open . pin(gangPin_sense_list).status("ALL_OFF");
	    relay_open . wait(relayClose_wait mS);

	           //task1.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
	           task1.add(ppmu_set).add(relay_close).add(meas_set).add(relay_open).add(ppmu_off);
	           task1.execute();




	    sListPins_s   = PinUtility.getDigitalPinNamesFromPinList(gangPin_sense_list, TM::ALL_DIGITAL);
	    iNumSensePin=sListPins_s.size();

	    FOR_EACH_SITE_BEGIN();
			int site_index=CURRENT_SITE_NUMBER()-1;
			for(int i =0;i<iNumSensePin;i++)
			{
				Vout.set(sListPins_s[i], site_index, iForce_Value, meas_set.getValue(sListPins_s[i]));
								if(DEBUG)
								{
									cout<<"site = "<<site_index+1 <<"  "<<sListPins_s[i]<<"_measured = "<<Vout.get(sListPins_s[i], site_index, iForce_Value)<<endl;
								}
			}
	    FOR_EACH_SITE_END();
}
//differnt gangPins for different force values
void IFVM_gangPPMU(STRING gangPin_force_list,STRING gangPin_sense_list, STRING gang_iForceValues,data Vout,int DEBUG,double active_value)
{

	  //************************************************************************************************
	    //* Parameter related variable defination
	    //************************************************************************************************
	    static vector<string> sList_gang,sListPins,sListPins_s;
	    static vector<double> sListCurr;
	    int iNumGang,iNumCurr,iNumPins,iNumSensePin;

	    double iforce_value;
	    //************************************************************************************************
	    //* ppmu_xxx/taskX defination
	    //************************************************************************************************
	    PPMU_SETTING  ppmu_set,ppmu_off;
	    PPMU_RELAY    relay_close,relay_open;
	    PPMU_MEASURE meas_set;
	    TASK_LIST     task1;

	    DOUBLE relayClose_wait = 1;     //msec
	    //************************************************************************************************
	    //* Split string-pins/string-iForce to array
	    //* Also get related number
	    //************************************************************************************************

	    tokenize(gangPin_force_list,sList_gang,",");  iNumGang=sList_gang.size();
	    tokenize(gang_iForceValues,sListCurr,","); iNumCurr=sListCurr.size();

	    if((iNumGang!=iNumCurr)) cout<<"gangPins number should same with currentForce numbers!! "<<endl;


	    for(int pin_index=0; pin_index<iNumGang;pin_index++)
	    {
	    	sListPins = PinUtility.getDigitalPinNamesFromPinList(sList_gang[pin_index],TM::ALL_DIGITAL);
	    	iNumPins=sListPins.size();
	    	iforce_value=sListCurr[pin_index]/iNumPins-active_value;

	    	ppmu_set.pin(sList_gang[pin_index]).iForce(iforce_value mA).iRange(20 mA).min(0 ).max(5 V);
	    }

	    if(abs(active_value)>0.01)
	    	relay_close. pin(gangPin_force_list).status("ACPM_ON");
	    else
	    	relay_close. pin(gangPin_force_list).status("PPMU_ON");
	    relay_close. wait(relayClose_wait mS);

	    meas_set.pin(gangPin_sense_list).execMode(TM::OVAL);
	    ppmu_set.pin(gangPin_sense_list).iForce(0.0 mA).iRange(20 mA).min(0 ).max(5 V);
	    ppmu_off.pin(gangPin_force_list).iForce(0.0).iRange(20 mA).min(0 ).max(4 V);

	    relay_close.pin(gangPin_sense_list).status("PPMU_ON");
	    relay_close. wait(relayClose_wait mS);


	    //"PPMU_OFF" will keep AC relay open, using "AC_ON" to switch PPMU off and keep AC relay on
	    relay_open . pin(gangPin_force_list).status("ALL_OFF");
	    relay_open . pin(gangPin_sense_list).status("ALL_OFF");
	    relay_open . wait(relayClose_wait mS);

	           //task1.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
	           task1.add(ppmu_set).add(relay_close).add(meas_set).add(relay_open).add(ppmu_off);
	           task1.execute();




	    sListPins_s   = PinUtility.getDigitalPinNamesFromPinList(gangPin_sense_list, TM::ALL_DIGITAL);
	    iNumSensePin=sListPins_s.size();

	    FOR_EACH_SITE_BEGIN();
			int site_index=CURRENT_SITE_NUMBER()-1;
			for(int i =0;i<iNumSensePin;i++)
			{

				Vout.set(sListPins_s[i], site_index, sListCurr[i], meas_set.getValue(sListPins_s[i]));
				if(DEBUG)
				{
					cout<<"site = "<<site_index+1 <<"  "<<sListPins_s[i]<<"_measured = "<<Vout.get(sListPins_s[i], site_index, sListCurr[i])<<endl;
				}
			}
	    FOR_EACH_SITE_END();
}

//active load used

///
void Vset_PPMU_noRelay(STRING pin_list, STRING vForce_list,double iRang_Value)

{
	 
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    static vector<string> sListPins;
    static vector<DOUBLE> sListVolt;
    int iNumGPs,iNumVolt;//,iNumPins;
    int pin_index;
        STRING_VECTOR pinNameList;

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set;
    
    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    tokenize(pin_list,sListPins,",");       iNumGPs=sListPins.size();
    tokenize(vForce_list,sListVolt,","); iNumVolt=sListVolt.size();

    if(iNumGPs!=iNumVolt) cout<<"number error!! "<<endl;
    
    for(pin_index=0;pin_index<iNumGPs;pin_index++)
     {        
     	 ppmu_set.pin(sListPins[pin_index]).vForce(sListVolt[pin_index] V).iRange(iRang_Value mA).min(0 ).max(20 mA);
     }

    ppmu_set.execute();
}


//


//void VM_BADC(SPMU_TASK& badc_task,STRING pin_list,data& Vout)
//{	
//		int iNumPins,iNumAllPins;
//		static vector<string> sListPins;
//		STRING_VECTOR pinNameList;
//        tokenize(pin_list,sListPins,";");        	iNumPins=sListPins.size();
//
//		badc_task.pin(pin_list).mode("VM_HIZ").settling(1 ms);
//
//		badc_task.pin(pin_list).relay("TERM").min(0 V).max(5 V);
//
//		badc_task.preAction("NONE").execMode("PVAL").execute();  
//
//	    //************************************************************************************************
//	    //* Extract the value per-pin per-site
//	    //************************************************************************************************
//	        
//	   FOR_EACH_SITE_BEGIN();
//	      int site_index=CURRENT_SITE_NUMBER()-1;
//	               for(int current_index=0;current_index<iNumPins;current_index++)
//	               {
//	                   pinNameList = PinUtility.getDigitalPinNamesFromPinList(sListPins[current_index], TM::IO_PIN, TRUE, TRUE);
//	                   iNumAllPins=pinNameList.size();
//	                   for(int pin_index=0;pin_index<iNumAllPins;pin_index++)
//	                   {
//	             	    	Vout.set(pinNameList[pin_index], site_index, 0,  badc_task.getValue(pinNameList[pin_index]));
//	             	    	if(0)  cout<<"site = "<<site_index+1 <<"  "<<pinNameList[pin_index]<<" HIZ V_measured = "<<Vout.get(pinNameList[pin_index], site_index, 0)<<" V"<<endl;
//	             	    	
//	             	    }
//	                   
//	               }
//	    FOR_EACH_SITE_END();
//	    
//
//}
//add debug mode
void VM_BADC(SPMU_TASK& badc_task,STRING pin_list, data& Vout,int DEBUG,int progIndex, double delay_time,int samples)
{	
		int iNumPins,iNumAllPins;
		static vector<string> sListPins;
		STRING_VECTOR pinNameList;
        tokenize(pin_list,sListPins,",");        	iNumPins=sListPins.size();

        badc_task.pin(pin_list).mode("VM_HIZ").settling(delay_time ms).samples(samples);
		badc_task.pin(pin_list).relay("TERM").min(0 V).max(5 V);//NTBBM//TERM
		badc_task.preAction("NONE").execMode("PVAL").execute();  

	    //************************************************************************************************
	    //* Extract the value per-pin per-site
	    //************************************************************************************************
	   FOR_EACH_SITE_BEGIN();
	      int site_index=CURRENT_SITE_NUMBER()-1;
	               for(int current_index=0;current_index<iNumPins;current_index++)
	               {
	                   pinNameList = PinUtility.getDigitalPinNamesFromPinList(sListPins[current_index], TM::IO_PIN, TRUE, TRUE);
	                   iNumAllPins=pinNameList.size();
	                   for(int pin_index=0;pin_index<iNumAllPins;pin_index++)
	                   {
	             	    	Vout.set(pinNameList[pin_index], site_index, progIndex,  badc_task.getValue(pinNameList[pin_index]));
	             	    	if(DEBUG)  cout<<"site = "<<site_index+1 <<"  "<<pinNameList[pin_index]<<" HIZ V_measured = "<<Vout.get(pinNameList[pin_index], site_index, 0)<<" V"<<endl;
	             	    	
	             	    }
	                   
	               }
	    FOR_EACH_SITE_END();
}
//******************************************************************
//no any relay actions for this measure
//you should set&reset the relays before and after the measurements
//******************************************************************
void VM_PPMU(STRING pin_list,data& Vout,int DEBUG, int lev_index)
{

    
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    int iNumPins;
    STRING_VECTOR pinNameList;

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_MEASURE  meas_set;
    //************************************************************************************************
    //* Split string-pins to array
    //* Also get related number
    //************************************************************************************************
    pinNameList = PinUtility.getDigitalPinNamesFromPinList(pin_list, TM::IO_PIN, TRUE, TRUE);

    meas_set.pin(pin_list).execMode(TM::OVAL).numberOfSamples(64);
    SMC_WAIT_TIME(2 ms);
    meas_set.execute();
    //************************************************************************************************
    //* Extract the value per-pin per-site
    //************************************************************************************************
    
    FOR_EACH_SITE_BEGIN();
      int   site_index=CURRENT_SITE_NUMBER()-1;
            
      iNumPins=pinNameList.size();
      for(int pin_index=0;pin_index<iNumPins;pin_index++)
      {
    	  Vout.set(pinNameList[pin_index], site_index, lev_index, meas_set.getValue(pinNameList[pin_index]));

    	  if(DEBUG)
    	  {
    		  cout<<"site = "<<site_index+1<<"  "<< pinNameList[pin_index]<<"_measured = "<<setprecision(3) << setw(7)<<Vout.get(pinNameList[pin_index],site_index, lev_index)<<endl;
    	  }
      }
      FOR_EACH_SITE_END();
}


void Meas_PPMU(STRING pin_list,data& VIout,int DEBUG,int pro_index)
{


    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    static vector<string> sListPins;
    int iNumGPs,iNumPins;
    int site_index;
    STRING_VECTOR pinNameList;

    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_MEASURE  meas_set;

    //************************************************************************************************
    //* Split string-pins to array
    //* Also get related number
    //************************************************************************************************
    tokenize(pin_list,sListPins,",");       iNumGPs=sListPins.size();

    meas_set.pin(pin_list).execMode(TM::PVAL).numberOfSamples(256).measurementType("PMUM");
    meas_set.execute();
    //************************************************************************************************
    //* Extract the value per-pin per-site
    //************************************************************************************************

    FOR_EACH_SITE_BEGIN();
          site_index=CURRENT_SITE_NUMBER()-1;

          for(int current_index=0;current_index<iNumGPs;current_index++)
          {
              pinNameList = PinUtility.getDigitalPinNamesFromPinList(sListPins[current_index], TM::IO_PIN, TRUE, TRUE);
              iNumPins=pinNameList.size();
              for(int pin_index=0;pin_index<iNumPins;pin_index++)
              {
            	  VIout.set(pinNameList[pin_index], site_index, pro_index, meas_set.getValue(pinNameList[pin_index]));

            	  if(DEBUG)
            	  {
            		  cout<<"site = "<<site_index+1<<"  "<< pinNameList[pin_index]<<"_measured = "<<setprecision(3) << setw(7)<<VIout.get(pinNameList[pin_index],site_index,pro_index)<<endl;
            	  }
              }
          }
     FOR_EACH_SITE_END();
}
void PN_Calc(ARRAY_D spec, int main_tone, DOUBLE RBW, STRING Freq_List, data& PPNout)
{
	int site_index=CURRENT_SITE_NUMBER()-1;
//    DOUBLE sampleRate = measParams[TM::MEAS_SAMPLE_RATE];
//    DOUBLE RBW= sampleRate/(double)numberOfSamples;


    //TX_PN_Calib(RF_Freq, TX_PPN_10K_loss[CURRENT_SITE_NUMBER()-1], TX_PPN_100K_loss[CURRENT_SITE_NUMBER()-1], TX_PPN_200K_loss[CURRENT_SITE_NUMBER()-1], TX_PPN_400K_loss[CURRENT_SITE_NUMBER()-1], CURRENT_SITE_NUMBER()-1);
	
	int iNumFreq;
	static vector<DOUBLE> sListFWs;
    tokenize(Freq_List,sListFWs,",");        	iNumFreq=sListFWs.size();
    double temp_value;
    double measuredPower=spec[main_tone];
    double Noise_power;
    
//    cout<<"******************************"<<endl;
//    cout<<"measuredPower"<<measuredPower<<endl;
	
    for(int f_index=0;f_index<iNumFreq;f_index++)
	{
	    int PPN_bin=main_tone+int(sListFWs[f_index]/RBW);
	    Noise_power=0;
	    
	    for (int i=PPN_bin;i<PPN_bin+10;i++)
	    {
	    	Noise_power+=pow(10,spec[i]/10);
	    }
	    cout<<"index = "<<f_index<<"  PPN_bin ="<<PPN_bin<< "  noise power1 ="<<Noise_power<<endl;

	    temp_value=10*log10(Noise_power)- 10.0* log10(10*RBW)-measuredPower;
	    
	    PPNout.set("",site_index,sListFWs[f_index],temp_value);
	    cout<<"index = "<<f_index<<"  PPN_bin ="<<PPN_bin<< "  noise figure ="<<temp_value<<endl;
	    	    
	}
	
}
////template <class T>
//double time_meas::freq(ARRAY_I src_datae)
//{
//    return test_period;
//}


void DigPin_Relay(STRING pin_list,STRING status)
{ 
	PPMU_RELAY    relay_close;
	DOUBLE relayClose_wait=1;
	
	relay_close. pin(pin_list).status(status);	
	relay_close. wait(relayClose_wait mS);
	relay_close.execute();
	         
}


void VM_SPMU_Hiz(SPMU_TASK& SPMU_task,STRING pin_list,data& Vout,int DEBUG, double delay_time,int samples)
{

    int iNumPins;
    int pin_index;
    int site_index;

    double iForce=0;
    
    // IFVM executing...
    SPMU_task.pin(pin_list).relay("NT").iForce(0 mA).min(0 V).max(3 V).mode("VM_HIZ").samples(samples).settling(delay_time ms);
    SPMU_task.execMode("PVAL").preAction("NONE").execute(); 
    
    //************************************************************************************************
    //* Split string-pins/string-iForce to array; Also get related number
    //************************************************************************************************
 
	vector<string> sListPins; 
	
	if ( string::npos == pin_list.find_first_of(",",0) ) {
		sListPins.push_back(pin_list);
	} else {
		tokenize(pin_list,sListPins,",");   
	}
    iNumPins=sListPins.size();
    //cout<<"iNumPins = "<<iNumPins<<endl;
        
    //************************************************************************************************
    //* Extract the value per-pin per-site
    //************************************************************************************************

	FOR_EACH_SITE_BEGIN();
      site_index=CURRENT_SITE_NUMBER()-1;

      for(pin_index=0;pin_index<iNumPins;pin_index++)
      {
          Vout.set(sListPins[pin_index], site_index,0, SPMU_task.getValue(sListPins[pin_index]));
          if(DEBUG)
          { 
        	  cout<<"site = "<<site_index+1 <<"  "<<sListPins[pin_index]<<"_Hiz_measured = "     <<setprecision(3) << setw(7)<<Vout.get(sListPins[pin_index], site_index, iForce)<< " V "<<endl;
        	 
              
          }
      }
    FOR_EACH_SITE_END();
}

void Meas_SPMU(SPMU_TASK& SPMU_task,STRING pin_list,data& VIout,int DEBUG, int pro_index, double delay_time,int samples)
{
	int iNumPins;
	int pin_index;
	int site_index;


	SPMU_task.pin(pin_list).relay("NO");
	SMC_WAIT_TIME(10 ms);
	SPMU_task.execMode("PVAL").execute();

	    //************************************************************************************************
	    //* Split string-pins/string-iForce to array; Also get related number
	    //************************************************************************************************

		vector<string> sListPins;

		if ( string::npos == pin_list.find_first_of(",",0) ) {
			sListPins.push_back(pin_list);
		} else {
			tokenize(pin_list,sListPins,",");
		}
	    iNumPins=sListPins.size();
	    //cout<<"iNumPins = "<<iNumPins<<endl;

	    //************************************************************************************************
	    //* Extract the value per-pin per-site
	    //************************************************************************************************

		FOR_EACH_SITE_BEGIN();
	      site_index=CURRENT_SITE_NUMBER()-1;

	      for(pin_index=0;pin_index<iNumPins;pin_index++)
	      {
	          VIout.set(sListPins[pin_index], site_index,pro_index, SPMU_task.getValue(sListPins[pin_index]));
	          if(DEBUG)
	          {
	        	  cout<<"site = "<<site_index+1 <<"  "<<sListPins[pin_index]<<"_measured = "     <<setprecision(3) << setw(7)<<VIout.get(sListPins[pin_index], site_index, pro_index)<< " V/A "<<endl;
	          }
	      }
	    FOR_EACH_SITE_END();
}

void Vset_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, double vForce_value,double iRang_Value)
{
			//************************************************************************************************
		    //* Parameter related variable defination
		    //************************************************************************************************

		    int samples=1;
		   // ON_FIRST_INVOCATION_BEGIN();
		    //************************************************************************************************
		    //* Split string-pins/string-iForce to array; Also get related number
		    //************************************************************************************************

		    // IFVM executing...
		    SPMU_task.pin(pin_list)
		    	.relay("NT")
		    	.vForce(vForce_value V)
		    	.clamp(iRang_Value mA)
		    	.max(iRang_Value mA)
		    	.min(0 mA)
		    	.iForceRange(iRang_Value mA)
		    	.mode("VFIM")
		    	.settling(5 ms)
		    	.samples(samples);

		    SPMU_task.execMode("FORCE_ON").execute();
		    //************************************************************************************************
		    //* Extract the value per-pin per-site
		    //************************************************************************************************
		
}


//added on oct08 2012
void Vset_HPMU(SPMU_TASK& SPMU_task,STRING pin_list, double vForce_value,double iRang_Value)
{
			//************************************************************************************************
		    //* Parameter related variable defination
		    //************************************************************************************************

		    int samples=1;
		   // ON_FIRST_INVOCATION_BEGIN();
		    //************************************************************************************************
		    //* Split string-pins/string-iForce to array; Also get related number
		    //************************************************************************************************

		    // IFVM executing...
		    SPMU_task.pin(pin_list)
		    	.relay("NTBBM")
		    	.vForce(vForce_value V)
		    	.clamp(iRang_Value mA)
		    	.max(iRang_Value mA)
		    	.min(0 mA)
		    	.iForceRange(iRang_Value mA)
		    	.mode("VFIM")
		    	.samples(samples);

		    SPMU_task.execMode("FORCE_ON").execute();
		    //************************************************************************************************
		    //* Extract the value per-pin per-site
		    //************************************************************************************************

}

///////////
void Vset_SPMU_noRelay(SPMU_TASK& SPMU_task,STRING pin_list, double vForce_value)
{
			//************************************************************************************************
		    //* Parameter related variable defination
		    //************************************************************************************************

		   // ON_FIRST_INVOCATION_BEGIN();
		    //************************************************************************************************
		    //* Split string-pins/string-iForce to array; Also get related number
		    //************************************************************************************************

		    // IFVM executing...
		    SPMU_task.pin(pin_list)
		    	.relay("NO")
		    	.vForce(vForce_value V)
//		    	.clamp(iRang_Value mA)
//		    	.max(iRang_Value mA)
//		    	.min(0 mA)
//		    	.iForceRange(iRang_Value mA)
//		    	.mode("VFIM")
//		    	.samples(samples)
		    	;

		    SPMU_task.execute();
		    //************************************************************************************************
		    //* Extract the value per-pin per-site
		    //************************************************************************************************
		
}

//Iset_SPMU
//void Iset_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, double iForce_value,double vClamp,double iForce_range)
//{
//			//************************************************************************************************
//		    //* Parameter related variable defination
//		    //************************************************************************************************
//		    STRING_VECTOR sListPins;
//		    int iNumPins;
//		    int pin_index,site_index;
//		    int samples=10;
//		   // ON_FIRST_INVOCATION_BEGIN();
//		    //************************************************************************************************
//		    //* Split string-pins/string-iForce to array; Also get related number
//		    //************************************************************************************************
//
//		    // IFVM executing...
//		    //SPMU_task.pin(pin_list)
//			SPMU_task.pin(pin_list)
//				.iForceRange(iForce_range mA)
//				.settling(10 ms)
//				//.preCharge(3.5 V)	
//				.clamp(vClamp V)
//				.iForce(iForce_value mA)
//				.max(3 V)
//				.min(0 V)
//				.mode("IFVM")
//				//.vMeasureRange(3 V)
//				.relay("NT")
//				.samples(samples);
//
//		    SPMU_task.preAction("NPRM").execMode("FORCE_ON").execute();
//		    
//		    //************************************************************************************************
//		    //* Extract the value per-pin per-site
//		    //************************************************************************************************
//		
//}

void Iset_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, double iForce_value,double vClamp,double iForce_range)
{
			//************************************************************************************************
		    //* Parameter related variable defination
		    //************************************************************************************************
		    STRING_VECTOR sListPins;


		   // ON_FIRST_INVOCATION_BEGIN();
		    //************************************************************************************************
		    //* Split string-pins/string-iForce to array; Also get related number
		    //************************************************************************************************

		    // IFVM executing...
		    //SPMU_task.pin(pin_list)
			SPMU_task.pin(pin_list)
					.iForceRange(iForce_range mA)
					.preCharge(1.0 V)
					.clamp(vClamp V)
					.iForce(iForce_value mA)
					.max(2 V)
					.min(0 V)
					.mode("IFVM")
					.vMeasureRange(2.99 V)
					.relay("NT")
					.settling( 5 ms);
					//.samples(samples);

		    SPMU_task.preAction("NPRM").execMode("FORCE_ON").execute();
		    
		    //************************************************************************************************
		    //* Extract the value per-pin per-site
		    //************************************************************************************************
		
}
void Reset_SPMU(SPMU_TASK& SPMU_task,STRING pin_list)
{
		    //************************************************************************************************
		    fwout << "PSFC ALL;RELS \"AC_OFF\",(" << pin_list << ")" << endl;
}

void disconnectSignalPin( const STRING& pin )
{
   fwout << "PSFC ALL"<<endl;
   fwout << "RELS \"AC_OFF\",(" << pin << ")" << endl;
}

//void Vset_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, double vForce_value,double iRang_Value)
//{
//			//************************************************************************************************
//		    //* Parameter related variable defination
//		    //************************************************************************************************
//		    STRING_VECTOR sListPins;
//		    int iNumPins;
//		    int pin_index,site_index;
//		    int samples=10;
//		   // ON_FIRST_INVOCATION_BEGIN();
//		    //************************************************************************************************
//		    //* Split string-pins/string-iForce to array; Also get related number
//		    //************************************************************************************************
//
//		    // IFVM executing...
//		    SPMU_task.pin(pin_list)
//		    	.relay("NT")
//		    	.vForce(vForce_value V)
//		    	.clamp(iRang_Value mA)
//		    	.max(iRang_Value mA)
//		    	.min(0 mA)
//		    	.mode("VFIM")
//		    	.samples(samples);
//
//		    SPMU_task.execMode("FORCE_ON").execute();
//		    //************************************************************************************************
//		    //* Extract the value per-pin per-site
//		    //************************************************************************************************
//		
//}


//******************************************
//Drive/Set VOLTAGE USING PPMU
//INPUT PIN AND VOLT ARE VECTORS
//******************************************
void Vset_SPMU(SPMU_TASK& SPMU_task,vector<string> sListPins, vector<DOUBLE> sListVolt,double iRang_Value)
{
	//************************************************************************************************
	//* Parameter related variable defination
	//************************************************************************************************

	int samples=1;
	// ON_FIRST_INVOCATION_BEGIN();
	//************************************************************************************************
	//* Split string-pins/string-iForce to array; Also get related number
	//************************************************************************************************

	// IFVM executing...
	for (unsigned int pin_index=0;pin_index<sListPins.size();pin_index++)
	{
		SPMU_task.pin(sListPins[pin_index])
		.relay("NT")
		.vForce(sListVolt[pin_index])
		.clamp(iRang_Value mA)
		.max(iRang_Value mA)
		.min(0 mA)
		.mode("VFIM")
		.samples(samples);
	}

	SPMU_task.execMode("FORCE_ON").execute();
	//************************************************************************************************
	//* Extract the value per-pin per-site
	//************************************************************************************************

}


void Vset_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, STRING vForce_list, STRING iRang_list)
{

	  //************************************************************************************************
	    //* Parameter related variable defination
	    //************************************************************************************************
	    static vector<string> sListPins;
	    static vector<DOUBLE> sListVolt;
	    static vector<DOUBLE> sListIRang;
	    int iNumVolt,iNumIRng,iNumGPs;
	    int pin_index;
	        STRING_VECTOR pinNameList;

	    	int samples=1;
	    //************************************************************************************************
	    //* ppmu_xxx/taskX defination
	    //************************************************************************************************
	    PPMU_SETTING  ppmu_set;
	    PPMU_RELAY    relay_close;
	    TASK_LIST     task1;

	    //************************************************************************************************
	    //* Split string-pins/string-iForce to array
	    //* Also get related number
	    //************************************************************************************************
	    tokenize(pin_list,sListPins,",");       iNumGPs=sListPins.size();
	    tokenize(vForce_list,sListVolt,","); iNumVolt=sListVolt.size();
	    tokenize(iRang_list,sListIRang,","); iNumIRng=sListIRang.size();
	    if(iNumGPs!=iNumVolt) cout<<"number error!! "<<endl;
	    

	// ON_FIRST_INVOCATION_BEGIN();
	//************************************************************************************************
	//* Split string-pins/string-iForce to array; Also get related number
	//************************************************************************************************

	// IFVM executing...
	for(pin_index=0;pin_index<iNumGPs;pin_index++)
	 {   
			SPMU_task.pin(sListPins[pin_index])
			.relay("NT")
			.vForce(sListVolt[pin_index])
			.clamp(sListIRang[pin_index] mA)
			.max(sListIRang[pin_index] mA)
			.min(0 mA)
			.mode("VFIM")
			.samples(samples);
		}

	SPMU_task.execMode("FORCE_ON").execute();
	//************************************************************************************************
	//* Extract the value per-pin per-site
	//************************************************************************************************

}


//add one func to set same irange for different conditions
void Vset_SPMU(SPMU_TASK& SPMU_task,STRING pin_list, STRING vForce_list, double iRang_Value)
{

	  //************************************************************************************************
	    //* Parameter related variable defination
	    //************************************************************************************************
	    static vector<string> sListPins;
	    static vector<DOUBLE> sListVolt;

	    int iNumVolt,iNumGPs;
	    int pin_index;
	        STRING_VECTOR pinNameList;

	    	int samples=1;
	    //************************************************************************************************
	    //* ppmu_xxx/taskX defination
	    //************************************************************************************************
	    PPMU_SETTING  ppmu_set;
	    PPMU_RELAY    relay_close;
	    TASK_LIST     task1;

	    //************************************************************************************************
	    //* Split string-pins/string-iForce to array
	    //* Also get related number
	    //************************************************************************************************
	    tokenize(pin_list,sListPins,",");       iNumGPs=sListPins.size();
	    tokenize(vForce_list,sListVolt,","); iNumVolt=sListVolt.size();

	    if(iNumGPs!=iNumVolt) cout<<"number error!! "<<endl;


	// ON_FIRST_INVOCATION_BEGIN();
	//************************************************************************************************
	//* Split string-pins/string-iForce to array; Also get related number
	//************************************************************************************************

	// IFVM executing...
	for(pin_index=0;pin_index<iNumGPs;pin_index++)
	 {
			SPMU_task.pin(sListPins[pin_index])
			.relay("NT")
			.vForce(sListVolt[pin_index])
			.clamp(iRang_Value mA)
			.max(iRang_Value mA)
			.min(0 mA)
			.mode("VFIM")
			.samples(samples);
		}

	SPMU_task.execMode("FORCE_ON").execute();
	//************************************************************************************************
	//* Extract the value per-pin per-site
	//************************************************************************************************

}
void VFIM_SPMU_noRelay(SPMU_TASK& SPMU_task,STRING pin_list, double vForce_value, data& Iout,int DEBUG, double delay_time,int samples)
{
	
		   //************************************************************************************************
		    //* Parameter related variable defination
		    //************************************************************************************************
		    STRING_VECTOR sListPins;
		    int iNumPins;
		    int pin_index,site_index;
		    
		   // ON_FIRST_INVOCATION_BEGIN();
		    //************************************************************************************************
		    //* Split string-pins/string-iForce to array; Also get related number
		    //************************************************************************************************
		    tokenize(pin_list,sListPins,",");       iNumPins=sListPins.size();

		    SPMU_task.pin(pin_list)
		    	.relay("NO")
		    	.settling(delay_time ms)
		    	.samples(samples)
		    	.vForce(vForce_value V);

		    SPMU_task.execMode("PVAL").preAction("NONE").execute();
		    
		    //************************************************************************************************
		    //* Extract the value per-pin per-site
		    //************************************************************************************************
		        
		   FOR_EACH_SITE_BEGIN();
		      site_index=CURRENT_SITE_NUMBER()-1;
		      for(pin_index=0;pin_index<iNumPins;pin_index++)
		      {
		    	  Iout.set(sListPins[pin_index], site_index,vForce_value, SPMU_task.getValue(sListPins[pin_index]));
		          if(DEBUG)
		          {
		            cout<<"site = "<<site_index+1 <<"  "<<sListPins[pin_index]<<"_"<<vForce_value<<"V_measured = "     <<setprecision(3) << setw(7)<<Iout.get(sListPins[pin_index], site_index, vForce_value)*1e3<<"mA"<<endl;
		          
		          }
		      }
		    FOR_EACH_SITE_END();

}
void IFVM_SPMU_noRelay(SPMU_TASK& SPMU_task,STRING pin_list, double iForce_value, double vClamp, data& Vout,int DEBUG, double delay_time,int samples)
{

		   //************************************************************************************************
		    //* Parameter related variable defination
		    //************************************************************************************************
		    STRING_VECTOR sListPins;
		    int iNumPins;
		    int pin_index,site_index;

		   // ON_FIRST_INVOCATION_BEGIN();
		    //************************************************************************************************
		    //* Split string-pins/string-iForce to array; Also get related number
		    //************************************************************************************************
		    tokenize(pin_list,sListPins,",");       iNumPins=sListPins.size();

		    SPMU_task.pin(pin_list)
		    	.settling(delay_time ms)
		    	.relay("NO")
		    	.iForce(iForce_value mA);

		    SPMU_task.execMode("PVAL").preAction("NONE").execute();
		    
		    //************************************************************************************************
		    //* Extract the value per-pin per-site
		    //************************************************************************************************
		        
		   FOR_EACH_SITE_BEGIN();
		      site_index=CURRENT_SITE_NUMBER()-1;
		      for(pin_index=0;pin_index<iNumPins;pin_index++)
		      {
		    	  Vout.set(sListPins[pin_index], site_index,iForce_value, SPMU_task.getValue(sListPins[pin_index]));
		          if(DEBUG)
		          {
		            cout<<"site = "<<site_index+1 <<"  "<<sListPins[pin_index]<<"_"<<iForce_value<<"mA_measured = "     <<setprecision(3) << setw(7)<<Vout.get(sListPins[pin_index], site_index, iForce_value)<<" V"<<endl;
		          
		          }
		      }
		    FOR_EACH_SITE_END();

}


int GetErrorNO(const STRING pin, int start_cycle, int no_of_cycles)
{
    int ErrorNumber=0;
    int site=CURRENT_SITE_NUMBER();
    if(site==1){
        fwout<<"PQFC 1"<<endl;
    }
    if(site==2){
         fwout<<"PQFC 2"<<endl;
    }
    if(site==3){
        fwout<<"PQFC 3"<<endl;
    }
    if(site==4){
         fwout<<"PQFC 4"<<endl;
    }
    fwout<<"ERCC? ERCP,"<<start_cycle<<","<<no_of_cycles<<",("<<pin<<")"<<endl;
    ErrorNumber=atoi(fwresult[0][4].c_str());
    //cout<<"          &&&&&&&&& error count number is "<<ErrorNumber<<endl;
    return ErrorNumber;
}


int GetErrorNO(const STRING pin)
{
    int ErrorNumber=0;
    int site=CURRENT_SITE_NUMBER();
    if(site==1){
        fwout<<"PQFC 1"<<endl;
    }
    if(site==2){
         fwout<<"PQFC 2"<<endl;
    }
    fwout<<"ERCT? ERCP,("<<pin<<")"<<endl;
    ErrorNumber=atoi(fwresult[0][2].c_str());
    
    return ErrorNumber;
}

void get_WaveDatas_fromString(STRING Vwaveform,DOUBLE Vstep,vector<DOUBLE>& wave_data)
{
	vector<STRING> sListData_sets;
	vector<DOUBLE> sListData;
	int m_dataset,n_data;    
	double temp_value;
	tokenize(Vwaveform,sListData_sets,";");       m_dataset=sListData_sets.size();
	wave_data.clear();
	for (int m=0;m<m_dataset;m++)
	{
			  tokenize(sListData_sets[m],sListData,"-");       n_data=sListData.size();
			  switch (n_data)
			  {
				  case 1:
					  wave_data.push_back(sListData[0]);

					  cout<<endl;
					  break;
					  
				  case 2:

					  temp_value=sListData[0];//start_V;
					  if (sListData[0]<sListData[1])  //ramp_up
						  
						  while (temp_value<sListData[1])
						  {
							  wave_data.push_back(temp_value);
							  temp_value=temp_value+Vstep;
						  }
					  else							//ramp_down
						  while (temp_value>sListData[1])
						  {
							  wave_data.push_back(temp_value);
							  temp_value=temp_value-Vstep;
						  }

						  
					  cout<<endl;
					  break;
			  
			  }
			}
			
			//cout<<"wave_data size ="<<wave_data.size()<<endl;
	
}


//template <class T>
////step is also in the waveform-string
////void get_WaveDatas_fromString(STRING Vwaveform,vector<DOUBLE>& wave_data)
//void get_WaveDatas_fromString(STRING Vwaveform,vector<DOUBLE>& wave_data)
//{
//	vector<STRING> sListData_sets,sListData_str;
//	vector<DOUBLE> sListData;
//	 int pos, pos2;
//	int m_dataset,n_data;    
//	double temp_value;
//	double Vstep=0;
//	string temp_string;
//	tokenize(Vwaveform,sListData_sets,";");       m_dataset=sListData_sets.size();
//	wave_data.clear();
//	string tmp, x_check;
//	
//	for (int m=0;m<m_dataset;m++)
//	{
//		
//		
//		//find if there is ":"(step value)
//		 pos = sListData_sets[m].find_first_of(":", 1);
//		 if (pos > 0)
//		 {
//			 string tmp,x_check;
//			 tmp = sListData_sets[m].substr(pos+1,sListData_sets[m].length());
//			 x_check = sListData_sets[m].substr(pos+2, 1);
//			 if ( x_check == "x" || x_check == "X" ) {
//				 Vstep=strtol(tmp.c_str(), (char **)NULL, 16);
//			 } else {
//				 
//				 Vstep=strtol(tmp.c_str(), (char **)NULL, 10);
//			     //Vstep=strtod(tmp.c_str(),NULL);
//		     }
//			 pos2=pos;
//			 
//		 }
//		 else
//		 {
//			 pos2=sListData_sets[m].length();
//			 Vstep=0;
//		 }
//		 
//		 cerr<<"Vstep = "<<Vstep<<endl;
//		 
//		 temp_string = sListData_sets[m].substr(0,pos2);
//		 
//		 tokenize(temp_string,sListData_str,"-");       n_data=sListData_str.size();
//		 sListData.resize(n_data);
//		 for(int ii=0;ii<n_data;ii++)
//		 {
//			 cerr<<" sListData_str["<< ii<< "] = "<<sListData_str[ii];// <<endl;
//
//			 x_check = sListData_str[ii].substr(1, 1);
//			
//			 
//			 cerr<<"   x_check  = "<<x_check<<endl;
//			 
//			 int tmpInt;
//			
//			 if ( x_check == "x" || x_check == "X" ) {
//				 tmpInt=strtol(sListData_str[ii].c_str(), (char **)NULL, 16);
//			 } else {
//				 tmpInt=strtol(tmp.c_str(), (char **)NULL, 10);
//			 }
//			 sListData[ii]=tmpInt;
//			 cerr<<" sListData["<< ii<< "] = "<<sListData[ii]<<endl;
//		 }
//		 
//			 switch (n_data)
//				  {
//				  case 1: //single point
//					  wave_data.push_back(sListData[0]);
//					  break;
//					  
//				  case 2: //has start and stop
//
//					  temp_value=sListData[0];//start_V;
//					  if (sListData[0]<sListData[1])  //ramp_up
//						  
//						  while (temp_value<=sListData[1])
//						  {
//							  wave_data.push_back(temp_value);
//							  temp_value=temp_value+Vstep;
//							  //cerr<<"temp_value = "<<temp_value;
//						  }
//					  else							//ramp_down
//						  while (temp_value>=sListData[1])
//						  {
//							  wave_data.push_back(temp_value);
//							  temp_value=temp_value-Vstep;
//						  }
//
//					  break;
//			  
//			  }
//	}
//			
//	cout<<"wave_data size ="<<wave_data.size()<<endl;
//	for (int jj=0; jj< wave_data.size();jj++)
//		cerr<<wave_data[jj]<<"  , ";
//	
//	cerr<<endl;
//	
//}
void get_WaveDatas_fromString(STRING Vwaveform,vector<DOUBLE>& wave_data)
{
	vector<STRING> sListData_sets;
	vector<DOUBLE> sListData;
	 int pos, pos2;
	int m_dataset,n_data;    
	double temp_value;
	double Vstep=0;
	string temp_string;
	tokenize(Vwaveform,sListData_sets,";");       m_dataset=sListData_sets.size();
	wave_data.clear();
	for (int m=0;m<m_dataset;m++)
	{

		//find if there is ":"(step value)
		 pos = sListData_sets[m].find_first_of(":", 1);
		 if (pos > 0)
		 {
			 string tmp,x_check;
			 tmp = sListData_sets[m].substr(pos+1,sListData_sets[m].length());
			 Vstep=strtod(tmp.c_str(),NULL);
		     pos2=pos;
			 
		 }
		 else
		 {
			 pos2=sListData_sets[m].length();
			 Vstep=1;
		 }
		 temp_string = sListData_sets[m].substr(0,pos2);
		 
		 tokenize(temp_string,sListData,"-");       n_data=sListData.size();
		 	  switch (n_data)
			  {
				  case 1://only one data
					  wave_data.push_back(sListData[0]);
					  break;
					  
				  case 2://start-stop(all

					  temp_value=sListData[0];//start_V;
					  if (sListData[0]<sListData[1])  //ramp_up
						  
						  while (temp_value<=sListData[1])
						  {
							  wave_data.push_back(temp_value);
							  temp_value=temp_value+Vstep;
							  
						  }
					  else							//ramp_down
						  while (temp_value>=sListData[1])
						  {
							  wave_data.push_back(temp_value);
							  temp_value=temp_value-Vstep;
						  }

					  break;
//				  case 3: //(there is negative value)
//				  case 4:
			  
			  }
			}
			
	
}
void get_WaveDatas_fromString(STRING Vwaveform,vector<INT>& wave_data)
{
	vector<STRING> sListData_sets,sListData_str;
	vector<INT> sListData;
	 int pos, pos2;
	int m_dataset,n_data;    
	int temp_value;
	int Vstep=0;
	string temp_string;
	tokenize(Vwaveform,sListData_sets,";");       m_dataset=sListData_sets.size();
	wave_data.clear();
	string tmp, x_check;
	
	for (int m=0;m<m_dataset;m++)
	{
		
		
		//find if there is ":"(step value)
		 pos = sListData_sets[m].find_first_of(":", 1);
		 if (pos > 0)
		 {
			 string tmp,x_check;
			 tmp = sListData_sets[m].substr(pos+1,sListData_sets[m].length());
			 x_check = sListData_sets[m].substr(pos+2, 1);
			 if ( x_check == "x" || x_check == "X" ) {
				 Vstep=strtol(tmp.c_str(), (char **)NULL, 16);
			 } else {
				 
				 Vstep=strtol(tmp.c_str(), (char **)NULL, 10);
			     //Vstep=strtod(tmp.c_str(),NULL);
		     }
			 pos2=pos;
			 
		 }
		 else
		 {
			 pos2=sListData_sets[m].length();
			 Vstep=0;
		 }
		 
		 //cerr<<"Vstep = "<<Vstep<<endl;
		 
		 temp_string = sListData_sets[m].substr(0,pos2);
		 
		 tokenize(temp_string,sListData_str,"-");       n_data=sListData_str.size();
		 sListData.resize(n_data);
		 for(int ii=0;ii<n_data;ii++)
		 {
			 cerr<<" sListData_str["<< ii<< "] = "<<sListData_str[ii];// <<endl;

			 x_check = sListData_str[ii].substr(1, 1);
			
			 
			 //cerr<<"   x_check  = "<<x_check<<endl;
			 
			 int tmpInt;
			
			 if ( x_check == "x" || x_check == "X" ) {
				 tmpInt=strtol(sListData_str[ii].c_str(), (char **)NULL, 16);
			 } else {
				 tmpInt=strtol(tmp.c_str(), (char **)NULL, 10);
			 }
			 sListData[ii]=tmpInt;
			 //cerr<<" sListData["<< ii<< "] = "<<sListData[ii]<<endl;
		 }
		 
			 switch (n_data)
				  {
				  case 1: //single point
					  wave_data.push_back(sListData[0]);
					  break;
					  
				  case 2: //has start and stop

					  temp_value=sListData[0];//start_V;
					  if (sListData[0]<sListData[1])  //ramp_up
						  
						  while (temp_value<=sListData[1])
						  {
							  wave_data.push_back(temp_value);
							  temp_value=temp_value+Vstep;
							  //cerr<<"temp_value = "<<temp_value;
						  }
					  else							//ramp_down
						  while (temp_value>=sListData[1])
						  {
							  wave_data.push_back(temp_value);
							  temp_value=temp_value-Vstep;
						  }

					  break;
			  
			  }
	}
			
	
}
//Add relay actions 
//Format relays_1:action;relays_2:action;relays_3:actions
//1. Using ":" to seperate relays and actions(ON/OFF) 
//2. Using ";" to seperate different actions(On/OFF)
//i.e. "k1,k2:ON; k3:OFF;K5:ON"

void Set_Relay_fromString(STRING Relay_actions,int DEBUG)
{

	vector<STRING> sListRelay_sets;
	vector<STRING> sListRelay;
	int m_dataset,n_data;
	if ((Relay_actions!="N/A")&& (Relay_actions!="NA")&&(Relay_actions!="n/a")&&(Relay_actions!="n/a")) 
	{
		tokenize(Relay_actions,sListRelay_sets,";");       m_dataset=sListRelay_sets.size();
		
		for (int m=0;m<m_dataset;m++)
		{
			
			tokenize(sListRelay_sets[m],sListRelay,":");       n_data=sListRelay.size();
			if((sListRelay[1]=="ON")||(sListRelay[1]=="on"))
			{
				Routing.util(sListRelay[0]).on();
				if(DEBUG) cerr<<"*** close relay: "<<sListRelay[0]<<endl;
			}
			else if((sListRelay[1]=="OFF")||(sListRelay[1]=="off"))
				{
					Routing.util(sListRelay[0]).off();
					if(DEBUG) cerr<<"***  open relay: "<<sListRelay[0]<<endl;
				}
				  
		}
		FLUSH();
	}	
}
void Set_Relay(STRING Relay_actions,int DEBUG)
{

	vector<STRING> sListRelay_sets;
	vector<STRING> sListRelay;
	int m_dataset,n_data;
	if ((Relay_actions!="N/A")&& (Relay_actions!="NA")&&(Relay_actions!="n/a")&&(Relay_actions!="n/a")) 
	{
		tokenize(Relay_actions,sListRelay_sets,";");       m_dataset=sListRelay_sets.size();
		
		for (int m=0;m<m_dataset;m++)
		{
			
			tokenize(sListRelay_sets[m],sListRelay,":");       n_data=sListRelay.size();
			if((sListRelay[1]=="ON")||(sListRelay[1]=="on"))
			{
				Routing.util(sListRelay[0]).on();
				if(DEBUG) cerr<<"*** close relay: "<<sListRelay[0]<<endl;
			}
			else if((sListRelay[1]=="OFF")||(sListRelay[1]=="off"))
				{
					Routing.util(sListRelay[0]).off();
					if(DEBUG) cerr<<"***  open relay: "<<sListRelay[0]<<endl;
				}
				  
		}
		FLUSH();
	}	
}
void get_PIN_VI_fromString(STRING Pin_Current,vector<STRING>& sListPins,vector<DOUBLE>& sListCurrent)
{
	int m_dataset,n_data;
	vector<STRING> PinCurr_sets;
	vector<STRING> sListPin_Curr;
	sListPins.clear();
	sListCurrent.clear();
	
	
	if ((Pin_Current!="N/A")&& (Pin_Current!="NA")&&(Pin_Current!="n/a")&&(Pin_Current!="n/a")) 
	{
		tokenize(Pin_Current,PinCurr_sets,";");       m_dataset=PinCurr_sets.size();
		
		for (int m=0;m<m_dataset;m++)
		{
			tokenize(PinCurr_sets[m],sListPin_Curr,":");       n_data=sListPin_Curr.size();
			
			sListPins.push_back(sListPin_Curr[0]);
			
			sListCurrent.push_back( atof(sListPin_Curr[1].c_str()) );
			
			//cerr<< sListPins <<endl;
			
				  
		}
	}	
}


double get_max(vector<DOUBLE>& sListValue)
{
	double temp;
	int i_number=sListValue.size();
	temp=sListValue[0];
	if(i_number>1)
		for (int ii=1;ii<i_number;ii++)
		{
			temp= (sListValue[ii-1]>sListValue[ii])?(sListValue[ii-1]):(sListValue[ii]);
		}
	
	return temp;

}

string get_string_fromVector(vector<STRING>& sListStr)
{
	int i_number=sListStr.size();
	string str_name=sListStr[0];
	for (int ii=1;ii<i_number;ii++)
	{
		str_name +=","+sListStr[ii];
	}
	return str_name;
}
//*************************************************************************
//* this function shift the string
// * eg: register_string Magic_registers
// * 
// * 		input parameters:
// * 			register_string
// * 		      format: "(0x1014,0x0303);(0x1004,0x0088);(0x0008,0x0088)"
// * 		output parameters:
// * 			addresses: "0x1014;0x1004;0x0008"
// * 			datas: "0x0303;0x0088;0x0088"
// * */
//*************************************************************************
void Get_Reg_AdrrData_from_String(string register_string, string& addresses, string& datas)
{

	
	addresses="",datas="";		//initialize;

//	string reg_sel = register_select;
//
//	if ((reg_sel=="")|(reg_sel=="N/A"))	return;
//	string return_string = Read_Register_Table_CSV(reg_sel);
//	if (return_string=="")	return;
	
	
	size_t addr_begin=0,addr_end=0,
		   data_begin=0,data_end=0;
	
	while(addr_begin!=string::npos){
		
		addr_end = register_string.find(",",addr_end+1,1);
		data_begin = addr_end;
		data_end = register_string.find(")",data_end+1,1);
		
		
		int A_begin = int(addr_begin)+1;
		int A_length = int(addr_end)-int(addr_begin)-1;
		int D_begin = int(data_begin)+1;
		int D_length = int(data_end)-int(data_begin)-1;
		
		
		string addr = register_string.substr(A_begin,A_length);
		string data = register_string.substr(D_begin,D_length);
		
		addresses = addresses + addr + ",";
		datas = datas + data + ",";
		
		addr_begin=register_string.find("(0x",addr_begin+1,3);
	}
	
	  addresses = addresses.substr(0,addresses.size()-1);		//to cut the last ";"
	  datas = datas.substr(0,datas.size()-1);
	  if(1)
	  {
		  cerr<<"Register adress :"<<addresses<<endl;
		  cerr<<"Register data   :"<<datas<<endl;
	  }
	  	  
}

//
////*********************************************************************************
void  SIM_AC_func(STRING MeasPin,double VPP, double ratio_20,double ratio_80,double ratio_50,STRING Label_Rising,STRING Label_falling,STRING Label_duty,data& ac_time,int Debug_Mode,int X_mode,double pattern_period)
{ 
	  
	  //*********************************************************************************
	  //* fix parameters
	  //* the timming related to meas_pin should be X8 mode
	  //* all pattern should be prepared
	  //* period is 10 ns
	  //*	  int X_mode= 8;				//x8 mode
	  //*	  double pattern_period= 10;    //period=10 ns
	  //*********************************************************************************  
	  INT compare_cycles_raising[2]={10,10};   // compare 10 cycles,each cycle 8 compare edge
	  INT compare_cycles_falling[2]={10,10};   // compare 10 cycles,each cycle 8 compare edge
	  INT compare_cycles_dutycycle[6]={20,20}; // compare 20 cycles,each cycle 8 compare edge

	  int eqn,spec,lset,n_PINS;
	  double time_meas;
	  
	  vector<STRING> sListPins;		
	   
	  
	  //********	prameter parsinging  *******************
	  tokenize(MeasPin,sListPins,","); n_PINS=sListPins.size();

	  Primary.label(Label_Rising);
	  fwout<<"DCGM FAIL,STRT,NONE"<<endl               //Only failed cycle
		  <<"DCRM EMAP,"<<X_mode<<",PF,(@)"<<endl      //x8 mode
		  <<"DCRT RAM, (@)"<<endl                      //Use external memory mode
		  <<"CYRM EDGE,,(@)"<<endl;                    //edge resolution
	  eqn=Primary.getLevelSpec().getEquation(); spec=Primary.getLevelSpec().getSpec(); lset=Primary.getLevel();
	  //raising time
	  fwout<<"RCLV "<<(100000*eqn+1000*spec+lset)<<","<<(1000*VPP*ratio_20)<<","<<(1000*VPP*ratio_80)<<",("<<MeasPin<<")"<<endl;
	  FLUSH(TM::APRM); 
	  SMC_WAIT_TIME(1 ms);
	  FUNCTIONAL_TEST();

	  FOR_EACH_SITE_BEGIN();
		  INT site=CURRENT_SITE_NUMBER()-1;
		  //n_PINS=sListPins.size();
	
		  for(int i=0;i<n_PINS;i++){
			  {
				  time_meas=(X_mode*compare_cycles_raising[i]-GetErrorNO(sListPins[i]))*(pattern_period/X_mode);
				  ac_time.set(sListPins[i],site,0,time_meas); //0 rising; 1 falling;2 duty
				  if(Debug_Mode) cout<<"site"<<site+1<<":raising time of "<<sListPins[i]<<"="<<time_meas<<" ns"<<endl;
			  }
		  }
	  FOR_EACH_SITE_END();
	
	  Primary.label(Label_falling);
	  FLUSH(TM::APRM);
	  FUNCTIONAL_TEST();
	  FOR_EACH_SITE_BEGIN();
		  INT site=CURRENT_SITE_NUMBER()-1;
		  for(int i=0;i<n_PINS;i++){
			  time_meas=(X_mode*compare_cycles_falling[i]-GetErrorNO(sListPins[i]))*(pattern_period/X_mode);
			  ac_time.set(sListPins[i],site,1,time_meas); //0 rising; 1 falling;2 duty
			  if(Debug_Mode) cout<<"site"<<site+1<<":falling time of "<<sListPins[i]<<"="<<time_meas<<" ns"<<endl;
		  }
	  FOR_EACH_SITE_END();
		     
	  //duty cycle
	  fwout<<"RCLV "<<(100000*eqn+1000*spec+lset)<<","<<(1000*VPP*ratio_50)<<","<<(1000*VPP*ratio_50)<<",("<<MeasPin<<")"<<endl;
	  Primary.label(Label_duty);
	  FLUSH(TM::APRM);
	  FUNCTIONAL_TEST();
	  FOR_EACH_SITE_BEGIN();
		  INT site=CURRENT_SITE_NUMBER()-1;
		  for(int i=0;i<n_PINS;i++){
			  time_meas=100*((X_mode*compare_cycles_dutycycle[i]-GetErrorNO(sListPins[i])))*(pattern_period/X_mode)/(compare_cycles_dutycycle[i]*pattern_period);
			  ac_time.set(sListPins[i],site,2,time_meas);//0 rising; 1 falling;2 duty
			  if(Debug_Mode) cout<<"site"<<site+1<<":Duty cycle of "<<sListPins[i]<<"="<<time_meas<<"%"<<endl;
		  }
	  FOR_EACH_SITE_END();
		

}






//***********************************************************************************************
//* Name: set_Voh_Vol
//* description: set Voh and Vol of specified pins
//***********************************************************************************************

void set_Vol_Voh(const STRING pin,double Vol_value, double Voh_value)
{
	  int eqn,spec,lset;
	  
	  eqn=Primary.getLevelSpec().getEquation();
	  spec=Primary.getLevelSpec().getSpec();
	  lset=Primary.getLevel();
	  //raising time
//	  int site_index =CURRENT_SITE_NUMBER()-1;
//	  fwout<< "PSFC "<<CURRENT_SITE_NUMBER()<<endl;
	  fwout<<"RCLV "<<(100000*eqn+1000*spec+lset)<<","<<(1000*Vol_value)<<","<<(1000*Voh_value)<<",("<<pin<<")"<<endl;
	  //FLUSH(TM::APRM);
	  //fwout<< "FLSH"<<endl;

}

void set_Vil_Vih(const STRING pin,double Vil_value, double Vih_value)
{
	  int eqn,spec,lset;
	  
	  eqn=Primary.getLevelSpec().getEquation(); spec=Primary.getLevelSpec().getSpec(); lset=Primary.getLevel();
	  //raising time
	  fwout<<"DRLV "<<(100000*eqn+1000*spec+lset)<<","<<(1000*Vil_value)<<","<<(1000*Vih_value)<<",("<<pin<<")"<<endl;
	  FLUSH(TM::APRM);
}
void set_Vihh(const STRING pin,double Vihh_value)
{
	  int eqn,spec,lset;
	  //<TERM 101001,HV,7500,,,,,(VPP)
		  eqn=Primary.getLevelSpec().getEquation(); spec=Primary.getLevelSpec().getSpec(); lset=Primary.getLevel();

		  fwout<<"PQFC 2"<<endl;
		  fwout<<"TERM "<<(100000*eqn+1000*spec+lset)<<",HV,"<<(1000*Vihh_value)<<",,,,,("<<pin<<")"<<endl;
		 // FLUSH(TM::APRM);

}
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

void FM_Waveform_Generate(int Nawg, double Fd_cyc, double Fm_cyc)
{

	  INT         i;

	  DOUBLE      Beta;
	  ARRAY_D     I_data, Q_data, FM, FM_IQ_COMB, dF;

	  I_data.resize(Nawg);
	  Q_data.resize(Nawg);
	  dF.resize(Nawg);

	  Beta = Fd_cyc/Fm_cyc;  // modulation index

	  for(i=0; i<Nawg; i++)
	  {
		  dF[i]= Beta*cos(2*M_PI*(DOUBLE)i/Nawg*Fm_cyc);

		  I_data[i]= cos(dF[i]) ;
		  Q_data[i]= sin(dF[i]) ;

	  }

	  PUT_DEBUG("TEST","I_data",I_data);        
	  PUT_DEBUG("TEST","Q_data",Q_data);
	  
    return;
  }


void IFVM_gangPPMU_LDO(STRING gangPin_force,STRING gangPin_sense,DOUBLE iForce, ARRAY_D& Vout)
{
	int DEBUG=0;
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    static vector<string> sList_gang,sListPins_s;
    static vector<DOUBLE> sListCurrent;
    int iNumForecePin,gang_pin_num,iNumSensePin;

    //* gang_pin number
    STRING_VECTOR pinNameList,pinSenseList;
    DOUBLE Iforce_perpin=0;
    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set,ppmu_off;
    PPMU_RELAY    relay_open, relay_close;
    PPMU_MEASURE  meas_set;
    TASK_LIST     task1;
    //* ppmu_xxx/taskX related parameter defination
    DOUBLE relayClose_wait = 1;     //msec
    DOUBLE relayOpen_wait = 1;      //msec
    int site_index;
    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    //* extract the current value per-pin by dividing the gangnumber
   // ON_FIRST_INVOCATION_BEGIN();

 //       tokenize(gangPin_force,sList_gang,",");       iNumGang=sList_gang.size();
        sList_gang=PinUtility.getDigitalPinNamesFromPinList(gangPin_force, TM::IO_PIN, TRUE, TRUE);
        sListPins_s=PinUtility.getDigitalPinNamesFromPinList(gangPin_sense, TM::IO_PIN, TRUE, TRUE);
        iNumForecePin=sListPins_s.size();
        iNumSensePin=sListPins_s.size();
        if (iNumForecePin !=iNumSensePin)
        {
            cout<<"error!";
            ABORT_TEST();
        }


        //************************************************************************************************
        //* IFVM executing...
        //************************************************************************************************

        for(int index=0;index<iNumForecePin;index++)
        {
 //           pinNameList=PinUtility.getDigitalPinNamesFromPinList(sList_gang[index], TM::IO_PIN, TRUE, TRUE);
 //           gang_pin_num =pinNameList.size();

//            if(active_load_flag)
//            {
//                if(DEBUG)
//               {
//                    cout<<"active load is enabled....!"<<endl;
//                    cout<<"active load perpin current is 35mA!"<<endl;
//                }

//               Iforce_perpin=iForce/gang_pin_num-35;
//                Iforce_perpin=iForce/gang_pin_num+35;
//            }
//        	if(index==2)
//        	{
//        		Iforce_perpin=-0;
//        	}
//            else
                    Iforce_perpin=iForce;
            if(DEBUG)
            {
            	cout<<sList_gang[index]<<" : PPMU_gang_num   = "<<gang_pin_num  <<endl;
            	cout<<sList_gang[index]<<" : PPMU_perpin_current = "<<Iforce_perpin<<" mA"<<endl;
            }

            ppmu_set.pin(sList_gang[index]).iForce(Iforce_perpin mA).iRange(Iforce_perpin mA).min(0 V).max(4 V);

        }
        ppmu_set.pin(gangPin_sense).iForce(0.001 mA).iRange(10 uA).min(0 V).max(4 V);
        ppmu_off.pin(gangPin_force).iForce(0.0).iRange(10 mA).min(0 ).max(4 V);
        meas_set.pin(gangPin_sense).execMode(TM::OVAL);

//        if(active_load_flag)
//            relay_close. pin(gangPin_force).status("_ON");
//        else
            relay_close. pin(gangPin_force).status("PPMU_ON");

        relay_close. pin(gangPin_sense).status("PPMU_ON");
        relay_close. wait(relayClose_wait mS);

        //"PPMU_OFF" will keep AC relay open, using "AC_ON" to switch PPMU off and keep AC relay on
        relay_open . pin(gangPin_force).status("ALL_OFF");
        relay_open . pin(gangPin_sense).status("ALL_OFF");
        relay_open . wait(relayOpen_wait mS);

        //task1.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
        task1.add(ppmu_set).add(relay_close).add(meas_set).add(relay_open).add(ppmu_off);
        task1.execute();

        //************************************************************************************************
        //* Extract the value per-pin per-site
        //************************************************************************************************
        FOR_EACH_SITE_BEGIN();
        	site_index=CURRENT_SITE_NUMBER()-1;
            for(int i =0;i<iNumSensePin;i++)
            {
                Vout[i] = meas_set.getValue(sListPins_s[i]);
                if(DEBUG)
                {
                    cout<<"site = "<<site_index+1 <<"  "<<sListPins_s[i]<<"_measured = "<<Vout[i]<<endl;
                }
            }
        FOR_EACH_SITE_END();
    //ON_FIRST_INVOCATION_END();



}


//##########################################################################################




void IFVM_gangPPMU_LDO_VOUT(STRING gangPin_force,STRING gangPin_sense,DOUBLE iForce, ARRAY_D& Vout)
{
	int DEBUG=0;
    //************************************************************************************************
    //* Parameter related variable defination
    //************************************************************************************************
    static vector<string> sList_gang,sListPins_s;
    static vector<DOUBLE> sListCurrent;
    int iNumForecePin,gang_pin_num,iNumSensePin;

    //* gang_pin number
    STRING_VECTOR pinNameList,pinSenseList;
    DOUBLE Iforce_perpin=0;
    //************************************************************************************************
    //* ppmu_xxx/taskX defination
    //************************************************************************************************
    PPMU_SETTING  ppmu_set,ppmu_off;
    PPMU_RELAY    relay_open, relay_close;
    PPMU_MEASURE  meas_set;
    TASK_LIST     task1;
    //* ppmu_xxx/taskX related parameter defination
    DOUBLE relayClose_wait = 1;     //msec
    DOUBLE relayOpen_wait = 1;      //msec
    int site_index;
    //************************************************************************************************
    //* Split string-pins/string-iForce to array
    //* Also get related number
    //************************************************************************************************
    //* extract the current value per-pin by dividing the gangnumber
   // ON_FIRST_INVOCATION_BEGIN();

 //       tokenize(gangPin_force,sList_gang,",");       iNumGang=sList_gang.size();
        sList_gang=PinUtility.getDigitalPinNamesFromPinList(gangPin_force, TM::IO_PIN, TRUE, TRUE);
        sListPins_s=PinUtility.getDigitalPinNamesFromPinList(gangPin_sense, TM::IO_PIN, TRUE, TRUE);
        iNumForecePin=sListPins_s.size();
        iNumSensePin=sListPins_s.size();
        if (iNumForecePin !=iNumSensePin)
        {
            cout<<"error!";
            ABORT_TEST();
        }


        //************************************************************************************************
        //* IFVM executing...
        //************************************************************************************************

        for(int index=0;index<iNumForecePin;index++)
        {
 //           pinNameList=PinUtility.getDigitalPinNamesFromPinList(sList_gang[index], TM::IO_PIN, TRUE, TRUE);
 //           gang_pin_num =pinNameList.size();

//            if(active_load_flag)
//            {
//                if(DEBUG)
//               {
//                    cout<<"active load is enabled....!"<<endl;
//                    cout<<"active load perpin current is 35mA!"<<endl;
//                }

//               Iforce_perpin=iForce/gang_pin_num-35;
//                Iforce_perpin=iForce/gang_pin_num+35;
//            }
        	if(index==4)
        	{
        		Iforce_perpin=-0;
        	}
            else
                    Iforce_perpin=iForce;
            if(DEBUG)
            {
            	cout<<sList_gang[index]<<" : PPMU_gang_num   = "<<gang_pin_num  <<endl;
            	cout<<sList_gang[index]<<" : PPMU_perpin_current = "<<Iforce_perpin<<" mA"<<endl;
            }

            ppmu_set.pin(sList_gang[index]).iForce(Iforce_perpin mA).iRange(Iforce_perpin mA).min(0 V).max(4 V);

        }
        ppmu_set.pin(gangPin_sense).iForce(0.001 mA).iRange(10 uA).min(0 V).max(4 V);
        ppmu_off.pin(gangPin_force).iForce(0.0).iRange(10 mA).min(0 ).max(4 V);
        meas_set.pin(gangPin_sense).execMode(TM::OVAL);

//        if(active_load_flag)
//            relay_close. pin(gangPin_force).status("_ON");
//        else
            relay_close. pin(gangPin_force).status("PPMU_ON");

        relay_close. pin(gangPin_sense).status("PPMU_ON");
        relay_close. wait(relayClose_wait mS);

        //"PPMU_OFF" will keep AC relay open, using "AC_ON" to switch PPMU off and keep AC relay on
        relay_open . pin(gangPin_force).status("ALL_OFF");
        relay_open . pin(gangPin_sense).status("ALL_OFF");
        relay_open . wait(relayOpen_wait mS);

        //task1.add(relay_close).add(ppmu_set).add(meas_set).add(ppmu_off).add(relay_open);
        task1.add(ppmu_set).add(relay_close).add(meas_set).add(relay_open).add(ppmu_off);
        task1.execute();

        //************************************************************************************************
        //* Extract the value per-pin per-site
        //************************************************************************************************
        FOR_EACH_SITE_BEGIN();
        	site_index=CURRENT_SITE_NUMBER()-1;
            for(int i =0;i<iNumSensePin;i++)
            {
                Vout[i] = meas_set.getValue(sListPins_s[i]);
                if(DEBUG)
                {
                    cout<<"site = "<<site_index+1 <<"  "<<sListPins_s[i]<<"_measured = "<<Vout[i]<<endl;
                }
            }
        FOR_EACH_SITE_END();
    //ON_FIRST_INVOCATION_END();



}
void Vec_Label_Write_OneVector(const STRING& labelName, const STRING& myPin,INT startVector, INT physWfIndex[] )
{
	    VEC_LABEL_EDIT myLabel(labelName, myPin);
	    VECTOR_DATA myVectorData[1] ;
	    FOR_EACH_SITE_BEGIN();
			int site_index=CURRENT_SITE_NUMBER()-1;
			myVectorData[0].vectorNum = startVector;
			myVectorData[0].phyWvfIndex =physWfIndex[site_index];
			myLabel.downloadUserVectors(myVectorData,1);
	    FOR_EACH_SITE_END();
}

//#########################################

void Vec_Label_WriteData(const STRING& labelName, const STRING& myPin,INT startVector, INT data[],int bits_length, int phyIndex_d0,int phyIndex_d1 )
{

	    VEC_LABEL_EDIT myLabel(labelName, myPin);
	    VECTOR_DATA myVectorData[bits_length] ;
	    INT a;


	    FOR_EACH_SITE_BEGIN();
			int site_index=CURRENT_SITE_NUMBER()-1;
			for(int bit_index=0;bit_index<bits_length;bit_index++)
			{
				myVectorData[bit_index].vectorNum = startVector+bit_index;
				a=(data[site_index]>>(bits_length-bit_index-1))& 0x1;
				if (a==0)
					myVectorData[bit_index].phyWvfIndex =phyIndex_d0;
				else
					myVectorData[bit_index].phyWvfIndex =phyIndex_d1;
			}
			myLabel.downloadUserVectors(myVectorData,bits_length);

	    FOR_EACH_SITE_END();


}

void IFVM_HV(STRING pin_list, double iForce,     data& Vout, int DEBUG, double delay_time)
{
//	//************************************************************************************************
//    //* ppmu_xxx/taskX defination
//    //************************************************************************************************
//
//    HV_DC_TASK hvDCtask;
//
//    hvDCtask.pin(pin_list).iForce(0.1 uA).iForceRange(0.1 uA).min(0 V).max(3 V).execMode("HV_PPMU_BADC_VAL");
//    hvDCtask.execute();
//
//    STRING_VECTOR svPinList = PinUtility.expandDigitalPinNamesFromPinList(pin_list,TM::I_PIN|TM::O_PIN|TM::IO_PIN,TM::IGNORE_MISSING_PIN_NAMES);
//
//      FOR_EACH_SITE_BEGIN();
//
//        for (STRING_VECTOR::iterator it = svPinList.begin(); it != svPinList.end(); ++it) {
//          cerr << (*it) << " measured " << setprecision(10) << hvDCtask.getValue((*it)) << "V\n";
//
//        }
//      FOR_EACH_SITE_END();
//
//
}
//void VM_BADC_HV(STRING pin_list,data& Vout,int DEBUG, int progIndex,double delay_time,int samples)
//{
//	    HV_DC_TASK hvDCtask;
//
//	    hvDCtask.pin(pin_list).iForce(0.1 uA).min(0 V).max(13 V).settling(delay_time ms ).execMode("VM_HIZ").samples(samples);
//	    hvDCtask.execute();
//
//	    STRING_VECTOR svPinList = PinUtility.expandDigitalPinNamesFromPinList(pin_list,TM::I_PIN|TM::O_PIN|TM::IO_PIN,TM::IGNORE_MISSING_PIN_NAMES);
//
//	      FOR_EACH_SITE_BEGIN();
//	      int site_index=CURRENT_SITE_NUMBER()-1;
//	        for (STRING_VECTOR::iterator it = svPinList.begin(); it != svPinList.end(); ++it) {
//	        Vout.set( *it,site_index,progIndex,hvDCtask.getValue( *it));
//	        //Vout.set(sListPins[pin_index], site_index,0, SPMU_task.getValue(sListPins[pin_index]));
//	         if(DEBUG) cerr << (*it) << " measured " << setprecision(10) << Vout.get( *it,site_index, progIndex) << "V\n";
//
//	        }
//	      FOR_EACH_SITE_END();
//}

void VM_BADC_HV(HV_DC_TASK hvDCtask,STRING pin_list,data& Vout,int DEBUG, int progIndex,double delay_time,int samples)
{
	    //HV_DC_TASK hvDCtask;

	    hvDCtask.pin(pin_list).iForce(0.1 uA).min(0 V).max(13 V).settling(delay_time ms ).execMode("VM_HIZ").samples(samples);
	    hvDCtask.execute();

	    STRING_VECTOR svPinList = PinUtility.expandDigitalPinNamesFromPinList(pin_list,TM::I_PIN|TM::O_PIN|TM::IO_PIN,TM::IGNORE_MISSING_PIN_NAMES);

	      FOR_EACH_SITE_BEGIN();
	      int site_index=CURRENT_SITE_NUMBER()-1;
	        for (STRING_VECTOR::iterator it = svPinList.begin(); it != svPinList.end(); ++it) {
	        Vout.set( *it,site_index,progIndex,hvDCtask.getValue( *it));
	        //Vout.set(sListPins[pin_index], site_index,0, SPMU_task.getValue(sListPins[pin_index]));
	         if(DEBUG) cerr << (*it) << " measured " << setprecision(10) << Vout.get( *it,site_index, progIndex) << "V\n";

	        }
	      FOR_EACH_SITE_END();
}



//******************************************************
//* Name: Get_data_from_data_file
//*****************
void Get_data_from_file(char* file_name,ARRAY_D& data_array,int data_size)
{
	char strn[50];
////   GetDevPath(device_path);
////    abcd=device_path+"/data_files/sync_aa.data";
//    char* device_path_name="/home/zhangxiaoxin/Devices/FARADAY/FS90AB092AA/TestMethod/testmethod_pll/data_files/dc_raptor.data";
    ifstream Adata(file_name);
    ifstream read_file;
    read_file.open(file_name);
    string one_line;
    while( std::getline(read_file, one_line ) )
    {
    	if( 0 == one_line.compare(0, one_line.length(), "#") )
    	{
    		cout << "Bypass Comment line :<" << one_line << ">" << endl;
    		continue;
    	}
    	string tmp_l = one_line;
    	cout << "<" << tmp_l << ">" << endl;
      	istringstream abc(tmp_l);
      	abc.precision(15);
        double dResult = 0.0;
        abc >> dResult;
        cout.precision(17);
        cout << "R:" << dResult << endl;
    }
    read_file.close();
//    iostream abcd;
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
////!!!!  Read The line content of the first file             //
////!!!!  Creat the data to be capturedirst file              //
////!!!!  Delete when using test method Debugging             //
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//
    data_array.resize(data_size);
    int i=0;
    while(!Adata.eof()&&i<data_size)    //Ignore when online debugging
    {
        Adata.getline(strn,20,'\n');   //read line
        if((strn[0]!='#')||(strn[0]!=' '))
        {
        	istringstream abc(strn);
        	double dResult;
        	abc >> data_array[i];
//        	=atof(strn);

            //Cap_Data[i]=atoi(strn);
        	cout<< strn << " , "<<data_array[i]<<endl;
            i++;
        }
    }
    if(!Adata.eof()) Adata.close();
//    *************************************

}
void Get_data_from_file(char* file_name,ARRAY_I& data_array,int data_size)
{
	char strn[20];
////   GetDevPath(device_path);
////    abcd=device_path+"/data_files/sync_aa.data";
//    char* device_path_name="/home/zhangxiaoxin/Devices/FARADAY/FS90AB092AA/TestMethod/testmethod_pll/data_files/dc_raptor.data";
    ifstream Adata(file_name);
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
////!!!!  Read The line content of the first file             //
////!!!!  Creat the data to be capturedirst file              //
////!!!!  Delete when using test method Debugging             //
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//
    data_array.resize(data_size);
    int i=0;
    while(!Adata.eof()&&i<data_size)    //Ignore when online debugging
    {
        Adata.getline(strn,20,'\n');   //read line
        if((strn[0]!='#')||(strn[0]!=' '))
        {
        	//data_array[i]=atof(strn);
            data_array[i]=atoi(strn);

            i++;
        }
    }
    if(!Adata.eof()) Adata.close();
//    *************************************

}
