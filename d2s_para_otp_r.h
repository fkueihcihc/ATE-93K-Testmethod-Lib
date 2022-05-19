
#include "generic_d2s/d2s_Generic.h"
#include <map>
#define start_RA_vector 4
#define start_RD_vector 5
#define start_RD_cycle 9


class d2s_para_otp_read : public d2s_Generic{
private:
	d2s_para_otp_read(std::string pName):d2s_Generic(pName){};
	d2s_para_otp_read(const d2s_para_otp_read&);
	d2s_para_otp_read& operator= (const d2s_para_otp_read&);

public:
    //singleton
    static d2s_para_otp_read& Instance(){
        static d2s_para_otp_read instance("@");
        d2s_para_otp_read& ref = instance;
        return ref;
    }

    /*general parameters*/

    int getAddressBits(){ return 10;}
    int getDataBits(){ return 8;}

    int getLowWaveformIndex(){ return 0;}
    int getHighWaveformIndex(){ return 1;}
    int getLowStrobeWaveformIndex(){ return 2;}
    int getHighStrobeWaveformIndex(){ return 3;}
    int getMaskStrobeWaveformIndex(){ return 4;}

    std::string getInterfaceName(){
    	switch(currentFormat){
    	case 0:
    		return "otp_r";
    		break;
    	}
    	return "";
    }

    int getPadding(){return 2;}


    std::string getReadTemplatePatternName(){
    	switch(currentFormat){
    	case 0: return "otp_read_temp";
    	//case 1: return "otherWritePattern";
    	}
    	return "";
    }
    void initReadAndExpectValueVectorMaps(){
        bool parametersValid = true;
         //TODO: check parametersValid in new generic implementation...
         assert(parametersValid);

         //address

         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("SDIO0_CMD", start_RA_vector)));//a03
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("SDIO0_DATA0", start_RA_vector)));//a04
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("SDIO0_DATA1", start_RA_vector)));//a05
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("SDIO0_DATA2", start_RA_vector)));//a06
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("SDIO0_DATA3", start_RA_vector)));//a07
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("SDIO0_CD_N", start_RA_vector)));//a08
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("SPDIF_OUT", start_RA_vector)));//a09
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("IR_IN", start_RA_vector)));//a10
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000100, PinVector ("AONGPIO5", start_RA_vector)));//a11
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000200, PinVector ("AONGPIO6", start_RA_vector)));//a12
         //data

         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("AONGPIO10", start_RD_vector)));//d00
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("BOOT_CTRL_MS18", start_RD_vector)));//d01
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("SDIO1_CLK",  start_RD_vector)));//d02
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("BOOT_SEL1", start_RD_vector)));//d03
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("AONGPIO16", start_RD_vector)));//d04
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("BOOT_SEL0", start_RD_vector)));//d05
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("AONGPIO17",  start_RD_vector)));//d06
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("BOOT_SEL2", start_RD_vector)));//d07



         readAndExpectValueMapInitialized = true;
    }
    void initReadValueCycleMaps(){
            bool parametersValid = true;
            //TODO: check parametersValid in new generic implementation...
            assert(parametersValid);

            //data
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("AONGPIO10", start_RD_cycle)));//d00
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("BOOT_CTRL_MS18", start_RD_cycle)));//d01
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("SDIO1_CLK",  start_RD_cycle)));//d02
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("BOOT_SEL1", start_RD_cycle)));//d03
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("AONGPIO16", start_RD_cycle)));//d04
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("BOOT_SEL0", start_RD_cycle)));//d05
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("AONGPIO17",  start_RD_cycle)));//d06
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("BOOT_SEL2", start_RD_cycle)));//d07


            readCycleMapInitialized = true;
        }
};


