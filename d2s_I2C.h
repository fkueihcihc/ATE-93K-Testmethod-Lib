#ifndef D2SSERIALPROTOCOLTEMPLATE_H_
#define D2SSERIALPROTOCOLTEMPLATE_H_
#include "d2s_Serial.h"

class d2s_I2C : public d2s_Serial{
private:
	d2s_I2C(std::string pName):d2s_Serial(pName){};
	d2s_I2C(const d2s_I2C&);
	d2s_I2C& operator= (const d2s_I2C&);

public:    
    //singleton
    static d2s_I2C& Instance(){
        static d2s_I2C instance("@");
        d2s_I2C& ref = instance;
        return ref;
    }
    
//    /*general parameters*/
//    int getAddressBits(){ return 8;}
//    int getDataBits(){ return 8;}
//    int getHighWaveformIndex(){ return 1;}
//    int getLowWaveformIndex(){ return 0;}
//    int getHighStrobeWaveformIndex(){ return 3;}
//    int getLowStrobeWaveformIndex(){ return 2;}
//    int getMaskStrobeWaveformIndex(){ return 5;}
//    std::string getInterfaceName(){ return "SSI_PMU"; }
//
//    /*write parameters*/
//    std::string getWriteTemplatePatternName(){ return "d2s_write";}
//    std::string getWritePinName() { return "SSI_PMU";}
//    int getWriteAddressVectorNumberLSB(){ return 21;}
//    int getWriteAddressVectorNumberMSB(){ return 14;}
//    int getWriteDataVectorNumberLSB(){ return 30;}
//    int getWriteDataVectorNumberMSB(){ return 23;}
//
//    /*read parameters*/
//    std::string getReadTemplatePatternName(){ return "d2s_read";}
//    std::string getReadAddressPinName() { return "SSI_PMU";}
//    std::string getReadPinName() { return "SSI_PMU";}
//    int getReadAddressVectorNumberLSB(){ return 21;}
//    int getReadAddressVectorNumberMSB(){ return 14;}
//    int getReadDataCycleNumberLSB(){ return 49;}
//    int getReadDataCycleNumberMSB(){ return 42;}
//    int getReadDataVectorNumberLSB(){ return 49;}
//    int getReadDataVectorNumberMSB(){ return 42;}
//
//    /*wait parameters*/
//    std::string getTimingPort(){ return "@";}
//    std::string getWaitTemplatePatternName(){ return "d2s_wait";}
//
//    std::string getDCScaleTemplatePatternName(){return "dcscale_adc";}//"dcscale_template"
    /*general parameters*/
     int getAddressBits(){ return 9;}//ori is 6
     int getDataBits(){ return 8;}  //ori is 16
     int getHighWaveformIndex(){ return 1;}
     int getLowWaveformIndex(){ return 0;}
     int getHighStrobeWaveformIndex(){ return 3;}//ori is 3
     int getLowStrobeWaveformIndex(){ return 2;}//ori is 2
     int getMaskStrobeWaveformIndex(){ return 4;}
     std::string getInterfaceName(){ return "pmu"; }//VrefTrim//BuckLdo/iLimit/LedCurr/iLimitTrim/commonRead

     /*write parameters*/
     std::string getWriteTemplatePatternName(){ return "d2s_pmu_write_template";}
     std::string getWritePinName() { return "SSI_PMU";}
     int getWriteAddressVectorNumberLSB(){ return 12;}
     int getWriteAddressVectorNumberMSB(){ return 4;}
     int getWriteDataVectorNumberLSB(){ return 20;}
     int getWriteDataVectorNumberMSB(){ return 13;}

     /*read parameters*/
     std::string getReadTemplatePatternName(){ return "d2s_pmu_read_template";}
     std::string getReadAddressPinName() { return "SSI_PMU";}
     std::string getReadPinName() { return "SSI_PMU";}
     int getReadAddressVectorNumberLSB(){ return 12;}
     int getReadAddressVectorNumberMSB(){ return 4;}
     int getReadDataCycleNumberLSB(){ return 23;}
     int getReadDataCycleNumberMSB(){ return 16;}
     int getReadDataVectorNumberLSB(){ return 23;}
     int getReadDataVectorNumberMSB(){ return 16;}

     /*wait parameters*/
     std::string getTimingPort(){ return "@";}
     std::string getWaitTemplatePatternName(){ return "d2s_pmu_wait_template";}
};

#endif
