/*
 * hdmi_cap.cpp
 *
 *  Created on: May 21, 2016
 *      Author: Bobo@Xi'an
 */

#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
#include "pmicComCode.h"
using namespace std;
using namespace V93kLimits;

/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class hdmi_cap_9G: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   *
   *Note: Test Method API should not be used in this method.
   */
	int Debug_Mode;
	int M_Cap,N_Code;
	int Loop_xN;
	string freq_type;

  virtual void initialize()
  {
    //Add your initialization code here
	  addParameter("Debug_Mode", "int", &Debug_Mode)
	                 .setDefault("1")
	                 .setOptions("0:1")
	                 .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");
      addParameter("sampleSize_pts",
                   "int",
                   &M_Cap,
                   testmethod::TM_PARAMETER_INPUT)
      	  	  	  .setDefault("8192")
      	  	  	  .setOptions("8192");
      addParameter("code_length",
                   "int",
                   &N_Code,
                   testmethod::TM_PARAMETER_INPUT)
				  .setDefault("5120")
				  .setOptions("1024:5120") //5120=1024 x 5
				  .setComment("low_freq:5120,hgih_freq:1024");

      addParameter("freq_type",
                   "string",
                   &freq_type,
                   testmethod::TM_PARAMETER_INPUT)
				  .setDefault("low_freq")
				  .setOptions("low_freq:high_freq");
	  addParameter("Loop_xN", "int", &Loop_xN)
	                 .setDefault("10")
	                 .setComment("Loop N times");
  }
  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
	  static ARRAY_I		Cap_TX0N[10],Cap_TX1N[10],Cap_TX2N[10];
	  static ARRAY_I		Cap_TX0P[10],Cap_TX1P[10],Cap_TX2P[10];

	  static ARRAY_I 		Expect_Code;


	  int					Flag_Pass=0;
	  int					Flag_TX0N=0,Flag_TX1N=0,Flag_TX2N=0;
	  int					Flag_TX0P=0,Flag_TX1P=0,Flag_TX2P=0;

	  static string 		VarCap_0N = "9G_HDMI_O_TX0N",VarCap_1N = "9G_HDMI_O_TX1N",VarCap_2N = "9G_HDMI_O_TX2N";//Capture Variable
	  static string 		VarCap_0P = "9G_HDMI_O_TX0P",VarCap_1P = "9G_HDMI_O_TX1P",VarCap_2P = "9G_HDMI_O_TX2P";//Capture Variable

	  static string 		sTsName;


			/********************************************************************************************
			 * move the strobe 10 times and Capture the data if 1 time is ok, set the test pass
			 ********************************************************************************************/
//============== initial the array ==========================//
	  for(int i=0;i<10;i++){
		  Cap_TX0N[i].resize(M_Cap);
		  Cap_TX0P[i].resize(M_Cap);
		  Cap_TX1N[i].resize(M_Cap);
		  Cap_TX1P[i].resize(M_Cap);
		  Cap_TX2N[i].resize(M_Cap);
		  Cap_TX2P[i].resize(M_Cap);

		  Cap_TX0N[i].init(0);
		  Cap_TX0P[i].init(0);
		  Cap_TX1N[i].init(0);
		  Cap_TX1P[i].init(0);
		  Cap_TX2N[i].init(0);
		  Cap_TX2P[i].init(0);
	  }
	  Expect_Code.resize(N_Code);
	  Expect_Code.init(0);

	  if(freq_type=="low_freq"){
		  // the expected code 0000011111..0000011111
		  	  int Flag=0;
		  	  for(int i=0;i<N_Code;i++){
		  		  if(i%5==0){
		  			  Flag++;
		  			  if(Flag%2==1){
		  				Expect_Code[i]=0;
		  			  }
		  			  else{
		  				Expect_Code[i]=1;
		  			  }
		  		  }
		  		  else{
		  			  Expect_Code[i]=Expect_Code[i-1];
		  		  }
		  	  }
	  }
	  else if(freq_type=="high_freq"){
		  // the expected code 01010101..01

		  	  for(int i=0;i<N_Code;i++){
		  		  if(i%2==0){
		  			  Expect_Code[i]=0;
		  		  }
		  		  else{
		  			  Expect_Code[i]=1;
		  		  }
		  	  }
	  }

	  if(Debug_Mode){
		cout<<" Expect_Code"<<"="<<Expect_Code<<endl;

	  }

	  GET_TESTSUITE_NAME(sTsName);

	  double period = 1000/Primary.getSpecification().getSpecValue("Freq@p9G_HDMI_DATA")/8;//8 strobes


	  ON_FIRST_INVOCATION_BEGIN();
		  CONNECT();
	  ON_FIRST_INVOCATION_END();

	  for(int i = 0;i<Loop_xN;i++){
		  ON_FIRST_INVOCATION_BEGIN();

			  Primary.getSpecification().change("offset@p9G_HDMI_DATA",i*period/Loop_xN);
			  FLUSH(TM::APRM);
			  DIGITAL_CAPTURE_TEST();
		  ON_FIRST_INVOCATION_END();

		  Cap_TX0N[i] = VECTOR(VarCap_0N).getVectors();
		  Cap_TX0P[i] = VECTOR(VarCap_0P).getVectors();
		  Cap_TX1N[i] = VECTOR(VarCap_1N).getVectors();
		  Cap_TX1P[i] = VECTOR(VarCap_1P).getVectors();
		  Cap_TX2N[i] = VECTOR(VarCap_2N).getVectors();
		  Cap_TX2P[i] = VECTOR(VarCap_2P).getVectors();
		  if(Debug_Mode){
			cout<<"Cap_TX0N:";
			PrintArray( Cap_TX0N[0], M_Cap);
			cout<<"Cap_TX0P:";
			PrintArray( Cap_TX0P[0], M_Cap);
		  }
	  }
	  //================= set back the original setup ==========//
	  Primary.getSpecification().restore();
	  FLUSH(TM::APRM);
///************************Compare the capture code with the expected code*************************************/
	  for(int i = 0; i<10; i++){
		  Flag_TX0N = Flag_TX0N | CodeCompare(Cap_TX0N[i],Expect_Code,M_Cap,N_Code,Flag_TX0N,Debug_Mode);
		  Flag_TX1N = Flag_TX1N | CodeCompare(Cap_TX1N[i],Expect_Code,M_Cap,N_Code,Flag_TX1N,Debug_Mode);
		  Flag_TX2N = Flag_TX2N | CodeCompare(Cap_TX2N[i],Expect_Code,M_Cap,N_Code,Flag_TX2N,Debug_Mode);
		  Flag_TX0P = Flag_TX0P | CodeCompare(Cap_TX0P[i],Expect_Code,M_Cap,N_Code,Flag_TX0P,Debug_Mode);
		  Flag_TX1P = Flag_TX1P | CodeCompare(Cap_TX1P[i],Expect_Code,M_Cap,N_Code,Flag_TX1P,Debug_Mode);
		  Flag_TX2P = Flag_TX2P | CodeCompare(Cap_TX2P[i],Expect_Code,M_Cap,N_Code,Flag_TX2P,Debug_Mode);
	  }

	  Flag_Pass = Flag_TX0N & Flag_TX1N & Flag_TX2N & Flag_TX0P & Flag_TX1P & Flag_TX2P;//

	  if(Debug_Mode){
		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_Pass="<<Flag_Pass<<endl;

		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX0N="<<Flag_TX0N<<endl;
		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX1N="<<Flag_TX1N<<endl;
		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX2N="<<Flag_TX2N<<endl;
		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX0P="<<Flag_TX0P<<endl;
		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX1P="<<Flag_TX1P<<endl;
		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX2P="<<Flag_TX2P<<endl;

	  }
	  //======Judge The Result===================//
	  TestSet.cont(true).TEST("","flag",tmLimits,Flag_Pass);

    return;
  }
  /**
   *This function will be invoked once the specified parameter's value is changed.
   *@param parameterIdentifier
   *
   *Note: Test Method API should not be used in this method.
   */
  int CodeCompare(ARRAY_I &Cap, ARRAY_I &Exp,int M,int N,int &Flag,int debug){
	  int max_cnt;
  	  if(!Flag){
  		  for(int y=0;y<M-N;y++){		//when the left code length is less than expected,then stop.
  		 	 	  int count = 0;
  		 	 	  for(int x=0;x<N;x++){
  		 	 	  if(Cap[x+y]!=Exp[x]){  //the first capture data compare
  		 	 		  break;
  		 	 	  }
  		 	 	  else{
  		 	 		  count ++;
  		 	 	  }
  		 	   }
  		 	   if(count==N){
  		 		   max_cnt=N;
  		 		   Flag = 1;
  		 	 	  break;
  		 	   }
  		 	   else{
  		 		  if(count>max_cnt) max_cnt=count;
  		 	 	  count = 0;// clear count
  		 	 	  Flag = 0;
  		 	   }
  		 	 }
  	  }
  	  if(debug) cout<<dec<<max_cnt<<endl;
  	  return Flag;
    }
  /*************************************************
   *Print Array
   *************************************************/
  void PrintArray(ARRAY_I &CAP,int M){
  	  for(int i=0;i<M;i++){
  			cout<<CAP[i];
  			if(i==M-1){
  				cout<<endl;
  			}
  		  }
  }
  virtual void postParameterChange(const string& parameterIdentifier)
  {
    //Add your code
//		bool visible = (freq_type=="high_freq");
//		getParameter("Debug_Mode").setVisible(visible);
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
REGISTER_TESTMETHOD("bm_ac_tml.hdmi_cap_9G", hdmi_cap_9G);


class hdmi_cap_ps1600: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   *
   *Note: Test Method API should not be used in this method.
   */

	int Debug_Mode,Loop_xN;

  virtual void initialize()
  {
    //Add your initialization code here
	  addParameter("Debug_Mode", "int", &Debug_Mode)
	                 .setDefault("1")
	                 .setOptions("0:1")
	                 .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");
	  addParameter("Loop_xN", "int", &Loop_xN)
	                 .setDefault("10")
	                 .setComment("Loop N times");

  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
   {
 	  static ARRAY_I		Cap_TX0N[10],Cap_TX1N[10],Cap_TX2N[10];
 	  static ARRAY_I		Cap_TX0P[10],Cap_TX1P[10],Cap_TX2P[10];

 	  static ARRAY_I 		Expect_Code;
 	  static data 			VIout;

 	  int			Flag_Pass=0;
 	  int			Flag_TX0N=0,Flag_TX1N=0,Flag_TX2N=0;
 	  int			Flag_TX0P=0,Flag_TX1P=0,Flag_TX2P=0;
      DOUBLE		TX0N_VT=0,TX0P_VT=0,TX1N_VT=0,TX1P_VT=0,TX2N_VT=0,TX2P_VT=0;
 	  int			M_Cap=2048,N_Code=200;

	  static string 		VarCap_0N = "1600_HDMI_O_TX0N",VarCap_1N = "1600_HDMI_O_TX1N",VarCap_2N = "1600_HDMI_O_TX2N";//Capture Variable
	  static string 		VarCap_0P = "1600_HDMI_O_TX0P",VarCap_1P = "1600_HDMI_O_TX1P",VarCap_2P = "1600_HDMI_O_TX2P";//Capture Variable

 	  static string 		sTsName;

 	  SPMU_TASK badc_task;

 			/********************************************************************************************
 			 * move the strobe 10 times and Capture the data if 1 time is ok, set the test pass
 			 ********************************************************************************************/
 //============== initial the array ==========================//

 	  Flag_Pass=0;
 	  Flag_TX0N=0,Flag_TX1N=0,Flag_TX2N=0;
 	  Flag_TX0P=0,Flag_TX1P=0,Flag_TX2P=0;

 	  for(int i=0;i<10;i++){
 		  Cap_TX0N[i].resize(M_Cap);
 		  Cap_TX0P[i].resize(M_Cap);
 		  Cap_TX1N[i].resize(M_Cap);
 		  Cap_TX1P[i].resize(M_Cap);
 		  Cap_TX2N[i].resize(M_Cap);
 		  Cap_TX2P[i].resize(M_Cap);

 		  Cap_TX0N[i].init(0);
 		  Cap_TX0P[i].init(0);
 		  Cap_TX1N[i].init(0);
 		  Cap_TX1P[i].init(0);
 		  Cap_TX2N[i].init(0);
 		  Cap_TX2P[i].init(0);
 	  }
 	  Expect_Code.resize(N_Code);
 	  Expect_Code.init(0);

	  // generate the expected code 01010101..01

	  for(int i=0;i<N_Code;i++){
		  if(i%2==0){
			  Expect_Code[i]=0;
		  }
		  else{
			  Expect_Code[i]=1;
		  }
	  }

 	  if(Debug_Mode)  cout<<" Expect_Code"<<"="<<Expect_Code<<endl;

 	  GET_TESTSUITE_NAME(sTsName);

 	  double period = 1000/Primary.getSpecification().getSpecValue("Freq@p1600_HDMI_DATA")/8;//8 strobes


 	  ON_FIRST_INVOCATION_BEGIN();
 		  CONNECT();

 		  DigPin_Relay("HDMI_O_TX0P,HDMI_O_TX0N,HDMI_O_TX1P,HDMI_O_TX1N,HDMI_O_TX2P,HDMI_O_TX2N","AC_OFF");
// 		  VM_BADC(badc_task,"HDMI_O_TX0P,HDMI_O_TX0N,HDMI_O_TX1P,HDMI_O_TX1N,HDMI_O_TX2P,HDMI_O_TX2N", VIout,Debug_Mode,0, 1,16);

		  Iset_PPMU("HDMI_O_TX0P,HDMI_O_TX0N,HDMI_O_TX1P,HDMI_O_TX1N,HDMI_O_TX2P,HDMI_O_TX2N",0);//set the measure condition
		  WAIT_TIME(10 ms);
		  VM_PPMU("HDMI_O_TX0P,HDMI_O_TX0N,HDMI_O_TX1P,HDMI_O_TX1N,HDMI_O_TX2P,HDMI_O_TX2N",VIout,Debug_Mode,0);//measure the dc offset value

		  FOR_EACH_SITE_BEGIN();

			int site_index =CURRENT_SITE_NUMBER()-1;

			TX0N_VT = VIout.get("HDMI_O_TX0N",site_index,0);
			Primary.getLevelSpec().change("v_VT_TX0N", TX0N_VT);

			TX0P_VT = VIout.get("HDMI_O_TX0P",site_index,0);
			Primary.getLevelSpec().change("v_VT_TX0P", TX0P_VT);

			TX1N_VT = VIout.get("HDMI_O_TX1N",site_index,0);
			Primary.getLevelSpec().change("v_VT_TX1N", TX1N_VT);

			TX1P_VT = VIout.get("HDMI_O_TX1P",site_index,0);
			Primary.getLevelSpec().change("v_VT_TX1P", TX1P_VT);

			TX2N_VT = VIout.get("HDMI_O_TX2N",site_index,0);
			Primary.getLevelSpec().change("v_VT_TX2N", TX2N_VT);

			TX2P_VT = VIout.get("HDMI_O_TX2P",site_index,0);
			Primary.getLevelSpec().change("v_VT_TX2P", TX2P_VT);

			FLUSH(TM::APRM);

			FOR_EACH_SITE_END();

		  FW_TASK("RLYC AC,OFF,(HDMI_O_TX0P,HDMI_O_TX0N,HDMI_O_TX1P,HDMI_O_TX1N,HDMI_O_TX2P,HDMI_O_TX2N)");// close ac relay for capture

		  ON_FIRST_INVOCATION_END();

 	  for(int i = 0;i<Loop_xN;i++){
 		  ON_FIRST_INVOCATION_BEGIN();
			  Primary.getSpecification().change("offset@p1600_HDMI_DATA",i*period/Loop_xN);
			  FLUSH(TM::APRM);
			  DIGITAL_CAPTURE_TEST();
 		  ON_FIRST_INVOCATION_END();

 		  Cap_TX0N[i] = VECTOR(VarCap_0N).getVectors();
 		  Cap_TX0P[i] = VECTOR(VarCap_0P).getVectors();
 		  Cap_TX1N[i] = VECTOR(VarCap_1N).getVectors();
 		  Cap_TX1P[i] = VECTOR(VarCap_1P).getVectors();
 		  Cap_TX2N[i] = VECTOR(VarCap_2N).getVectors();
 		  Cap_TX2P[i] = VECTOR(VarCap_2P).getVectors();

 		  if(Debug_Mode){
	  		cout<<"Cap_TX0N:";
	  		PrintArray( Cap_TX0N[i], M_Cap);
	  		cout<<"Cap_TX0P:";
	  		PrintArray( Cap_TX0P[i], M_Cap);
	  		cout<<"Cap_TX1N:";
	  		PrintArray( Cap_TX1N[i], M_Cap);
	  		cout<<"Cap_TX1P:";
	  		PrintArray( Cap_TX1P[i], M_Cap);
	  		cout<<"Cap_TX2N:";
	  		PrintArray( Cap_TX2N[i], M_Cap);
	  		cout<<"Cap_TX2P:";
	  		PrintArray( Cap_TX2P[i], M_Cap);
		  }
 	  }
 	  //================= set back the original setup ==========//
 	  Primary.getSpecification().restore();
 	  FLUSH(TM::APRM);
 ///************************Compare the capture code with the expected code*************************************/
	  for(int i = 0; i<10; i++){
		  Flag_TX0N = Flag_TX0N | CodeCompare(Cap_TX0N[i],Expect_Code,M_Cap,N_Code,Flag_TX0N,Debug_Mode);
		  Flag_TX1N = Flag_TX1N | CodeCompare(Cap_TX1N[i],Expect_Code,M_Cap,N_Code,Flag_TX1N,Debug_Mode);
		  Flag_TX2N = Flag_TX2N | CodeCompare(Cap_TX2N[i],Expect_Code,M_Cap,N_Code,Flag_TX2N,Debug_Mode);
		  Flag_TX0P = Flag_TX0P | CodeCompare(Cap_TX0P[i],Expect_Code,M_Cap,N_Code,Flag_TX0P,Debug_Mode);
		  Flag_TX1P = Flag_TX1P | CodeCompare(Cap_TX1P[i],Expect_Code,M_Cap,N_Code,Flag_TX1P,Debug_Mode);
		  Flag_TX2P = Flag_TX2P | CodeCompare(Cap_TX2P[i],Expect_Code,M_Cap,N_Code,Flag_TX2P,Debug_Mode);
	  }

 	  Flag_Pass = Flag_TX0N & Flag_TX1N & Flag_TX2N & Flag_TX0P & Flag_TX1P & Flag_TX2P;//

 	  if(Debug_Mode){
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_Pass="<<Flag_Pass<<endl;

 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX0N="<<Flag_TX0N<<endl;
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX1N="<<Flag_TX1N<<endl;
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX2N="<<Flag_TX2N<<endl;
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX0P="<<Flag_TX0P<<endl;
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX1P="<<Flag_TX1P<<endl;
 		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX2P="<<Flag_TX2P<<endl;

 	  }
 	  //======Judge The Result===================//
 	 int site_index =CURRENT_SITE_NUMBER()-1;
 	TestSet.cont(true).TEST("","flag",tmLimits,Flag_Pass);
	TestSet.cont(true).TEST("","TX0P_VT",tmLimits,VIout.get("HDMI_O_TX0P",site_index,0));
	TestSet.cont(true).TEST("","TX0N_VT",tmLimits,VIout.get("HDMI_O_TX0N",site_index,0));
	TestSet.cont(true).TEST("","TX1P_VT",tmLimits,VIout.get("HDMI_O_TX1P",site_index,0));
	TestSet.cont(true).TEST("","TX1N_VT",tmLimits,VIout.get("HDMI_O_TX1N",site_index,0));
	TestSet.cont(true).TEST("","TX2P_VT",tmLimits,VIout.get("HDMI_O_TX2P",site_index,0));
	TestSet.cont(true).TEST("","TX2N_VT",tmLimits,VIout.get("HDMI_O_TX2N",site_index,0));


     return;
   }
  /**
   *This function will be invoked once the specified parameter's value is changed.
   *@param parameterIdentifier
   *
   *Note: Test Method API should not be used in this method.
   */

  /*************************************************
   *Compare the capture data with the expected code
   *************************************************/


  int CodeCompare(ARRAY_I &Cap, ARRAY_I &Exp,int M,int N,int &Flag,int debug){
	  int max_cnt=0;
  	  if(!Flag){
  		  for(int y=0;y<M-N;y++){		//when the left code length is less than expected,then stop.
  		 	 	  int count = 0;
  		 	 	  for(int x=0;x<N;x++){
  		 	 	  if(Cap[x+y]!=Exp[x]){  //the first capture data compare
  		 	 		  break;
  		 	 	  }
  		 	 	  else{
  		 	 		  count ++;
  		 	 	  }
  		 	   }
  		 	   if(count==N){
  		 		   max_cnt=N;
  		 		   Flag = 1;
  		 	 	  break;
  		 	   }
  		 	   else{
  		 		  if(count>max_cnt) max_cnt=count;
  		 	 	  count = 0;// clear count
  		 	 	  Flag = 0;
  		 	   }
  		 	 }
  	  }
  	  if(debug) cout<<dec<<max_cnt<<endl;
  	  return Flag;
    }


  /*************************************************
   *Print Array
   *************************************************/
  void PrintArray(ARRAY_I &CAP,int M){
  	  for(int i=0;i<M;i++){
  			cout<<CAP[i];
  			if(i==M-1){
  				cout<<endl;
  			}
  		  }
  }



  virtual void postParameterChange(const string& parameterIdentifier)
  {
    //Add your code



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
REGISTER_TESTMETHOD("bm_ac_tml.hdmi_cap_ps1600", hdmi_cap_ps1600);

class hdmi_cap: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   *
   *Note: Test Method API should not be used in this method.
   */
	int Debug_Mode;
	int M_Cap,N_Code;
	string freq_type;
	string card_type;
	string VarCap_0N,VarCap_0P,VarCap_1N,VarCap_1P,VarCap_2N,VarCap_2P;
	string mPort;

  virtual void initialize()
  {
    //Add your initialization code here
	  addParameter("Debug_Mode", "int", &Debug_Mode)
	                 .setDefault("1")
	                 .setOptions("0:1")
	                 .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");
      addParameter("sampleSize_pts",
                   "int",
                   &M_Cap,
                   testmethod::TM_PARAMETER_INPUT)
      	  	  	  .setDefault("8192")
      	  	  	  .setOptions("8192");
      addParameter("code_length",
                   "int",
                   &N_Code,
                   testmethod::TM_PARAMETER_INPUT)
				  .setDefault("1024")
				  .setOptions("1024:5120") //5120=1024 x 5
				  .setComment("low_freq:5120,hgih_freq:1024");

      addParameter("freq_type",
                   "string",
                   &freq_type,
                   testmethod::TM_PARAMETER_INPUT)
				  .setDefault("low_freq")
				  .setOptions("low_freq:high_freq");

      addParameter("card_type",
                   "string",
                   &card_type,
                   testmethod::TM_PARAMETER_INPUT)
				  .setDefault("PS1600")
				  .setOptions("PS1600:PS9G");

      addParameter("VarCap_0N",
                   "string",
                   &VarCap_0N,
                   testmethod::TM_PARAMETER_INPUT)
				  .setDefault("Var_TX2N_PS1600")
				  .setOptions("Var_TX2N_PS1600:Var_9G_TX0N")
      	  	  	  .setComment("Capture Variable");
      addParameter("VarCap_1N",
                   "string",
                   &VarCap_1N,
                   testmethod::TM_PARAMETER_INPUT)
				  .setDefault("Var_TX1N_PS1600")
				  .setOptions("Var_TX1N_PS1600:Var_9G_TX1N")
      	  	  	  .setComment("Capture Variable");
      addParameter("VarCap_2N",
                   "string",
                   &VarCap_2N,
                   testmethod::TM_PARAMETER_INPUT)
				  .setDefault("Var_TX2N_PS1600")
				  .setOptions("Var_TX2N_PS1600:Var_9G_TX2N")
      	  	  	  .setComment("Capture Variable");
      addParameter("VarCap_0P",
                   "string",
                   &VarCap_0P,
                   testmethod::TM_PARAMETER_INPUT)
				  .setDefault("Var_TX0P_PS1600")
				  .setOptions("Var_TX0P_PS1600:Var_9G_TX0P")
      	  	  	  .setComment("Capture Variable");
      addParameter("VarCap_1P",
                   "string",
                   &VarCap_1P,
                   testmethod::TM_PARAMETER_INPUT)
				  .setDefault("Var_TX1P_PS1600")
				  .setOptions("Var_TX1P_PS1600:Var_9G_TX1P")
      	  	  	  .setComment("Capture Variable");
      addParameter("VarCap_2P",
                   "string",
                   &VarCap_2P,
                   testmethod::TM_PARAMETER_INPUT)
				  .setDefault("Var_TX2P_PS1600")
				  .setOptions("Var_TX2P_PS1600:Var_9G_TX2P")
      	  	  	  .setComment("Capture Variable");
      addParameter("Port",
                   "string",
                   &mPort,
                   testmethod::TM_PARAMETER_INPUT)
				  .setDefault("pHDMI_DataOut_PS1600")
				  .setOptions("pHDMI_9G_Data:pHDMI_DataOut_PS1600")
      	  	  	  .setComment("which port used");
  }
  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
	  static ARRAY_I		Cap_TX0N[10],Cap_TX1N[10],Cap_TX2N[10];
	  static ARRAY_I		Cap_TX0P[10],Cap_TX1P[10],Cap_TX2P[10];

	  static ARRAY_I 		Expect_Code;


	  int					Flag_Pass=0;
	  int					Flag_TX0N=0,Flag_TX1N=0,Flag_TX2N=0;
	  int					Flag_TX0P=0,Flag_TX1P=0,Flag_TX2P=0;


	  static string 		sTsName;
	  static string 		mFreq="Freq@"+mPort;
	  static string 		mOffset="offset@"+mPort;

			/********************************************************************************************
			 * move the strobe 10 times and Capture the data if 1 time is ok, set the test pass
			 ********************************************************************************************/
//============== initial the array ==========================//
	  for(int i=0;i<10;i++){
		  Cap_TX0N[i].resize(M_Cap);
		  Cap_TX0P[i].resize(M_Cap);
		  Cap_TX1N[i].resize(M_Cap);
		  Cap_TX1P[i].resize(M_Cap);
		  Cap_TX2N[i].resize(M_Cap);
		  Cap_TX2P[i].resize(M_Cap);

		  Cap_TX0N[i].init(0);
		  Cap_TX0P[i].init(0);
		  Cap_TX1N[i].init(0);
		  Cap_TX1P[i].init(0);
		  Cap_TX2N[i].init(0);
		  Cap_TX2P[i].init(0);
	  }
	  Expect_Code.resize(N_Code);
	  Expect_Code.init(0);
/***********************************************************************
 *   generate the expected code
 * *********************************************************************/
	  if(freq_type=="low_freq"&&card_type=="PS9G"){
		  // the expected code 0000011111..0000011111
		  	  int Flag=0;
		  	  for(int i=0;i<N_Code;i++){
		  		  if(i%5==0){
		  			  Flag++;
		  			  if(Flag%2==1){
		  				Expect_Code[i]=0;
		  			  }
		  			  else{
		  				Expect_Code[i]=1;
		  			  }
		  		  }
		  		  else{
		  			  Expect_Code[i]=Expect_Code[i-1];
		  		  }
		  	  }
	  }
	  else {
		  // the expected code 01010101..01

		  	  for(int i=0;i<N_Code;i++){
		  		  if(i%2==0){
		  			  Expect_Code[i]=0;
		  		  }
		  		  else{
		  			  Expect_Code[i]=1;
		  		  }
		  	  }
	  }

	  if(Debug_Mode)	cout<<" Expect_Code"<<"="<<Expect_Code<<endl;



	  GET_TESTSUITE_NAME(sTsName);

	  double period = 1000/Primary.getSpecification().getSpecValue(mFreq)/8;//8 strobes


	  ON_FIRST_INVOCATION_BEGIN();
		  CONNECT();
	  ON_FIRST_INVOCATION_END();

	  for(int i = 0;i<10;i++){
		  ON_FIRST_INVOCATION_BEGIN();
			  Primary.getSpecification().change(mOffset,i*period/10);
			  cout<<"offset is "<<Primary.getSpecification().getSpecValue(mOffset)<<endl;
			  FLUSH(TM::APRM);
//			  DIGITAL_CAPTURE_TEST();
		  ON_FIRST_INVOCATION_END();

		  Cap_TX0N[i] = VECTOR(VarCap_0N).getVectors();
		  Cap_TX0P[i] = VECTOR(VarCap_0P).getVectors();
		  Cap_TX1N[i] = VECTOR(VarCap_1N).getVectors();
		  Cap_TX1P[i] = VECTOR(VarCap_1P).getVectors();
		  Cap_TX2N[i] = VECTOR(VarCap_2N).getVectors();
		  Cap_TX2P[i] = VECTOR(VarCap_2P).getVectors();
		  if(Debug_Mode){
			cout<<"Cap_TX0N:";
			PrintArray( Cap_TX0N[0], M_Cap);
			cout<<"Cap_TX0P:";
			PrintArray( Cap_TX0P[0], M_Cap);
		  }
	  }
	  //================= set back the original setup ==========//
	  Primary.getSpecification().restore();
	  FLUSH(TM::APRM);
///************************Compare the capture code with the expected code*************************************/
	  for(int i = 0; i<10; i++){
		  Flag_TX0N = Flag_TX0N | CodeCompare(Cap_TX0N[i],Expect_Code,M_Cap,N_Code,Flag_TX0N,Debug_Mode);
		  Flag_TX1N = Flag_TX1N | CodeCompare(Cap_TX1N[i],Expect_Code,M_Cap,N_Code,Flag_TX1N,Debug_Mode);
		  Flag_TX2N = Flag_TX2N | CodeCompare(Cap_TX2N[i],Expect_Code,M_Cap,N_Code,Flag_TX2N,Debug_Mode);
		  Flag_TX0P = Flag_TX0P | CodeCompare(Cap_TX0P[i],Expect_Code,M_Cap,N_Code,Flag_TX0P,Debug_Mode);
		  Flag_TX1P = Flag_TX1P | CodeCompare(Cap_TX1P[i],Expect_Code,M_Cap,N_Code,Flag_TX1P,Debug_Mode);
		  Flag_TX2P = Flag_TX2P | CodeCompare(Cap_TX2P[i],Expect_Code,M_Cap,N_Code,Flag_TX2P,Debug_Mode);
	  }

	  Flag_Pass = Flag_TX0N & Flag_TX1N & Flag_TX2N & Flag_TX0P & Flag_TX1P & Flag_TX2P;//

	  if(Debug_Mode){
		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_Pass="<<Flag_Pass<<endl;

		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX0N="<<Flag_TX0N<<endl;
		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX1N="<<Flag_TX1N<<endl;
		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX2N="<<Flag_TX2N<<endl;
		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX0P="<<Flag_TX0P<<endl;
		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX1P="<<Flag_TX1P<<endl;
		cout<<"site"<<CURRENT_SITE_NUMBER()<<" Flag_TX2P="<<Flag_TX2P<<endl;

	  }
	  //======Judge The Result===================//
	  TestSet.cont(true).TEST("","flag",tmLimits,Flag_Pass);

    return;
  }
  /**
   *This function will be invoked once the specified parameter's value is changed.
   *@param parameterIdentifier
   *
   *Note: Test Method API should not be used in this method.
   */
  int CodeCompare(ARRAY_I &Cap, ARRAY_I &Exp,int M,int N,int &Flag,int debug){
	  int max_cnt;
  	  if(!Flag){
  		  for(int y=0;y<M-N;y++){		//when the left code length is less than expected,then stop.
  		 	 	  int count = 0;
  		 	 	  for(int x=0;x<N;x++){
  		 	 	  if(Cap[x+y]!=Exp[x]){  //the first capture data compare
  		 	 		  break;
  		 	 	  }
  		 	 	  else{
  		 	 		  count ++;
  		 	 	  }
  		 	   }
  		 	   if(count==N){
  		 		   Flag = 1;
  		 	 	  break;
  		 	   }
  		 	   else{
  		 		  if(count>max_cnt) max_cnt=count;
  		 	 	  count = 0;// clear count
  		 	 	  Flag = 0;
  		 	   }
  		 	 }
  	  }
  	  if(debug) cout<<max_cnt<<endl;
  	  return Flag;
    }
  /*************************************************
   *Print Array
   *************************************************/
  void PrintArray(ARRAY_I &CAP,int M){
  	  for(int i=0;i<M;i++){
  			cout<<CAP[i];
  			if(i==M-1){
  				cout<<endl;
  			}
  		  }
  }
  virtual void postParameterChange(const string& parameterIdentifier)
  {
    //Add your code
		bool visible = (card_type=="PS1600");
		getParameter("freq_type").setVisible(visible);
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
REGISTER_TESTMETHOD("bm_ac_tml.hdmi_cap", hdmi_cap);
