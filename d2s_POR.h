#ifndef D2SPARLLEL_H_
#define D2SPARLLEL_H_
#include "generic_d2s/d2s_Generic.h"
#include <map>
#define start_cycle 1
#define start_R_cycle 2
#define start_WA_cycle 0
#define start_WD_cycle 1

class d2s_POR : public d2s_Generic{
private:
	d2s_POR(std::string pName):d2s_Generic(pName){};
	d2s_POR(const d2s_POR&);
	d2s_POR& operator= (const d2s_POR&);

public:
    //singleton
    static d2s_POR& Instance(){
        static d2s_POR instance("@");
        d2s_POR& ref = instance;
        return ref;
    }

    /*general parameters*/

    int getAddressBits(){ return 4;}
    int getDataBits(){ return 4;}

    int getLowWaveformIndex(){ return 0;}
    int getHighWaveformIndex(){ return 1;}
    int getLowStrobeWaveformIndex(){ return 3;}
    int getHighStrobeWaveformIndex(){ return 4;}
    int getMaskStrobeWaveformIndex(){ return 5;}

    std::string getInterfaceName(){
    	switch(currentFormat){
    	case 0:
    		return "por";
    		break;
    	}
    	return "";
    }

    int getPadding(){return 1;}

    int getPadding_R(){return 1;}

    /*write parameters*/
    std::string getWriteTemplatePatternName(){
    	switch(currentFormat){
    	case 0: return "POR_Trim_Temp_1";
    	//case 1: return "otherWritePattern";
    	}
    	return "";
    }

    void initWriteValueVectorMaps(){
        bool parametersValid = true;
        //TODO: check parametersValid in new generic implementation...
        assert(parametersValid);

        //address
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("GMII_TXD4", start_WA_cycle)));//a00
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("GMII_TXD5", start_WA_cycle)));//a01
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("GMII_TXD6", start_WA_cycle)));//a02
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("GMII_TXD7", start_WA_cycle)));//a03
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("JTAG_TMS", start_WA_cycle)));//a00
//                writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("JTAG_TDO", start_WA_cycle)));//a01
//                writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("JTAG_TRSTN", start_WA_cycle)));//a02
//                writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("JTAG_TCK", start_WA_cycle)));//a03

        //data
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("GMII_TXD4", start_WA_cycle)));//d00
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("GMII_TXD5", start_WA_cycle)));//d01
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("GMII_TXD6",  start_WA_cycle)));//d02
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("GMII_TXD7", start_WA_cycle)));//d03
//        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("JTAG_TMS", start_WA_cycle)));//d00
//                writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("JTAG_TDO", start_WA_cycle)));//d01
//                writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("JTAG_TRSTN",  start_WA_cycle)));//d02
//                writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("JTAG_TCK", start_WA_cycle)));//d03


        writeMapInitialized = true;
    }



    /*wait parameters*/
    std::string getTimingPort(){return "@";}

    std::string getWaitTemplatePatternName(){
    	return "PARALLEL_WAIT_TEMP";
    }
};

class d2s_POR_1 : public d2s_Generic{
private:
	d2s_POR_1(std::string pName):d2s_Generic(pName){};
	d2s_POR_1(const d2s_POR_1&);
	d2s_POR_1& operator= (const d2s_POR_1&);

public:
    //singleton
    static d2s_POR_1& Instance(){
        static d2s_POR_1 instance("@");
        d2s_POR_1& ref = instance;
        return ref;
    }

    /*general parameters*/

    int getAddressBits(){ return 4;}
    int getDataBits(){ return 4;}

    int getLowWaveformIndex(){ return 0;}
    int getHighWaveformIndex(){ return 1;}
    int getLowStrobeWaveformIndex(){ return 3;}
    int getHighStrobeWaveformIndex(){ return 4;}
    int getMaskStrobeWaveformIndex(){ return 5;}

    std::string getInterfaceName(){
    	switch(currentFormat){
    	case 0:
    		return "por_1";
    		break;
    	}
    	return "";
    }

    int getPadding(){return 1;}

    int getPadding_R(){return 1;}

    /*write parameters*/
    std::string getWriteTemplatePatternName(){
    	switch(currentFormat){
    	case 0: return "POR_Trim_Temp";
    	//case 1: return "otherWritePattern";
    	}
    	return "";
    }

    void initWriteValueVectorMaps(){
        bool parametersValid = true;
        //TODO: check parametersValid in new generic implementation...
        assert(parametersValid);

        //address
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("GMII_TXD4", start_WA_cycle)));//a00
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("GMII_TXD5", start_WA_cycle)));//a01
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("GMII_TXD6", start_WA_cycle)));//a02
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("GMII_TXD7", start_WA_cycle)));//a03
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("JTAG_TMS", start_WA_cycle)));//a00
//                writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("JTAG_TDO", start_WA_cycle)));//a01
//                writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("JTAG_TRSTN", start_WA_cycle)));//a02
//                writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("JTAG_TCK", start_WA_cycle)));//a03

        //data
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("GMII_TXD4", start_WA_cycle)));//d00
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("GMII_TXD5", start_WA_cycle)));//d01
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("GMII_TXD6",  start_WA_cycle)));//d02
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("GMII_TXD7", start_WA_cycle)));//d03
//        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("JTAG_TMS", start_WA_cycle)));//d00
//                writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("JTAG_TDO", start_WA_cycle)));//d01
//                writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("JTAG_TRSTN",  start_WA_cycle)));//d02
//                writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("JTAG_TCK", start_WA_cycle)));//d03


        writeMapInitialized = true;
    }



    /*wait parameters*/
    std::string getTimingPort(){return "@";}

    std::string getWaitTemplatePatternName(){
    	return "PARALLEL_WAIT_TEMP";
    }
};
#endif
