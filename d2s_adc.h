#ifndef D2SSERIALPROTOCOLTEMPLATE_H_
#define D2SSERIALPROTOCOLTEMPLATE_H_
#include "d2s_Serial.h"

class d2s_adc : public d2s_Serial{
private:
	d2s_adc(std::string pName):d2s_Serial(pName){};
	d2s_adc(const d2s_adc&);
	d2s_adc& operator= (const d2s_adc&);

public:    
    //singleton
    static d2s_adc& Instance(){
        static d2s_adc instance("@");
        d2s_adc& ref = instance;
        return ref;
    }
    
    /*general parameters*/
    int getAddressBits(){ return 8;}
    int getDataBits(){ return 8;}
    int getHighWaveformIndex(){ return 1;}
    int getLowWaveformIndex(){ return 0;}
    int getHighStrobeWaveformIndex(){ return 3;}
    int getLowStrobeWaveformIndex(){ return 2;}
    int getMaskStrobeWaveformIndex(){ return 5;}
    std::string getInterfaceName(){ return "SDA"; }

    /*write parameters*/
    std::string getWriteTemplatePatternName(){ return "d2s_write_adc";}
    std::string getWritePinName() { return "SDA";}
    int getWriteAddressVectorNumberLSB(){ return 21;}
    int getWriteAddressVectorNumberMSB(){ return 14;}
    int getWriteDataVectorNumberLSB(){ return 30;}
    int getWriteDataVectorNumberMSB(){ return 23;}

    /*read parameters*/
    std::string getReadTemplatePatternName(){ return "d2s_read_adc";}
    std::string getReadAddressPinName() { return "SDA";}
    std::string getReadPinName() { return "SDA";}
    int getReadAddressVectorNumberLSB(){ return 21;}
    int getReadAddressVectorNumberMSB(){ return 14;}
    int getReadDataCycleNumberLSB(){ return 49;}
    int getReadDataCycleNumberMSB(){ return 42;}
    int getReadDataVectorNumberLSB(){ return 49;}
    int getReadDataVectorNumberMSB(){ return 42;}
    
    /*wait parameters*/
    std::string getTimingPort(){ return "@";}
    std::string getWaitTemplatePatternName(){ return "d2s_wait_adc";}

    std::string getDCScaleTemplatePatternName(){return "dcscale_adc";}
};

#endif
