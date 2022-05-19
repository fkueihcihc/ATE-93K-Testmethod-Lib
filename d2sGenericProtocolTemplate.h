#ifndef D2SGENERICPROTOCOLTEMPLATE_H_
#define D2SGENERICPROTOCOLTEMPLATE_H_
#include "d2s_Generic.h"
#include <map>

class d2sGenericProtocolTemplate : public d2s_Generic{
private:
  d2sGenericProtocolTemplate(std::string pName):d2s_Generic(pName){};
  d2sGenericProtocolTemplate(const d2sGenericProtocolTemplate&);
  d2sGenericProtocolTemplate& operator= (const d2sGenericProtocolTemplate&);

public:    
  //singleton
  static d2sGenericProtocolTemplate& Instance(){
    static d2sGenericProtocolTemplate instance("@");
    d2sGenericProtocolTemplate& ref = instance;
    return ref;
  }

  /*general parameters*/
  int getAddressBits(){ return 6;}
  int getDataBits(){ return 16;}
  int getLowWaveformIndex(){ return 0;}
  int getHighWaveformIndex(){ return 1;}

  int getLowStrobeWaveformIndex(){ return 2;}
  int getHighStrobeWaveformIndex(){ return 3;}
  int getMaskStrobeWaveformIndex(){ return 4;}

  std::string getInterfaceName(){ return "generic"; }
  int getPadding(){return 1;}

  /*write parameters*/
  std::string getWriteTemplatePatternName(){ return "d2s_simple_write_templ";}

  void initWriteValueVectorMaps(){

    //address
    writeAddressValueVectorMap[0x00000001] = std::pair<std::string, long long> ("di_A",   8);//a00
    writeAddressValueVectorMap[0x00000002] = std::pair<std::string, long long> ("di_A",  7);//a01
    writeAddressValueVectorMap[0x00000004] = std::pair<std::string, long long> ("di_A",  6);//a02
    writeAddressValueVectorMap[0x00000008] = std::pair<std::string, long long> ("di_A", 5);//a03
    writeAddressValueVectorMap[0x00000010] = std::pair<std::string, long long> ("di_A", 4);//a04
    writeAddressValueVectorMap[0x00000020] = std::pair<std::string, long long> ("di_A",  3);//a05

    //data
    writeDataValueVectorMap[0x00000001] = std::pair<std::string, long long> ("di_A",   24);//d00
    writeDataValueVectorMap[0x00000002] = std::pair<std::string, long long> ("di_A",  23);//d01
    writeDataValueVectorMap[0x00000004] = std::pair<std::string, long long> ("di_A",  22);//d02
    writeDataValueVectorMap[0x00000008] = std::pair<std::string, long long> ("di_A", 21);//d03
    writeDataValueVectorMap[0x00000010] = std::pair<std::string, long long> ("di_A", 20);//d04
    writeDataValueVectorMap[0x00000020] = std::pair<std::string, long long> ("di_A",  19);//d05
    writeDataValueVectorMap[0x00000040] = std::pair<std::string, long long> ("di_A",  18);//d06
    writeDataValueVectorMap[0x00000080] = std::pair<std::string, long long> ("di_A",  17);//d07

    writeDataValueVectorMap[0x00000100] = std::pair<std::string, long long> ("di_A",   16);//d08
    writeDataValueVectorMap[0x00000200] = std::pair<std::string, long long> ("di_A",  15);//d09
    writeDataValueVectorMap[0x00000400] = std::pair<std::string, long long> ("di_A",  14);//d10
    writeDataValueVectorMap[0x00000800] = std::pair<std::string, long long> ("di_A", 13);//d11
    writeDataValueVectorMap[0x00001000] = std::pair<std::string, long long> ("di_A", 12);//d12
    writeDataValueVectorMap[0x00002000] = std::pair<std::string, long long> ("di_A",  11);//d13
    writeDataValueVectorMap[0x00004000] = std::pair<std::string, long long> ("di_A",  10);//d14
    writeDataValueVectorMap[0x00008000] = std::pair<std::string, long long> ("di_A",  9);//d15
  }


  /*read parameters*/
  std::string getReadTemplatePatternName(){ return "d2s_simple_read_templ";}

  void initReadAndExpectValueVectorMaps(){

    //address
    readAndExpectAddressValueVectorMap[0x00000001] = std::pair<std::string, long long> ("di_A",   8);//a00
    readAndExpectAddressValueVectorMap[0x00000002] = std::pair<std::string, long long> ("di_A",  7);//a01
    readAndExpectAddressValueVectorMap[0x00000004] = std::pair<std::string, long long> ("di_A",  6);//a02
    readAndExpectAddressValueVectorMap[0x00000008] = std::pair<std::string, long long> ("di_A", 5);//a03
    readAndExpectAddressValueVectorMap[0x00000010] = std::pair<std::string, long long> ("di_A", 4);//a04
    readAndExpectAddressValueVectorMap[0x00000020] = std::pair<std::string, long long> ("di_A",  3);//a05

    //data
    readAndExpectDataValueVectorMap[0x00000001] = std::pair<std::string, long long> ("do_A",   24);//d00
    readAndExpectDataValueVectorMap[0x00000002] = std::pair<std::string, long long> ("do_A",  23);//d01
    readAndExpectDataValueVectorMap[0x00000004] = std::pair<std::string, long long> ("do_A",  22);//d02
    readAndExpectDataValueVectorMap[0x00000008] = std::pair<std::string, long long> ("do_A", 21);//d03
    readAndExpectDataValueVectorMap[0x00000010] = std::pair<std::string, long long> ("do_A", 20);//d04
    readAndExpectDataValueVectorMap[0x00000020] = std::pair<std::string, long long> ("do_A",  19);//d05
    readAndExpectDataValueVectorMap[0x00000040] = std::pair<std::string, long long> ("do_A",  18);//d06
    readAndExpectDataValueVectorMap[0x00000080] = std::pair<std::string, long long> ("do_A",  17);//d07

    readAndExpectDataValueVectorMap[0x00000100] = std::pair<std::string, long long> ("do_A",   16);//d08
    readAndExpectDataValueVectorMap[0x00000200] = std::pair<std::string, long long> ("do_A",  15);//d09
    readAndExpectDataValueVectorMap[0x00000400] = std::pair<std::string, long long> ("do_A",  14);//d10
    readAndExpectDataValueVectorMap[0x00000800] = std::pair<std::string, long long> ("do_A", 13);//d11
    readAndExpectDataValueVectorMap[0x00001000] = std::pair<std::string, long long> ("do_A", 12);//d12
    readAndExpectDataValueVectorMap[0x00002000] = std::pair<std::string, long long> ("do_A",  11);//d13
    readAndExpectDataValueVectorMap[0x00004000] = std::pair<std::string, long long> ("do_A",  10);//d14
    readAndExpectDataValueVectorMap[0x00008000] = std::pair<std::string, long long> ("do_A",  9);//d15
  }

  void initReadValueCycleMaps(){

    //data
    readDataValueCycleMap[0x00000001] = std::pair<std::string, long long> ("do_A",   24);//d00
    readDataValueCycleMap[0x00000002] = std::pair<std::string, long long> ("do_A",  23);//d01
    readDataValueCycleMap[0x00000004] = std::pair<std::string, long long> ("do_A",  22);//d02
    readDataValueCycleMap[0x00000008] = std::pair<std::string, long long> ("do_A", 21);//d03
    readDataValueCycleMap[0x00000010] = std::pair<std::string, long long> ("do_A", 20);//d04
    readDataValueCycleMap[0x00000020] = std::pair<std::string, long long> ("do_A",  19);//d05
    readDataValueCycleMap[0x00000040] = std::pair<std::string, long long> ("do_A",  18);//d06
    readDataValueCycleMap[0x00000080] = std::pair<std::string, long long> ("do_A",  17);//d07

    readDataValueCycleMap[0x00000100] = std::pair<std::string, long long> ("do_A",   16);//d08
    readDataValueCycleMap[0x00000200] = std::pair<std::string, long long> ("do_A",  15);//d09
    readDataValueCycleMap[0x00000400] = std::pair<std::string, long long> ("do_A",  14);//d10
    readDataValueCycleMap[0x00000800] = std::pair<std::string, long long> ("do_A", 13);//d11
    readDataValueCycleMap[0x00001000] = std::pair<std::string, long long> ("do_A", 12);//d12
    readDataValueCycleMap[0x00002000] = std::pair<std::string, long long> ("do_A",  11);//d13
    readDataValueCycleMap[0x00004000] = std::pair<std::string, long long> ("do_A",  10);//d14
    readDataValueCycleMap[0x00008000] = std::pair<std::string, long long> ("do_A",  9);//d15
  }


  /*wait parameters*/
  std::string getTimingPort(){return "@";}
  std::string getWaitTemplatePatternName(){ return "d2s_simple_wait_templ";}
};

#endif
