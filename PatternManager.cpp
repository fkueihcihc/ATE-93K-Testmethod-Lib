#include "PatternManager.h"
#include "FW.h"
#include "PortChecker.h"
#include "TestMethod.h"
#ifdef __linux__
#include "/opt/hp93000/soc/pws/lib/tpi_c.h"
#include "/opt/hp93000/soc/com/include/ci_types.h"
#include "libcicpi.h"
#endif
#include <unistd.h>
#include <assert.h>

using namespace std;

PatternManager::PatternManager()
{
}

PatternManager::~PatternManager()
{
}

int PatternManager::deleteLabel(string label, string port){
    int debug=0;
    if(debug)cerr << "deleting label >>" << label << "<<" << endl;
    if(port == "" || port == "@"){
         if(Primary.getLabel() == label){
        	  
            //current start label is the one to be deleted
            //make sure start label is set to a different one using Primary.label() to keep cache in sync
            Primary.label("");
            FLUSH();    
        }
    }
    
    if(port == ""){
        fwout << "DLTS \"" << label << "\"" << endl;    
    }
    else{
        fwout << "DLTS \"" << label << "\",("<< port << ")" << endl;        
    }
    
    //workaround: update pattern master file
    //TODO: is workaround still needed?
    //updatePatternManager();  
   
    return 1;
}

int PatternManager::renameLabel(string sourceLabel, string destinationLabel){
    assert(destinationLabel.size() <= 128);
	fwout << "SQLB? \"" << sourceLabel << "\",ALL,(@@)" << endl;
    //cout << "SQLB? \"" << sourceLabel << "\",ALL,(@@)" << endl;
    
    if(fwresult.size() != 1){
        if(fwresult.size() >1){
            cerr << "renameLabel - source-label not unique: " << sourceLabel << endl;
        }
        else{
            cerr << "renameLabel - source-label not found: " << sourceLabel << endl;            
        }
        return -1;        
    }
    //cout << fwresult.size() << " - " << fwresult[0].count() << endl;

    if(fwresult[0].count()==6){
        //mp label    
        //cout << "SQLB \"" << destinationLabel << "\"," << fwresult[0][1] << "," << fwresult[0][2] << "," << fwresult[0][3] << ",\"" <<fwresult[0][4] << "\",("<< fwresult[0][5] <<")" << endl;
        fwout << "SQLB \"" << destinationLabel << "\"," << fwresult[0][1] << "," << fwresult[0][2] << "," << fwresult[0][3] << ",\"" <<fwresult[0][4] << "\",("<< fwresult[0][5] <<")" << endl;        
    }
    else{
        //cout << "SQLB \"" << destinationLabel << "\"," << fwresult[0][1] << "," << fwresult[0][2] << "," << fwresult[0][3] << ",\"" <<fwresult[0][4] << "\"" << endl;                
        fwout << "SQLB \"" << destinationLabel << "\"," << fwresult[0][1] << "," << fwresult[0][2] << "," << fwresult[0][3] << ",\"" <<fwresult[0][4] << "\"" << endl;        

    }
    updatePatternManager(); 
    return 1;
}

int PatternManager::saveLabel(string labelName){
    char dev_path[1024];
    char tmp_path[1024]; //TODO: is that sufficient?
    char pathToLabelFile[1024]; //TODO: is that sufficient?
    GetDevPath(dev_path);
    GetTmpPath(tmp_path);
    
    sprintf(pathToLabelFile,"%s/Labels",tmp_path);
    fstream labelFile(pathToLabelFile,ios::out);
    labelFile << "1" << endl << labelName << endl;
    labelFile.close();
    INT32 retState;
    DataManager(dm_vector, dm_save, dev_path, const_cast<char*>(labelName.c_str()), 49 | 0x80, &retState);
    //updatePatternManager();
    if(retState<0){
        cerr << "could not save the file \"" << labelName << "\"." << endl;
        return -1;
    }
    else{
        return 1;    
    }
}

int PatternManager::mergeLabelFromFile(string fileName){

    char dev_path[1024];
    GetDevPath(dev_path);
    INT32 retState;
    //merge in the just created new label 
    //cerr << "merging in " << fileName << endl;
    DataManager(dm_vector, dm_append, dev_path, const_cast<char*>(fileName.c_str()), 0, &retState);  
    if(retState<0){
        cerr << "could not merge the file \"" << fileName << "\"." << endl;
        return -1;
    }  
    
    //workaround: need to set new merged pattern out of data. Otherwise PatternManager has an issue    
    fwout << "TSTL \"" << fileName << "\"," << endl;
    fwout << "TCMT? 0,1" << endl;
    if(fwresult.size()>0) fwout << "tcmt " << fwresult[0][0] << ",\"" << fwresult[0][1] << "\"" << endl;
    else fwout << "tcmt 0,\"\"" << endl;

    //workaround: update pattern manager
    updatePatternManager(); 
    return 1;    
}

int PatternManager::copyLabel(string sourceLabel, string destinationLabel){
    int debug = 0;
    if(debug) cerr << "copying " << sourceLabel << " to " << destinationLabel << endl;
    
    if(!isPatternAvailable(sourceLabel)){
    	cerr << sourceLabel << " doesn't exist. Cannot create copy." << endl;
    	return 0;
    }
    	
    //check if destination already exists
    if(isPatternAvailable(destinationLabel)){
        cerr << destinationLabel << " already exists: will be deleted first." << endl;
        deleteLabel(destinationLabel);
    }
    
    //workaround: update pattern manager
    //updatePatternManager(); 
    
    //rename the label
    renameLabel(sourceLabel, destinationLabel);

    saveLabel(destinationLabel);
        
    //rename original back
    renameLabel(destinationLabel, sourceLabel);    
    
    //merge in the just created new label 
    mergeLabelFromFile(destinationLabel);
    
    return 1;
}

int PatternManager::copyCycles(const std::string& labelName, unsigned int vector, unsigned int count,const std::string& port) {
    // duplicate the vector AFTER the first (LSB) cycle count times
    fwout << "TSTL \"" << labelName << "\",;VPAR? " << vector << "," << vector << endl;
    unsigned int cmd=fwresult[0].getIntParam(0);
    fwout << "NODV?" << endl;
    unsigned int vectorCount=fwresult[0].getIntParam(0);
    fwout << "APDV " << cmd << "," << count << ";"
          << "CPYV " << vector << "," << vector+count << "," << vectorCount-vector << ",(" << port << ");"
          << "CMVC " << vector << "," << vector+count << "," << vectorCount-vector << ";" << endl;
    
    for(unsigned int destVector=vector+1 ; destVector<vector+count ; destVector++) {
        fwout << "CPYV " << vector << "," << destVector << ",1,(" << port << ")" << endl;
    }
    
    return 1;
}

/*
 * WAIT sequencer command not working until SmarTest 6.3.4. See CR42942
int PatternManager::createWaitPattern(string patternName, string port, string vmOrSm, string wavetableName, double timeInS){
    int debug = 1;
    //get current SQPG size
    fwout << "DMAS? SQPG,SM,(" << port << ")" << endl;
    int SMcurrentSize = fwresult[0].getIntParam(2);
    if(debug) cerr << "SM memory size before: " << SMcurrentSize << endl;

    //get current PARA size
    fwout << "DMAS? PARA,"<< vmOrSm << ",(" << port << ")" << endl;
    int ParaCurrentSize = fwresult[0].getIntParam(2);
    if(debug) cerr << "Para memory size before: " << ParaCurrentSize << endl;    
    
    if(timeInS > 1.0){
        cerr << "wait pattern cannot be created for wait-times > 1s. Please reduce time. Will use 1 s instead." << endl;
        timeInS = 1.0;
    }
    unsigned int timeNS = (unsigned int) (timeInS * 1e9 + 0.5);//round up
    if(debug) cerr << "creating wait-pattern for " << timeNS << " ns" << endl;
    int noOfSeqCmd = 0;
    unsigned int timeInNano[179]; //max is 1s -> 179*5592402
    while(timeNS >= 5592402){
        timeInNano[noOfSeqCmd] = 5592402;
        noOfSeqCmd++;
        timeNS -= 5592402;
    }
    if(timeNS !=0){
        //we do have some remainding
        timeInNano[noOfSeqCmd] = timeNS;
        noOfSeqCmd++;  
    }
    if(debug) cerr << "need " << noOfSeqCmd << " seq. cmds to achieve wait time" << endl;

    fwout << "DMAS SQPG,SM," << SMcurrentSize+2+noOfSeqCmd+1  << ",(" << port << ")" << endl;
    fwout << "SQLB \"" << patternName << "\",MAIN," << SMcurrentSize << "," << SMcurrentSize+2+noOfSeqCmd << ",\"" << wavetableName << "\",(" << port << ")" << endl;
    fwout << "SQLA LBL,\"" << patternName << "\",\"PARA_MEM=" << vmOrSm << ",SCAN_MEM=VM,PARA_MCTX=DEFAULT\"" << endl;
    fwout << "SQPG " << SMcurrentSize++ << ",STVA," << ParaCurrentSize << ",,"<< vmOrSm << ",(" << port << ")" << endl;
    fwout << "SQPG " << SMcurrentSize++ << ",STSA,,,"<< vmOrSm << ",(" << port << ")" << endl;

    for(int i=0; i<noOfSeqCmd; i++){
        fwout << "SQPG " << SMcurrentSize++ << ",WAIT," << timeInNano[i] << ",,,(" << port << ")" << endl;
    }
    fwout << "SQPG " << SMcurrentSize++ << ",STOP,,,,(" << port << ")" << endl;
    
    //workaround: update pattern manager
    updatePatternManager();
    return 1;
}
*/


int PatternManager::createBurst(string burstName, vector<string> labelsToCall, std::string portName, bool warnEmpty){
	assert(burstName.size() <= 128);
	int debug = 0;
    bool isMultiport = false;
    string port;
    string wavetable = "";
    if(labelsToCall.size() == 0){
        if(warnEmpty) cerr << "createBurst: no labelsToCall specified. Creating empty burst \"" << burstName << "\"" << endl; 
        if(portName == ""){
        	port = "@";
        }
        else {
        	port = portName;
        }
    }
    else{
        if(portName == ""){
            //when no portName is specified; map to first port (for compatibility)
            fwout << "SQLB? \"" << labelsToCall[0] << "\",MAIN,(@@)" << endl;
            if(fwresult[0].count() == 6){
               //multiport
               port = fwresult[0][5];
            }
            else{
                port = "@";    
            }
        }
        else{
            port = portName;    
        }     
        //get wavetable
        fwout << "SQLB? \"" << labelsToCall[0] << "\",MAIN,(" << port << ")" << endl;
        wavetable = fwresult[0][4];
    }
    
    if(port == "@") isMultiport = false;
    else isMultiport = true;
    
    if(debug) cerr << "multiport = " << isMultiport << " port = " << port << endl;

    if(isPatternAvailable(burstName, isMultiport?"MPBU":"BRST", port)){       
        // check if existing burst has the same content as the new one
        mapStringVectorStringType portBurstLabels;
        queryMultiPortBurst(burstName, portBurstLabels);
        
        bool deleteExistingBurst = false;
        mapStringVectorStringType::const_iterator itPort=portBurstLabels.find(port);
        if(itPort==portBurstLabels.end() || itPort->second.size()!=labelsToCall.size()) {
            deleteExistingBurst = true;
        }
        else {
            for(unsigned int i=0 ; i<itPort->second.size() ; i++) {
                if(labelsToCall[i] != itPort->second[i]) {
                    deleteExistingBurst = true;
                }
            }
        }
        
        if(!deleteExistingBurst) {
            return 1;
        }
        // delete label
        cerr << "Warning: Burst \"" << burstName << "\" on port \"" << port << "\" already exists with different content and will be overwritten." << endl;
        
        deleteLabel(burstName, port);
    }

    int smSizeOfBurst;
    smSizeOfBurst = labelsToCall.size();
    smSizeOfBurst++; //one more sequencer inst. is needed: "BEND"
    
    
    //get current SM size
    fwout << "DMAS? SQPG,SM,(" << port << ")" << endl;
    int SMcurrentSize = fwresult[0].getIntParam(2);
    if(debug) cerr << "memory size before: " << SMcurrentSize << endl;
    
    //check if there is enough memory
    fwout << "DMAS? MFRE, SM, (@)" << endl;
    if(fwresult[0].getIntParam(2) < ((smSizeOfBurst + 1) * 8)){
        //the unit of DMAS? MFRE is [vectorSize]
        //one sequencer command needs 8 * vectorSize
        cerr << "not enough memory available to create burst " << burstName << endl;
        return -1;    
    }
        
    if(debug) cerr << "creating new BurstLabel: " << burstName << " for port " << port << endl;
    fwout << "SAOF APP" << endl; //enter append mode    
    fwout << "DMAS SQPG,SM," << smSizeOfBurst << ",(" << port << ")" << endl;
    if(isMultiport){
        fwout << "SQLB \"" << burstName << "\",MPBU,0," << smSizeOfBurst-1 << ",,(" << port << ")" << endl;
    }
    else{
        fwout << "SQLB \"" << burstName << "\",BRST,0," << smSizeOfBurst-1 << ",\"" << wavetable << "\",(" << port << ")" << endl;        
    }

    for(unsigned int i=0; i<labelsToCall.size(); i++){
        if(debug) cerr << "adding " << labelsToCall[i] << endl;
        fwout << "SQPG " << i << ",CALL,,\"" << labelsToCall[i] << "\",,(" << port << ")" << endl;
    }
    fwout << "SQPG " << smSizeOfBurst-1 << ",BEND,,,,(" << port << ")" << endl;
    fwout << "SAOF ZERO" << endl; //leave append mode
    
    //workaround: update pattern manager
    updatePatternManager();

    cerr << "Warning: Don't forget to save the new label." << endl;

    return 1;
}

bool PatternManager::isPatternAvailable(string labelFile, string labelType, string port){
    fwout << "SQLB? \"" << labelFile << "\"," << labelType << ",(" << port << ")" << endl;
    if(fwresult.size() >0){
        return true;
    }
    else{
        return false;    
    }
}

bool PatternManager::isMultiport(string labelName){
    fwout << "SQLB? \"" << labelName << "\",ALL,(@@)" << endl;  
    if(fwresult.empty()) {
        cerr << "label " << labelName << "doesn't exist. Can't return isMultiport()" << endl;
        return false;
    }
    if(fwresult.size()>1) return true;
    else{
        if(fwresult[0][1] != "MAIN") cerr << "Type >>" <<fwresult[0][1] << "<<  for " << labelName << " not supported. Can't return isMultiport()" << endl;
        else{
            if(fwresult[0].count() == 5) return false;
            else return true;
        }
    }
    cerr << "error!!" << endl;
    return false;
}

int PatternManager::updatePatternManager(){
    //TODO: find out if it's a pattern master file
    INT32 retState;
    DataManager(dm_pattern_master_file, dm_pldump , (char*)"..", (char*)"comm_file", 0, &retState);
    return retState;    
}



unsigned long long PatternManager::getCyclesFromLabel(std::string label, std::string port){
    int seqNoStart;
    int seqNoStop; 
    
    fwout << "SQLB? \"" << label << "\",MAIN,(@@)" << endl;
    if(fwresult.empty()){
    	cerr << "Error in readCyclesFromLabel(\"" << label << "\",\"" << port << "\"): label not found." << endl;
    	return 0;
    }
    
    seqNoStart = fwresult[0].getIntParam(2);
    seqNoStop =  fwresult[0].getIntParam(3);
                        
    for(int i=seqNoStart; i<seqNoStop; i++){
        fwout << "SQPG? " << i << ",(" << port << ")" << endl;
        if(fwresult[0][1] == "MBGN" || fwresult[0][1] == "WTER" || fwresult[0][1] == "MJPE" || fwresult[0][1] == "MEND" || fwresult[0][1] == "JMPE" || fwresult[0][1] == "RETC"|| fwresult[0][1] == "MACT"|| fwresult[0][1] == "MRPT"){
        	cerr << "Error in readCyclesFromLabel( \"" << label << "\",\"" << port << "\"): Not possible to determine cycles for label that contains " << fwresult[0][1] << endl;
            return 0;
        }
    }

    //valid label --> get cycles
    int cycles;
    //save old label for restoration later
    fwout << "SQSL?" << endl;    
    string previousLabel = fwresult[0][0];
    string uptdBefore;
    fwout << "UPTD? VEC" << endl;
    //query updt status of vector before creating dummy vector
    uptdBefore = fwresult[0][1];

	//reset cycle reconstruction. Workaround for a SmarTest bug (already fixed)
//    vector <string> dummy;
//    createBurst("NOP.dummy", dummy, "@", false);
//    fwout << "SQSL \"NOP.dummy\";FTST?" << endl;
//    deleteLabel("NOP.dummy");
	 
    if(isMultiport(label)){
        //need to create a dummy multiport burst to determine cycles using vrcs-fw-command
        vector<string> dummy;
        dummy.push_back(label);
        createBurst("dummyToDetermineCycles", dummy);
        fwout << "sqsl \"" << "dummyToDetermineCycles" << "\"" << endl;    
    }else{
        fwout << "sqsl \"" << label << "\"" << endl;
    }
    fwout << "vrcs? strt,(" << port << ")" << endl;
    int paddedSize = fwresult[0].getIntParam(2);
    fwout << "getv? " << paddedSize-1 << ", 1,(" << port << ")" << endl;
    if(fwresult[0][4] == "BRK") cycles = fwresult[0].getIntParam(0);
    else cycles = paddedSize;
    
    //restore old start label
    fwout << "sqsl \"" << previousLabel << "\"" << endl;
    
    if(isMultiport(label)){
        //delete dummy label
        deleteLabel("dummyToDetermineCycles");
    }
    //reset uptd status
    fwout << "UPTD VEC," << uptdBefore << endl;

    return cycles;  
}

void PatternManager::queryPortBurst(std::string portBurstName, string port, vectorStringType &calledLabels){
	calledLabels.clear();
	fwout << "SQLB? \"" << portBurstName << "\",ALL,(" << port << ")" << endl; 
	if(fwresult.empty()){
		cerr << "queryPortBurst(): label doesn't exist!" << endl;
		return;
	}
	if(fwresult.size() != 1){
		cerr << "queryPortBurst(): found multiple labels with same name on same port -> not expected!!" << endl;
		return;
	}
	if(fwresult[0][1] != "MPBU" && fwresult[0][1] != "BRST"){
		cerr <<"queryPortBurst(): found wrong label-type -> not expected!!" << endl;
		return;
	}
	
	fwout << "SQPG? \"" << fwresult[0][2] << "," << fwresult[0][3] << "\",(" << port << ")" << endl;
	for(unsigned int i=0; i< fwresult.size(); i++){
		if(fwresult[i][1] == "CALL"){
			calledLabels.push_back(fwresult[i][3]);
		}
	}
}

void PatternManager::queryMultiPortBurst(std::string mpStartLabel, mapStringVectorStringType &allPortLabelsSet){
	allPortLabelsSet.clear();
	fwout << "SQLB? \"" << mpStartLabel << "\",ALL,(@@)" << endl;  
    if(fwresult.empty()) {
        cerr << "label " << mpStartLabel << "doesn't exist. Can't query content" << endl;
        return;
    }
	set<string> portBursts;
	
	//store all ports
	for(unsigned int i=0; i<fwresult.size(); i++){
    	if(fwresult[i][1] == "BRST"){
    		if(fwresult.size()!=1){
        		cerr << "queryMultiPortBurst: can only be BRST or MPBU. If BRST only 1 is expected" << endl;
        		return;    			
    		}
    		portBursts.insert("@");
    	}
    	else if(fwresult[i][1] == "MPBU") {
    		portBursts.insert(fwresult[i][5]);
    	} 
    	else{
    		cerr << "queryMultiPortBurst: can only query Burst and MP-Burst." << endl;
    		return;
    	}
	}
	
	//loop over ports and query labels
    for(set<string>::const_iterator port = portBursts.begin(); port != portBursts.end(); port++){
    	vectorStringType calledLabels;
    	queryPortBurst(mpStartLabel, *port, calledLabels);
    	allPortLabelsSet[*port] = calledLabels;
    }
}

bool PatternManager::createMultiPortBurst(std::string multiPortBurstName, mapStringVectorStringType &portLabels){

	set<string> portsToCheck;
	cerr << "createMultiPortBurst(): creating burst for ports:" << endl;
	for(mapStringVectorStringType::const_iterator port = portLabels.begin(); port != portLabels.end(); port++){
		cerr << "createMultiPortBurst(): " << port->first << endl;
		portsToCheck.insert(port->first);
	}

	PortChecker& portchecker = PortChecker::Instance();
	if(portchecker.arePortsIntersecting(portsToCheck)){
		cerr << "createMultiPortBurst(): ports are intersecting. Cannot create Burst!" << endl;
		return false;
	}
	
    if(isPatternAvailable(multiPortBurstName, "MPBU", "@@")){
        //delete label if it already exists
        cerr << "Warning: MultiPort-Burst \"" << multiPortBurstName << " already existed and will be overwritten." << endl;
        deleteLabel(multiPortBurstName, ""); //empty port means delete all
    }
	//now create Bursts
	for(mapStringVectorStringType::const_iterator port = portLabels.begin(); port != portLabels.end(); port++){
		createBurst(multiPortBurstName, port->second, port->first);
	}

	return true;
	
}


bool PatternManager::combineBurstsToNewBurst(std::string newMPBurstName, const vectorStringType &burstLabelsToCombine){
	mapStringVectorStringType newBurstContent;
	for(vectorStringType::const_iterator burst = burstLabelsToCombine.begin(); burst != burstLabelsToCombine.end(); burst++){
		mapStringVectorStringType contentOfCurrentBurst;
		queryMultiPortBurst(*burst, contentOfCurrentBurst);
		for(mapStringVectorStringType::const_iterator port = contentOfCurrentBurst.begin(); port != contentOfCurrentBurst.end(); port++){
			newBurstContent[port->first].insert(newBurstContent[port->first].end(), port->second.begin(),port->second.end());
		}
	}

	createMultiPortBurst(newMPBurstName, newBurstContent);
	return true;
}

void PatternManager::getDefinedLabels(setStringType &definedLabels, const string portFilter, const string typeFilter){
	definedLabels.clear();
	if(!(typeFilter == "ALL" || typeFilter == "MPBU" || typeFilter == "BRST" || typeFilter == "MAIN")){
		cerr << "PatternManager::getDefinedLabels - filter can only be \"ALL\", \"MPBU\", \"BRST\" or \"MAIN\"" << endl;
		return;
	}
	
	fwout << "SQLB? ALL," << typeFilter << ",(" << portFilter << ")" << endl;
	for(unsigned int i=0; i<fwresult.size(); i++){
		definedLabels.insert(fwresult[i][0]);
	}
}


