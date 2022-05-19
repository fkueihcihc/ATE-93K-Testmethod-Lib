#ifndef D2SSERIALPROTOCOLTEMPLATE_H
#define D2SSERIALPROTOCOLTEMPLATE_H
#include "./generic_d2s/d2s_Serial.h"

class d2s_ps_jtag : public d2s_Serial{
private:
	d2s_ps_jtag(std::string pName):d2s_Serial(pName){};
	d2s_ps_jtag(const d2s_ps_jtag&);
	d2s_ps_jtag& operator= (const d2s_ps_jtag&);

public:
    //singleton
    static d2s_ps_jtag& Instance(){
        static d2s_ps_jtag instance("@");
        d2s_ps_jtag& ref = instance;
        return ref;
    }

    /*general parameters*/
    int getAddressBits(){ return 32;}
    int getDataBits(){ return 32;}
    int getLowWaveformIndex(){ return 0;}
    int getHighWaveformIndex(){ return 1;}
    int getLowStrobeWaveformIndex(){ return 2;}
    int getHighStrobeWaveformIndex(){ return 3;}
    int getMaskStrobeWaveformIndex(){ return 4;}
    std::string getInterfaceName(){ return "PS_JTAG"; }

    /*write parameters*/
    std::string getWriteTemplatePatternName(){ return "ps_jtag_write_temp";}
    std::string getWritePinName() { return "JTAG_TDI";}
    int getWriteAddressVectorNumberLSB(){ return 6;}
    int getWriteAddressVectorNumberMSB(){ return 37;}
    int getWriteDataVectorNumberLSB(){ return 38;}
    int getWriteDataVectorNumberMSB(){ return 69;}

    /*read parameters*/
    std::string getReadTemplatePatternName(){ return "ps_jtag_read_temp";}
    std::string getReadAddressPinName() { return "JTAG_TDI";}
    std::string getReadPinName() { return "JTAG_TDO";}
    int getReadAddressVectorNumberLSB(){ return 6;}
    int getReadAddressVectorNumberMSB(){ return 37;}
    int getReadDataCycleNumberLSB(){ return 38;}
    int getReadDataCycleNumberMSB(){ return 69;}
    int getReadDataVectorNumberLSB(){ return 38;}
    int getReadDataVectorNumberMSB(){ return 69;}

    /*wait parameters*/
    std::string getTimingPort(){ return "@";}
    std::string getWaitTemplatePatternName(){ return "ps_jtag_wait_temp";}
};

#endif
