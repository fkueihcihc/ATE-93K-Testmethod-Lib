/*
 * d2s_APB_w.h
 *
 *  Created on:  20170821
 *      Author:
 */
#include "../d2s_tml/generic_d2s/d2s_Generic.h"
#include <map>

#define start_WA_vector 3   //keep ?
#define start_WD_vector 3



class d2s_APB_w : public d2s_Generic{
private:
	d2s_APB_w(std::string pName):d2s_Generic(pName){};
	d2s_APB_w(const d2s_APB_w&);
	d2s_APB_w& operator= (const d2s_APB_w&);

public:
    //singleton
    static d2s_APB_w& Instance(){
        static d2s_APB_w instance("@");
        d2s_APB_w& ref = instance;
        return ref;
    }

    /*general parameters*/
    int getAddressBits(){ return 8;}
    int getDataBits(){ return 8;}
    int getLowWaveformIndex(){ return 1;}
    int getHighWaveformIndex(){ return 2;}
    int getLowStrobeWaveformIndex(){ return 3;}
    int getHighStrobeWaveformIndex(){ return 4;}
    int getMaskStrobeWaveformIndex(){ return 5;}
    int getPadding(){return 1;}    //4 keep?
    std::string getInterfaceName(){return "gen_Parallel";}

    /*write parameters*/
    std::string getWriteTemplatePatternName(){return "APB_write_templete";}

    void initWriteValueVectorMaps(){
        bool parametersValid = true;
        //TODO: check parametersValid in new generic implementation...
        assert(parametersValid);



        //addr_index:0~7
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (00000001, PinVector ("SDIO1_DATA2", start_WA_vector)));//a00
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (00000010, PinVector ("SDIO1_DATA3", start_WA_vector)));//a01
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (00000100, PinVector ("SDIO1_CD", start_WA_vector)));//a02
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (00001000, PinVector ("AONGPIO30", start_WA_vector)));//a03
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (00010000, PinVector ("AONGPIO24", start_WA_vector)));//a04
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (00100000, PinVector ("AONGPIO25", start_WA_vector)));//a05
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (01000000, PinVector ("SDIO0_CLK", start_WA_vector)));//a06
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (10000000, PinVector ("SDIO0_CMD", start_WA_vector)));//a07


        //8 bit_per_addr
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (00000001, PinVector ("SDIO0_DATA2", start_WA_vector)));//a00
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (00000010, PinVector ("SDIO0_DATA3", start_WA_vector)));//a01
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (00000100, PinVector ("SDIO0_CD", start_WA_vector)));//a02
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (00001000, PinVector ("SPDIF_OUT", start_WA_vector)));//a03
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (00010000, PinVector ("IR_IN",  start_WA_vector)));//a04
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (00100000, PinVector ("AONGPIO5", start_WA_vector)));//a05
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (01000000, PinVector ("AONGPIO6", start_WA_vector)));//a06
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (10000000, PinVector ("AONGPIO7", start_WA_vector)));//a07






//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("SDIO1_DATA2", start_WA_vector)));//a00
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("SDIO1_DATA3", start_WA_vector)));//a01
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("SDIO1_CD", start_WA_vector)));//a02
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("AONGPIO30", start_WA_vector)));//a03
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("AONGPIO24", start_WA_vector)));//a04
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("AONGPIO25", start_WA_vector)));//a05
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("SDIO0_CLK", start_WA_vector)));//a06
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000100, PinVector ("SDIO0_CMD", start_WA_vector)));//a07
//
//
//        //8 bit_per_addr
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("SDIO0_DATA2", start_WA_vector)));//a00
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("SDIO0_DATA3", start_WA_vector)));//a01
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("SDIO0_CD", start_WA_vector)));//a02
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("SPDIF_OUT", start_WA_vector)));//a03
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("IR_IN",  start_WA_vector)));//a04
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("AONGPIO5", start_WA_vector)));//a05
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("AONGPIO6", start_WA_vector)));//a06
//        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000100, PinVector ("AONGPIO7", start_WA_vector)));//a07
//


//
//
//        //data_stb
//        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("SPI1_SDO", start_WD_vector)));//d00
//
//        //Example: Dec 958= 29*32+30; [A4:A0]=29;  [A9:A5]=30;

        writeMapInitialized = true;
    }

};

