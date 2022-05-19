
#include "generic_d2s/d2s_Generic.h"
#include <map>
#define start_cycle 1
#define start_R_cycle 2
#define start_WA_cycle 0
#define start_WD_cycle 0

class d2s_SPUART_w : public d2s_Generic{
private:
	d2s_SPUART_w(std::string pName):d2s_Generic(pName){};
	d2s_SPUART_w(const d2s_SPUART_w&);
	d2s_SPUART_w& operator= (const d2s_SPUART_w&);

public:
    //singleton
    static d2s_SPUART_w& Instance(){
        static d2s_SPUART_w instance("@");
        d2s_SPUART_w& ref = instance;
        return ref;
    }

    /*general parameters*/

    int getAddressBits(){ return 1;}
    int getDataBits(){ return 8;}

    int getLowWaveformIndex(){ return 0;}
    int getHighWaveformIndex(){ return 1;}
    int getLowStrobeWaveformIndex(){ return 3;}
    int getHighStrobeWaveformIndex(){ return 4;}
    int getMaskStrobeWaveformIndex(){ return 6;}

    std::string getInterfaceName(){
    	switch(currentFormat){
    	case 0:
    		return "spuart_w";
    		break;
    	}
    	return "";
    }

    int getPadding(){return 1;}

    int getPadding_R(){return 1;}

    /*write parameters*/
    std::string getWriteTemplatePatternName(){
    	switch(currentFormat){
    	case 0: return "spuart_write_template";
    	//case 1: return "otherWritePattern";
    	}
    	return "";
    }

    void initWriteValueVectorMaps(){
        bool parametersValid = true;
        //TODO: check parametersValid in new generic implementation...
        assert(parametersValid);

        //address:1bits,not actually useful
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("PIN_MODE",  0)));//fake address not actually use

        //data:8bits, one byte write once
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("INV_CLKO", start_WD_cycle)));//d00
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("P_CORE_EN",  start_WD_cycle)));//d01
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("BI_CLKO",start_WD_cycle)));//d02
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("CI_NRSTO", start_WD_cycle)));//d03

        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("INV_CLKO", start_WD_cycle+1)));//d04
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("P_CORE_EN",  start_WD_cycle+1)));//d05
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("BI_CLKO",start_WD_cycle+1)));//d06
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("CI_NRSTO", start_WD_cycle+1)));//d07


        writeMapInitialized = true;
    }



    /*wait parameters*/
    std::string getTimingPort(){return "@";}

    std::string getWaitTemplatePatternName(){
    	return "spuart_wait_template";
    }
};

