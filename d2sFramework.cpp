#include <iostream> 
#include "TestMethod.h"
#include "d2sFramework.h"
#include "PortChecker.h"
#include "PatternManager.h"
#include "FW.h"


using namespace std;

d2sFramework::d2sFramework()
{
    currentExecutionMode = d2sExecutionModeType::FUNCTIONAL_TST;
    changeFrameworkMode(d2sFrameWorkModeType::DefaultMode);
	currentIdentifier = "";
}

d2sFramework::~d2sFramework()
{
}

string d2sFramework::getCurrentBurstName(){
    return  "d2s_" + currentIdentifier;
}

void d2sFramework::d2s_LABEL_BEGIN(std::string identifier, d2sFrameWorkModeType::Enum mode, d2sExecutionModeType::Enum execMode){
    if(currentIdentifier != ""){
		cerr << "ERROR: a d2s_LABEL_BEGIN-block with the following identifier is still open: " << currentIdentifier << endl;
		cerr << "Please check your code to close using d2s_LABEL_END() before entering the block with the identifier: " << identifier << endl;
    }

    //update registered TransactionPorts on block begin
    for(TransactionPortSetIter it = registeredTransactionPorts.begin(); it != registeredTransactionPorts.end(); it++){
        (*it)->setD2sBlockBegin();
    }
    currentIdentifier = identifier;
    currentExecutionMode = execMode;
    changeFrameworkMode(mode);
}

void d2sFramework::d2s_LABEL_END(){
	bool executionWasSuccessful = false;
    //inform TransactionPorts about preExec
    for(TransactionPortSetIter it = registeredTransactionPorts.begin(); it != registeredTransactionPorts.end(); it++){
        (*it)->preExec(getCurrentBurstName());
    }
    
    //execBurst
    if(currentFrameworkMode != d2sFrameWorkModeType::ProductionMode){
        //skip test in ProductionMode for Troughput
        if(!PatternManager::isPatternAvailable(getCurrentBurstName())){
            cerr << "Burst doesn't contain any port-labels. Cannot execute Burst for \"" << currentIdentifier << "\"" << endl;    
        }
        else {
        	executionWasSuccessful = setPrimariesAndExecuteStartLabel(getCurrentBurstName());
        }
    }
    else {
    	executionWasSuccessful = setPrimariesAndExecuteStartLabel(getCurrentBurstName());
    }
    
    //inform TransactionPorts about postExec
    for(TransactionPortSetIter it = registeredTransactionPorts.begin(); it != registeredTransactionPorts.end(); it++){
        (*it)->postExec(currentIdentifier, executionWasSuccessful);
    }
    
    //reset states
    currentIdentifier = "";  
    currentExecutionMode = d2sExecutionModeType::FUNCTIONAL_TST;
    changeFrameworkMode(d2sFrameWorkModeType::DefaultMode);
    
    //update registered TransactionPorts on block end
    for(TransactionPortSetIter it = registeredTransactionPorts.begin(); it != registeredTransactionPorts.end(); it++){
        (*it)->setD2sBlockEnd();
    }    
}

void d2sFramework::setPrimaries(){
    
}

void d2sFramework::synchronizePorts(){
	//synch with all registered ports
	synchronizePorts(registeredTransactionPorts);
}

void d2sFramework::synchronizePorts(TransactionPort &port1, TransactionPort &port2){
	TransactionPortSet portsToSync;
	portsToSync.insert(&port1);
	portsToSync.insert(&port2);
	synchronizePorts(portsToSync);
}
void d2sFramework::synchronizePorts(TransactionPort &port1, TransactionPort &port2, TransactionPort &port3){
	TransactionPortSet portsToSync;
	portsToSync.insert(&port1);
	portsToSync.insert(&port2);
	portsToSync.insert(&port3);
	synchronizePorts(portsToSync);
}
void d2sFramework::synchronizePorts(TransactionPort &port1, TransactionPort &port2, TransactionPort &port3, TransactionPort &port4){
	TransactionPortSet portsToSync;
	portsToSync.insert(&port1);
	portsToSync.insert(&port2);
	portsToSync.insert(&port3);
	portsToSync.insert(&port4);
	synchronizePorts(portsToSync);
}
void d2sFramework::synchronizePorts(TransactionPort &port1, TransactionPort &port2, TransactionPort &port3, TransactionPort &port4, TransactionPort &port5){
	TransactionPortSet portsToSync;
	portsToSync.insert(&port1);
	portsToSync.insert(&port2);
	portsToSync.insert(&port3);
	portsToSync.insert(&port4);
	portsToSync.insert(&port5);
	synchronizePorts(portsToSync);
}
void d2sFramework::synchronizePorts(TransactionPort &port1, TransactionPort &port2, TransactionPort &port3, TransactionPort &port4, TransactionPort &port5, TransactionPort &port6){
	TransactionPortSet portsToSync;
	portsToSync.insert(&port1);
	portsToSync.insert(&port2);
	portsToSync.insert(&port3);
	portsToSync.insert(&port4);
	portsToSync.insert(&port5);
	portsToSync.insert(&port6);
	synchronizePorts(portsToSync);
}
void d2sFramework::synchronizePorts(TransactionPort &port1, TransactionPort &port2, TransactionPort &port3, TransactionPort &port4, TransactionPort &port5, TransactionPort &port6, TransactionPort &port7){
	TransactionPortSet portsToSync;
	portsToSync.insert(&port1);
	portsToSync.insert(&port2);
	portsToSync.insert(&port3);
	portsToSync.insert(&port4);
	portsToSync.insert(&port5);
	portsToSync.insert(&port6);
	portsToSync.insert(&port7);
	synchronizePorts(portsToSync);
}
void d2sFramework::synchronizePorts(TransactionPort &port1, TransactionPort &port2, TransactionPort &port3, TransactionPort &port4, TransactionPort &port5, TransactionPort &port6, TransactionPort &port7, TransactionPort &port8){
	TransactionPortSet portsToSync;
	portsToSync.insert(&port1);
	portsToSync.insert(&port2);
	portsToSync.insert(&port3);
	portsToSync.insert(&port4);
	portsToSync.insert(&port5);
	portsToSync.insert(&port6);
	portsToSync.insert(&port7);
	portsToSync.insert(&port8);
	synchronizePorts(portsToSync);
}

void d2sFramework::synchronizePorts(TransactionPortSet selectedPortsToSync){
	double maxElapsedTime = 0.0;
	//find longest port
	for(TransactionPortSetIter it = registeredTransactionPorts.begin(); it != registeredTransactionPorts.end(); it++){
		double curElapsedTime = (*it)->getCurrentTimeStamp();

		if(curElapsedTime > maxElapsedTime) maxElapsedTime = curElapsedTime;
	}

	//adjust other ports to longest port
	for(TransactionPortSetIter it = registeredTransactionPorts.begin(); it != registeredTransactionPorts.end(); it++){
		double curElapsedTime = (*it)->getCurrentTimeStamp();
		if((maxElapsedTime - curElapsedTime) > 0){
			(*it)->wait((maxElapsedTime - curElapsedTime) ns);//wait is in seconds
		}
	}

}

void d2sFramework::changeFrameworkMode(d2sFrameWorkModeType::Enum newMode){
    currentFrameworkMode = newMode;
    
    //update registered TransactionPorts on mode change
    for(TransactionPortSetIter it = registeredTransactionPorts.begin(); it != registeredTransactionPorts.end(); it++){
        (*it)->setFrameworkMode(currentFrameworkMode);
    }
}

d2sFrameWorkModeType::Enum d2sFramework::getCurrentMode(){
    return currentFrameworkMode;
}

d2sExecutionModeType::Enum d2sFramework::getCurrentExecutionMode(){
    return currentExecutionMode;
}  

bool d2sFramework::registerTransactionPort(TransactionPort &portToAdd){
    PortChecker &portChecker = PortChecker::Instance();
    
    //add new Port
    string strPortToAdd = portToAdd.getPortName();
    pair<set<string>::iterator, bool>  ret = registeredPorts.insert(strPortToAdd);
    if(ret.second == false) {
    	cerr << "registerTransactionPort(): cannot register Port \"" << strPortToAdd << "\". Port with the same name already registered." << endl;
    	return false;
    }
    
    if(portChecker.arePortsIntersecting(registeredPorts)){
        cerr << "registerTransactionPort(): cannot register Port! Port has common pins with already registered ports and so they cannot be used together!" << endl;
        registeredPorts.erase(strPortToAdd);
        return false;
    }
    else{
        registeredTransactionPorts.insert(&portToAdd);
        return true;    
    }
}
   

bool d2sFramework::setPrimariesAndExecuteStartLabel(std::string startLabel){

	if(currentExecutionMode != d2sExecutionModeType::NONE){
		try{
			setPrimaries();
			Primary.label(startLabel);
			switch(currentExecutionMode){
			case d2sExecutionModeType::FUNCTIONAL_TST:
				FUNCTIONAL_TEST();
				break;
			case d2sExecutionModeType::EXECUTE_TST:
				EXECUTE_TEST();
				break;
			case d2sExecutionModeType::DIGITAL_CAPTURE_TST:
				DIGITAL_CAPTURE_TEST();
				break;
			case d2sExecutionModeType::SMC_FUNCTIONAL_TST:
				SMC_FUNCTIONAL_TEST();
				break;
			case d2sExecutionModeType::SMC_EXECUTE_TST:
				SMC_EXECUTE_TEST();
				break;
			case d2sExecutionModeType::SMC_DIGITAL_CAPTURE_TST:
				SMC_DIGITAL_CAPTURE_TEST();
				break;
			default:
				cerr << "execMode not known. Please check your execMode!" << endl;
			}
		}
		catch(...){
			string testSuiteName;
			GET_TESTSUITE_NAME(testSuiteName);
			cerr << "Setting and Executing label \"" << startLabel << "\" in test suite \"" << testSuiteName << "\" wasn't possible!" << endl;
			cerr << "Please check d2s-block: \"" << currentIdentifier << "\"" << endl;
			return false;
		}
		return true;
	}
	else{
		return false;
	}
}
