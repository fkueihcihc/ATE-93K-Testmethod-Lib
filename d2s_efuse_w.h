#ifndef D2SPARLLEL_H_
#define D2SPARLLEL_H_
#include "generic_d2s/d2s_Generic.h"
#include <map>
#define start_cycle 0
#define start_R_cycle 0
#define start_WA_cycle 0
#define start_WD_cycle 0

class d2s_efuse_write : public d2s_Generic{
private:
	d2s_efuse_write(std::string pName):d2s_Generic(pName){};
	d2s_efuse_write(const d2s_efuse_write&);
	d2s_efuse_write& operator= (const d2s_efuse_write&);

public:
    //singleton
    static d2s_efuse_write& Instance(){
        static d2s_efuse_write instance("@");
        d2s_efuse_write& ref = instance;
        return ref;
    }

    /*general parameters*/

    int getAddressBits(){ return 12;}
    int getDataBits(){ return 1;}

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

    int getPadding(){return 1;}


    /*write parameters*/
    std::string getWriteTemplatePatternName(){
    	switch(currentFormat){
    		case 0: return "efuse_write_temp";
    	//case 1: return "otherWritePattern";
    	}
    	return "";
    }

    void initWriteValueVectorMaps(){
        bool parametersValid = true;
        //TODO: check parametersValid in new generic implementation...
        assert(parametersValid);

        //address
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("GMII_TX_EN", start_WA_cycle)));//a00
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("GMII_RX_CLK", start_WA_cycle)));//a01
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("GMII_RXD0", start_WA_cycle)));//a02
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("GMII_RXD1", start_WA_cycle)));//a03
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("GMII_RXD2", start_WA_cycle)));//a04
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("GMII_RXD3", start_WA_cycle)));//a05
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("GMII_RXD4", start_WA_cycle)));//a06
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("GMII_RXD5", start_WA_cycle)));//a07
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000100, PinVector ("GMII_RXD6", start_WA_cycle)));//a08
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000200, PinVector ("GMII_RXD7",  start_WA_cycle)));//a09
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000400, PinVector ("GMII_RX_ER", start_WA_cycle)));//a10
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000800, PinVector ("GMII_RX_DV", start_WA_cycle)));//a11

        //data
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("GMII_COL", start_WD_cycle)));//d00

        writeMapInitialized = true;
    }

    /*wait parameters*/
    std::string getTimingPort(){return "@";}

    std::string getWaitTemplatePatternName(){
    	return "parallel_wait_temp";
    }
};

#endif
