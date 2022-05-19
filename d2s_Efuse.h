#ifndef D2SPARLLEL_H_
#define D2SPARLLEL_H_
#include "generic_d2s/d2s_Generic.h"
#include <map>
#define start_cycle 0
#define start_R_cycle 0
#define start_WA_cycle 0
#define start_WD_cycle 0

class d2s_Efuse_Write : public d2s_Generic{
private:
	d2s_Efuse_Write(std::string pName):d2s_Generic(pName){};
	d2s_Efuse_Write(const d2s_Efuse_Write&);
	d2s_Efuse_Write& operator= (const d2s_Efuse_Write&);

public:
    //singleton
    static d2s_Efuse_Write& Instance(){
        static d2s_Efuse_Write instance("@");
        d2s_Efuse_Write& ref = instance;
        return ref;
    }

    /*general parameters*/

    int getAddressBits(){ return 7;}
    int getDataBits(){ return 32;}

    int getLowWaveformIndex(){ return 0;}
    int getHighWaveformIndex(){ return 1;}
    int getLowStrobeWaveformIndex(){ return 2;}
    int getHighStrobeWaveformIndex(){ return 3;}
    int getMaskStrobeWaveformIndex(){ return 4;}

    std::string getInterfaceName(){
    	switch(currentFormat){
    	case 0:
    		return "efuse_write";
    		break;
    	}
    	return "";
    }

    int getPadding(){return 1;}

    int getPadding_R(){return 1;}

    /*write parameters*/
    std::string getWriteTemplatePatternName(){
    	switch(currentFormat){
    	case 0: return "Efuse_Write_Temp";
    	//case 1: return "otherWritePattern";
    	}
    	return "";
    }
    std::string getReadTemplatePatternName(){
    	switch(currentFormat){
    	case 0: return "Efuse_Read_Temp";
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
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("GMII_RXD2", start_WA_cycle)));//a03
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("GMII_RXD3", start_WA_cycle)));//a03
        writeAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("GMII_RXD4", start_WA_cycle)));//a03

        //data
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("GMII_RXD5", start_WA_cycle)));//d00
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("GMII_RXD6", start_WA_cycle)));//d01
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("GMII_RXD7",  start_WA_cycle)));//d02
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("GMII_RX_ER", start_WA_cycle)));//d03
        writeDataValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("GMII_RX_DV", start_WA_cycle)));//d03

        writeMapInitialized = true;
    }
    //void initReadAndExpectValueVectorMaps(){
    void initReadValueCycleMaps(){
            bool parametersValid = true;
            //TODO: check parametersValid in new generic implementation...
            assert(parametersValid);

            //address
            readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("GMII_TX_EN", start_WA_cycle)));//a00
            readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("GMII_RX_CLK", start_WA_cycle)));//a01
            readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("GMII_RXD0", start_WA_cycle)));//a02
            readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("GMII_RXD1", start_WA_cycle)));//a03
            readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("GMII_RXD2", start_WA_cycle)));//a04
            readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("GMII_RXD3", start_WA_cycle)));//a05
            readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("GMII_RXD4", start_WA_cycle)));//a06
            //data
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("GMII_CRS", start_WA_cycle)));//d00
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("GMII_MDC", start_WA_cycle)));//d01
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("GMII_MDIO",  start_WA_cycle)));//d02
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("GMAC_125M_IN", start_WA_cycle)));//d03
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("GMAC_25M_OUT", start_WA_cycle)));//d00
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("UART0_RXD", start_WA_cycle)));//d01
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("UART0_TXD",  start_WA_cycle)));//d02
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("SDIO0_CLK", start_WA_cycle)));//d03
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000100, PinVector ("SDIO0_CMD", start_WA_cycle)));//d00
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000200, PinVector ("SDIO0_DATA0", start_WA_cycle)));//d01
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000400, PinVector ("SDIO0_DATA1",  start_WA_cycle)));//d02
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000800, PinVector ("SDIO0_DATA2", start_WA_cycle)));//d03
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00001000, PinVector ("SDIO0_DATA3", start_WA_cycle)));//d00
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00002000, PinVector ("SDIO0_CD", start_WA_cycle)));//d01
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00004000, PinVector ("SDIO0_WP",  start_WA_cycle)));//d02
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00008000, PinVector ("SPDIF_OUT", start_WA_cycle)));//d03
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00010000, PinVector ("IR_IN", start_WA_cycle)));//d00
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00020000, PinVector ("SPI0_CLK", start_WA_cycle)));//d01
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00040000, PinVector ("SPI0_CS",  start_WA_cycle)));//d02
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00080000, PinVector ("SPI0_TXD", start_WA_cycle)));//d03
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00100000, PinVector ("SPI0_RXD", start_WA_cycle)));//d00
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00200000, PinVector ("KEY_COL0", start_WA_cycle)));//d01
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00400000, PinVector ("KEY_COL1",  start_WA_cycle)));//d02
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00800000, PinVector ("KEY_COL2", start_WA_cycle)));//d03
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x01000000, PinVector ("KEY_ROW0", start_WA_cycle)));//d00
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x02000000, PinVector ("KEY_ROW1", start_WA_cycle)));//d01
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x04000000, PinVector ("KEY_ROW2",  start_WA_cycle)));//d02
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x08000000, PinVector ("HDMI_SCL", start_WA_cycle)));//d03
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x10000000, PinVector ("HDMI_SDA", start_WA_cycle)));//d00
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x20000000, PinVector ("HDMI_CEC", start_WA_cycle)));//d01
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x40000000, PinVector ("HDMI_HPD",  start_WA_cycle)));//d02
            readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x80000000, PinVector ("AONGPIO31", start_WA_cycle)));//d03

            readAndExpectValueMapInitialized = true;
        }


    /*wait parameters*/
    std::string getTimingPort(){return "@";}

    std::string getWaitTemplatePatternName(){
    	return "PARALLEL_WAIT_TEMP";
    }
};

#endif
