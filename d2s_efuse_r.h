#ifndef D2SPARLLEL_H_
#define D2SPARLLEL_H_
#include "generic_d2s/d2s_Generic.h"
#include <map>
#define start_cycle 0
#define start_R_cycle 0
#define start_WA_cycle 0
#define start_WD_cycle 0

class d2s_efuse_read : public d2s_Generic{
private:
	d2s_efuse_read(std::string pName):d2s_Generic(pName){};
	d2s_efuse_read(const d2s_efuse_read&);
	d2s_efuse_read& operator= (const d2s_efuse_read&);

public:
    //singleton
    static d2s_efuse_read& Instance(){
        static d2s_efuse_read instance("@");
        d2s_efuse_read& ref = instance;
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
    		return "gen_Parallel";
    		break;
    	}
    	return "";
    }

    int getPadding(){return 1;}


    std::string getReadTemplatePatternName(){
    	switch(currentFormat){
    	case 0: return "efuse_read_temp";
    	//case 1: return "otherWritePattern";
    	}
    	return "";
    }
    void initReadAndExpectValueVectorMaps(){
        bool parametersValid = true;
         //TODO: check parametersValid in new generic implementation...
         assert(parametersValid);

         //address
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("GMII_TX_EN", start_WD_cycle)));//a00
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("GMII_RX_CLK", start_WD_cycle)));//a01
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("GMII_RXD0", start_WD_cycle)));//a02
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("GMII_RXD1", start_WD_cycle)));//a03
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("GMII_RXD2", start_WD_cycle)));//a04
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("GMII_RXD3", start_WD_cycle)));//a05
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("GMII_RXD4", start_WD_cycle)));//a06
         //data
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("GMII_CRS", start_WD_cycle)));//d00
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("GMII_MDC", start_WD_cycle)));//d01
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("GMII_MDIO",  start_WD_cycle)));//d02
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("GMAC_125M_IN", start_WD_cycle)));//d03
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("GMAC_25M_OUT", start_WD_cycle)));//d04
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("UART0_RXD", start_WD_cycle)));//d05
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("UART0_TXD",  start_WD_cycle)));//d06
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("SDIO0_CLK", start_WD_cycle)));//d07
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000100, PinVector ("SDIO0_CMD", start_WD_cycle)));//d08
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000200, PinVector ("SDIO0_DATA0", start_WD_cycle)));//d09
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000400, PinVector ("SDIO0_DATA1",  start_WD_cycle)));//d10
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000800, PinVector ("SDIO0_DATA2", start_WD_cycle)));//d11
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00001000, PinVector ("SDIO0_DATA3", start_WD_cycle)));//d12
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00002000, PinVector ("SDIO0_CD", start_WD_cycle)));//d13
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00004000, PinVector ("SDIO0_WP",  start_WD_cycle)));//d14
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00008000, PinVector ("SPDIF_OUT", start_WD_cycle)));//d15
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00010000, PinVector ("IR_IN", start_WD_cycle)));//d16
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00020000, PinVector ("SPI0_CLK", start_WD_cycle)));//d17
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00040000, PinVector ("SPI0_CS",  start_WD_cycle)));//d18
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00080000, PinVector ("SPI0_TXD", start_WD_cycle)));//d19
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00100000, PinVector ("SPI0_RXD", start_WD_cycle)));//d20
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00200000, PinVector ("KEY_COL0", start_WD_cycle)));//d21
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00400000, PinVector ("KEY_COL1",  start_WD_cycle)));//d22
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00800000, PinVector ("KEY_COL2", start_WD_cycle)));//d23
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x01000000, PinVector ("KEY_ROW0", start_WD_cycle)));//d24
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x02000000, PinVector ("KEY_ROW1", start_WD_cycle)));//d25
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x04000000, PinVector ("KEY_ROW2",  start_WD_cycle)));//d26
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x08000000, PinVector ("HDMI_SCL", start_WD_cycle)));//d27
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x10000000, PinVector ("HDMI_SDA", start_WD_cycle)));//d28
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x20000000, PinVector ("HDMI_CEC", start_WD_cycle)));//d29
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x40000000, PinVector ("HDMI_HPD",  start_WD_cycle)));//d30
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x80000000, PinVector ("AONGPIO31", start_WD_cycle)));//d31

         readAndExpectValueMapInitialized = true;
    }
    void initReadValueCycleMaps(){
            bool parametersValid = true;
            //TODO: check parametersValid in new generic implementation...
            assert(parametersValid);

            //data
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("GMII_CRS", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("GMII_MDC", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("GMII_MDIO",  start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("GMAC_125M_IN", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("GMAC_25M_OUT", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("UART0_RXD", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("UART0_TXD",  start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("SDIO0_CLK", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000100, PinVector ("SDIO0_CMD", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000200, PinVector ("SDIO0_DATA0", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000400, PinVector ("SDIO0_DATA1",  start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000800, PinVector ("SDIO0_DATA2", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00001000, PinVector ("SDIO0_DATA3", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00002000, PinVector ("SDIO0_CD", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00004000, PinVector ("SDIO0_WP",  start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00008000, PinVector ("SPDIF_OUT", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00010000, PinVector ("IR_IN", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00020000, PinVector ("SPI0_CLK", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00040000, PinVector ("SPI0_CS",  start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00080000, PinVector ("SPI0_TXD", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00100000, PinVector ("SPI0_RXD", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00200000, PinVector ("KEY_COL0", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00400000, PinVector ("KEY_COL1",  start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00800000, PinVector ("KEY_COL2", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x01000000, PinVector ("KEY_ROW0", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x02000000, PinVector ("KEY_ROW1", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x04000000, PinVector ("KEY_ROW2",  start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x08000000, PinVector ("HDMI_SCL", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x10000000, PinVector ("HDMI_SDA", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x20000000, PinVector ("HDMI_CEC", start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x40000000, PinVector ("HDMI_HPD",  start_WD_cycle)));//
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x80000000, PinVector ("AONGPIO31", start_WD_cycle)));//

            readCycleMapInitialized = true;
        }
};

#endif
