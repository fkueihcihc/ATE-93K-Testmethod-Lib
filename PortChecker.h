#ifndef PORTCHECKER_H_
#define PORTCHECKER_H_

#include <set>
#include <map>
#include <vector>
#include <string>

class PortChecker
{
private:
	PortChecker();
	PortChecker(const PortChecker&);
    PortChecker& operator= (const PortChecker&);

public:    
    //singleton
    static PortChecker& Instance();
    bool arePortsIntersecting(const std::set<std::string> &portNames);
    void printPortIntersectionTable();
    
private:    
    //void getMembersOfPort(const std::string &portName, std::set<std::string> &portMembers);
    void splitString(const std::string &stringToBeSplitted, const std::string &separator, std::set<std::string> &returnSet);
    
    std::map<std::string, int> portIndex;
    std::vector<std::vector< int> > intersectingPortTable;
    
};

#endif /*PORTCHECKER_H_*/
