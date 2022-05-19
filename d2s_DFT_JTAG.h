
#include "./generic_d2s/d2s_Serial.h"

class d2s_dft_jtag : public d2s_Serial{
private:
	d2s_dft_jtag(std::string pName):d2s_Serial(pName){};
	d2s_dft_jtag(const d2s_dft_jtag&);
	d2s_dft_jtag& operator= (const d2s_dft_jtag&);

public:
    //singleton
    static d2s_dft_jtag& Instance(){
        static d2s_dft_jtag instance("@");
        d2s_dft_jtag& ref = instance;
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
    int getWriteAddressVectorNumberLSB(){ return 5;}
    int getWriteAddressVectorNumberMSB(){ return 13;}
    int getWriteDataVectorNumberLSB(){ return 19;}
    int getWriteDataVectorNumberMSB(){ return 22;}

    /*read parameters*/
    std::string getReadTemplatePatternName(){ return "dft_jtag_read_temp";}
    std::string getReadAddressPinName() { return "JTAG_TDI";}
    std::string getReadPinName() { return "JTAG_TDO";}
    int getReadAddressVectorNumberLSB(){ return 5;}
    int getReadAddressVectorNumberMSB(){ return 13;}
    int getReadDataCycleNumberLSB(){ return 19;}
    int getReadDataCycleNumberMSB(){ return 22;}
    int getReadDataVectorNumberLSB(){ return 19;}
    int getReadDataVectorNumberMSB(){ return 22;}

    /*wait parameters*/
    std::string getTimingPort(){ return "@";}
    std::string getWaitTemplatePatternName(){ return "dft_jtag_wait_temp";}
};

