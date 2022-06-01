#ifndef TRANSACTIONPORT_H_
#define TRANSACTIONPORT_H_

#include <string>
#include <vector>
#include <map>
#include "d2sModes.h"

class d2sFramework;

class TransactionPort
{
public:
	TransactionPort(std::string portName);
	virtual ~TransactionPort();
    std::string getPortName();
    
    void setFrameworkMode(const d2sFrameWorkModeType::Enum mode);
    virtual void preExec(std::string burstName);
    virtual void postExec(std::string executedId);
    
    virtual void execLabel(std::string labelName, unsigned long long cycles=0);
    void enablePassFailCheckForNextExecution();
    bool hasPassed(std::string id);
    void setD2sBlockBegin();
    void setD2sBlockEnd();
    unsigned long long getCurrentCycleOffset();
    
protected:
    d2sFrameWorkModeType::Enum currentFrameworkMode;
    std::vector<std::string> burstLabels;    
    unsigned long long cycleOffset;
    std::string portName;
    bool inD2sBlock;
    
    bool checkPassFail;
    typedef std::map<int, std::map<std::string, std::map<std::string, bool> > > sitePortIdPassFailType;
    static sitePortIdPassFailType globalPassFail;        
            
private:
    
    
};

#endif /*TRANSACTIONPORT_H_*/
