#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;
#include "../d2s_tml/d2s_PS_JTAG.h"
#include "../d2s_tml/d2s_DFT_JTAG.h"
#include "../d2s_tml/d2s_CPU_JTAG.h"
#include "../d2s_tml/generic_d2s/d2sFramework.h"
using namespace V93kLimits;
/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */


class cpr_sensor: public testmethod::TestMethod {

protected:
	int 		D2SMode;
	int			Debug_Mode;
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
		 addParameter("D2SMode", "int", &D2SMode)
					   .setDefault("1")
					   .setOptions("1:2")
					   .setComment("d2s mode  1cpr_sensor:LearningMode, 2:ProductionMode");
		  	addParameter("Debug_Mode", "int", &Debug_Mode)
		                 .setDefault("1")
		                 .setOptions("0:1")
		                 .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

  }

  /**
   *This test is invoked per site.
   */
	virtual void run() {

		d2sFramework fr_dft,fr_cpu,fr_ps;
		d2s_ps_jtag& myD2S_ps = d2s_ps_jtag::Instance();
		d2s_dft_jtag& myD2S_dft = d2s_dft_jtag::Instance();
		d2s_cpu_jtag& myD2S_cpu = d2s_cpu_jtag::Instance();

		fr_dft.registerTransactionPort(myD2S_dft);
		fr_ps.registerTransactionPort(myD2S_ps);
		fr_cpu.registerTransactionPort(myD2S_cpu);

		d2sFrameWorkModeType::Enum d2s_mode;

		switch ( D2SMode ){
				case 0:
				case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
				case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
			}

		STRING TestsuitName;
		GET_TESTSUITE_NAME(TestsuitName);

		ON_FIRST_INVOCATION_BEGIN();

			CONNECT();

//			fr_dft.d2s_LABEL_BEGIN("ps_cpr_sensor_sel",d2s_mode);//add
//				myD2S_dft.execLabel("dft_jtag_reset",9);
//				myD2S_dft.write(0x1B5,0xf);//ps mode selected
//				myD2S_dft.wait(10 us);
//				myD2S_dft.write(0x1a4,1) ;//ps mode reset
//				myD2S_dft.wait(10 us);
////				if(Debug_Mode){
////
////					myD2S_dft.read(0x1a4,"0x1a4");
////					myD2S_dft.read(0x1b5,"0x1B5");
////
////				}
//
//			fr_dft.d2s_LABEL_END();
//
//			fr_cpu.d2s_LABEL_BEGIN("ps_cpr_sensor_init",d2s_mode);//add
//				myD2S_cpu.write(0x024,1);//cpuperip mode
//				myD2S_cpu.wait(10 us);
//
//				myD2S_cpu.write(0x004,1);//a53 mode
//				myD2S_cpu.wait(10 us);
//
//				myD2S_cpu.write(0x021,0);//PS JTAG IR select
//
//				myD2S_cpu.wait(10 us);
////				if(Debug_Mode){
////					myD2S_cpu.read(0x024,"0x024");
////					myD2S_cpu.read(0x004,"0x004");
////					myD2S_cpu.read(0x021,"0x021");
////				}
//			fr_cpu.d2s_LABEL_END();
//
			fr_ps.d2s_LABEL_BEGIN("ps_cpr_sensor_cfg",d2s_mode);//add

				myD2S_ps.execLabel("ps_cpr_sensor_sel",539);
				myD2S_ps.execLabel("ps_cpr_sensor_init",794);

				myD2S_ps.write(0x028020c0+0x4,0x1);//enable testpoint reg:SPOT_SEL
				myD2S_ps.wait(10 us);


				myD2S_ps.write(0x028020c0+0x8,0x10);//enable CPR sensor reg:SENS_SEL
				myD2S_ps.wait(10 us);


				myD2S_ps.write(0x028020c0+0x40,0xc8);//enable 24MHz ref clk counter reg:REF_COUNTER
				myD2S_ps.wait(10 us);


				myD2S_ps.write(0x028020c0+0x48,0x4);//enable PS SENSE reg:SENS_RUN
				myD2S_ps.wait(10 us);

//				if(Debug_Mode){

					myD2S_ps.read(0x028020c4,"0x028020c4_1");
					myD2S_ps.wait(10 us);
					myD2S_ps.read(0x028020c4,"0x028020c4");

					myD2S_ps.read(0x028020c8,"0x028020c8_1");
					myD2S_ps.wait(10 us);
					myD2S_ps.read(0x028020c8,"0x028020c8");

					myD2S_ps.read(0x028020c0+0x40,"0x02802100_1");
					myD2S_ps.wait(10 us);
					myD2S_ps.read(0x028020c0+0x40,"0x02802100");

					myD2S_ps.read(0x028020c0+0x48,"0x02802108_1");
					myD2S_ps.wait(10 us);
					myD2S_ps.read(0x028020c0+0x48,"0x02802108_1");
					myD2S_ps.wait(10 us);
					myD2S_ps.read(0x028020c0+0x48,"0x02802108");
					myD2S_ps.wait(10 us);

//				}

				myD2S_ps.read(0x028020c0+0x4c,"ready_1");//read REG:READY
				myD2S_ps.wait(10 us);
				myD2S_ps.read(0x028020c0+0x4c,"ready");//read REG:READY
				myD2S_ps.wait(10 us);

				myD2S_ps.read(0x028020c0+0x58,"SPOT0_1");//read counts and overflow flag
				myD2S_ps.wait(10 us);
				myD2S_ps.read(0x028020c0+0x58,"SPOT0");//read counts and overflow flag



				myD2S_ps.write(0x028020c0+0x4c,0x0);//clear ready REG:READY

			fr_ps.d2s_LABEL_END();

		ON_FIRST_INVOCATION_END();

		if(Debug_Mode){
//			cout << "0x1B5 : "<< dec << myD2S_dft.getReadValue("0x1B5") <<endl; //
//			cout << "0x1a4 : "<< dec << myD2S_dft.getReadValue("0x1a4") <<endl; //
//			cout << "0x024 : "<< dec << myD2S_cpu.getReadValue("0x024") <<endl; //
//			cout << "0x004 : "<< dec << myD2S_cpu.getReadValue("0x004") <<endl; //
//			cout << "0x021 : "<< dec << myD2S_cpu.getReadValue("0x021") <<endl; //
			cout << "REG 0x028020c4 :0x"<< hex << myD2S_ps.getReadValue("0x028020c4") <<endl; //
			cout << "REG 0x028020c8 :0x"<< hex << myD2S_ps.getReadValue("0x028020c8") <<endl; //
			cout << "REG 0x02802100 :0x"<< hex << myD2S_ps.getReadValue("0x02802100") <<endl; //
			cout << "REG 0x02802108 :0x"<< hex << myD2S_ps.getReadValue("0x02802108") <<endl; //
//			cout << "REG READY :0x"<< hex << myD2S_ps.getReadValue("ready") <<endl; //
			cout << "REG READY :0x"<< hex << myD2S_ps.getReadValue("ready") <<endl; //
//			cout << "REG SPOT0 :0x"<< hex << myD2S_ps.getReadValue("SPOT0") <<endl; //
			cout << "REG COUNT :0x"<< hex << (myD2S_ps.getReadValue("SPOT0")& 0xffff) <<endl; //
			cout << "REG OVERFLOW :0x"<< hex << (myD2S_ps.getReadValue("SPOT0")>>18 & 0x1) <<endl; //

		}
//

		TESTSET().cont(TRUE).judgeAndLog_ParametricTest("",TestsuitName,tmLimits,myD2S_ps.getReadValue("SPOT0")& 0xffff);
		return;
	}

  /**
   *This function will be invoked once the specified parameter's value is changed.
   *@param parameterIdentifier
   */
  virtual void postParameterChange(const string& parameterIdentifier)
  {
    //Add your code
    //Note: Test Method API should not be used in this method!
    return;
  }

  /**
   *This function will be invoked once the Select Test Method Dialog is opened.
   */
  virtual const string getComment() const
  {
    string comment = " please add your comment for this method.";
    return comment;
  }
};
REGISTER_TESTMETHOD("bm_ac_tml.cpr_sensor", cpr_sensor);

class ringo_svt_sensor: public testmethod::TestMethod {

protected:
	int 		D2SMode;
	int			Debug_Mode;
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
		 addParameter("D2SMode", "int", &D2SMode)
					   .setDefault("1")
					   .setOptions("1:2")
					   .setComment("d2s mode  1cpr_sensor:LearningMode, 2:ProductionMode");
		  	addParameter("Debug_Mode", "int", &Debug_Mode)
		                 .setDefault("1")
		                 .setOptions("0:1")
		                 .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

  }

  /**
   *This test is invoked per site.
   */
	virtual void run() {

		d2sFramework fr_dft,fr_cpu,fr_ps;
		d2s_ps_jtag& myD2S_ps = d2s_ps_jtag::Instance();
		d2s_dft_jtag& myD2S_dft = d2s_dft_jtag::Instance();
		d2s_cpu_jtag& myD2S_cpu = d2s_cpu_jtag::Instance();

		fr_dft.registerTransactionPort(myD2S_dft);
		fr_ps.registerTransactionPort(myD2S_ps);
		fr_cpu.registerTransactionPort(myD2S_cpu);

		d2sFrameWorkModeType::Enum d2s_mode;

		switch ( D2SMode ){
				case 0:
				case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
				case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
			}

		STRING TestsuitName;
		GET_TESTSUITE_NAME(TestsuitName);

		ON_FIRST_INVOCATION_BEGIN();

			CONNECT();

			fr_ps.d2s_LABEL_BEGIN("ps_ringo_svt_sensor_cfg",d2s_mode);//add

				myD2S_ps.execLabel("ps_cpr_sensor_sel",539);
				myD2S_ps.execLabel("ps_cpr_sensor_init",794);

				myD2S_ps.write(0x028020c0+0x4,0x1);//enable testpoint reg:SPOT_SEL
				myD2S_ps.wait(10 us);


				myD2S_ps.write(0x028020c0+0x8,0x0A01);//enable Ringo SVT sensor reg:SENS_SEL
				myD2S_ps.wait(10 us);


				myD2S_ps.write(0x028020c0+0x40,0x200);//enable 24MHz ref clk counter reg:REF_COUNTER
				myD2S_ps.wait(10 us);


				myD2S_ps.write(0x028020c0+0x48,0x4);//enable PS SENSE reg:SENS_RUN
				myD2S_ps.wait(10 us);

//				if(Debug_Mode){

					myD2S_ps.read(0x028020c4,"0x028020c4_1");
					myD2S_ps.wait(10 us);
					myD2S_ps.read(0x028020c4,"0x028020c4");

					myD2S_ps.read(0x028020c8,"0x028020c8_1");
					myD2S_ps.wait(10 us);
					myD2S_ps.read(0x028020c8,"0x028020c8");

					myD2S_ps.read(0x028020c0+0x40,"0x02802100_1");
					myD2S_ps.wait(10 us);
					myD2S_ps.read(0x028020c0+0x40,"0x02802100");

					myD2S_ps.read(0x028020c0+0x48,"0x02802108_1");
					myD2S_ps.wait(10 us);
					myD2S_ps.read(0x028020c0+0x48,"0x02802108_1");
					myD2S_ps.wait(10 us);
					myD2S_ps.read(0x028020c0+0x48,"0x02802108");
					myD2S_ps.wait(10 us);

//				}

				myD2S_ps.read(0x028020c0+0x4c,"ready_1");//read REG:READY
				myD2S_ps.wait(10 us);
				myD2S_ps.read(0x028020c0+0x4c,"ready");//read REG:READY
				myD2S_ps.wait(10 us);

				myD2S_ps.read(0x028020c0+0x50,"count_1");//read counts [15:0]
				myD2S_ps.wait(10 us);
				myD2S_ps.read(0x028020c0+0x50,"count");//read counts [15:0]

				myD2S_ps.read(0x028020c0+0x58,"overflow_1");//read overflow [16]
				myD2S_ps.wait(10 us);
				myD2S_ps.read(0x028020c0+0x58,"overflow");//read overflow [16]



				myD2S_ps.write(0x028020c0+0x4c,0x0);//clear ready REG:READY

			fr_ps.d2s_LABEL_END();

		ON_FIRST_INVOCATION_END();

		if(Debug_Mode){

			cout << "REG 0x028020c4 :0x"<< hex << myD2S_ps.getReadValue("0x028020c4") <<endl; //
			cout << "REG 0x028020c8 :0x"<< hex << myD2S_ps.getReadValue("0x028020c8") <<endl; //
			cout << "REG 0x02802100 :0x"<< hex << myD2S_ps.getReadValue("0x02802100") <<endl; //
			cout << "REG 0x02802108 :0x"<< hex << myD2S_ps.getReadValue("0x02802108") <<endl; //
			cout << "REG READY :0x"<< hex << myD2S_ps.getReadValue("ready") <<endl; //
			cout << "REG COUNT :0x"<< hex <<( myD2S_ps.getReadValue("count") & 0xffff) <<endl; //
			cout << "REG OVERFLOW :0x"<< hex << (myD2S_ps.getReadValue("overflow")>>16 & 0x1) <<endl; //

		}
//

		TESTSET().cont(TRUE).judgeAndLog_ParametricTest("",TestsuitName,tmLimits,myD2S_ps.getReadValue("count") & 0xffff);
		return;
	}

  /**
   *This function will be invoked once the specified parameter's value is changed.
   *@param parameterIdentifier
   */
  virtual void postParameterChange(const string& parameterIdentifier)
  {
    //Add your code
    //Note: Test Method API should not be used in this method!
    return;
  }

  /**
   *This function will be invoked once the Select Test Method Dialog is opened.
   */
  virtual const string getComment() const
  {
    string comment = " please add your comment for this method.";
    return comment;
  }
};
REGISTER_TESTMETHOD("bm_ac_tml.ringo_svt_sensor", ringo_svt_sensor);

class ringo_hvt_sensor: public testmethod::TestMethod {

protected:
	int 		D2SMode;
	int			Debug_Mode;
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
		 addParameter("D2SMode", "int", &D2SMode)
					   .setDefault("1")
					   .setOptions("1:2")
					   .setComment("d2s mode  1cpr_sensor:LearningMode, 2:ProductionMode");
		  	addParameter("Debug_Mode", "int", &Debug_Mode)
		                 .setDefault("1")
		                 .setOptions("0:1")
		                 .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

  }

  /**
   *This test is invoked per site.
   */
	virtual void run() {

		d2sFramework fr_dft,fr_cpu,fr_ps;
		d2s_ps_jtag& myD2S_ps = d2s_ps_jtag::Instance();
		d2s_dft_jtag& myD2S_dft = d2s_dft_jtag::Instance();
		d2s_cpu_jtag& myD2S_cpu = d2s_cpu_jtag::Instance();

		fr_dft.registerTransactionPort(myD2S_dft);
		fr_ps.registerTransactionPort(myD2S_ps);
		fr_cpu.registerTransactionPort(myD2S_cpu);

		d2sFrameWorkModeType::Enum d2s_mode;

		switch ( D2SMode ){
				case 0:
				case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
				case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
			}

		STRING TestsuitName;
		GET_TESTSUITE_NAME(TestsuitName);

		ON_FIRST_INVOCATION_BEGIN();

			CONNECT();


			fr_ps.d2s_LABEL_BEGIN("ps_ringo_hvt_sensor_cfg",d2s_mode);//add

				myD2S_ps.execLabel("ps_cpr_sensor_sel",539);
				myD2S_ps.execLabel("ps_cpr_sensor_init",794);


				myD2S_ps.write(0x028020c0+0x4,0x1);//enable testpoint reg:SPOT_SEL
				myD2S_ps.wait(10 us);


				myD2S_ps.write(0x028020c0+0x8,0x0602);//enable Ringo HVT sensor reg:SENS_SEL
				myD2S_ps.wait(10 us);


				myD2S_ps.write(0x028020c0+0x40,0x0600);//enable 24MHz ref clk counter reg:REF_COUNTER
				myD2S_ps.wait(10 us);


				myD2S_ps.write(0x028020c0+0x48,0x4);//enable PS SENSE reg:SENS_RUN
				myD2S_ps.wait(10 us);

//				if(Debug_Mode){

					myD2S_ps.read(0x028020c4,"0x028020c4_1");
					myD2S_ps.wait(10 us);
					myD2S_ps.read(0x028020c4,"0x028020c4");

					myD2S_ps.read(0x028020c8,"0x028020c8_1");
					myD2S_ps.wait(10 us);
					myD2S_ps.read(0x028020c8,"0x028020c8");

					myD2S_ps.read(0x028020c0+0x40,"0x02802100_1");
					myD2S_ps.wait(10 us);
					myD2S_ps.read(0x028020c0+0x40,"0x02802100");

					myD2S_ps.read(0x028020c0+0x48,"0x02802108_1");
					myD2S_ps.wait(10 us);
					myD2S_ps.read(0x028020c0+0x48,"0x02802108_1");
					myD2S_ps.wait(10 us);
					myD2S_ps.read(0x028020c0+0x48,"0x02802108");
					myD2S_ps.wait(10 us);

//				}

				myD2S_ps.read(0x028020c0+0x4c,"ready_1");//read REG:READY
				myD2S_ps.wait(10 us);
				myD2S_ps.read(0x028020c0+0x4c,"ready");//read REG:READY
				myD2S_ps.wait(10 us);

				myD2S_ps.read(0x028020c0+0x50,"count_1");//read counts [31:16]
				myD2S_ps.wait(10 us);
				myD2S_ps.read(0x028020c0+0x50,"count");//read counts [31:16]

				myD2S_ps.read(0x028020c0+0x58,"overflow_1");//read overflow [17]
				myD2S_ps.wait(10 us);
				myD2S_ps.read(0x028020c0+0x58,"overflow");//read overflow [17]



				myD2S_ps.write(0x028020c0+0x4c,0x0);//clear ready REG:READY

			fr_ps.d2s_LABEL_END();

		ON_FIRST_INVOCATION_END();

		if(Debug_Mode){

			cout << "REG 0x028020c4 :0x"<< hex << myD2S_ps.getReadValue("0x028020c4") <<endl; //
			cout << "REG 0x028020c8 :0x"<< hex << myD2S_ps.getReadValue("0x028020c8") <<endl; //
			cout << "REG 0x02802100 :0x"<< hex << myD2S_ps.getReadValue("0x02802100") <<endl; //
			cout << "REG 0x02802108 :0x"<< hex << myD2S_ps.getReadValue("0x02802108") <<endl; //

			cout << "REG READY :0x"<< hex << myD2S_ps.getReadValue("ready") <<endl; //
			cout << "REG COUNT :0x"<< hex << (myD2S_ps.getReadValue("count")>>16 & 0xffff) <<endl; //
			cout << "REG OVERFLOW :0x"<< hex << (myD2S_ps.getReadValue("overflow")>>17 & 0x1) <<endl; //

		}
		TESTSET().cont(TRUE).judgeAndLog_ParametricTest("",TestsuitName,tmLimits,myD2S_ps.getReadValue("count")>>16 & 0xffff);
		return;
	}

  /**
   *This function will be invoked once the specified parameter's value is changed.
   *@param parameterIdentifier
   */
  virtual void postParameterChange(const string& parameterIdentifier)
  {
    //Add your code
    //Note: Test Method API should not be used in this method!
    return;
  }

  /**
   *This function will be invoked once the Select Test Method Dialog is opened.
   */
  virtual const string getComment() const
  {
    string comment = " please add your comment for this method.";
    return comment;
  }
};
REGISTER_TESTMETHOD("bm_ac_tml.ringo_hvt_sensor", ringo_hvt_sensor);

