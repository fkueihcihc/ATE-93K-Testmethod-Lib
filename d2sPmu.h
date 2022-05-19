#ifndef D2SPMU_H_
#define D2SPMU_H_
#include "generic_d2s/d2s_Serial.h"

class d2sPmu : public d2s_Serial{
private:
    d2sPmu(std::string pName):d2s_Serial(pName){};
    d2sPmu(const d2sPmu&);
    d2sPmu& operator= (const d2sPmu&);

public:    
    //singleton
    static d2sPmu& Instance(){        
        static d2sPmu instance("@");//ori is ("sbiA");
        d2sPmu& ref = instance;
        return ref;
    }
    
    /*general parameters*/
    int getAddressBits(){ return 10;}//ori is 6
    int getDataBits(){ return 8;}  //ori is 16
    int getHighWaveformIndex(){ return 1;}
    int getLowWaveformIndex(){ return 0;}
    int getHighStrobeWaveformIndex(){ return 3;}//ori is 3
    int getLowStrobeWaveformIndex(){ return 2;}//ori is 2
    int getMaskStrobeWaveformIndex(){ return 4;}
    std::string getInterfaceName(){ return "pmu"; }//VrefTrim//BuckLdo/iLimit/LedCurr/iLimitTrim/commonRead

    /*write parameters*/
    std::string getWriteTemplatePatternName(){ return "pmu_write";}
    std::string getWritePinName() { return "PMU0_SSI";}
    int getWriteAddressVectorNumberLSB(){ return 19;}
    int getWriteAddressVectorNumberMSB(){ return 10;}
    int getWriteDataVectorNumberLSB(){ return 27;}
    int getWriteDataVectorNumberMSB(){ return 20;}

    /*read parameters*/
    std::string getReadTemplatePatternName(){ return "pmu_read";}
    std::string getReadAddressPinName() { return "PMU0_SSI";}
    std::string getReadPinName() { return "PMU0_SSI";}
    int getReadAddressVectorNumberLSB(){ return 19;}
    int getReadAddressVectorNumberMSB(){ return 10;}
    int getReadDataCycleNumberLSB(){ return 31;}
    int getReadDataCycleNumberMSB(){ return 24;}
    int getReadDataVectorNumberLSB(){ return 31;}
    int getReadDataVectorNumberMSB(){ return 24;}
    
    /*wait parameters*/
    std::string getTimingPort(){ return "@";}
    std::string getWaitTemplatePatternName(){ return "pmu_wait";}
};


#endif /*D2SPMU_H_*/
