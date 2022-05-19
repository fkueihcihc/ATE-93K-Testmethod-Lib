/*
 * d2s_efuse_r.h
 *
 *  Created on: Mar 4, 2017
 *      Author: liaorundong
 */

#include "../d2s_tml/generic_d2s/d2s_Generic.h"
#include <map>
#define start_RA_vector 3


#define start_RD_vector 3
#define start_RD_cycle 3


class d2s_APB_r : public d2s_Generic{
private:
	d2s_APB_r(std::string pName):d2s_Generic(pName){};
	d2s_APB_r(const d2s_APB_r&);
	d2s_APB_r& operator= (const d2s_APB_r&);

public:
    //singleton
    static d2s_APB_r& Instance(){
        static d2s_APB_r instance("@");
        d2s_APB_r& ref = instance;
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
    int getPadding(){return 1;}   //4  keep?
    std::string getInterfaceName(){return "gen_Parallel";}


    /*read parameters*/
    std::string getReadTemplatePatternName(){return"APB_read_templete";}

    void initReadAndExpectValueVectorMaps(){
        bool parametersValid = true;
         //TODO: check parametersValid in new generic implementation...
         assert(parametersValid);

         //address
         //data_depth: addr 0~31
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("SDIO1_DATA2", start_RA_vector)));//a00  //modify start_RA_cycle to start_RA_vector
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("SDIO1_DATA3", start_RA_vector)));//a01
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("SDIO1_CD", start_RA_vector)));//a02
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("AONGPIO30", start_RA_vector)));//a03
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("AONGPIO24", start_RA_vector)));//a04
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("AONGPIO25", start_RA_vector)));//a05
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("SDIO0_CLK", start_RA_vector)));//a06
         readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("SDIO0_CMD", start_RA_vector)));//a07



         //data

         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("SDIO0_DATA2", start_RD_vector)));//q0
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("SDIO0_DATA3", start_RD_vector)));//q01
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("SDIO0_CD",  start_RD_vector)));//q02
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("SPDIF_OUT", start_RD_vector)));//q03
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("IR_IN",     start_RD_vector)));//q04
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("AONGPIO5", start_RD_vector)));//q05
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("AONGPIO6",  start_RD_vector)));//q06
         readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("AONGPIO7",  start_RD_vector)));//q07




         readAndExpectValueMapInitialized = true;
    }
    void initReadValueCycleMaps(){
            bool parametersValid = true;
            //TODO: check parametersValid in new generic implementation...
            assert(parametersValid);

            //data

            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("SDIO0_DATA2", start_RD_cycle)));//q0
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("SDIO0_DATA3", start_RD_cycle)));//q01
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("SDIO0_CD",  start_RD_cycle)));//q02
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("SPDIF_OUT", start_RD_cycle)));//q03
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("IR_IN", start_RD_cycle)));//q04
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("AONGPIO5", start_RD_cycle)));//q05
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("AONGPIO6",  start_RD_cycle)));//q06
            readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("AONGPIO7",  start_RD_cycle)));//q07



            readCycleMapInitialized = true;
        }
};
