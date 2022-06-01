// --------------------------------------------------------------
// Verigy 20th October 2008
// Revision 0.8
//
// This file contains class implementation, for classes needed
// to generate DYNAMIC Setups
//
// --------------------------------------------------------------
// forceVolt.cpp

//#include "tml_base.h"
//extern CComModule _Module;
//#include "tml_tcom.h"

#if __GNUC__ >= 3
using namespace std;
#endif

#include <TestMethod.h>
#include "Event.h"

using namespace DC_SCALE;

EVENT::EVENT (const STRING &id, const STRING &type)
{
  eventId = id; 
  event  = "<td id=\"" + id   + '\"';
  event += " type=\""   + type + "\">\n";
}

EVENT &EVENT::addSetRng  (const DC_SCALE::SET_MEAS_RNG &setMeasRng, const DOUBLE &minValue, const DOUBLE &maxValue)
{  
  ostringstream minValueStr, maxValueStr;
  STRING setMeasRngStr="";
  
  minValueStr << minValue;
  maxValueStr << maxValue;
  
  switch ( setMeasRng )
  {
    case DC_SCALE::CURR_RNG:
      setMeasRngStr = "\"current\" ";
      break;
    case DC_SCALE::VOLT_RNG:
      setMeasRngStr = "\"voltage\" ";
      break;
    default:
      cout << "ERROR: Unknown Measure Mode" << endl;    
  }
  
  event += "<setMeasureRange type=";
  event += setMeasRngStr;
  event += "minValue=\"" + minValueStr.str() + "\" ";  
  event += "maxValue=\"" + maxValueStr.str() + "\"/>\n";
    
  return *this;
}

EVENT &EVENT::addFrcVolt (const DOUBLE &value) 
{ 
  ostringstream valueStr;
  
  valueStr << value;
  
  event += "<setForceValue type=\"voltage\" ";
  event += "value=\"" + valueStr.str() + "\"/>\n";  

  return *this;
}

EVENT &EVENT::addMeasCurr (const INT &samples)
{ 
  STRING samplesStr="";
  
  if (samples !=0)
  {
    ostringstream samplesStream;
    samplesStream << samples;
    
    samplesStr =" samples=\"" + samplesStream.str() +'\"';      
  }
  
  event += "<measure type=\"current\"";
  event += samplesStr;  
  event += "/>\n";
  
  return *this;
}

EVENT &EVENT::addMeasCurr (const DOUBLE &limitLow, const DOUBLE &limitHigh,const INT &samples)
{
  STRING samplesStr="";
  ostringstream limitLowStr,limitHighStr;
  
  limitLowStr  << limitLow;
  limitHighStr << limitHigh;

  if (samples !=0)
  {
    ostringstream samplesStream;
    samplesStream << samples;
    
    samplesStr =" samples=\"" + samplesStream.str() +'\"';      
  }
    
  event += "<measure type=\"current\" ";
  event += "limitLow=\"" + limitLowStr.str() +"\" ";
  event += "limitHigh=\"" + limitHighStr.str() +"\"";
  event += samplesStr;  
  event += "/>\n";
    
  return *this;
}

EVENT &EVENT::addMeasVolt (const INT &samples)
{ 
  STRING samplesStr="";
  
  if (samples !=0)
  {
    ostringstream samplesStream;
    samplesStream << samples;
    
    samplesStr =" samples=\"" + samplesStream.str() +'\"';      
  }
  
  event += "<measure type=\"voltage\"";
  event += samplesStr;  
  event += "/>\n";
  
  return *this;
}

EVENT &EVENT::addMeasVolt (const DOUBLE &limitLow, const DOUBLE &limitHigh, const INT &samples)
{ 
  STRING samplesStr="";
  ostringstream limitLowStr,limitHighStr;
  
  limitLowStr  << limitLow;
  limitHighStr << limitHigh;

  if (samples !=0)
  {
    ostringstream samplesStream;
    samplesStream << samples;
    
    samplesStr =" samples=\"" + samplesStream.str() +'\"';      
  }
    
  event += "<measure type=\"voltage\" ";
  event += "limitLow=\"" + limitLowStr.str() +"\" ";
  event += "limitHigh=\"" + limitHighStr.str() +"\"";
  event += samplesStr;  
  event += "/>\n";
  
  return *this;
}

STRING &EVENT::getEventStr ()
{  
  tmpEvent = event;
  tmpEvent += "</td>\n";
  
  return tmpEvent;   
}

STRING &EVENT::getEventId ()
{
  return eventId;
}

TEST_POINTS::TEST_POINTS()
{
  testPointsStr  = "<?xml version=\"1.0\"?>\n";
  testPointsStr += "<sequence>\n";
  
  testDataStr  = "<?xml version=\"1.0\"?>\n";
  testDataStr += "<testdata>\n";  
}

TEST_POINTS &TEST_POINTS::addPoint ( const INT &anchor, EVENT &event )
{
  ostringstream  anchorStr;
  anchorStr << anchor;
    
  testPointsStr += "<tp anchor=\"";
  testPointsStr += anchorStr.str();
  testPointsStr += "\" td=\"";
  testPointsStr += event.getEventId();
  testPointsStr += "\"/>\n";
  
  createTestData(event);
  return *this;
} 
TEST_POINTS &TEST_POINTS::addPoint ( EVENT &event )
{
  createTestData(event);
  return *this;
}

STRING &TEST_POINTS::getTestDataStr ()
{
  ostringstream length, lengthBin;
  
  tmpDataStr = testDataStr;
  tmpDataStr += "</testdata>";
  
  length << tmpDataStr.size();
  lengthBin << length.str().size();
  
  tmpDataStr = ",#" + lengthBin.str() + length.str() + tmpDataStr +'\n';

  return tmpDataStr;  
}

STRING &TEST_POINTS::getTestPointsStr ()
{
  ostringstream length, lengthBin;

  tmpPointsStr = testPointsStr;
  tmpPointsStr += "</sequence>";

  length << tmpPointsStr.size();
  lengthBin << length.str().size();
  
  tmpPointsStr = ",#" + lengthBin.str() + length.str() + tmpPointsStr +'\n';

  if ( length.str() == "44" ) // check for no events
  {
    tmpPointsStr = "";
  }

  return tmpPointsStr;
}

void TEST_POINTS::createTestData(EVENT &event)
{
  Boolean newEvent = true;

  set<STRING>::iterator it;
  it = eventIdSet.find(event.getEventId());

  if ( it != eventIdSet.end() )
  {
      newEvent = false;
  }

  if ( newEvent )
  {
    eventIdSet.insert(event.getEventId());
    testDataStr += event.getEventStr();
  }
}

DYN_SETUP::DYN_SETUP ( const STRING &labelName, const STRING &labelType,
                       const STRING &mode, const STRING & pinList,
                       TEST_POINTS &testPoints )
{
  //create DFTD;
  dynSetup  = "DFTD ";
  dynSetup += "\""+ labelName + "\"";
  dynSetup += "," + mode;
  dynSetup += ",(" + pinList + ")";
  dynSetup += testPoints.getTestDataStr();
  dynSetup +="UPTD LEV,1;UPTD DPS,1\n"; 

  //create TPSE;
  if ( testPoints.getTestPointsStr() != "" )
  {
    dynSetup += "TPSE ";
    dynSetup += "\""+ labelName + "\"";
    dynSetup += "," + labelType;
    dynSetup += ",\"" + labelName + "\"";
    dynSetup += "," + mode;
    dynSetup += ",(" + pinList + ")";
    dynSetup += testPoints.getTestPointsStr();
    dynSetup += "UPTD VEC,1\n";  
  }
}

void DYN_SETUP::download(INT site)
{
  STRING fwTask;
  
  if ( site !=-1)
  { 
    ostringstream siteStr;
    siteStr << site;
    
    fwTask="PSFC ";
    fwTask +=siteStr.str();;
    fwTask += "\n";
  }
  fwTask += dynSetup; 
 
  if ( site !=-1)
  {
    fwTask +="PSFC ALL\n";
  }
  
  FW_TASK ( fwTask );  
}

STRING &DYN_SETUP::getDynSetupStr()
{
  return dynSetup;
}

WAVE_FORM::WAVE_FORM()
{
  ;
}
WAVE_FORM::WAVE_FORM ( const INT &anchor, const INT &dist, const INT &points,
                       const DC_SCALE::WAVEFORM_TYPE &type, const DOUBLE &min, const DOUBLE &max,
                       const WAVEFORM_DEBUG &debug)
{
  define ( anchor, dist, points, type, min, max, debug );
}
                         
void WAVE_FORM::define ( const INT &anchor, const INT &dist, const INT &points,
                         const DC_SCALE::WAVEFORM_TYPE &type, const DOUBLE &min, const DOUBLE &max,
                         const WAVEFORM_DEBUG &debug)
{
  INT actAnchor = anchor;
  TEST_POINTS testPointsTmp;
  
  waveFormAnchor = anchor;
  waveFormDist   = dist;
  waveFormPoints = points;
   
  switch ( type )
  {
    case DC_SCALE::V_SINUS:
      {
        const DOUBLE PI = 3.14159265;
        const DOUBLE step = 360.0/points;
        const DOUBLE mid_value = (max+min)/2;
        const DOUBLE pos_max = (max-min)/2; 
                
        for ( int i=0; i<points; i++ )
        { 
          ostringstream eventId;
          DOUBLE actVolt;
          
          eventId << actAnchor;
          actVolt = mid_value + pos_max * sin ((step*i)*PI/180);
          
          EVENT event(eventId.str(), "dcset");
          event.addFrcVolt(actVolt);
          addDebugInfo(event,debug);
          
          addPoint(actAnchor, event);
          waveFormMap.insert( make_pair ( actAnchor, actVolt ) );
      
          actAnchor += dist;    
        }
      }
      break;
    case DC_SCALE::V_RAMP:
      {
        DOUBLE stepVoltage = (max - min)/(points -1);
    
        for ( int i=0; i<points; i++ )
        {
          ostringstream eventId;
          DOUBLE actVolt;

          eventId << actAnchor;
          actVolt = min + (i * stepVoltage);
  
          EVENT event(eventId.str(), "dcset");
          event.addFrcVolt(actVolt);
          addDebugInfo(event,debug);
          
          addPoint(actAnchor, event);
          
          waveFormMap.insert( make_pair ( actAnchor, actVolt ) );
          
          actAnchor += dist;        
        }
      }
      break;
    default:
      cout << "ERROR: Unknown Waveform Type" << endl; 
  }
}

DOUBLE WAVE_FORM::getPrevValue (const INT &anchor)
{
  INT i=0;
  
  if ( anchor < waveFormAnchor ) // anchor < waveFormAnchor => Error
  {
    cout << "Error: Anchor is set before Waveform start" << endl;
    return 99999.00; 
  }
  else if ( anchor >= ((waveFormPoints-1) * waveFormDist + waveFormAnchor))
  {
    i =  ((waveFormPoints-1) * waveFormDist + waveFormAnchor);
  }
  else
  {
    i = waveFormAnchor;
    
    do
    {
      i = i + waveFormDist;
    } while ( i <= anchor );
    
    if ( i != anchor )
    {
      i = i - waveFormDist;
    }
  }
    
  return waveFormMap.find(i)->second;
}

void WAVE_FORM::addDebugInfo (EVENT &event, const WAVEFORM_DEBUG &debug)
{
  switch ( debug )
  {
    case V_MEAS:
      event.addMeasVolt();
      break;
    case C_MEAS:
      event.addMeasCurr();
      break;
    case NO_DEBUG:
      break;
    default: 
      cout << "Error: Unknown Debug Mode" << endl;
  }
}
