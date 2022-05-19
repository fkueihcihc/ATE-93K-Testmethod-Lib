//**************************************************************************************************
//*  Author:      ZTE - Xi'an, China  liguangyao
//*  Description: DPS common code
//*  SmarTest:    Recommend 7.2 and above version
//*  Functions:

#include "Common.h"
#include "PinUtility.h"
#include "mapi.hpp"
#include "TestControl.h"
#include <string>
#include <sstream>
#include "FW.h"
#include "dpsComCode.h"


using namespace std;
using namespace V93kLimits;


dpsComCode::dpsComCode(){

}

dpsComCode::~dpsComCode() {
	// TODO Auto-generated destructor stub
}

double dpsComCode::calc_DPS_iMeasRange(const string & DPSpin, double lowerl, double upperl) {
	string pintype;
	double maxlimit;

	maxlimit = max(abs(lowerl), abs(upperl));

	const DPS_SETTING &dps_setting = get_DPSsettingRef(DPSpin);


	pintype = PinUtility.getPinType(DPSpin.c_str());
	/* DPS type
	 * "DCS-DPS128HC"
	 * "DCS-DPS128HV"
	 * "DCS-VI32"
	 * "DCS-DPS32"
	 * "DCS-UHC4T"
	 * "DCS-UHC4"
	 * "PS1600"
	 */

	if(pintype == "DCS-DPS128HC" || pintype == "DCS-DPS64HC")  // refer to TDC 142027
	{
		if(dps_setting.ganged == 1 || dps_setting.connect_state == "UNGANG") {  // unganged DPS
			if(maxlimit < 12.5 uA) return 12.5 uA;
			else if (maxlimit < 25 uA) return 25 uA;
			else if (maxlimit < 125 uA) return 125 uA;
			else if (maxlimit < 250 uA) return 250 uA;
			else if (maxlimit < 1.25 mA) return 1.25 mA;
			else if (maxlimit < 2.5 mA) return 2.5 mA;
			else if (maxlimit < 12.5 mA) return 12.5 mA;
			else if (maxlimit < 25 mA) return 25 mA;
			else if (maxlimit < 100 mA) return 100 mA;
			else if (maxlimit < 200 mA) return 200 mA;
			else if (maxlimit < 1 A) return 1 A;
			else{  // error: unsupported range
				cerr << "Error: unsupported range for measurement limit! DPS " << DPSpin << " max limit exceeds 1A"<< endl;
			    throw Error("Error", "Error in dpsComCode");
			}
		}
		else {
			return dps_setting.ganged * 1 A;  // for ganged DPS, the range is n*(+/-1A)
		}

	}
	else if(pintype == "DCS-DPS128HV" || pintype == "DCS-DPS64HV") // refer to TDC 144183
	{
		if(dps_setting.ganged == 1 || dps_setting.connect_state == "UNGANG") {  // unganged DPS
			if (maxlimit < 25 uA) return 25 uA;
			else if (maxlimit < 250 uA) return 250 uA;
			else if (maxlimit < 2.5 mA) return 2.5 mA;
			else if (maxlimit < 25 mA) return 25 mA;
			else if (maxlimit < 200 mA) return 200 mA;
			else{  // error: unsupported range
				cerr << "Error: unsupported range for measurement limit! DPS " << DPSpin << " max limit exceeds 200mA"<< endl;
			    throw Error("Error", "Error in dpsComCode");
			}
		}
		else {
			return dps_setting.ganged * 200 mA;  // for ganged DPS, the range is n*(+/-200mA)
		}
	}
	else if(pintype == "DCS-DPS32")  // refer to TDC 108000
	{
		if(dps_setting.ganged == 1 || dps_setting.connect_state == "UNGANG") {  // unganged DPS
			if (maxlimit < 100 uA && ((dps_setting.iout_clamp_rng == 100 uA) || (dps_setting.iout_clamp_rng == 1.5 A)) ) return 100 uA;
			else if (maxlimit < 2 mA && ((dps_setting.iout_clamp_rng == 2 mA) || (dps_setting.iout_clamp_rng == 1.5 A)) ) return 2 mA;
			else if (maxlimit < 50 mA && ((dps_setting.iout_clamp_rng == 50 mA) || (dps_setting.iout_clamp_rng == 1.5 A)) ) return 50 mA;
			else if (maxlimit < 1.5 A && (dps_setting.iout_clamp_rng >= 1.5 A) ) return 1.5 A; // for ganged dps, the iout_clamp_rng can be n*1.5 even in UNGANG mode
			else{  // error: unsupported range
				cerr << "Error: unsupported range for measurement limit! DPS " << DPSpin << " max limit exceeds 1.5A"<< endl;
			    throw Error("Error", "Error in dpsComCode");
			}
		}
		else {
			return dps_setting.ganged * 1.5 A;  // for ganged DPS, the range is n*(+/-1.5A)
		}
	}
	else if ((pintype == "DCS-UHC4T") || (pintype == "DCS-UHC4")) // refer to TDC 147086 and 106878
	{
		if(dps_setting.ganged == 1 || dps_setting.connect_state == "UNGANG") {  // unganged DPS
			if (maxlimit < 2 mA && dps_setting.iout_clamp_rng == 1.5 A ) return 2 mA;
			else if (maxlimit < 50 mA && dps_setting.iout_clamp_rng == 1.5 A ) return 50 mA;
			else if (maxlimit < 1.5 A && dps_setting.iout_clamp_rng == 1.5 A ) return 1.5 A;
			else if (maxlimit < 10 A && dps_setting.iout_clamp_rng == 10 A ) return 10 A;
			else if (maxlimit < 10 A && dps_setting.iout_clamp_rng == 40 A ) return 10 A;
			else if (maxlimit < 40 A && dps_setting.iout_clamp_rng == 40 A ) return 40 A;
			else{  // error: unsupported range
				cerr << "Error: unsupported range for measurement limit! DPS " << DPSpin << " max limit exceeds 40A"<< endl;
			    throw Error("Error", "Error in dpsComCode");
			}
		}
		else {
			return dps_setting.ganged * 40 A;  // for ganged DPS, the range is n*(+/-40A)
		}

	}
	else if(pintype == "DCS-VI32") // refer to TDC 108001
	{
		if(dps_setting.ganged == 1 || dps_setting.connect_state == "UNGANG") {  // unganged DPS
			if (maxlimit < 10 uA && ((dps_setting.iout_clamp_rng == 10 uA) || (dps_setting.iout_clamp_rng == 0.5 A)) ) return 10 uA;
			else if (maxlimit < 100 uA && ((dps_setting.iout_clamp_rng == 100 uA) || (dps_setting.iout_clamp_rng == 0.5 A)) ) return 100 uA;
			else if (maxlimit < 2 mA && ((dps_setting.iout_clamp_rng == 2 mA) || (dps_setting.iout_clamp_rng == 0.5 A)) ) return 2 mA;
			else if (maxlimit < 50 mA && ((dps_setting.iout_clamp_rng == 50 mA) || (dps_setting.iout_clamp_rng == 0.5 A)) ) return 50 mA;
			else if (maxlimit < 0.5 A && (dps_setting.iout_clamp_rng == 0.5 A) ) return 0.5 A;
			else{  // error: unsupported range
				cerr << "Error: unsupported range for measurement limit! DPS " << DPSpin << " max limit exceeds 0.5A"<< endl;
			    throw Error("Error", "Error in dpsComCode");
			}
		}
		else {
			return dps_setting.ganged * 0.5 A;  // for ganged DPS, the range is n*(+/-0.5A)
		}
	}
	else
	{
		cout << "Error: in calc_DPS_iMeasRange(), unsupported DPS card type: " << pintype << endl;
	    throw Error("Error", "Error in dpsComCode");
		return 0;
	}

	return 0;

}

double dpsComCode::calc_DPS_vMeasRange(const string & DPSpin) {
	string pintype;
	const DPS_SETTING &dps_setting = get_DPSsettingRef(DPSpin);
	pintype = dps_setting.pintype;

	if(pintype == "DCS-DPS128HC" || pintype == "DCS-DPS64HC")  // refer to TDC 142027
	{
		return 7.0;
	}
	else if(pintype == "DCS-DPS128HV" && pintype == "DCS-DPS64HV") // refer to TDC 144183
	{
		return 15.0;
	}
	else if(pintype == "DCS-DPS32")  // refer to TDC 108000
	{
		return 7.0;
	}
	else if ((pintype == "DCS-UHC4T") || (pintype == "DCS-UHC4")) // refer to TDC 147086 and 106878
	{
		return 4.0;
	}
	else if(pintype == "DCS-VI32") // refer to TDC 108001
	{
		return 25.0; // 2 ranges: 8.0V or 25.0V ; use 25.0V by default
	}
	else
	{
		cout << "Error: in calc_DPS_vMeasRange(), unsupported DPS card type: " << pintype << endl;
	    throw Error("Error", "Error in dpsComCode");
		return 0;
	}
	return 0;
}

void dpsComCode::read_DPS_Setting(const string & DPSpin, DPS_SETTING & dps_setting, int debug_mode) {  // get the DPS settings: ranges, ilimit etc.
	long eqn_no = Primary.getLevelSpec().getEquation();
	long spec_no = Primary.getLevelSpec().getSpec();
	// 100000*equation_set_no + 1000*specset_no+1
	long dps_set_id = 100000 * eqn_no + 1000 * spec_no + 1;

	//PSLR? {dps_No|dps_set_id}, {(pinlist)}
	//PSLR {dps_No|dps_set_id}, [vout_frc_rng], [iout_clamp_rng], [current_limiter,] [const_curr_check], [connect_state], {(pinlist)}
	//PSLV? {dps_No|dps_set_id}, {(pinlist)}
	//PSLV {dps_No|dps_set_id}, [voltage], [source_current], [sink_current,] [impedance], [setup_time], {(pinlist)}

	fwout.clear();
	fwresult.clear();
	fwout << "PSLR? " << dps_set_id << ",(" << DPSpin << ")"<< endl;  //PSLR? 1001001,(AVDD0P9)
	// cmd return: PSLR 1001001,7,0.002,ON,NORM,(AVDD0P9) // return example
	if(fwresult.size() == 0) { // error: no return of firmware command
		cerr << "Error: in read_DPS_Setting(), PSLR? command returns no result" << endl;
	    throw Error("Error", "Error in dpsComCode");
	}


	dps_setting.vout_frc_rng = fwresult[0].getDoubleParam(1);
	dps_setting.iout_clamp_rng = fwresult[0].getDoubleParam(2);
	if (fwresult[0].count() == 6) {  // no current_limiter
		dps_setting.const_curr_check = fwresult[0].getParam<string>(3);
		dps_setting.connect_state = fwresult[0].getParam<string>(4);
		dps_setting.dps_pinname = fwresult[0].getParam<string>(5);
	}
	else if (fwresult[0].count() == 7) { // include current_limiter
		dps_setting.current_limiter = fwresult[0].getParam<string>(3);
		dps_setting.const_curr_check = fwresult[0].getParam<string>(4);
		dps_setting.connect_state = fwresult[0].getParam<string>(5);
		dps_setting.dps_pinname = fwresult[0].getParam<string>(6);
	}
	else {  // error
		cerr << "Error: in read_DPS_Setting(), PSLR? command returns unexpected result" << endl;
	    throw Error("Error", "Error in dpsComCode");
	}


	fwout.clear();
	fwresult.clear();
	fwout << "PSLV? " << dps_set_id << ",(" << DPSpin << ")"<< endl;  //PSLV? 1001001,(AVDD0P9)
	// cmd return: PSLV 1001001,0.855,0.001,HIZ,2,(AVDD0P9)
	dps_setting.voltage = fwresult[0].getDoubleParam(1);
	dps_setting.source_current = fwresult[0].getDoubleParam(2);
	if (fwresult[0].count() == 6) { // no sink_current(when sink current is the same with source current)
		dps_setting.sink_current = dps_setting.source_current;
		dps_setting.impedance = fwresult[0].getParam<string>(3);
		dps_setting.setup_time = fwresult[0].getDoubleParam(4);
	}
	else if (fwresult[0].count() == 7) { // include sink_current(when sink current is different with source current)
		dps_setting.sink_current = fwresult[0].getDoubleParam(3);
		dps_setting.impedance = fwresult[0].getParam<string>(4);
		dps_setting.setup_time = fwresult[0].getDoubleParam(5);
	}
	else {  // error
		cerr << "Error: in read_DPS_Setting(), PSLV? command returns unexpected result" << endl;
	    throw Error("Error", "Error in dpsComCode");
	}
}


void dpsComCode::get_All_DPS_pins(vector<string> & all_names) {
	fwout.clear();
	fwresult.clear();
	fwout << "DFPS? (@)" << endl;  //
	// the return is like:
//	DFPS 22916,POS,(AVDD0P9)
//	DFPS (22913,22914,22915),POS,(DVDD0P9)
	all_names.resize(fwresult.size());
	for(unsigned int i = 0; i < fwresult.size(); i++) {
		cout << "DPS" << i << "  ";
		cout << fwresult[i][0] << " " << fwresult[i][1] << "  " << fwresult[i][2] << endl;

		all_names[i] = fwresult[i][2];
	}

}

void dpsComCode::read_All_DPS_Setting(int debug_mode) { // real settings of all dps pins
	read_All_DPS_Setting(all_dps_names, all_dps_settings, debug_mode);
}

void dpsComCode::read_All_DPS_Setting(vector<string> & all_names, map<string, DPS_SETTING> & all_settings, int debug_mode) { // real settings of all dps pins
	vector<unsigned int> all_ganged;
	DPS_SETTING setting;

//1. clear vector, map
	all_settings.clear();
	all_names.clear();

	fwout.clear();
	fwresult.clear();
	fwout << "DFPS? (@)" << endl;  // query all defined DPS pins
	// the return is like:
//	DFPS 22916,POS,(AVDD0P9)
//	DFPS (22913,22914,22915),POS,(DVDD0P9)
	all_names.resize(fwresult.size());
	all_ganged.resize(fwresult.size());
//2. extract DPS name and the ganged number
	if(debug_mode == 1) {
		cout << "Defined DPS are:" << endl;
	}
	for(unsigned int i = 0; i < fwresult.size(); i++) {
		if(debug_mode == 1) {
			cout << "DPS " << i << "  ";
			cout << fwresult[i][0] << " " << fwresult[i][1] << "  " << fwresult[i][2] << endl;
		}
		all_names[i] = fwresult[i][2];

		unsigned int ganged_num = 1;   // default: 1 DPS channel
		for(size_t j = 0; j < fwresult[i][0].size(); j++) {  // find the number of DPS channels
			j = fwresult[i][0].find(',', j);
			if(j == string::npos) {
				break;
			}
			else {
				ganged_num++;
			}
		}
		all_ganged[i] = ganged_num;
	}
//3. read DPS setting, one by one DPS
	for(unsigned int i = 0; i < all_names.size(); i++) {
		// attention: Firmware command operation which will change fwresult
		dpsComCode::read_DPS_Setting(all_names[i], setting, debug_mode);  // here, read DPS setting
		setting.pintype = PinUtility.getPinType(all_names[i].c_str());  // store pin type
		all_settings[all_names[i]] = setting;
		all_settings[all_names[i]].ganged = all_ganged[i];
	}
	if(debug_mode == 1) {
		cout << "DPS settings are:" << endl;
		for(unsigned int i = 0; i < all_names.size(); i++) {
			cout << "vout: " << all_names[i] << ": " << all_settings[all_names[i]].voltage << endl;
		}
		for(unsigned int i = 0; i < all_names.size(); i++) {
			cout << "source_current: " << all_names[i] << ": " << all_settings[all_names[i]].source_current << endl;
		}
		for(unsigned int i = 0; i < all_names.size(); i++) {
			cout << "iout_clamp_rng: " << all_names[i] << ": " << all_settings[all_names[i]].iout_clamp_rng << endl;
		}
		for(unsigned int i = 0; i < all_names.size(); i++) {
			cout << "connect_state: " << all_names[i] << ": " << all_settings[all_names[i]].connect_state << endl;
		}
	}


}


const DPS_SETTING & dpsComCode::get_DPSsettingRef(const string & dpsname){ // get dps setting reference
	return all_dps_settings[dpsname];
}


int dpsComCode::check_powershort_condition(const vector<string> & vDPSpins, const vector<double> & vUpperl, const vector<double> & vLowerl, int debug_mode) {
	double lowerl, upperl;
	double maxlimit;
	string DPSpin;
	DPS_SETTING setting;
	vector<DPS_SETTING *> vSettingPointer;

	for(unsigned int i = 0; i < vDPSpins.size(); i++ ) { // loop for all dps pins
		DPSpin = vDPSpins[i];
		lowerl = vLowerl[i];
		upperl = vUpperl[i];
		maxlimit = max(abs(lowerl), abs(upperl));

		const DPS_SETTING &dps_setting = get_DPSsettingRef(DPSpin);
		//DPS_SETTING dps_setting = all_dps_settings[DPSpin];


	// 0. Rule 0: the lower limit should not be too small, a small minus limit is because of the measurement accuracy.
		// the limit should not be too large, let's set 1A as the temporary maximum limit value;
		if(lowerl < -1.5 mA) {
			cerr << "Error: lower limit can not be smaller than -1.5mA! DPS " << DPSpin << " lower limit = " << lowerl << " A" << endl;
			throw Error("Error", "Error in dpsComCode");
		}
		if((upperl > 10 mA) && (lowerl < 0) ) {
			cerr << "Error: lower limit can not be smaller than 0 mA when upper limit is larger than 10mA! DPS " << DPSpin << " lower limit = " << lowerl << " A" << endl;
			throw Error("Error", "Error in dpsComCode");
		}

		if((dps_setting.pintype == "DCS-UHC4T") || (dps_setting.pintype == "DCS-UHC4")) {
			if((upperl > 2.0 A) || (upperl < 0 mA)) {
				cerr << "Error: upper limit can not be larger than 2.0A or lower than 0A for UHC4T! DPS " << DPSpin << " upper limit = " << upperl << " A" << endl;
				throw Error("Error", "Error in dpsComCode");
			}
		}
		else {
			if((upperl > 1.0 A) || (upperl < 0 mA)) {
				cerr << "Error: upper limit can not be larger than 1.0A or lower than 0A for Non-UHC4T! DPS " << DPSpin << " upper limit = " << upperl << " A" << endl;
				throw Error("Error", "Error in dpsComCode");
			}
		}


		// 1. Rule 1: The limit(measurement range) should be smaller than force range
		// because, the limit is for current measurement, and in 93K, the measurement range
		// must be smaller than the force range.
		if(maxlimit >= dps_setting.iout_clamp_rng) { // error
			  cerr << "Error: pass/fail limits are larger than iout_clamp_rng for DPS " << DPSpin << endl;
			  cerr << "Error: maximum pass/fail limit is " << maxlimit << ", iout_clamp_rng=" << dps_setting.iout_clamp_rng << endl;
			  throw Error("Error", "Error in dpsComCode");
		}
	// 2. Rule 2: Rule 2.1 the DPS.ilimit should be larger than the limit
		// because, if the ilimit is smaller, then the measurement value will be clamped
		// then failed dut would be judged as pass;
		if((upperl > 0 && upperl >= dps_setting.source_current)) {  // error   //
			  cerr << "Error: pass/fail limits are larger than ilimit for DPS " << DPSpin << endl;
			  cerr << "upper limit = "<< upperl << ", source ilimit = " << dps_setting.source_current  << endl;
			  cerr << "lower limit = "<< upperl << ", sink ilimit = " << dps_setting.sink_current  << endl;
			  throw Error("Error", "Error in dpsComCode");
		}
		// Rule 2.2: ilimit should be no larger than 1.5x limit to protect the hardware like probe card.
		// because if the ilimit is too large, the the short dut will cause large current which may damage the hardware
		if(dps_setting.source_current > 100 mA) {  // only for VDD of large power short current; to prevent hardware damage
			if((upperl > 0 && 1.5*upperl < dps_setting.source_current)) {  // error
				  cerr << "Error: ilimit value is too larger than pass/fail limits for DPS " << DPSpin << endl;
				  cerr << "upper limit = "<< upperl << ", source ilimit = " << dps_setting.source_current  << endl;
				  cerr << "lower limit = "<< upperl << ", sink ilimit = " << dps_setting.sink_current  << endl;
				  throw Error("Error", "Error in dpsComCode");
			}
		}

	// 3. Rule 3: ilimit must be smaller than iout_clamp_rng //
		// 93K will report an error if you set the ilimit larger than iout_clamp_rng


		// 6. Rule 6: DPS must be set to ungang mode
		if(dps_setting.connect_state != "UNGANG" ) {
			cerr << "Error: DPS must be in UNGANG mode in power short test! \n"
					<< "DPS " << vDPSpins[i] << " is in ganged mode" << endl;
			throw Error("Error", "Error in dpsComCode");
		}

		// 7. Rule 7: for UHC4/UHC4T, the iout_clamp_rng must be smaller than 1.5A
		// (maybe not suitable in the future, if the chip size is very large and power short current exceeds 1.5A in high temperature
		// because the measurement accuracy of 10/40A is +/-50mA which is too larger for powershort test
		if((dps_setting.pintype == "DCS-UHC4T") || (dps_setting.pintype == "DCS-UHC4")) {
			if( (upperl < 1.5 A) && (dps_setting.iout_clamp_rng > 1.5 A) ) {  // small upper limit should be with precision mode
				cerr << "Error: UHC4/UHC4T must be in precision mode if expected value is under 1.5A! iout_clamp_rng should be 1.5A/50mA/2mA\n"
						<< "Error: DPS " << vDPSpins[i] << " has wrong iout_clamp_rng=" << dps_setting.iout_clamp_rng << endl;
				throw Error("Error", "Error in dpsComCode");
			}
		}

	}
	// 4. Rule 4: voltages of DPS under test can not be zero, and can not be the same
	vector<double> voltages;
	voltages.resize(vDPSpins.size());
	for(unsigned int i = 0; i < vDPSpins.size(); i++ ) {
		voltages[i] = all_dps_settings[vDPSpins[i]].voltage;
		vSettingPointer.push_back(&all_dps_settings[vDPSpins[i]]);  // store the reference/ may not work
	}
	for(unsigned int i = 0; i < voltages.size(); i++) {
		if(voltages[i] == 0) {
			cerr << "Error: voltages of DPS can not be zero in power short test! "
					<< "DPS " << vDPSpins[i] << " has zero voltage " <<  endl;
			throw Error("Error", "Error in dpsComCode");
		}
		for(unsigned int j = i+1; j < voltages.size(); j++) {
			if(voltages[i] == voltages[j]) {
				cerr << "Error: voltages of DPS can not be the same in power short test! \n"
						<< "DPS " << vDPSpins[i] << " and DPS " << vDPSpins[j] << " have the same voltage " << voltages[i]
						<< "\n you can set voltages like 0.1, 0.11, 0.12"<< endl;
				throw Error("Error", "Error in dpsComCode");
			}
		}
	}

	// 5. Rule 5: voltages of DPS not tested should be set to 0 V
	vector<string> other_dps;
	vector<double> other_voltages;
	for(unsigned int i = 0; i < all_dps_names.size(); i++) {
		int isflag = 0;
		for(unsigned int j = 0; j < vDPSpins.size(); j++) {
			if(all_dps_names[i] == vDPSpins[j]) {
				isflag = 1;
				break;
			}
		}
		if(isflag == 0) {
			other_dps.push_back(all_dps_names[i]);
		}
	}
	other_voltages.resize(other_dps.size()); // resize the vector
	for(unsigned int i = 0; i < other_dps.size(); i++ ) {
		other_voltages[i] = all_dps_settings[other_dps[i]].voltage;
		if(other_voltages[i] != 0) {
			cerr << "Error: voltages of DPS which are not under test should be zero in power short test! "
					<< "DPS " << other_dps[i] << " has non-zero voltage " <<  endl;
			throw Error("Error", "Error in dpsComCode");
		}
	}

	return 0;  // no error

}

int dpsComCode::check_OS_condition(int debug_mode)
{
	map<string, DPS_SETTING>::iterator it;
	for(it = all_dps_settings.begin(); it != all_dps_settings.end(); it++)
	{
		if(it->second.voltage != 0)
		{
			cerr << "Error: DPS must be set to 0V in OS test!" << endl;
			throw(Error("Error", "Error in OS test"));
		}
	}

	return 0;
}

// measure all DPS voltage
// call this method in DPS connect mode
void dpsComCode::measure_voltage(double wait_s) {
	DPS_TASK dpsTask;
	for(unsigned int i = 0; i < all_dps_names.size(); i++) {
		// DPS128/64: -2.5V~7V; DPS32: 0~7V; UHC4/UHC4T: 0~4V // so use 4V as max value
		dpsTask.pin(all_dps_names[i]).min(0 V).max(4 V).measurementMode(TM::MEASURE_VOLTAGE);
	}
	dpsTask.wait(wait_s).samples(8).execMode(TM::PVAL);
	dpsTask.execute();

	int site_index;
	FOR_EACH_SITE_BEGIN();
		site_index = CURRENT_SITE_NUMBER() - 1;
		for(unsigned int i = 0; i < all_dps_names.size(); i++) {
			measuredVolt[all_dps_names[i]][site_index] = dpsTask.getValue(all_dps_names[i]);
			cout << "site: " << site_index + 1 << " Measured Voltage: " <<  all_dps_names[i] << ": " << measuredVolt[all_dps_names[i]][site_index] << endl;
			// DPS128/64/32: Force Accuracy: +/-3mV; Measure Accuracy: +/-2mV
			// UHC4/UHC4T: Force Accuracy: +/-2.5mV; Measure Accuracy: +/-2mV
			if(abs(measuredVolt[all_dps_names[i]][site_index] - all_dps_settings[all_dps_names[i]].voltage) > 10 mV) {  //
				cerr << "Error: The difference between the measured voltage and the expected setting is larger than 10mV. DPS: " << all_dps_names[i] << endl;
			}
		}
	FOR_EACH_SITE_END();

}

void dpsComCode::measure_voltage_waveform(const string &dpsPinList, double interval_s) {
	//DC Profiling Voltage Measurement
	map<string, map<int, ARRAY_D> > values;
	DC_RESULT_ACCESSOR profileResult;
	ostringstream sstr;

	DC_PROFILING_ON(dpsPinList, TM::VOLTAGE, 7 V, interval_s, 1, TM::RESTORE_RANGE);
	FUNCTIONAL_TEST();
	DC_PROFILING_OFF(dpsPinList);
	profileResult.uploadResult(dpsPinList, TM::RESULT_INDEX);

	vector<string> vpins = PinUtility.getDpsPinNamesFromPinList(dpsPinList, TRUE);
	int site_index;
	FOR_EACH_SITE_BEGIN();
		site_index = CURRENT_SITE_NUMBER() - 1;
		for(unsigned int i = 0; i < vpins.size(); i++) {
			//values[vpins[i]];
			values[vpins[i]][site_index] = profileResult.getValues(vpins[i]);
			WAVE_LOG log_obj(values[vpins[i]][site_index]);
			sstr << "Sampling interval " << interval_s / unitAsDouble("us") << "us";  // unit us
			log_obj.xTitle(sstr.str());
			log_obj.yTitle("Voltage");
			log_obj.yUnit("V");
			PUT_DEBUG(vpins[i], "Voltage Waveform of "  + vpins[i], log_obj);
			sstr.clear();  // clear the string buffer
		}
	FOR_EACH_SITE_END();
}


