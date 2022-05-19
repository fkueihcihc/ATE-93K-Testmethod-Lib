
#include "generic_d2s/d2s_Generic.h"
#include <map>
#define start_cycle 1
#define start_R_cycle 3
#define start_WA_cycle 0
#define start_WD_cycle 1

class d2s_SPUART_r : public d2s_Generic{
private:
	d2s_SPUART_r(std::string pName):d2s_Generic(pName){};
	d2s_SPUART_r(const d2s_SPUART_r&);
	d2s_SPUART_r& operator= (const d2s_SPUART_r&);

public:
    //singleton
    static d2s_SPUART_r& Instance(){
        static d2s_SPUART_r instance("@");
        d2s_SPUART_r& ref = instance;
        return ref;
    }

    /*general parameters*/

    int getAddressBits(){ return 8;}
    int getDataBits(){ return 32;}

    int getLowWaveformIndex(){ return 0;}
    int getHighWaveformIndex(){ return 1;}
    int getLowStrobeWaveformIndex(){ return 3;}
    int getHighStrobeWaveformIndex(){ return 4;}
    int getMaskStrobeWaveformIndex(){ return 6;}
    int getPadding(){return 1;}

    std::string getInterfaceName(){return "spuart_r";}

    /*read parameters*/
    std::string getReadTemplatePatternName(){return"spuart_read_template";}

    void initReadAndExpectValueVectorMaps(){
        bool parametersValid = true;
        //TODO: check parametersValid in new generic implementation...
        assert(parametersValid);

        //address:8bits
        readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("INV_CLKO",  start_WA_cycle)));//a00
        readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("P_CORE_EN",   start_WA_cycle)));//a01
        readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("BI_CLKO", start_WA_cycle)));//a02
        readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("CI_NRSTO",  start_WA_cycle)));//a03

        readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("INV_CLKO",  start_WA_cycle+1)));//a04
        readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("P_CORE_EN",   start_WA_cycle+1)));//a05
        readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("BI_CLKO", start_WA_cycle+1)));//a06
        readAndExpectAddressValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("CI_NRSTO",  start_WA_cycle+1)));//a07


        //data:32bits
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("RO_RI", start_R_cycle)));//d00
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("RO_RI",  start_R_cycle+1)));//d01
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("RO_RI",start_R_cycle+2)));//d02
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("RO_RI", start_R_cycle+3)));//d03
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("RO_RI", start_R_cycle+4)));//d04
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("RO_RI",  start_R_cycle+5)));//d05
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("RO_RI",start_R_cycle+6)));//d06
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("RO_RI", start_R_cycle+7)));//d07

        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000100, PinVector ("RO_RI", start_R_cycle+1+8)));//d08
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000200, PinVector ("RO_RI",  start_R_cycle+1+9)));//d09
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000400, PinVector ("RO_RI",start_R_cycle+1+10)));//d10
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00000800, PinVector ("RO_RI", start_R_cycle+1+11)));//d11
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00001000, PinVector ("RO_RI", start_R_cycle+1+12)));//d12
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00002000, PinVector ("RO_RI",  start_R_cycle+1+13)));//d13
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00004000, PinVector ("RO_RI",start_R_cycle+1+14)));//d14
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00008000, PinVector ("RO_RI", start_R_cycle+1+15)));//d15

        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00010000, PinVector ("RO_RI", start_R_cycle+2+16)));//d16
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00020000, PinVector ("RO_RI",  start_R_cycle+2+17)));//d17
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00040000, PinVector ("RO_RI",start_R_cycle+2+18)));//d18
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00080000, PinVector ("RO_RI", start_R_cycle+2+19)));//d19
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00100000, PinVector ("RO_RI", start_R_cycle+2+20)));//d20
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00200000, PinVector ("RO_RI",  start_R_cycle+2+21)));//d21
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00400000, PinVector ("RO_RI",start_R_cycle+2+22)));//d22
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x00800000, PinVector ("RO_RI", start_R_cycle+2+23)));//d23

        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x01000000, PinVector ("RO_RI", start_R_cycle+3+24)));//d24
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x02000000, PinVector ("RO_RI",  start_R_cycle+3+25)));//d25
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x04000000, PinVector ("RO_RI",start_R_cycle+3+26)));//d26
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x08000000, PinVector ("RO_RI", start_R_cycle+3+27)));//d27
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x10000000, PinVector ("RO_RI", start_R_cycle+3+28)));//d28
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x20000000, PinVector ("RO_RI",  start_R_cycle+3+29)));//d29
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x40000000, PinVector ("RO_RI",start_R_cycle+3+30)));//d30
        readAndExpectDataValueVectorMap[0].insert(BitvaluePinVector (0x80000000, PinVector ("RO_RI", start_R_cycle+3+31)));//d31


        readAndExpectValueMapInitialized = true;
    }

    void initReadValueCycleMaps(){
        bool parametersValid = true;
        //TODO: check parametersValid in new generic implementation...
        assert(parametersValid);

        //data:32bits
        readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000001, PinVector ("RO_RI", start_R_cycle)));//d00
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000002, PinVector ("RO_RI",  start_R_cycle+1)));//d01
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000004, PinVector ("RO_RI",start_R_cycle+2)));//d02
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000008, PinVector ("RO_RI", start_R_cycle+3)));//d03
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000010, PinVector ("RO_RI", start_R_cycle+4)));//d04
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000020, PinVector ("RO_RI",  start_R_cycle+5)));//d05
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000040, PinVector ("RO_RI",start_R_cycle+6)));//d06
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000080, PinVector ("RO_RI", start_R_cycle+7)));//d07

                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000100, PinVector ("RO_RI", start_R_cycle+1+8)));//d08
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000200, PinVector ("RO_RI",  start_R_cycle+1+9)));//d09
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000400, PinVector ("RO_RI",start_R_cycle+1+10)));//d10
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00000800, PinVector ("RO_RI", start_R_cycle+1+11)));//d11
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00001000, PinVector ("RO_RI", start_R_cycle+1+12)));//d12
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00002000, PinVector ("RO_RI",  start_R_cycle+1+13)));//d13
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00004000, PinVector ("RO_RI",start_R_cycle+1+14)));//d14
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00008000, PinVector ("RO_RI", start_R_cycle+1+15)));//d15

                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00010000, PinVector ("RO_RI", start_R_cycle+2+16)));//d16
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00020000, PinVector ("RO_RI",  start_R_cycle+2+17)));//d17
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00040000, PinVector ("RO_RI",start_R_cycle+2+18)));//d18
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00080000, PinVector ("RO_RI", start_R_cycle+2+19)));//d19
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00100000, PinVector ("RO_RI", start_R_cycle+2+20)));//d20
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00200000, PinVector ("RO_RI",  start_R_cycle+2+21)));//d21
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00400000, PinVector ("RO_RI",start_R_cycle+2+22)));//d22
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x00800000, PinVector ("RO_RI", start_R_cycle+2+23)));//d23

                readDataValueCycleMap[0].insert(BitvaluePinVector (0x01000000, PinVector ("RO_RI", start_R_cycle+3+24)));//d24
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x02000000, PinVector ("RO_RI",  start_R_cycle+3+25)));//d25
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x04000000, PinVector ("RO_RI",start_R_cycle+3+26)));//d26
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x08000000, PinVector ("RO_RI", start_R_cycle+3+27)));//d27
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x10000000, PinVector ("RO_RI", start_R_cycle+3+28)));//d28
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x20000000, PinVector ("RO_RI",  start_R_cycle+3+29)));//d29
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x40000000, PinVector ("RO_RI",start_R_cycle+3+30)));//d30
                readDataValueCycleMap[0].insert(BitvaluePinVector (0x80000000, PinVector ("RO_RI", start_R_cycle+3+31)));//d31


        readCycleMapInitialized = true;
    }
};

