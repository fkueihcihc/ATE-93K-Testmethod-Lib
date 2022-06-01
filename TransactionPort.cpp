#include <iostream>
#include <assert.h>

#include "TransactionPort.h"
#include "PatternManager.h"
#include "FW.h"
#include "TestMethod.h"


using namespace std;


TransactionPort::sitePortIdPassFailType TransactionPort::globalPassFail; 

TransactionPort::TransactionPort(std::string pName)
{
    currentFrameworkMode = d2sFrameWorkModeType::DefaultMode;
    portName = pName;
    checkPassFail = false;
    inD2sBlock = false;
    //timingPeriod = 0.0;
}


TransactionPort::~TransactionPort()
{
}


void TransactionPort::setD2sBlockBegin(){
    if(inD2sBlock){
        cerr << "Error: TransactionPort on port " << portName << " - No nesting d2s-blocks allowed." << endl; 
        assert(!inD2sBlock);
    }
    inD2sBlock = true;        
}

void TransactionPort::setD2sBlockEnd(){
    inD2sBlock = false;    
}

std::string TransactionPort::getPortName(){
    return portName;
}


void TransactionPort::preExec(std::string labelName){
    //clearing the passFail cache only if testsuite name changed
    static string lastTestSuiteName = "";
    string testSuiteName;
    GET_TESTSUITE_NAME(testSuiteName);
    if(lastTestSuiteName != testSuiteName){
        lastTestSuiteName = testSuiteName;
        globalPassFail.clear();
    }

	if(currentFrameworkMode == d2sFrameWorkModeType::LearningMode){
        if(burstLabels.size() != 0){
            PatternManager::createBurst(labelName, burstLabels, portName);
        }
        else{
            //noting to create
            //delete Burst if it was available before
            if(PatternManager::isPatternAvailable(labelName, "MAIN", portName)) PatternManager::deleteLabel(labelName, portName);
        }
    }
}

void TransactionPort::setFrameworkMode(const d2sFrameWorkModeType::Enum mode){
    currentFrameworkMode = mode;
}


void TransactionPort::postExec(std::string executedId, bool executionWasSuccessful){
    if(checkPassFail){
        //store query focus
        string prevSite;
        fwout << "PQFC?" << endl;  
        prevSite = fwresult[0][0];
        
        //query pass/fail for port and store information
        FOR_EACH_SITE_BEGIN();
            int site = CURRENT_SITE_NUMBER();
            bool passFail = false;
            if(executionWasSuccessful){
				fwout << "PQFC " << site << endl;
				fwout << "PASS? (" << portName << ")" << endl; //would checking global pass/fail (for all ports) first be faster? [PRLT?]
				passFail = (fwresult[0][0]=="P")?true:false;
            }
            else{
            	passFail = false;
            }
            pair<map<string, bool>::iterator,bool> ret = globalPassFail[site][portName].insert(pair<string, bool> (executedId, passFail)); 
            if(ret.second == false){
                //replacing existing value from last execution. Maybe should change stl-map to stl-set? 
                globalPassFail[site][portName].erase(executedId);
                globalPassFail[site][portName].insert(pair<string, bool> (executedId, passFail));    
            }
            //cerr << "site " << site << ": result of " << executedId << " for port " << portName << " was " << passFail << endl; 
        FOR_EACH_SITE_END();
        //restore query focus
        fwout << "PQFC " << prevSite << endl;
    }
    
    checkPassFail = false;
    burstLabels.clear();
    cycleOffset = 0;
}

void TransactionPort::execLabel(std::string labelName, unsigned long long cycles){
    if(!inD2sBlock){
        cerr << "execLabel can only be used when inside a d2s_LABEL_BEGIN/END-block" << endl;
        assert(inD2sBlock);    
    }
    if(currentFrameworkMode != d2sFrameWorkModeType::ProductionMode){
        //do error checks only in Learning Mode
        if(!PatternManager::isPatternAvailable(labelName, "MAIN", portName)){
            cerr << "Error - execLabel: specificed label \"" << labelName << "\" does NOT exist for port \"" << portName << "\"!" << endl;
            cerr << "Skipping label from burst" << endl;
            return;
        }
        //check if specified cycles do match the cycles
        unsigned long long smarTestCalculatedCycles = PatternManager::getCyclesFromLabel(labelName, portName);
        if(smarTestCalculatedCycles != cycles){
            cerr << "Error - execLabel: specificed cycles don't match the label! Subsequent reads will not work correctly!" << endl;  
            cerr << "SmarTest calculated " << smarTestCalculatedCycles << " cycles for label \"" << labelName << "\"";
        }
    }
    
    cycleOffset += cycles;
    switch(currentFrameworkMode){
            case d2sFrameWorkModeType::LearningMode: 
                    burstLabels.push_back(labelName);
                    break;
            case d2sFrameWorkModeType::ProductionMode:
                    //will run the label later
                    break;
        default:
                cerr << "Error! cannot execLabel: wrong state" << endl;
                break;                     
    }        
}

void TransactionPort::wait(double timeInS){
	//TODO: Implement wait using wait-sequencer command!
	cerr << "wait using sequencer commands not yet implemented for TransactionPort. Cannot be used! Please use DeviceRegisterTransactionPort for the time being." << endl;
	assert(false);
}

void TransactionPort::enablePassFailCheckForNextExecution(){
    checkPassFail = true;
}

bool TransactionPort::hasPassed(std::string id){
    int site = CURRENT_SITE_NUMBER();
    std::map<std::string, bool>::const_iterator iter(globalPassFail[site][portName].find(id));
    if(iter != globalPassFail[site][portName].end()) return iter->second;
    else{
        cout << "ERROR: " << portName << ": hasPassed() of ID \"" << id << "\" not possible! Please switch on passFail-check using >>enablePassFailCheckForNextExecution()<< " << endl;
        return false;     
    }
}

unsigned long long TransactionPort::getCurrentCycleOffset(){
    return cycleOffset;    
}

double TransactionPort::getTimingPeriod(){
    FLUSH();
    fwout << "PCLK? PRM, PRM, (" << getTimingPort() << "), EXACT" << endl;
    if(fwresult.size() != 1){
        cerr << "ERROR couldn't evaluate timing period for timing-port: " << getTimingPort() << endl;
        return 0;
    }else{
        return fwresult[0].getDoubleParam(2);
    }
}

double TransactionPort::getCurrentTimeStamp(){
    return getTimingPeriod() * getCurrentCycleOffset();
}

string TransactionPort::getTimingPort(){
    return portName;
}
