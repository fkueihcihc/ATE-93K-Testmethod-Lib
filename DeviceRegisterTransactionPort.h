#ifndef DEVICEREGISTERTRANSACTIONPORT_H_
#define DEVICEREGISTERTRANSACTIONPORT_H_
#include "TransactionPort.h"
#include <set>
#define D2S_NO_DCSCALE
#ifndef D2S_NO_DCSCALE
    #include "Event.h"
#endif

class DeviceRegisterTransactionPort : public TransactionPort
{
public:
	DeviceRegisterTransactionPort(std::string pName);
	virtual ~DeviceRegisterTransactionPort();
    
    virtual void preExec(std::string burstName);
    virtual void postExec(std::string executedId);    

    void write(long long address, long long data);  
    void writeDynamic(long long address, long long data, std::string id);

    void read(long long address, std::string id);
    long long getReadValue(std::string id);
    
    void expectValue(long long address, long long value, long long mask=0LL);

    void wait(double timeInS);

    void ignoreWarnings(bool newValue) { ignoreWarning = newValue; }

#ifndef D2S_NO_DCSCALE
    void dcScaleForceVoltage(std::string pinList, double voltValue);
    void dcScaleForceVoltageMeasureVoltage(std::string pinList, int samples, double voltValue);
    void dcScaleForceVoltageMeasureVoltage(std::string pinList, int samples, const double limitLow, const double limitHigh, double voltValue);    
    void dcScaleForceVoltageMeasureCurrent(std::string pinList, int samples, double voltValue);
    void dcScaleForceVoltageMeasureCurrent(std::string pinList, int samples, const double limitLow, const double limitHigh, double voltValue);
    void dcScaleMeasureVoltage(std::string pinList, int samples);
    void dcScaleMeasureVoltage(std::string pinList, int samples, const double limitLow, const double limitHigh);    
    void dcScaleMeasureCurrent(std::string pinList, int samples);
    void dcScaleMeasureCurrent(std::string pinList, int samples, const double limitLow, const double limitHigh);
    void dcScaleSetMeasureRange(std::string pinList, const DC_SCALE::SET_MEAS_RNG setMeasRng, const double minValue, const double maxValue);
#endif    

protected:  
	
    /**
     * @brief getInterfaceName is a pure virtual method. This method defines the name of the device protocol.
     * 
     * This method needs to be implemented in the inherited port specific class. It defines how the labels are named for the framework.
     */ 
    virtual std::string getInterfaceName() = 0;

    virtual void prepareDynamicWriteLabel(long long address, long long data, const std::string& labelName);
    
    /**
     * @brief getWriteTemplatePatternName specifies the label name of your write template label
     */       
    virtual std::string getWriteTemplatePatternName();
    
    virtual std::string getNewWriteLabelName(long long address, long long data); 
    virtual std::string getNewWriteDynamicLabelName(long long address, long long data, std::string id);
    virtual std::string getNewReadLabelName(long long address, std::string id);    

    
    virtual void prepareDynamicReadOrExpectValueLabel(long long address, const std::string& labelName, long long value=0LL, long long mask=0LL);
    
    /**
     * @brief getReadTemplatePatternName specifies the label name of the read-template
     * 
     */
    virtual std::string getReadTemplatePatternName(); 
    virtual int getReadCycles();
    virtual long long readFromErrorMap(int cycleOffset);


    virtual void prepareDynamicWaitLabel(unsigned int cycles);
    virtual std::string getWaitTemplatePatternName();
    virtual std::string getTimingPort();  
    
#ifndef D2S_NO_DCSCALE
    virtual std::string getDCScaleTemplatePatternName();
#endif    
       
    
    virtual int getWriteCycles();
    
    void waitModeLearning(double timeInS);
    void checkWrite();
    
    std::string getWaitLabelName(double timeInS);
    unsigned int calculateWaitCycles(double timeInS); 
    
    typedef std::map<std::string, int> idOffsetType;
    idOffsetType readOffsets;  
    typedef std::map<std::string, std::map<int, std::map<std::string, long long> > > portSiteIdValueType;
    static portSiteIdValueType readIdValues;
    void evalutateReads();
    void readAndStoreValues(std::string id, int offset=0);  
    void addReadLabelOffset(std::string id, int cycleOffset);
    void readModeLearning(long long address, std::string id);  
    void expectValueModeLearning(long long address, long long value, long long mask);
    void checkReadAndExpectValue();    

#ifndef D2S_NO_DCSCALE        
    void checkDcScale();
    virtual int getDcScaleCycles();
    void dcScaleForceVoltageLearninMode(std::string pinList, double voltValue);
    void dcScaleMeasure(std::string measureType, std::string pinList, int samples, const double limitLow, const double limitHigh, bool ignoreLimit, bool doForceVoltage=false, double voltValue=0.0);
    void dcScaleMeasureLearningMode(std::string measureType, std::string pinList, int samples, const double limitLow, const double limitHigh, bool ignoreLimit, bool doForceVoltage=false, double voltValue=0.0);
    void dcScaleSetMeasureRangeLearningMode(std::string pinList, const DC_SCALE::SET_MEAS_RNG setMeasRng, const double minValue, const double maxValue);
#endif    
        
    int getTimingPeriod();
    int timingPeriod;
    unsigned int readCycles;
    unsigned int writeCycles;
#ifndef D2S_NO_DCSCALE    
    unsigned int dcScaleCycles;
#endif    
    bool writeChecked;
    bool readAndExpectValueChecked;
    bool waitChecked;
    bool ignoreWarning;
#ifndef D2S_NO_DCSCALE    
    bool dcScaleChecked;
#endif    
    
    std::set<std::string> writeDynamicIDs;   
    std::string getLabelNamePrefix();
};

#endif /*DEVICEREGISTERTRANSACTIONPORT_H_*/
