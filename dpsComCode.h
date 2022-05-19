
//**************************************************************************************************
//*  Author:      ZTE - Xi'an, China  liguangyao
//*  Description: DPS common code
//*  SmarTest:    Recommend 7.2 and above version
//*  Functions:
#include "TestTableMgr.hpp"
#include <string>
#include <vector>
#include <map>
#include "mapi.hpp"
using namespace std;

//PSLR? {dps_No|dps_set_id}, {(pinlist)}
//PSLR {dps_No|dps_set_id}, [vout_frc_rng], [iout_clamp_rng], [current_limiter,] [const_curr_check], [connect_state], {(pinlist)}
//PSLV? {dps_No|dps_set_id}, {(pinlist)}
//PSLV {dps_No|dps_set_id}, [voltage], [source_current], [sink_current,] [impedance], [setup_time], {(pinlist)}

struct DPS_SETTING {
	long dps_No;
	long dps_set_id;
	double vout_frc_rng;  // unit V
	double iout_clamp_rng; // unit A   // attention: in 93K level file, the unit is mA
	string current_limiter;
	string const_curr_check;
	string connect_state;
	double voltage;  // unit V
	double source_current;  // unit A
	double sink_current;    // unit A
	string impedance;
	double setup_time;      // unit ms
	string dps_pinname;
	unsigned int ganged; // ganged number of DPS
	string pintype; // DPS type
};



class dpsComCode {

private:
	map<string, DPS_SETTING> all_dps_settings;      // map index is the DPS name
	//vector<DPS_SETTING> dps_setting; //
	vector<string> all_dps_names; // dps pin names
	map<string, map<int, double> > measuredVolt; // measured voltage, DPS name -> site number -> voltage

public:
	dpsComCode();
	virtual ~dpsComCode();


	static double calc_DPS_iMeasRange(const string &, double, double, DPS_SETTING &);
	double calc_DPS_iMeasRange(const string &, double, double);
	double calc_DPS_vMeasRange(const string &);
	static void read_DPS_Setting(const string &, DPS_SETTING &, int debug_mode=0);  // get the DPS settings: ranges, ilimit etc.

	void get_All_DPS_pins(vector<string> & all_names);  // get all dps pin names
	void read_All_DPS_Setting(vector<string> &, map<string, DPS_SETTING> &, int debug_mode=0);  // real settings of all dps pins
	void read_All_DPS_Setting(int debug_mode=0);  // real settings of all dps pins
	//void refresh_all_settings(); // refresh DPS settings
	const DPS_SETTING & get_DPSsettingRef(const string & dpsname); // get dps setting reference

	int check_powershort_condition(const vector<string> &, const vector<double> &, const vector<double> &, int debug_mode=0);
	int check_OS_condition(int debug_mode=0);

	// dummy test methods
//	int powershort_test();
//	int iddq_test();
//	void measure_current();
	// measure voltage of all defined DPS; 1st argument: wait time in ms before measurement
	// must be called within ON_FIRST_INVOCATION block
	void measure_voltage(double wait_s);
	// sample voltage while running function test
	// must be called within ON_FIRST_INVOCATION block
	void measure_voltage_waveform(const string &dpsPinList, double interval_s);
	// sample current while running function test
	// must be called within ON_FIRST_INVOCATION block
	void measure_current_waveform(const string &dpsPinList, double interval_s);
};
