#include "DeviceRegisterTransactionPort.h"
#include "FW.h"
#include "TestMethod.h"
#include <assert.h>
#include "PatternManager.h"


#define MAX_WAIT_CYCLES 16777208 //=(2^24-8) <- max repetition of RPTV

DeviceRegisterTransactionPort::portSiteIdValueType DeviceRegisterTransactionPort::readIdValues; 

DeviceRegisterTransactionPort::DeviceRegisterTransactionPort(std::string pName):TransactionPort(pName)
{
    readOffsets.clear();
    writeChecked = false;
    readAndExpectValueChecked = false;
    waitChecked = false;
#ifndef D2S_NO_DCSCALE    
    dcScaleCycles = 0;
    dcScaleChecked = false; 
#endif    
    
    
}

DeviceRegisterTransactionPort::~DeviceRegisterTransactionPort()
{
}

std::string DeviceRegisterTransactionPort::getWriteTemplatePatternName(){
    cerr << "ERROR in framework: getWriteTemplatePatternName needs to be overridden!" << endl;
    bool notOverridden = false;
    assert(notOverridden);
    return "";    
}

void DeviceRegisterTransactionPort::prepareDynamicWriteLabel(long long address, long long data, const std::string& labelName){
    cerr << "ERROR in framework: prepareDynamicWriteLabel needs to be overridden! Cannot modify write label." << endl;
    bool notOverridden = false;
    assert(notOverridden);    
}

std::string DeviceRegisterTransactionPort::getReadTemplatePatternName(){
    cerr << "ERROR in framework: getReadTemplatePatternName needs to be overridden!" << endl;
    bool notOverridden = false;
    assert(notOverridden);    
    return "";
}

void DeviceRegisterTransactionPort::prepareDynamicReadOrExpectValueLabel(long long address, const std::string& labelName, long long value, long long mask){
    cerr << "ERROR in framework: prepareDynamicReadOrExpectValueLabel needs to be overridden! Cannot modify read/expected-value label." << endl;
    bool notOverridden = false;
    assert(notOverridden);        
}

long long DeviceRegisterTransactionPort::readFromErrorMap(int cycleOffset){
    cerr << "ERROR in framework: readFromErrorMap needs to be overridden! Cannot read from error map." << endl;
    bool notOverridden = false;
    assert(notOverridden);    
    return 0;
}

std::string DeviceRegisterTransactionPort::getWaitTemplatePatternName(){
    cerr << "ERROR in framework: getWaitTemplatePatternName needs to be overridden!" << endl;
    bool notOverridden = false;
    assert(notOverridden);    
    return "";        
}

#ifndef D2S_NO_DCSCALE
std::string DeviceRegisterTransactionPort::getDCScaleTemplatePatternName(){
    cerr << "ERROR in framework: getDCScaleTemplatePatternName needs to be overridden!" << endl;
    bool notOverridden = false;
    assert(notOverridden);    
    return "";         
}
#endif  



void DeviceRegisterTransactionPort::preExec(std::string labelName){

    //clearing the read cache only if testsuite name changed
    static string lastTestSuiteName = "";
    string testSuiteName;
    GET_TESTSUITE_NAME(testSuiteName);
    if(lastTestSuiteName != testSuiteName){
        lastTestSuiteName = testSuiteName;
        //cerr << "new Testsuite " << testSuiteName << " - clearing readIdValues" << endl;
        readIdValues.clear();
    }
    
    TransactionPort::preExec(labelName);
}    
    

void DeviceRegisterTransactionPort::postExec(std::string executedId, bool executionWasSuccessful){
    if(executionWasSuccessful) evalutateReads();
    readOffsets.clear();
    writeDynamicIDs.clear();
    
    TransactionPort::postExec(executedId, executionWasSuccessful);
}

void DeviceRegisterTransactionPort::checkWrite(){
    if(!writeChecked){
        if(getWriteTemplatePatternName() == ""){
            cerr << "getWriteTemplatePatternName cannot return \"\""<< endl;
            assert(getWriteTemplatePatternName() == "");
        }
        if(!PatternManager::isPatternAvailable(getWriteTemplatePatternName(), "MAIN", portName)){
            cerr << "specified write template pattern doesn't exist: \"" << getWriteTemplatePatternName() << "\"" << endl;
            assert(!PatternManager::isPatternAvailable(getWriteTemplatePatternName(), "MAIN", portName));  
        }
        writeChecked = true;
    }
}

string DeviceRegisterTransactionPort::getNewWriteLabelName(long long address, long long data){
	ostringstream newLabelName;
	newLabelName << getLabelNamePrefix() << "_W_0x" << hex << address << "_0x" << data;
	return newLabelName.str();
}

void DeviceRegisterTransactionPort::write(long long address, long long data){
    checkWrite();
    if(!inD2sBlock){
        cerr << "write can only be used when inside a d2s_LABEL_BEGIN/END-block" << endl;
        assert(inD2sBlock);    
    }
    cycleOffset += getWriteCycles();
    
    if(currentFrameworkMode == d2sFrameWorkModeType::LearningMode){
        
        string newLabelName = getNewWriteLabelName(address, data);
        
        if(!PatternManager::isPatternAvailable(newLabelName)){
            prepareDynamicWriteLabel(address, data, getWriteTemplatePatternName());
            PatternManager::copyLabel(getWriteTemplatePatternName(), newLabelName);
        }

        burstLabels.push_back(newLabelName); 
    }
}

string DeviceRegisterTransactionPort::getNewWriteDynamicLabelName(long long address, long long data, string id){
    ostringstream templateLabelName;
    templateLabelName << getLabelNamePrefix() << "_DW_" << "_" << id;
    return templateLabelName.str();
}

void DeviceRegisterTransactionPort::writeDynamic(long long address, long long data, std::string id){
    checkWrite();
    if(!inD2sBlock){
        cerr << "writeDynamic can only be used when inside a d2s_LABEL_BEGIN/END-block" << endl;
        assert(inD2sBlock);    
    }   
    
    string templateLabelName = getNewWriteDynamicLabelName(address, data, id);
    pair<set<string>::iterator, bool> ret = writeDynamicIDs.insert(id);
    if(ret.second != false){
        //first time for this id; add label to burst (in learning mode)
        cycleOffset += getWriteCycles();
    
        if(currentFrameworkMode == d2sFrameWorkModeType::LearningMode){
            //copy template pattern
            if(!PatternManager::isPatternAvailable(templateLabelName)){
                PatternManager::copyLabel(getWriteTemplatePatternName(), templateLabelName);
            }
            burstLabels.push_back(templateLabelName); 
        }
    }
    
    prepareDynamicWriteLabel(address, data, templateLabelName);
}


int DeviceRegisterTransactionPort::getWriteCycles(){
    return PatternManager::getCyclesFromLabel(getWriteTemplatePatternName(), portName);
}



void DeviceRegisterTransactionPort::wait(double timeInS){
    int debug =0;
	double timingPeriod = getTimingPeriod();
    double waitInNs = timeInS * 1e9;

    if(waitInNs < timingPeriod/2) return; //wait time too small; do nothing

    if(debug) cerr << dec << "wait time is " << waitInNs << " ns" << endl;
    if(debug) cerr << "period is " << timingPeriod << " ns" << endl;
    
    unsigned int cycles = (int)((waitInNs + timingPeriod/2) / timingPeriod); //+0.5*period to round up to next cycle
    if(debug) cerr << cycles << " cycles are needed to achieve achieve wait time" << endl;
    
    waitCycles(cycles);
}

void DeviceRegisterTransactionPort::waitCycles(unsigned int cycles){
    if(!waitChecked){
        if(getWaitTemplatePatternName() == ""){
            cerr << "getWaitTemplatePatternName cannot return \"\""<< endl;
            assert(getWaitTemplatePatternName() == "");
        }
        if(!PatternManager::isPatternAvailable(getWaitTemplatePatternName(), "MAIN", portName)){
            cerr << "specified wait template pattern doesn't exist: \"" << getWaitTemplatePatternName() << "\"" << endl;
            assert(!PatternManager::isPatternAvailable(getWaitTemplatePatternName(), "MAIN", portName));  
        }
        waitChecked = true;
    }
    if(!inD2sBlock){
        cerr << "wait can only be used when inside a d2s_LABEL_BEGIN/END-block" << endl;
        assert(inD2sBlock);    
    }

	cycleOffset += cycles;
    switch(currentFrameworkMode){
        case d2sFrameWorkModeType::LearningMode: 
                waitCyclesModeLearning(cycles);
                break;
        case d2sFrameWorkModeType::ProductionMode:
                //will run the pre-generated label on END_DYN_SETUP
                break;
        default:
                cerr << "Error! cannot wait: wrong state" << endl;
                break;                     
   }
}


void DeviceRegisterTransactionPort::waitCyclesModeLearning(unsigned int cycles){
    if(cycles > MAX_WAIT_CYCLES){
        cerr << "ERROR: " << cycles << " of wait-cycles is too big! Cannot adjust repeat count with the given timing period." << endl;
        cerr << "ERROR: no WAIT added to burst" << endl;
        return;     
    }
        
    string newLabelName = getWaitLabelName(cycles);
    if(!PatternManager::isPatternAvailable(newLabelName)){
        prepareDynamicWaitLabel(cycles);
        PatternManager::copyLabel(getWaitTemplatePatternName(), newLabelName);
    }
    burstLabels.push_back(newLabelName);
}

string DeviceRegisterTransactionPort::getWaitLabelName(unsigned int cycles){
    ostringstream newLabelName;
    newLabelName << dec << getLabelNamePrefix() << "_Wait_" <<  cycles << "Cycles";
    return newLabelName.str();    
}

void DeviceRegisterTransactionPort::prepareDynamicWaitLabel(unsigned int cycles){ 
    
    //default is to only modify the first sequencer command on label to adjust for wait-time
    int seqNoStart;
    int seqNoStop; 
    string port;   
    fwout << "SQLB? \"" << getWaitTemplatePatternName() << "\",MAIN,(@@)" << endl;
    if(fwresult.size() != 1){
        cerr << "ERROR while figuring out repeat sequencer command" << endl;
        return;
    }
    
    if(fwresult[0].count() == 6) port = fwresult[0][5];
    else port = "@";
    
    seqNoStart = fwresult[0].getIntParam(2);
    seqNoStop =  fwresult[0].getIntParam(3);
                        
    bool foundRPTV = false;
    for(int i=seqNoStart; i<seqNoStop; i++){
        fwout << "SQPG? " << i << ",(" << port << ")" << endl;
        if(fwresult[0][1] == "RPTV"){
            //cerr << "adjusting repeat count to " << cycles << endl;
        	string memory = fwresult[0][4];
        	int amountOfCyclesInRPTV = fwresult[0].getIntParam(2);
        	int repeatCount = cycles / amountOfCyclesInRPTV;
        	if(foundRPTV) cerr << "Error: multiple RPTV-sequencer commands found in " << getWaitTemplatePatternName() << " but only 1 is allowed!" << endl; 
            fwout << "SQPG " << i << ",RPTV," << amountOfCyclesInRPTV << "," << repeatCount << "," << memory << ",(" << port << ")" << endl;
            foundRPTV = true;
        }
    }
    if(!foundRPTV){
        cerr << "ERROR: no SQPG RPTV found in " << getWaitTemplatePatternName() << endl;
        cerr << "ERROR: couldn't adjust repeat count to achieve wait time" << endl;
    }
}


void DeviceRegisterTransactionPort::addReadLabelOffset(string id, int cycleOffset){
    readOffsets.insert(pair<string, int>(id, cycleOffset));
}

string DeviceRegisterTransactionPort::getNewReadLabelName(long long address, string id){
    ostringstream newLabelName;
    newLabelName << getLabelNamePrefix() << "_R_0x" << hex << address;
    return newLabelName.str();
}

void DeviceRegisterTransactionPort::readModeLearning(long long address, string id){

	string newLabelName = getNewReadLabelName(address, id);
    if(!PatternManager::isPatternAvailable(newLabelName)){
        prepareDynamicReadOrExpectValueLabel(address,getReadTemplatePatternName());
        PatternManager::copyLabel(getReadTemplatePatternName(), newLabelName);
    }
    burstLabels.push_back(newLabelName);
}

void DeviceRegisterTransactionPort::checkReadAndExpectValue(){
    if(!readAndExpectValueChecked){
        if(getReadTemplatePatternName() == ""){
            cerr << "getReadTemplatePatternName cannot return \"\""<< endl;
            assert(getReadTemplatePatternName() == "");
        }
        if(!PatternManager::isPatternAvailable(getReadTemplatePatternName(), "MAIN", portName)){
            cerr << "specified read template pattern doesn't exist: \"" << getReadTemplatePatternName() << "\"" << endl;
            assert(!PatternManager::isPatternAvailable(getReadTemplatePatternName(), "MAIN", portName));  
        }
        readAndExpectValueChecked = true;
    }
}


void DeviceRegisterTransactionPort::read(long long address, string id){
    checkReadAndExpectValue();

    if(!inD2sBlock){
        cerr << "read can only be used when inside a d2s_LABEL_BEGIN/END-block" << endl;
        assert(inD2sBlock);    
    }     
    if(checkPassFail){
        cerr << "Error: check pass/fail cannot be combined with \"read\" within 1 d2s_LABEL_BEGIN/END-block. Reading is done using the error map and so the port is supposed to fail!" << endl;
        cerr << "Please either switch off pass/fail check, convert the read to expectValue or split into two d2s_LABEL_BEGIN/END-blocks." << endl;            
    }
    addReadLabelOffset(id, cycleOffset);
    cycleOffset += getReadCycles();
    switch(currentFrameworkMode){
        case d2sFrameWorkModeType::LearningMode: 
                readModeLearning(address, id);
                break;
        case d2sFrameWorkModeType::ProductionMode:
                //will run the pre-generated label on END_DYN_SETUP
                break;
        default:
                cerr << "Error! cannot read: wrong state" << endl;
                break;             
    }    
}

int DeviceRegisterTransactionPort::getReadCycles(){
    return PatternManager::getCyclesFromLabel(getReadTemplatePatternName(), portName);
}

void DeviceRegisterTransactionPort::evalutateReads(){
    
    idOffsetType::const_iterator readIDs = readOffsets.begin();
    if(readIDs != readOffsets.end()){
        //something to read
        string prevSite;
        fwout << "PQFC?" << endl;  
        prevSite = fwresult[0][0];
        for( ; readIDs != readOffsets.end(); readIDs++){
            //get and store value of all sites
            readAndStoreValues(readIDs->first, readIDs->second);
        }    
        fwout << "PQFC " << prevSite << endl;
    }    
}

void DeviceRegisterTransactionPort::readAndStoreValues(string id, int offset){
    //cerr << "reading " << id << "@" << offset << endl;
    FOR_EACH_SITE_BEGIN();
        int site = CURRENT_SITE_NUMBER();
        fwout << "PQFC " << site << endl;
        long long readValue = readFromErrorMap(offset);
        pair<map<string, long long>::iterator,bool> ret = readIdValues[portName][site].insert(pair<string, long long> (id, readValue));
        if(ret.second == false){
            cout << "WARNING: ID \"" << id << "\" already used. Please use unique IDs. ID \"" << id << "\" for site " << site << " will be overwritten." << endl; 
            readIdValues[portName][site].erase(id);
            readIdValues[portName][site].insert(pair<string, long long> (id, readValue));    
        }
    FOR_EACH_SITE_END();
}

long long DeviceRegisterTransactionPort::getReadValue(std::string id){
    int site = CURRENT_SITE_NUMBER();
    std::map<std::string, long long>::const_iterator iter(readIdValues[portName][site].find(id));
    if(iter != readIdValues[portName][site].end()) return iter->second;
    else{
        cout << "ERROR: port \"" << portName << "\" getReadValue of ID \"" << id << "\" failed. ID not available." << endl;
        return 0xdeadbeaf;     
    }
}
void DeviceRegisterTransactionPort::expectValueModeLearning(long long address, long long value, long long mask){
    ostringstream newLabelName;
    newLabelName << getLabelNamePrefix() << "_EV_0x" << hex << address << "_0x" << value << "_0x" << mask;
    
    if(!PatternManager::isPatternAvailable(newLabelName.str())){
        prepareDynamicReadOrExpectValueLabel(address, getReadTemplatePatternName(), value, mask);
        PatternManager::copyLabel(getReadTemplatePatternName(), newLabelName.str());
    }

    burstLabels.push_back(newLabelName.str());      
}

void DeviceRegisterTransactionPort::expectValue(long long address, long long value, long long mask){
    checkReadAndExpectValue();
    if(!inD2sBlock){
        cerr << "expectValue can only be used when inside a d2s_LABEL_BEGIN/END-block" << endl;
        assert(inD2sBlock);    
    }   
    if(!checkPassFail){
        cerr << "Warning: expectValue only makes sense when pass/fail check for this port is switched on. Please enable pass/fail check using \"enablePassFailCheckForNextExecution()\"" << endl;    
    }
    switch(currentFrameworkMode){
        case d2sFrameWorkModeType::LearningMode: 
                cycleOffset += getReadCycles();
                expectValueModeLearning(address, value, mask);
                break;
        case d2sFrameWorkModeType::ProductionMode:
                cycleOffset += getReadCycles();
                //will run the pre-generated label later
                break;
        default:
                cerr << "Error! cannot expect value: wrong state" << endl;
                break;                      
    }
}

std::string DeviceRegisterTransactionPort::getLabelNamePrefix(){
    ostringstream prefix;
    
    if(portName == "@") prefix << "d2s_" << getInterfaceName() << "_AT";
    else prefix << "d2s_" << getInterfaceName() << "_" << portName;
    
    return prefix.str();    
}



#ifndef D2S_NO_DCSCALE

int DeviceRegisterTransactionPort::getDcScaleCycles(){
    if(dcScaleCycles == 0) dcScaleCycles = PatternManager::getCyclesFromLabel(getDCScaleTemplatePatternName(), portName);
    return dcScaleCycles;
}

void DeviceRegisterTransactionPort::checkDcScale(){
    if(!dcScaleChecked){
        if(getDCScaleTemplatePatternName() == ""){
            cerr << "ERROR: getDCScaleTemplatePatternName cannot return \"\""<< endl;
            assert(getDCScaleTemplatePatternName() != "");
        }
        if(!PatternManager::isPatternAvailable(getDCScaleTemplatePatternName(), "MAIN", portName)){
            cerr << "ERROR: specified read template pattern doesn't exist: \"" << getDCScaleTemplatePatternName() << "\"" << endl;
            assert(PatternManager::isPatternAvailable(getDCScaleTemplatePatternName(), "MAIN", portName));  
        }
        if(getDcScaleCycles() < 32){
            cerr << "ERROR: " << getDCScaleTemplatePatternName() << " has to generate at least 32 cycles (using 1 GENV-instruction)!" << endl;
            assert(getDcScaleCycles() >= 32);
        }
        int seqNoStart;
        int seqNoStop; 
        string port;   
        fwout << "SQLB? \"" << getDCScaleTemplatePatternName() << "\",MAIN,(@@)" << endl;
        if(fwresult.size() != 1){
            cerr << "ERROR: while checking dcScale pattern. Found multiple patterns." << endl;
            assert(fwresult.size() == 1);
        }
        if(fwresult[0][1] != "MAIN"){
            cerr << "ERROR: while checking dcScale pattern. It has to be a \"MAIN\"-pattern." << endl;
            assert(fwresult[0][1] == "MAIN");            
        }
        if(fwresult[0].count() == 6) port = fwresult[0][5];
        else port = "@";
        
        seqNoStart = fwresult[0].getIntParam(2);
        seqNoStop =  fwresult[0].getIntParam(3);
        
        fwout << "SQPG? " << seqNoStop-1 << ",(" << port << ")" << endl;
        if(fwresult[0][1] != "GENV"){
        	cerr << "ERROR: last sequencer command of template pattern " << getDCScaleTemplatePatternName() << " has to be a \"GENV\"" << endl;
        	assert(fwresult[0][1] == "GENV"); 
        }
        int genvVectorAmount = fwresult[0].getIntParam(2); 
        if(genvVectorAmount < 32){
        	cerr << "ERROR: \"GENV\" sequencer command of template pattern " << getDCScaleTemplatePatternName() << " has to generate at least 32 vectors" << endl;
        	assert(genvVectorAmount >=32);
        }
        if(genvVectorAmount != getDcScaleCycles()){
        	cerr << "ERROR: template pattern " << getDCScaleTemplatePatternName() << " has to consist of a single \"GENV\" command" << endl;
        	assert(genvVectorAmount == getDcScaleCycles());
        }

        /*	
        bool lastFoundIsGENV = false;
        for(int i=seqNoStart; i<seqNoStop; i++){
            fwout << "SQPG? " << i << ",(" << port << ")" << endl;
            if(fwresult[0][1] == "GENV" && fwresult[0].getIntParam(2) >= 32){
            	lastFoundIsGENV = true;
            }
        }
        if(!foundGENV){
            cerr << "ERROR: " << getDCScaleTemplatePatternName() << " should only exist of 1 GENV-command creatig 32 vectors" << endl;
            assert(!foundGENV);
        }*/        
        dcScaleChecked = true;
    }
}

void DeviceRegisterTransactionPort::dcScaleForceVoltageLearninMode(std::string pinList, double voltValue){
    
	//*************************************************
	//* ADC enhance
	//* add to repalce "," with "_"in pin_list names
	//* this is enhancement for multipin dcscale measure
	//*************************************************
	string::size_type pos=0;
    std::string pinList_modify =  pinList;
	//cout << "pinList_modify = " << pinList_modify << endl;
	while( (pos= pinList_modify.find(",") ) != string::npos )
	{
      //  cout << "pos = " << pos << endl;
  		pinList_modify.replace(pos,1,"_");
		//cout << "pinList_modify = " << pinList_modify << endl;
	}
	
	//creating measurement pattern
    std::ostringstream newLabelName;
    newLabelName << getLabelNamePrefix() << "_Frc_" << voltValue << "V_on_" << pinList_modify;  //* ADC
    //TODO check that new label name is not >128char
    
    if(!PatternManager::isPatternAvailable(newLabelName.str())){
        PatternManager::copyLabel(getDCScaleTemplatePatternName(), newLabelName.str());
    }
    
    //adding DC measurement definiton
    std::ostringstream id;
    id << "FRC_" << voltValue << "V";
    DC_SCALE::EVENT ev0(id.str(),"dcset"); 
    ev0.addFrcVolt(voltValue); 
    DC_SCALE::TEST_POINTS tp; 
    tp.addPoint(31, ev0); 
    DC_SCALE::DYN_SETUP dynSetup(newLabelName.str(),"MAIN","REPLACE",pinList,tp); 
    dynSetup.download();
    burstLabels.push_back(newLabelName.str());    
}


void DeviceRegisterTransactionPort::dcScaleForceVoltage(std::string pinList, double voltValue){
    checkDcScale();
    if(!inD2sBlock){
        cerr << "dcScaleForceVoltage can only be used when inside a d2s_LABEL_BEGIN/END-block" << endl;
        assert(inD2sBlock);    
    } 
    
    cycleOffset += getDcScaleCycles();
    switch(currentFrameworkMode){
            case d2sFrameWorkModeType::LearningMode:
                    dcScaleForceVoltageLearninMode(pinList, voltValue);
                    break;
            case d2sFrameWorkModeType::ProductionMode:
                    //will run the label later
                    break;
            default:
                cerr << "Error! cannot dcScaleForceVoltage: wrong state" << endl;
                break;                     
    }         
}

void DeviceRegisterTransactionPort::dcScaleMeasureLearningMode(string measureType, std::string pinList, int samples, const double limitLow, const double limitHigh, bool ignoreLimit, bool doForceVoltage, double voltValue){
    
	//*************************************************
	//* ADC enhance
	//* add to repalce "," with "_"in pin_list names
	//* this is enhancement for multipin dcscale measure
	//*************************************************
	string::size_type pos=0;
    std::string pinList_modify =  pinList;
	//cout << "pinList_modify = " << pinList_modify << endl;
	while( (pos= pinList_modify.find(",") ) != string::npos )
	{
      //  cout << "pos = " << pos << endl;
  		pinList_modify.replace(pos,1,"_");
		//cout << "pinList_modify = " << pinList_modify << endl;
	}
	//creating measurement pattern
    std::ostringstream newLabelName;
    newLabelName << getLabelNamePrefix();
    if(doForceVoltage){
        newLabelName << "_FrcVMeas";
    }
    else{
        newLabelName << "_Meas";    
    }
    newLabelName << measureType << "_" << samples << "avg";
    if(!ignoreLimit){
    	newLabelName << "_ll" << limitLow << "_hl" << limitHigh;
    }
    if(doForceVoltage){
        newLabelName << "_" << voltValue << "V";
    }    
    newLabelName << "_on_" << pinList_modify;// ADC pinList;
    //TODO check that new label name is not >128char
    
    if(!PatternManager::isPatternAvailable(newLabelName.str())){
        PatternManager::copyLabel(getDCScaleTemplatePatternName(), newLabelName.str());
    }
    
    //adding measurement definiton
    std::ostringstream id;
    if(doForceVoltage){
        id << "FrcVMeas" << measureType;
    }
    else{
        id << "Meas" << measureType;
    }
    
    DC_SCALE::EVENT ev0(id.str(),"dcset");
    if(doForceVoltage){
        ev0.addFrcVolt(voltValue);    
    } 
    if(measureType == "C"){
    	//measure current
        if(ignoreLimit){
    		ev0.addMeasCurr(samples);
    	}
    	else{
    		ev0.addMeasCurr(limitLow, limitHigh, samples);
    	}
    }
    else{
        //measure voltage
    	if(ignoreLimit){
    		ev0.addMeasVolt(samples);
    	}
    	else{
    		ev0.addMeasVolt(limitLow, limitHigh, samples);
    	}    	
    }
    
    DC_SCALE::TEST_POINTS tp; 
    tp.addPoint(31, ev0); 
    DC_SCALE::DYN_SETUP dynSetup(newLabelName.str(),"MAIN","REPLACE",pinList,tp); 
    dynSetup.download();
    burstLabels.push_back(newLabelName.str());		
}

void DeviceRegisterTransactionPort::dcScaleMeasure(string measureType, std::string pinList, int samples, const double limitLow, const double limitHigh, bool ignoreLimit, bool doForceVoltage, double voltValue) {
	checkDcScale();
    if(!inD2sBlock){
        cerr << "dcScaleMeasure can only be used when inside a d2s_LABEL_BEGIN/END-block" << endl;
        assert(inD2sBlock);    
    }

	cycleOffset += getDcScaleCycles();
	switch (currentFrameworkMode) {
	case d2sFrameWorkModeType::LearningMode:
		dcScaleMeasureLearningMode(measureType, pinList, samples, limitLow, limitHigh, ignoreLimit, doForceVoltage, voltValue);
		break;
	case d2sFrameWorkModeType::ProductionMode:
		//will run the label later
		break;
	default:
		cerr << "Error! cannot dcScaleMeasure: wrong state" << endl;
		break;
	}
}
    
void DeviceRegisterTransactionPort::dcScaleMeasureVoltage(std::string pinList, int samples){
	dcScaleMeasure("V", pinList, samples, 0, 0, true);
}

void DeviceRegisterTransactionPort::dcScaleMeasureVoltage(std::string pinList, int samples, const double limitLow, const double limitHigh){
	dcScaleMeasure("V", pinList, samples, limitLow, limitHigh, false);
}

void DeviceRegisterTransactionPort::dcScaleMeasureCurrent(std::string pinList, int samples){
	dcScaleMeasure("C", pinList, samples, 0, 0, true);
}

void DeviceRegisterTransactionPort::dcScaleMeasureCurrent(std::string pinList, int samples, const double limitLow, const double limitHigh){
	dcScaleMeasure("C", pinList, samples, limitLow, limitHigh, false);
}

void DeviceRegisterTransactionPort::dcScaleForceVoltageMeasureVoltage(std::string pinList, int samples, double forceVoltageValue){
    dcScaleMeasure("V", pinList, samples, 0, 0, true, true, forceVoltageValue);
}

void DeviceRegisterTransactionPort::dcScaleForceVoltageMeasureVoltage(std::string pinList, int samples, const double limitLow, const double limitHigh, double forceVoltageValue){
    dcScaleMeasure("V", pinList, samples, limitLow, limitHigh, false, true, forceVoltageValue);
}

void DeviceRegisterTransactionPort::dcScaleForceVoltageMeasureCurrent(std::string pinList, int samples, double forceVoltageValue){
    dcScaleMeasure("C", pinList, samples, 0, 0, true, true, forceVoltageValue);
}

void DeviceRegisterTransactionPort::dcScaleForceVoltageMeasureCurrent(std::string pinList, int samples, const double limitLow, const double limitHigh, double forceVoltageValue){
    dcScaleMeasure("C", pinList, samples, limitLow, limitHigh, false, true, forceVoltageValue);
}


void DeviceRegisterTransactionPort::dcScaleSetMeasureRangeLearningMode(std::string pinList, const DC_SCALE::SET_MEAS_RNG setMeasRng, const double minValue, const double maxValue){
    
	//*************************************************
	//* ADC enhance
	//* add to repalce "," with "_"in pin_list names
	//* this is enhancement for multipin dcscale measure
	//*************************************************
	string::size_type pos=0;
    std::string pinList_modify =  pinList;
	//cout << "pinList_modify = " << pinList_modify << endl;
	while( (pos= pinList_modify.find(",") ) != string::npos )
	{
      //  cout << "pos = " << pos << endl;
  		pinList_modify.replace(pos,1,"_");
		//cout << "pinList_modify = " << pinList_modify << endl;
	}
	//creating measurement pattern
    std::ostringstream newLabelName;
    newLabelName << getLabelNamePrefix() << "_Rng";
    switch(setMeasRng){
    	case DC_SCALE::CURR_RNG:
    		newLabelName << "C";
    		break;
    	case DC_SCALE::VOLT_RNG:
    		newLabelName << "V";
    		break;
    	default:
    		cerr << "dcScaleSetMeasureRangeLearningMode(): range " << setMeasRng << " not supported!" << endl;
    }
    		
    newLabelName << "_ll" << minValue << "_hl" << maxValue;
    newLabelName << "_on_" << pinList_modify;// ADC pinList;
    //TODO check that new label name is not >128char
    
    if(!PatternManager::isPatternAvailable(newLabelName.str())){
        PatternManager::copyLabel(getDCScaleTemplatePatternName(), newLabelName.str());
    }
    
    //adding measurement definiton
    std::ostringstream id;
    id << "Rng";
    DC_SCALE::EVENT ev0(id.str(),"dcset"); 
    ev0.addSetRng(setMeasRng, minValue, maxValue);
    DC_SCALE::TEST_POINTS tp;
    tp.addPoint(31, ev0); 
    DC_SCALE::DYN_SETUP dynSetup(newLabelName.str(),"MAIN","REPLACE",pinList,tp); 
    dynSetup.download();
    burstLabels.push_back(newLabelName.str());		
}


void DeviceRegisterTransactionPort::dcScaleSetMeasureRange(std::string pinList, const DC_SCALE::SET_MEAS_RNG setMeasRng, const double minValue, const double maxValue){
	checkDcScale();
    if(!inD2sBlock){
        cerr << "dcScaleSetMeasureRange can only be used when inside a d2s_LABEL_BEGIN/END-block" << endl;
        assert(inD2sBlock);    
    }

	cycleOffset += getDcScaleCycles();
	switch (currentFrameworkMode) {
	case d2sFrameWorkModeType::LearningMode:
		dcScaleSetMeasureRangeLearningMode(pinList, setMeasRng, minValue, maxValue);
		break;
	case d2sFrameWorkModeType::ProductionMode:
		//will run the label later
		break;
	default:
		cerr << "Error! cannot dcScaleMeasure: wrong state" << endl;
		break;
	}	
}
#endif
