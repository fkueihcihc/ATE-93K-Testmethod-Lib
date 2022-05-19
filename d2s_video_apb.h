#include "generic_d2s/d2s_Generic.h"
#include <map>


#define start_cycle 0
#define read_data 1

class d2s_vdac_apb : public d2s_Generic{
private:
	d2s_vdac_apb(std::string pName):d2s_Generic(pName){};
	d2s_vdac_apb(const d2s_vdac_apb&);
	d2s_vdac_apb& operator= (const d2s_vdac_apb&);

public:
    //singleton
    static d2s_vdac_apb& Instance(){
        static d2s_vdac_apb instance("@");
        d2s_vdac_apb& ref = instance;
        return ref;
    }

    /*general parameters*/

    int getAddressBits(){ return 8;}
    int getDataBits(){ return 8;}

    int getLowWaveformIndex(){ return 0;}
    int getHighWaveformIndex(){ return 1;}
    int getLowStrobeWaveformIndex(){ return 2;}
    int getHighStrobeWaveformIndex(){ return 3;}
    int getMaskStrobeWaveformIndex(){ return 4;}

    std::string getInterfaceName(){
    	switch(currentFormat){
    	case 0:
    		return "gen_Parallel_vdac_apb";
    		break;
    	}
    	return "";
    }

    int getPadding(){return 3;}


    /*write parameters*/
    std::string getWriteTemplatePatternName(){
    	switch(currentFormat){
    		case 0: return "vdac_apb_write_temp";
    	//case 1: return "otherWritePattern";
    	}
    	return "";
    }

    std::string getReadTemplatePatternName(){
    	switch(currentFormat){
    	case 0: return "vdac_apb_read_temp";
    	//case 1: return "otherWritePattern";
    	}
    	return "";
    }

    void initWriteValueVectorMaps(){
        bool parametersValid = true;
        //TODO: check parametersValid in new generic implementation...
        assert(parametersValid);

        //address
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("AONGPIO24", start_cycle)));//a00
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("AONGPIO25", start_cycle)));//a01
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("SDIO0_CLK", start_cycle)));//a02
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("SDIO0_CMD", start_cycle)));//a03
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("SDIO0_DATA0", start_cycle)));//a04
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("SDIO0_DATA1", start_cycle)));//a05
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("SDIO0_DATA2", start_cycle)));//a06
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("SDIO0_DATA3", start_cycle)));//a07

        //data

        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("AONGPIO7", start_cycle)));//d00
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("AONGPIO8", start_cycle)));//d01
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("AONGPIO9", start_cycle)));//d02
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("AONGPIO10", start_cycle)));//d03
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("AONGPIO16", start_cycle)));//d04
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("AONGPIO17", start_cycle)));//d05
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("BOOT_CTRL_MS18", start_cycle)));//d06
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("BOOT_SEL0", start_cycle)));//d07

        writeMapInitialized = true;
    }

    void initReadAndExpectValueVectorMaps(){
        bool parametersValid = true;
         //TODO: check parametersValid in new generic implementation...
         assert(parametersValid);

         //address

         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("AONGPIO24", start_cycle)));//a00
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("AONGPIO25", start_cycle)));//a01
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("SDIO0_CLK", start_cycle)));//a02
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("SDIO0_CMD", start_cycle)));//a03
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("SDIO0_DATA0", start_cycle)));//a04
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("SDIO0_DATA1", start_cycle)));//a05
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("SDIO0_DATA2", start_cycle)));//a06
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("SDIO0_DATA3", start_cycle)));//a07

         //data

         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("AONGPIO7", read_data)));//d00
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("AONGPIO8", read_data)));//d01
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("AONGPIO9", read_data)));//d02
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("AONGPIO10", read_data)));//d03
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("AONGPIO16", read_data)));//d04
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("AONGPIO17", read_data)));//d05
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("BOOT_CTRL_MS18", read_data)));//d06
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("BOOT_SEL0", read_data)));//d07

         readAndExpectValueMapInitialized = true;
    }
    void initReadValueCycleMaps(){
            bool parametersValid = true;
            //TODO: check parametersValid in new generic implementation...
            assert(parametersValid);

            //data

            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("AONGPIO7", read_data)));//d00
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("AONGPIO8", read_data)));//d01
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("AONGPIO9", read_data)));//d02
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("AONGPIO10", read_data)));//d03
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("AONGPIO16", read_data)));//d04
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("AONGPIO17", read_data)));//d05
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("BOOT_CTRL_MS18", read_data)));//d06
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("BOOT_SEL0", read_data)));//d07

            readCycleMapInitialized = true;
        }

    /*wait parameters*/
    std::string getTimingPort(){return "@";}

    std::string getWaitTemplatePatternName(){
    	return "vdac_apb_wait_temp";
    }
};


