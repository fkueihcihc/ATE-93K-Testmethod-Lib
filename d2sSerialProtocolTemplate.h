#ifndef D2SSERIALPROTOCOLTEMPLATE_H_
#define D2SSERIALPROTOCOLTEMPLATE_H_
#include "d2s_Serial.h"

class d2sSerialProtocolTemplate : public d2s_Serial{
private:
    d2sSerialProtocolTemplate(std::string pName):d2s_Serial(pName){};
    d2sSerialProtocolTemplate(const d2sSerialProtocolTemplate&);
    d2sSerialProtocolTemplate& operator= (const d2sSerialProtocolTemplate&);

public:    
    //singleton
    static d2sSerialProtocolTemplate& Instance(){        
        static d2sSerialProtocolTemplate instance("sbiA");
        d2sSerialProtocolTemplate& ref = instance;
        return ref;
    }
    
    /*general parameters*/
    int getAddressBits(){ return 6;}
    int getDataBits(){ return 16;}      
    int getHighWaveformIndex(){ return 1;}
    int getLowWaveformIndex(){ return 0;}
    int getHighStrobeWaveformIndex(){ return 3;}
    int getLowStrobeWaveformIndex(){ return 2;}
    int getMaskStrobeWaveformIndex(){ return 4;}
    std::string getInterfaceName(){ return "PA"; }   

    /*write parameters*/
    std::string getWriteTemplatePatternName(){ return "d2s_sbiA_write_template";}
    std::string getWritePinName() { return "di_A";}
    int getWriteAddressVectorNumberLSB(){ return 8;}
    int getWriteAddressVectorNumberMSB(){ return 3;}
    int getWriteDataVectorNumberLSB(){ return 24;}
    int getWriteDataVectorNumberMSB(){ return 9;}  

    /*read parameters*/
    std::string getReadTemplatePatternName(){ return "d2s_sbiA_read_template";}
    std::string getReadAddressPinName() { return "di_A";}
    std::string getReadPinName() { return "do_A";}    
    int getReadAddressVectorNumberLSB(){ return 8;}
    int getReadAddressVectorNumberMSB(){ return 3;}
    int getReadDataCycleNumberLSB(){ return 24;}
    int getReadDataCycleNumberMSB(){ return 9;}
    int getReadDataVectorNumberLSB(){ return 24;}
    int getReadDataVectorNumberMSB(){ return 9;} 
    
    /*wait parameters*/
    std::string getTimingPort(){ return "@";}
    std::string getWaitTemplatePatternName(){ return "d2s_sbiA_wait_template";}
};

#endif
