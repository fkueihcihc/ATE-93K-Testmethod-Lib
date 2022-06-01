#ifndef D2SFRAMEWORK_H_
#define D2SFRAMEWORK_H_

#include <string>
#include <set>
#include "TransactionPort.h"
#include "d2sModes.h"


class d2sFramework
{
public:

	d2sFramework();
	virtual ~d2sFramework();
    
    void d2s_LABEL_BEGIN(std::string identifier, d2sFrameWorkModeType::Enum mode, d2sExecutionModeType::Enum execMode=d2sExecutionModeType::FUNCTIONAL_TST);
    void d2s_LABEL_END();
    virtual void setPrimaries();
    d2sFrameWorkModeType::Enum getCurrentMode();
    d2sExecutionModeType::Enum getCurrentExecutionMode();  
    bool registerTransactionPort(TransactionPort &port);
    
protected:
    virtual void setPrimariesAndExecuteStartLabel(std::string startLabel);

    typedef std::set<TransactionPort*> TransactionPortSet;
    typedef TransactionPortSet::iterator TransactionPortSetIter;
    typedef std::set<std::string> StringSet;
    TransactionPortSet registeredTransactionPorts;
    StringSet registeredPorts;
    d2sExecutionModeType::Enum currentExecutionMode;
    d2sFrameWorkModeType::Enum currentFrameworkMode;
    
private:
    void changeFrameworkMode(d2sFrameWorkModeType::Enum newMode);
    std::string getCurrentBurstName();  
    
    std::string currentIdentifier;  
};

#endif /*D2SFRAMEWORK_H_*/
