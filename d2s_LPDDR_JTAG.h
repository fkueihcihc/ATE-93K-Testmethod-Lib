#ifndef D2SSERIALPROTOCOLTEMPLATE_H
#define D2SSERIALPROTOCOLTEMPLATE_H
#include "./generic_d2s/d2s_Serial.h"

class 		d2s_LPDDR_JTAG_Interface : public d2s_Serial{
private:
			d2s_LPDDR_JTAG_Interface(std::string pName):d2s_Serial(pName){};
			d2s_LPDDR_JTAG_Interface(const 		d2s_LPDDR_JTAG_Interface&);
			d2s_LPDDR_JTAG_Interface& operator= (const 		d2s_LPDDR_JTAG_Interface&);

public:
    //singleton
    static 		d2s_LPDDR_JTAG_Interface& Instance(){
        static 		d2s_LPDDR_JTAG_Interface instance("@");
        		d2s_LPDDR_JTAG_Interface& ref = instance;
        return ref;
    }

    /*general parameters*/
    int getAddressBits(){ return 9;}
    int getDataBits(){ return 4;}
    int getLowWaveformIndex(){ return 0;}
    int getHighWaveformIndex(){ return 1;}
    int getLowStrobeWaveformIndex(){ return 2;}
    int getHighStrobeWaveformIndex(){ return 3;}
    int getMaskStrobeWaveformIndex(){ return 4;}
    std::string getInterfaceName(){ return "DFT_JTAG"; }

    /*write parameters*/
    std::string getWriteTemplatePatternName(){ return "dft_jtag_write_temp";}
    std::string getWritePinName() { return "JTAG_TDI";}
    int getWriteAddressVectorNumberLSB(){ return 13;}
    int getWriteAddressVectorNumberMSB(){ return 21;}
    int getWriteDataVectorNumberLSB(){ return 27;}
    int getWriteDataVectorNumberMSB(){ return 30;}

    /*read parameters*/
    std::string getReadTemplatePatternName(){ return "dft_jtag_read_temp";}
    std::string getReadAddressPinName() { return "JTAG_TDI";}
    std::string getReadPinName() { return "JTAG_TDO";}
    int getReadAddressVectorNumberLSB(){ return 3;}
    int getReadAddressVectorNumberMSB(){ return 11;}
    int getReadDataCycleNumberLSB(){ return 250;}
    int getReadDataCycleNumberMSB(){ return 281;}
    int getReadDataVectorNumberLSB(){ return 250;}
    int getReadDataVectorNumberMSB(){ return 281;}

    /*wait parameters*/
    std::string getTimingPort(){ return "@";}
    std::string getWaitTemplatePatternName(){ return "dft_jtag_wait_temp";}
};

#endif
