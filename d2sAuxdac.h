#ifndef D2SAUXDAC_H_
#define D2SAUXDAC_H_
#include "generic_d2s/d2s_Serial.h"

class d2sAuxdac : public d2s_Serial{
private:
    d2sAuxdac(std::string pName):d2s_Serial(pName){};
    d2sAuxdac(const d2sAuxdac&);
    d2sAuxdac& operator= (const d2sAuxdac&);

public:    
    //singleton
    static d2sAuxdac& Instance(){        
        static d2sAuxdac instance("@");//ori is ("sbiA");
        d2sAuxdac& ref = instance;
        return ref;
    }
    
    /*general parameters*/
    int getAddressBits(){ return 7;}//ori is 6
    int getDataBits(){ return 8;}  //ori is 16
    int getHighWaveformIndex(){ return 1;}
    int getLowWaveformIndex(){ return 0;}
    int getHighStrobeWaveformIndex(){ return 3;}//ori is 3
    int getLowStrobeWaveformIndex(){ return 2;}//ori is 2
    int getMaskStrobeWaveformIndex(){ return 4;}
    std::string getInterfaceName(){ return "Com"; }//VrefTrim//BuckLdo/iLimit/LedCurr/iLimitTrim/commonRead

    /*write parameters*/
    std::string getWriteTemplatePatternName(){ return "d2sSsiAuxdacWriteTempl.binl";}
    std::string getWritePinName() { return "SSI_PMU";}
    int getWriteAddressVectorNumberLSB(){ return 10;}
    int getWriteAddressVectorNumberMSB(){ return 4;}
    int getWriteDataVectorNumberLSB(){ return 19;}
    int getWriteDataVectorNumberMSB(){ return 12;}  

    /*read parameters*/
    std::string getReadTemplatePatternName(){ return "d2sSsiAuxdacReadComTempl.binl";}
    std::string getReadAddressPinName() { return "SSI_PMU";}
    std::string getReadPinName() { return "SSI_PMU";}    
    int getReadAddressVectorNumberLSB(){ return 10;}
    int getReadAddressVectorNumberMSB(){ return 4;}
    int getReadDataCycleNumberLSB(){ return 20;}
    int getReadDataCycleNumberMSB(){ return 13;}
    int getReadDataVectorNumberLSB(){ return 20;}
    int getReadDataVectorNumberMSB(){ return 13;}
    
    /*wait parameters*/
    std::string getTimingPort(){ return "@";}
    std::string getWaitTemplatePatternName(){ return "d2sSsiAuxdacWaitComTempl.binl";}
};
#endif /*D2SSSIAUXDAC_H_*/
