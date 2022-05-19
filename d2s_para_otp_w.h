
#include "generic_d2s/d2s_Generic.h"
#include <map>


#define start_WA_vector 4
#define start_WD_vector 4

class d2s_para_otp_write : public d2s_Generic{
private:
	d2s_para_otp_write(std::string pName):d2s_Generic(pName){};
	d2s_para_otp_write(const d2s_para_otp_write&);
	d2s_para_otp_write& operator= (const d2s_para_otp_write&);

public:
    //singleton
    static d2s_para_otp_write& Instance(){
        static d2s_para_otp_write instance("@");
        d2s_para_otp_write& ref = instance;
        return ref;
    }

    /*general parameters*/

    int getAddressBits(){ return 13;}
    int getDataBits(){ return 1;}

    int getLowWaveformIndex(){ return 0;}
    int getHighWaveformIndex(){ return 1;}
    int getLowStrobeWaveformIndex(){ return 2;}
    int getHighStrobeWaveformIndex(){ return 3;}
    int getMaskStrobeWaveformIndex(){ return 4;}

    std::string getInterfaceName(){
    	switch(currentFormat){
    	case 0:
    		return "otp_w";
    		break;
    	}
    	return "";
    }

    int getPadding(){return 15;}


    /*write parameters*/
    std::string getWriteTemplatePatternName(){
    	switch(currentFormat){
    		case 0: return "otp_write_temp";
    	//case 1: return "otherWritePattern";
    	}
    	return "";
    }

    void initWriteValueVectorMaps(){
        bool parametersValid = true;
        //TODO: check parametersValid in new generic implementation...
        assert(parametersValid);

        //address
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("AONGPIO24", start_WA_vector)));//a00
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("AONGPIO25", start_WA_vector)));//a01
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("SDIO0_CLK", start_WA_vector)));//a02
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("SDIO0_CMD", start_WA_vector)));//a03
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("SDIO0_DATA0", start_WA_vector)));//a04
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("SDIO0_DATA1", start_WA_vector)));//a05
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("SDIO0_DATA2", start_WA_vector)));//a06
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("SDIO0_DATA3", start_WA_vector)));//a07
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000100, PinVector ("SDIO0_CD_N", start_WA_vector)));//a08
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000200, PinVector ("SPDIF_OUT", start_WA_vector)));//a09
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000400, PinVector ("IR_IN", start_WA_vector)));//a10
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000800, PinVector ("AONGPIO5", start_WA_vector)));//a11
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00001000, PinVector ("AONGPIO6", start_WA_vector)));//a12

        //data
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("AONGPIO7", start_WD_vector)));//d00

        writeMapInitialized = true;
    }

    /*wait parameters*/
    std::string getTimingPort(){return "@";}

    std::string getWaitTemplatePatternName(){
    	return "otp_wait_temp";
    }
};


