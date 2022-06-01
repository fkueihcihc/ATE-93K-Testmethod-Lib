#ifndef D2S_SERIAL_H_
#define D2S_SERIAL_H_
#include "DeviceRegisterTransactionPort.h"

 
class d2s_Serial : public DeviceRegisterTransactionPort
{
private:
    typedef std::map<unsigned long long, int> valueCycleMapType;
    valueCycleMapType writeAddressValueVectorMap;
    valueCycleMapType writeDataValueVectorMap;
    valueCycleMapType readAndExpectAddressValueVectorMap;
    valueCycleMapType readAndExpectDataValueVectorMap;    
    
    bool writeMapInitialized;
    bool readAndExpectValueMapInitialized;
        
    void initWriteValueVectorMaps();
    void initReadAndExpectValueVectorMaps();
    
protected:
	d2s_Serial(std::string pName);
	virtual ~d2s_Serial();
    void prepareDynamicWriteLabel(long long address, long long data, const std::string& patternName);
    void prepareDynamicReadOrExpectValueLabel(long long address, const std::string& labelName,long long value=0LL, long long mask=0LL);
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

    /**
     * @brief getWritePinName specifies the pinname that drives the address and data.
     */            
    virtual std::string getWritePinName();

    
    /**
     * @brief getWriteAddressVectorNumberLSB specifies the vector number that contains the least significant ADDRESS bit (LSB).
     * 
     * In case of padding !=1 this is the first vector number before additional padding vectors
     */     
    virtual int getWriteAddressVectorNumberLSB();
    
    /**
     * @brief getWriteAddressVectorNumberMSB specifies the vector number that contains the most significant ADDRESS bit (MSB).
     * 
     * In case of padding !=1 this is the first vector number before additional padding vectors
     */
    virtual int getWriteAddressVectorNumberMSB();
    
    /**
     * @brief getWriteDataVectorNumberLSB specifies the vector number that contains the least significant bit DATA (LSB) bit.
     * 
     * In case of padding !=1 this is the first vector number before additional padding vectors
     */    
    virtual int getWriteDataVectorNumberLSB();
    
    /**
     * @brief getWriteDataVectorNumberMSB specifies the vector number that contains the most significant DATA bit (MSB).
     * 
     * In case of padding !=1 this is the first vector number before additional padding vectors
     */
    virtual int getWriteDataVectorNumberMSB();


    /*read parameters*/    

    /**
     * @brief getWriteAddressPinName specifies the pinname that drives the address.
     */
    virtual std::string getReadAddressPinName();
    
    /**
     * @brief getReadPinName specifies the output pinname that contains the data
     */
    virtual std::string getReadPinName();    
    
    /**
     * @brief getReadAddressVectorNumberLSB specifies the vector number that contains the least significant ADDRESS bit (LSB).
     * 
     * In case of padding !=1 this is the first vector number before additional padding vectors
     */   
    virtual int getReadAddressVectorNumberLSB();
    
    /**
     * @brief getReadAddressVectorNumberMSB specifies the vector number that contains the most significant ADDRESS bit (MSB).
     * 
     * In case of padding !=1 this is the first vector number before additional padding vectors
     */       
    virtual int getReadAddressVectorNumberMSB();    

    /**
     * @brief getReadDataCycleNumberLSB specifies the cycle number that contains the least significant ADDRESS bit (LSB).
     * 
     * In case of padding !=1 this is the first cycle number before additional padding vectors
     */       
    virtual int getReadDataCycleNumberLSB();

    /**
     * @brief getReadDataCycleNumberLSB specifies the cycle number that contains the most significant ADDRESS bit (MSB).
     * 
     * In case of padding !=1 this is the first cycle number before additional padding vectors
     */           
    virtual int getReadDataCycleNumberMSB();  
    
    virtual int getReadDataVectorNumberLSB();
    
    virtual int getReadDataVectorNumberMSB();
      
};

#endif 
