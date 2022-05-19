#ifndef D2SHKADC_H_
#define D2SHKADC_H_
#include "generic_d2s/d2s_Serial.h"

class d2sHkadc : public d2s_Serial{
private:
    d2sHkadc(std::string pName):d2s_Serial(pName){};
    d2sHkadc(const d2sHkadc&);
    d2sHkadc& operator= (const d2sHkadc&);

public:    
    //singleton
    static d2sHkadc& Instance(){        
        static d2sHkadc instance("@");//ori is ("sbiA");
        d2sHkadc& ref = instance;
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
    std::string getInterfaceName(){ return "hkadc"; }//VrefTrim//BuckLdo/iLimit/LedCurr/iLimitTrim/commonRead

    /*write parameters*/
    std::string getWriteTemplatePatternName(){ return "HKADC_write";}
    std::string getWritePinName() { return "HKADC_SSI";}
    int getWriteAddressVectorNumberLSB(){ return 10;}
    int getWriteAddressVectorNumberMSB(){ return 4;}
    int getWriteDataVectorNumberLSB(){ return 18;}
    int getWriteDataVectorNumberMSB(){ return 11;}

    /*read parameters*/
    std::string getReadTemplatePatternName(){ return "HKADC_read";}
    std::string getReadAddressPinName() { return "HKADC_SSI";}
    std::string getReadPinName() { return "HKADC_SSI";}
    int getReadAddressVectorNumberLSB(){ return 10;}
    int getReadAddressVectorNumberMSB(){ return 4;}
    int getReadDataCycleNumberLSB(){ return 21;}
    int getReadDataCycleNumberMSB(){ return 14;}
    int getReadDataVectorNumberLSB(){ return 21;}
    int getReadDataVectorNumberMSB(){ return 14;}
    
    /*wait parameters*/
    std::string getTimingPort(){ return "@";}
    std::string getWaitTemplatePatternName(){ return "HKADC_wait";}
};



#endif /*D2SSSIHKADC_H_*/
