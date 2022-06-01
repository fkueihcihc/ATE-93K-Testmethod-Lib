#ifndef PATTERNMANAGER_H_
#define PATTERNMANAGER_H_

#include <string>
#include <vector>
#include <map>
#include <set>


class PatternManager
{
public:
	PatternManager();
	virtual ~PatternManager();
    
    static int copyLabel(std::string sourceLabel, std::string destinationLabel);
    static int renameLabel(std::string sourceLabel, std::string destinationLabel);
    static int saveLabel(std::string labelName);
    static int mergeLabelFromFile(std::string fileName);
    static int createBurst(std::string burstName, std::vector<std::string> labelsToCall, std::string portName="");
    static int deleteLabel(std::string label, std::string portName="");
    static bool isPatternAvailable(std::string labelFile, std::string labelType="ALL", std::string port="@@");
    static int updatePatternManager();
    static bool isMultiport(std::string labelName); 
    static unsigned long long getCyclesFromLabel(std::string label, std::string portName="@");
    static int copyCycles(const std::string& labelName, unsigned int vector, unsigned int count,const std::string& port);
    
    typedef std::vector<std::string> vectorStringType;
	 typedef std::set<std::string> setStringType;
    typedef std::map<std::string, std::vector<std::string> > mapStringVectorStringType; 
    static void queryMultiPortBurst(std::string mpStartLabel, mapStringVectorStringType &portLabels);
    static void queryPortBurst(std::string portBurstName, std::string port, vectorStringType &calledLabels);
    static bool createMultiPortBurst(std::string multiPortBurstName, mapStringVectorStringType &portLabels);
    static bool combineBurstsToNewBurst(const std::string newMPBurstName, const vectorStringType &burstLabelsToCombine);
	static void getDefinedLabels(setStringType &definedLabels, const std::string portFilter="@@", const std::string typeFilter="ALL");

    
    /*
     *WAIT sequencer command not working until SmarTest 6.3.4. See CR42942
    static int createWaitPattern(string patternName, string port, string vmOrSm, string wavetableName, double timeInS);
    */
};

#endif /*PATTERNMANAGER_H_*/
