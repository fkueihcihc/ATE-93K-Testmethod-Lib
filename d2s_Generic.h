#ifndef D2S_GENERIC_H_
#define D2S_GENERIC_H_
#include "DeviceRegisterTransactionPort.h"

 
class d2s_Generic : public DeviceRegisterTransactionPort
{
protected:
    typedef std::pair<std::string, long long> PinVector;
    typedef PinVector PinCycle;
    typedef std::pair<long long, PinVector > BitvaluePinVector;
    typedef BitvaluePinVector BitvaluePinCycle;

    typedef std::multimap<unsigned long long, PinVector > bitvaluePinVector;
    typedef std::map<int, bitvaluePinVector > formatBitvaluePinVectorMapType;
    formatBitvaluePinVectorMapType writeAddressValueVectorMap;
    formatBitvaluePinVectorMapType writeDataValueVectorMap;
    formatBitvaluePinVectorMapType readAndExpectAddressValueVectorMap;
    formatBitvaluePinVectorMapType readAndExpectDataValueVectorMap;
    formatBitvaluePinVectorMapType readDataValueCycleMap;

    bool writeMapInitialized;
    bool readAndExpectValueMapInitialized;
    bool readCycleMapInitialized;
    int currentFormat;
    std::map<int, unsigned int> writeCycles;
    std::map<int, unsigned int> readCycles;
	
	d2s_Generic(std::string pName);
	virtual ~d2s_Generic();
    void prepareDynamicWriteLabel(long long address, long long data, const std::string& templatePatternName);
    void prepareDynamicReadOrExpectValueLabel(long long address, const std::string& labelName, long long value=0LL, long long mask=0LL);
    virtual long long readFromErrorMap(int cycleOffset);
    

    /*general parameters*/

    /**
     * @brief getAddressBits specifies how many bits are used to code the register address
     * 
     * This method has to return the number of address bits
     */        
    virtual int getAddressBits();

    /**
     * @brief getDataBits specifies how many data bits your device register has
     * 
     * This method has to return the number of data bits
     */            
    virtual int getDataBits();

    /**
     * @brief getPadding specifies how many padding vectors are specified in your templates
     * 
     * Default padding is 1.
     * 
     * This method has to return the number of padding vectors
     */      
    virtual int getPadding();
    //sophia modified it :adding a WR_mode to diff W or R @May 30-2012   virtual int getPadding(); 
    virtual int getPadding_R(); 

    /**
     * @brief getHighWaveformIndex specifies the waveform index of your wavetable that is used for driving a 1
     */         
    virtual int getHighWaveformIndex();

    /**
     * @brief getLowWaveformIndex specifies the waveform index of your wavetable that is used for driving a 0
     */         
    virtual int getLowWaveformIndex();        

    /**
     * @brief getHighWaveformIndex specifies the waveform index of your wavetable that is used for comparing for a 1
     */         
    virtual int getHighStrobeWaveformIndex();

    /**
     * @brief getLowWaveformIndex specifies the waveform index of your wavetable that is used for comparing for a 0
     */         
    virtual int getLowStrobeWaveformIndex();  
    
    /**
     * @brief getLowWaveformIndex specifies the waveform index of your wavetable that is used for comparing for a X
     */         
    virtual int getMaskStrobeWaveformIndex(); 
    

    /*write parameters*/
    
    virtual void initWriteValueVectorMaps();
    virtual int getWriteCycles();


    /*read parameters*/    

    virtual void initReadAndExpectValueVectorMaps();
    
    
    virtual void initReadValueCycleMaps();
    virtual int getReadCycles();
    
public:
    virtual void setFormat(int format);

};

#endif 
