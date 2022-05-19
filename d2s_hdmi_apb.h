#include "generic_d2s/d2s_Generic.h"
#include <map>


#define start_cycle 0

class d2s_apb : public d2s_Generic{
private:
	d2s_apb(std::string pName):d2s_Generic(pName){};
	d2s_apb(const d2s_apb&);
	d2s_apb& operator= (const d2s_apb&);

public:
    //singleton
    static d2s_apb& Instance(){
        static d2s_apb instance("pHDMI_TEST_MODE");
        d2s_apb& ref = instance;
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
    		return "gen_Parallel";
    		break;
    	}
    	return "";
    }

    int getPadding(){return 3;}


    /*write parameters*/
    std::string getWriteTemplatePatternName(){
    	switch(currentFormat){
    		case 0: return "hdmi_apb_write_temp";
    	//case 1: return "otherWritePattern";
    	}
    	return "";
    }

    std::string getReadTemplatePatternName(){
    	switch(currentFormat){
    	case 0: return "hdmi_apb_read_temp";
    	//case 1: return "otherWritePattern";
    	}
    	return "";
    }

    void initWriteValueVectorMaps(){
        bool parametersValid = true;
        //TODO: check parametersValid in new generic implementation...
        assert(parametersValid);

        //address
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("AONGPIO25", start_cycle)));//a00
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("SDIO0_CLK", start_cycle)));//a01
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("SDIO0_CMD", start_cycle)));//a02
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("SDIO0_DATA0", start_cycle)));//a03
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("SDIO0_DATA1", start_cycle)));//a04
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("SDIO0_DATA2", start_cycle)));//a05
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("SDIO0_DATA3", start_cycle)));//a06
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("SDIO0_CD_N", start_cycle)));//a07

        //data

        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("SPDIF_OUT", start_cycle)));//d00
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("IR_IN", start_cycle)));//d01
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("AONGPIO5", start_cycle)));//d02
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("AONGPIO6", start_cycle)));//d03
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("AONGPIO7", start_cycle)));//d04
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("AONGPIO8", start_cycle)));//d05
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("AONGPIO9", start_cycle)));//d06
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("AONGPIO10", start_cycle)));//d07

        writeMapInitialized = true;
    }

    void initReadAndExpectValueVectorMaps(){
        bool parametersValid = true;
         //TODO: check parametersValid in new generic implementation...
         assert(parametersValid);

         //address

         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("AONGPIO25", start_cycle)));//a00
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("SDIO0_CLK", start_cycle)));//a01
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("SDIO0_CMD", start_cycle)));//a02
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("SDIO0_DATA0", start_cycle)));//a03
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("SDIO0_DATA1", start_cycle)));//a04
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("SDIO0_DATA2", start_cycle)));//a05
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("SDIO0_DATA3", start_cycle)));//a06
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("SDIO0_CD_N", start_cycle)));//a07

         //data

         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("SPDIF_OUT", start_cycle)));//d00
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("IR_IN", start_cycle)));//d01
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("AONGPIO5", start_cycle)));//d02
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("AONGPIO6", start_cycle)));//d03
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("AONGPIO7", start_cycle)));//d04
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("AONGPIO8", start_cycle)));//d05
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("AONGPIO9", start_cycle)));//d06
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("AONGPIO10", start_cycle)));//d07

         readAndExpectValueMapInitialized = true;
    }
    void initReadValueCycleMaps(){
            bool parametersValid = true;
            //TODO: check parametersValid in new generic implementation...
            assert(parametersValid);

            //data

            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("SPDIF_OUT", start_cycle)));//d00
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("IR_IN", start_cycle)));//d01
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("AONGPIO5", start_cycle)));//d02
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("AONGPIO6", start_cycle)));//d03
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("AONGPIO7", start_cycle)));//d04
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("AONGPIO8", start_cycle)));//d05
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("AONGPIO9", start_cycle)));//d06
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("AONGPIO10", start_cycle)));//d07

            readCycleMapInitialized = true;
        }

    /*wait parameters*/
    std::string getTimingPort(){return "pHDMI_TEST_MODE";}

    std::string getWaitTemplatePatternName(){
    	return "hdmi_apb_wait_temp";
    }
};


