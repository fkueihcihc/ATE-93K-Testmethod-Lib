// --------------------------------------------------------------
// Verigy 20th October 2008
// Revision 0.8
//
// This file contains class header files, for classes needed
// to generate DYNAMIC Setups
//
// --------------------------------------------------------------

#ifndef INCLUDED_EVENT
#define INCLUDED_EVENT

#include "TestMethod.h"

#if __GNUC__ >= 3
#include <sstream>
#else
#include <strstream.h>
#endif
#include <set>
#include <map>

namespace DC_SCALE {
  enum SET_MEAS_RNG   { CURR_RNG, VOLT_RNG};
  enum WAVEFORM_TYPE  { V_RAMP, V_SINUS };
  enum WAVEFORM_DEBUG { V_MEAS, C_MEAS, NO_DEBUG };

class EVENT
{
   public:
     EVENT (const STRING &id, const STRING &type);
     EVENT &addSetRng  (const DC_SCALE::SET_MEAS_RNG &setMeasRng, const DOUBLE &minValue, const DOUBLE &maxValue);
     EVENT &addFrcVolt (const DOUBLE &value); 
     EVENT &addMeasCurr (const INT &samples=0);
     EVENT &addMeasCurr (const DOUBLE &limitLow, const DOUBLE &limitHigh,const INT &samples=0);
     EVENT &addMeasVolt (const INT &samples=0);
     EVENT &addMeasVolt (const DOUBLE &limitLow, const DOUBLE &limitHigh, const INT &samples=0);    
       
     STRING &getEventStr ();
     STRING &getEventId ();          
    private:
     STRING tmpEvent;
     STRING event;
     STRING eventId;
};

class TEST_POINTS
{
  public:
    TEST_POINTS();
    TEST_POINTS &addPoint ( const INT &anchor, EVENT &event );
    TEST_POINTS &addPoint ( EVENT &event );
    STRING &getTestDataStr (); 
    STRING &getTestPointsStr (); 
  private:
    void createTestData(EVENT &event);
    std::set<STRING> eventIdSet;
    STRING tmpDataStr;
    STRING testDataStr;
    STRING tmpPointsStr;
    STRING testPointsStr;
};

class WAVE_FORM:public TEST_POINTS
{
  public:
    WAVE_FORM ();
    WAVE_FORM ( const INT &anchor, const INT &dist, const INT &points,
                const DC_SCALE::WAVEFORM_TYPE &type, const DOUBLE &min, const DOUBLE &max,
                const WAVEFORM_DEBUG &debug=NO_DEBUG);
    
    void define ( const INT &anchor, const INT &dist, const INT &points,
                  const DC_SCALE::WAVEFORM_TYPE &type, const DOUBLE &min, const DOUBLE &max,
                  const WAVEFORM_DEBUG &debug=NO_DEBUG);
                  
    DOUBLE        getPrevValue (const INT &anchor);    
  private:
    void addDebugInfo (EVENT &event, const WAVEFORM_DEBUG &debug);
    std::map<INT,DOUBLE> waveFormMap;
    INT             waveFormAnchor;
    INT             waveFormDist;
    INT             waveFormPoints;
};

class DYN_SETUP
{
  public:
    DYN_SETUP ( const STRING &labelName, const STRING &labelType,
                const STRING &mode, const STRING & pinList,
                TEST_POINTS &testPoints );
                
    STRING &getDynSetupStr();
    void download(INT site=-1);
  private:
    STRING dynSetup;
};

}

#endif
