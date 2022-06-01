#include "PortChecker.h"
#include "FW.h"

#include <algorithm>

using namespace std;
#define DEBUG 0

PortChecker::PortChecker(){
    
    fwout << "DFPT? (@)" << endl;
    int numberOfPorts = fwresult.size();
    std::set<std::string> *ports = new std::set<std::string>[numberOfPorts];
    for(unsigned int i=0; i<fwresult.size(); i++){
        string portName = fwresult[i][1];
        portIndex.insert(pair<string, int> (portName, i));
        
        //get port members
        string portList = fwresult[i][0];
        #if DEBUG     
            cerr << "portMember: " << portList << endl;
        #endif        
        splitString(portList, ",", ports[i]);   
    }
    
    #if DEBUG 
    for(std::map<std::string, int>::const_iterator mapIt=portIndex.begin(); mapIt!=portIndex.end(); mapIt++){
        cerr << "port " << mapIt->first << " consists of :" << endl;
        for(std::set<std::string>::const_iterator setIt=ports[portIndex[mapIt->first]].begin(); setIt!=ports[portIndex[mapIt->first]].end(); setIt++){
            cerr << *setIt << endl;
        }
    }
    #endif
    
    //check if ports are intersecting within each other and build table
    
    intersectingPortTable.resize(numberOfPorts);
    for(int i=0; i<numberOfPorts; i++){
        intersectingPortTable[i].resize(numberOfPorts, -1);    
    }

    for(std::map<std::string, int>::const_iterator portA=portIndex.begin(); portA!=portIndex.end(); portA++){
        for(std::map<std::string, int>::const_iterator portB=portIndex.begin(); portB!=portIndex.end(); portB++){
            string portAName = portA->first;
            int portAIndex = portA->second;
            string portBName = portB->first;
            int portBIndex = portB->second;
            if(portAName == portBName){
                intersectingPortTable[portAIndex][portBIndex] = 1;
                intersectingPortTable[portBIndex][portAIndex] = 1;
            }
            else{
                //check members
                set<string> commonPins;
                set<string> portAMembers = ports[portAIndex];
                set<string> portBMembers = ports[portBIndex];
                set_intersection(portAMembers.begin(), portAMembers.end(), portBMembers.begin(), portBMembers.end(), inserter(commonPins, commonPins.begin()));
                if(commonPins.size()!=0){
                    intersectingPortTable[portAIndex][portBIndex] = 1;
                    intersectingPortTable[portBIndex][portAIndex] = 1;
                    #if DEBUG 
                    cerr << "ports intersect!" << endl;  
                    cerr << "intersecting pins are: "<< endl;
                    for(std::set<std::string>::const_iterator it=commonPins.begin(); it!=commonPins.end(); it++){
                        cerr << *it << endl;    
                    }
                    #endif
                }
                else{
                    intersectingPortTable[portAIndex][portBIndex] = 0;
                    intersectingPortTable[portBIndex][portAIndex] = 0;
                    //cerr << "ports are not intersecting" << endl;   
                }    
            }
        }    
    }
        
    delete[ ] ports;
}

void PortChecker::printPortIntersectionTable(){
    for(unsigned int i=0; i<intersectingPortTable.size(); i++){
        cerr << i << " || ";
        for(unsigned int j=0; j< intersectingPortTable[i].size(); j++){
            cerr << intersectingPortTable[i][j] << " | ";
        }
        cerr << endl;
    }   
}

void PortChecker::splitString(const std::string &stringToBeSplitted, const std::string &separator, std::set<std::string> &returnSet) 
{ 
   std::string::size_type start = 0; 
   std::string::size_type end = 0; 

   while ((end=stringToBeSplitted.find (separator, start)) != std::string::npos) 
   { 
      returnSet.insert(stringToBeSplitted.substr (start, end-start)); 
      start = end+separator.size(); 
   } 

   returnSet.insert(stringToBeSplitted.substr(start)); 
} 

PortChecker& PortChecker::Instance(){        
        static PortChecker instance;
        PortChecker& ref = instance;
        return ref;
}

bool PortChecker::arePortsIntersecting(const std::set<std::string> &portNames){
    bool portsDoIntersect = false;
    for(std::set<std::string>::const_iterator portA_it=portNames.begin(); portA_it!=portNames.end(); portA_it++){
        std::set<std::string>::const_iterator tmpIt = portA_it;
        tmpIt++;
        for(std::set<std::string>::const_iterator portB_it=tmpIt; portB_it!=portNames.end(); portB_it++){
            //cerr << "checking port \"" << *portA_it << "\" with \"" << *portB_it << "\"" << endl;
            if(intersectingPortTable[portIndex[*portA_it]][portIndex[*portB_it]] == 1) portsDoIntersect = true;
        }
    }
    return portsDoIntersect;    
}

