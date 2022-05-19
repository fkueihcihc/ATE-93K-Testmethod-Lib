
#include "./generic_d2s/d2s_Serial.h"

class d2s_cpu_jtag : public d2s_Serial{
private:
	d2s_cpu_jtag(std::string pName):d2s_Serial(pName){};
	d2s_cpu_jtag(const d2s_cpu_jtag&);
	d2s_cpu_jtag& operator= (const d2s_cpu_jtag&);

public:
    //singleton
    static d2s_cpu_jtag& Instance(){
        static d2s_cpu_jtag instance("@");
        d2s_cpu_jtag& ref = instance;
        return ref;
    }

    /*general parameters*/
    int getAddressBits(){ return 9;}
    int getDataBits(){ return 8;}
    int getLowWaveformIndex(){ return 0;}
    int getHighWaveformIndex(){ return 1;}
    int getLowStrobeWaveformIndex(){ return 2;}
    int getHighStrobeWaveformIndex(){ return 3;}
    int getMaskStrobeWaveformIndex(){ return 4;}
    std::string getInterfaceName(){ return "CPU_JTAG"; }

    /*write parameters*/
    std::string getWriteTemplatePatternName(){ return "cpu_jtag_write_temp";}
    std::string getWritePinName() { return "JTAG_TDI";}
    int getWriteAddressVectorNumberLSB(){ return 5;}
    int getWriteAddressVectorNumberMSB(){ return 13;}
    int getWriteDataVectorNumberLSB(){ return 19;}
    int getWriteDataVectorNumberMSB(){ return 26;}

    /*read parameters*/
    std::string getReadTemplatePatternName(){ return "cpu_jtag_read_temp";}
    std::string getReadAddressPinName() { return "JTAG_TDI";}
    std::string getReadPinName() { return "JTAG_TDO";}
    int getReadAddressVectorNumberLSB(){ return 5;}
    int getReadAddressVectorNumberMSB(){ return 13;}
    int getReadDataCycleNumberLSB(){ return 19;}
    int getReadDataCycleNumberMSB(){ return 26;}
    int getReadDataVectorNumberLSB(){ return 19;}
    int getReadDataVectorNumberMSB(){ return 26;}

    /*wait parameters*/
    std::string getTimingPort(){ return "@";}
    std::string getWaitTemplatePatternName(){ return "cpu_jtag_wait_temp";}
};


