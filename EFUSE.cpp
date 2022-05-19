#include "testmethod.hpp"
#include "mapi.hpp"
#include "libcicpi.h"

#include "efuseComCode.h"

#include "d2sFramework.h"
#include "d2s_para_otp_w.h"
#include "d2s_para_otp_r.h"

#define __D2SREADBLOCKCONFIG__      d2s_para_otp_read &myD2S = d2s_para_otp_read::Instance();

#define __D2SWRITEBLOCKCONFIG__     d2s_para_otp_write &myD2S = d2s_para_otp_write::Instance();

 using namespace std;
 using namespace V93kLimits;
 /**
  * Test method class.
  *
  * For each testsuite using this test method, one object of this
  * class is created.
  */

class Efuse_FT_Write_D2s: public testmethod::TestMethod {

 protected:
   /**
	*Initialize the parameter interface to the testflow.
	*This method is called just once after a testsuite is created.
	*/
   int 		debug_mode;
   int 		D2SMode;

   virtual void initialize()
   {
	 //Add your initialization code here
	 //Note: Test Method API should not be used in this method!
		 addParameter("debug_mode",
					  "int",
					  &debug_mode)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

		 addParameter("D2SMode", "int", &D2SMode)
					   .setDefault("1")
					   .setOptions("1:2")
					   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");
   }

   /**
	*This test is invoked per site.
	*/
   virtual void run()
   {

	  ON_FIRST_INVOCATION_BEGIN();

		  __D2SWRITEBLOCKCONFIG__

		  d2sFramework fr;
		  fr.registerTransactionPort(myD2S);
		  d2sFrameWorkModeType::Enum d2s_mode;

		  switch ( D2SMode )
		  {
				case 0:
				case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
				case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
		  }

	  	  EFUSE Efuse;

	  	  Efuse.ReadCsv();

		  fr.d2s_LABEL_BEGIN("efuse_ft_write",d2s_mode);

			  FOR_EACH_SITE_BEGIN();

			  Efuse.FTEfuseCodeCalc(debug_mode);

			  vector<Register> mWriteInfo;
			  mWriteInfo = Efuse.getFTWriteInfo();

			  for(size_t i=0;i<mWriteInfo.size();i++){
				  char tmpchar[256];
				  sprintf(tmpchar,"efuse_ft_write_addr_%ld",mWriteInfo[i].addr);
				  myD2S.writeDynamic(mWriteInfo[i].addr,mWriteInfo[i].data,tmpchar);//
			  }

			  FOR_EACH_SITE_END();

		  fr.d2s_LABEL_END();

	  ON_FIRST_INVOCATION_END();

	 //Add your test code here.
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
     *This function will be invoked once the Select Test Method Dialog is opened
     */
    virtual const string getComment() const
    {
      string comment = " please add your comment for this method.";
      return comment;
    }
  };
REGISTER_TESTMETHOD("bm_efuse_tml.FT.Efuse_FT_Write_D2s", Efuse_FT_Write_D2s);

class Efuse_CP_Write_D2s: public testmethod::TestMethod {

 protected:
   /**
	*Initialize the parameter interface to the testflow.
	*This method is called just once after a testsuite is created.
	*/
   int 		debug_mode;
   int 		D2SMode;
   string	mWafer_ID;
   string	mLot_ID;

  virtual void initialize()
  {
	 //Add your initialization code here
	 //Note: Test Method API should not be used in this method!

		  addParameter("Wafer_ID_Var","string",&mWafer_ID)
				.setDefault("Wafer_ID")
				.setComment("Wafer ID named in application model file ");

		  addParameter("Lot_ID_Var","string",&mLot_ID)
				.setDefault("Lot_ID")
				.setComment("Lot ID named in application model file ");

		 addParameter("debug_mode",
					  "int",
					  &debug_mode)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

		 addParameter("D2SMode", "int", &D2SMode)
					   .setDefault("1")
					   .setOptions("1:2")
					   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");
   }

   /**
	*This test is invoked per site.
	*/
   virtual void run()
   {

	  ON_FIRST_INVOCATION_BEGIN();

		  __D2SWRITEBLOCKCONFIG__

		  d2sFramework fr;
		  fr.registerTransactionPort(myD2S);
		  d2sFrameWorkModeType::Enum d2s_mode;

		  switch ( D2SMode )
		  {
				case 0:
				case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
				case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
		  }

		  char   Lot_ID[6]={'a','b','c','d','e','f'};
		  char   Wafer_ID[5];

	  	  EFUSE Efuse;

	  	  Efuse.ReadCsv();

		  fr.d2s_LABEL_BEGIN("efuse_cp_write",d2s_mode);

			  FOR_EACH_SITE_BEGIN();
			  //get lot id value and set back the value to uservar
			  GetModelfileString(const_cast<char*>(mLot_ID.c_str()),Lot_ID);
			  long tmp_Lot_ID = (long)Lot_ID[0]<<40 | (long)Lot_ID[1]<<32 | (long)Lot_ID[2]<<24 | (long)Lot_ID[3]<<16 | (long)Lot_ID[4]<<8 | (long)Lot_ID[5] ;
			  SET_USER_DOUBLE("lot_id",tmp_Lot_ID);//set back to user var

			  //get wafer id value and set back the value to uservar
			  GetModelfileString(const_cast<char*>(mWafer_ID.c_str()), Wafer_ID);
			  int tmp_Wafer_ID=atoi(Wafer_ID);
			  SET_USER_DOUBLE("wafer_number",tmp_Wafer_ID);//set back to user var

			  //get x ,y address value and set back the value to uservar
			  long  x_addr=0;
			  long  y_addr=0;
			  GetDiePosXYOfSite(CURRENT_SITE_NUMBER(),&x_addr,&y_addr);
			  SET_USER_DOUBLE("x_address",x_addr);
			  SET_USER_DOUBLE("y_address",y_addr);

			  Efuse.CPEfuseCodeCalc(debug_mode);

			  vector<Register> mWriteInfo;
			  mWriteInfo = Efuse.getCPWriteInfo();

			  for(size_t i=0;i<mWriteInfo.size();i++){
				  char tmpchar[256];
				  sprintf(tmpchar,"efuse_cp_write_addr_%ld",mWriteInfo[i].addr);
				  myD2S.writeDynamic(mWriteInfo[i].addr,mWriteInfo[i].data,tmpchar);
			  }

			  if(debug_mode){
				  cout<<"site:"<<CURRENT_SITE_NUMBER()<<endl;
				  cout<<"lot_id:"<<hex<<"0x"<<(long)Lot_ID[0]<<" "<<(long)Lot_ID[1]<<" "<<(long)Lot_ID[2]<<" "<<(long)Lot_ID[3]<<" "<<(long)Lot_ID[4]<<" "<<(long)Lot_ID[5]<<endl;
				  cout<<"wafer number:"<<dec<<tmp_Wafer_ID<<endl;
				  cout<<"x address:"<<dec<<x_addr<<endl;
				  cout<<"y address:"<<dec<<y_addr<<endl;
			  }

			  FOR_EACH_SITE_END();

		  fr.d2s_LABEL_END();

	  ON_FIRST_INVOCATION_END();

	 //Add your test code here.
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
     *This function will be invoked once the Select Test Method Dialog is opened
     */
    virtual const string getComment() const
    {
      string comment = " please add your comment for this method.";
      return comment;
    }
  };
REGISTER_TESTMETHOD("bm_efuse_tml.CP.Efuse_CP_Write_D2s", Efuse_CP_Write_D2s);
//
class Efuse_FT_Write_Label_Edit: public testmethod::TestMethod {

 protected:
   /**
	*Initialize the parameter interface to the testflow.
	*This method is called just once after a testsuite is created.
	*/
	  int 		debug_mode;
	  int 		D2SMode;
	  string    PinName;
	  int  		StartVector;
	  int		deltaVectors;
   virtual void initialize()
   {
	 //Add your initialization code here
	 //Note: Test Method API should not be used in this method!
		 addParameter("debug_mode",
					  "int",
					  &debug_mode)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

		  addParameter("PinName","string",&PinName)
				.setDefault("DVI1_D7")
				.setComment("OTP data input pins ");
		  addParameter("StartVector", "int", &StartVector)
				   .setDefault("10")
				   .setComment("StartVector number");
		  addParameter("deltaVectors", "int", &deltaVectors)
				   .setDefault("33")
				   .setComment("the delta number between current vector number and the next vector number");
   }

   /**
	*This test is invoked per site.
	*/
   virtual void run()
   {

	  ON_FIRST_INVOCATION_BEGIN();

		 EFUSE Efuse;

		 Efuse.ReadCsv();

		 STRING vectorlabel = Primary.getLabel();
		 VEC_LABEL_EDIT myLabel(vectorlabel,PinName);
		 myLabel.storeVectorData(PinName.c_str());

		 FOR_EACH_SITE_BEGIN();

		  Efuse.FTEfuseCodeCalc(debug_mode);

		  vector<Register> mWriteInfo;
		  mWriteInfo = Efuse.getFTWriteInfo();

		  int n=mWriteInfo.size();
		  VECTOR_DATA myVectorData[n];
		  int vecNum = StartVector;

		  for(int i=0;i<n;i++){
			  myVectorData[i].vectorNum=vecNum;
			  myVectorData[i].phyWvfIndex=mWriteInfo[i].data;
			  vecNum=vecNum+deltaVectors;
		  }

		  myLabel.downloadUserVectors(myVectorData,n);

		 FOR_EACH_SITE_END();

		 FUNCTIONAL_TEST();

		 myLabel.restoreVectorData(PinName.c_str());

	  ON_FIRST_INVOCATION_END();

	 //Add your test code here.
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
     *This function will be invoked once the Select Test Method Dialog is opened
     */
    virtual const string getComment() const
    {
      string comment = " please add your comment for this method.";
      return comment;
    }
  };
REGISTER_TESTMETHOD("bm_efuse_tml.FT.Efuse_FT_Write_Label_Edit", Efuse_FT_Write_Label_Edit);
//
class Efuse_CP_Write_Label_Edit: public testmethod::TestMethod {

   protected:
     /**
  	*Initialize the parameter interface to the testflow.
  	*This method is called just once after a testsuite is created.
  	*/
	  int 		debug_mode;
	  int 		D2SMode;
	  string    PinName;
	  int  		StartVector;
	  int		deltaVectors;
	  string	mWafer_ID;
	  string	mLot_ID;

	 virtual void initialize()
	 {
	 //Add your initialization code here
	 //Note: Test Method API should not be used in this method!

		  addParameter("Wafer_ID_Var","string",&mWafer_ID)
				.setDefault("Wafer_ID")
				.setComment("Wafer ID named in application model file ");

		  addParameter("Lot_ID_Var","string",&mLot_ID)
				.setDefault("Lot_ID")
				.setComment("Lot ID named in application model file ");
		  addParameter("debug_mode",
					  "int",
					  &debug_mode)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

		  addParameter("PinName","string",&PinName)
				.setDefault("DVI1_D7")
				.setComment("OTP data input pins ");
		  addParameter("StartVector", "int", &StartVector)
				   .setDefault("10")
				   .setComment("StartVector number");
		  addParameter("deltaVectors", "int", &deltaVectors)
				   .setDefault("33")
				   .setComment("the delta number between current vector number and the next vector number");
	 }

	 /**
	*This test is invoked per site.
	*/
	 virtual void run()
	 {

	  ON_FIRST_INVOCATION_BEGIN();

		  char   Lot_ID[6]={'a','b','c','d','e','f'};
		  char   Wafer_ID[5];

		  EFUSE Efuse;

		  Efuse.ReadCsv();

		  STRING vectorlabel = Primary.getLabel();
		  VEC_LABEL_EDIT myLabel(vectorlabel,PinName);
		  myLabel.storeVectorData(PinName.c_str());
		  FOR_EACH_SITE_BEGIN();

			  //get lot id value and set back the value to uservar
			  GetModelfileString(const_cast<char*>(mLot_ID.c_str()),Lot_ID);
			  long tmp_Lot_ID = (long)Lot_ID[0]<<40 | (long)Lot_ID[1]<<32 | (long)Lot_ID[2]<<24 | (long)Lot_ID[3]<<16 | (long)Lot_ID[4]<<8 | (long)Lot_ID[5] ;
			  SET_USER_DOUBLE("lot_id",tmp_Lot_ID);//set back to user var

			  //get wafer id value and set back the value to uservar
			  GetModelfileString(const_cast<char*>(mWafer_ID.c_str()), Wafer_ID);
			  int tmp_Wafer_ID=atoi(Wafer_ID);
			  SET_USER_DOUBLE("wafer_number",tmp_Wafer_ID);//set back to user var

			  //get x ,y address value and set back the value to uservar
			  long  x_addr=0;
			  long  y_addr=0;
			  GetDiePosXYOfSite(CURRENT_SITE_NUMBER(),&x_addr,&y_addr);
			  SET_USER_DOUBLE("x_address",x_addr);
			  SET_USER_DOUBLE("y_address",y_addr);

			  Efuse.CPEfuseCodeCalc(debug_mode);

			  vector<Register> mWriteInfo;
			  mWriteInfo = Efuse.getCPWriteInfo();

			  int n=mWriteInfo.size();
			  VECTOR_DATA myVectorData[n];
			  int vecNum = StartVector;

			  for(int i=0;i<n;i++){
				  myVectorData[i].vectorNum=vecNum;
				  myVectorData[i].phyWvfIndex=mWriteInfo[i].data;
				  vecNum=vecNum+deltaVectors;
			  }

			  myLabel.downloadUserVectors(myVectorData,n);

			  if(debug_mode){
				  cout<<"site:"<<CURRENT_SITE_NUMBER()<<endl;
				  cout<<"lot_id:"<<hex<<"0x"<<(long)Lot_ID[0]<<" "<<(long)Lot_ID[1]<<" "<<(long)Lot_ID[2]<<" "<<(long)Lot_ID[3]<<" "<<(long)Lot_ID[4]<<" "<<(long)Lot_ID[5]<<endl;
				  cout<<"wafer number:"<<dec<<tmp_Wafer_ID<<endl;
				  cout<<"x address:"<<dec<<x_addr<<endl;
				  cout<<"y address:"<<dec<<y_addr<<endl;
			  }

		  FOR_EACH_SITE_END();

		  FUNCTIONAL_TEST();
		  myLabel.restoreVectorData(PinName.c_str());

	  ON_FIRST_INVOCATION_END();

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
   *This function will be invoked once the Select Test Method Dialog is opened
   */
  virtual const string getComment() const
  {
	string comment = " please add your comment for this method.";
	return comment;
  }
};
REGISTER_TESTMETHOD("bm_efuse_tml.CP.Efuse_CP_Write_Label_Edit", Efuse_CP_Write_Label_Edit);
////
class Efuse_FT_Post_Check: public testmethod::TestMethod {

 protected:
   /**
	*Initialize the parameter interface to the testflow.
	*This method is called just once after a testsuite is created.
	*/
   int 		debug_mode;
   int 		D2SMode;
   int   	mGen_testTable;
   virtual void initialize()
   {
	 //Add your initialization code here
	 //Note: Test Method API should not be used in this method!
		 addParameter("debug_mode",
					  "int",
					  &debug_mode)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

		 addParameter("D2SMode", "int", &D2SMode)
					   .setDefault("1")
					   .setOptions("1:2")
					   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");
		 addParameter("Gen_testTable",
					  "int",
					  &mGen_testTable)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("Gen_testTable  0:no_need_Gen_testTable, 1:Gen_testTable");
   }

   /**
	*This test is invoked per site.
	*/
   virtual void run()
   {

	  __D2SREADBLOCKCONFIG__

	  d2sFramework fr;
	  fr.registerTransactionPort(myD2S);
	  d2sFrameWorkModeType::Enum d2s_mode;

	  switch ( D2SMode )
	  {
			case 0:
			case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
			case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
	  }

	  static vector<int> ADDRS;

	  static EFUSE Efuse;

	  ON_FIRST_INVOCATION_BEGIN();

		  Efuse.ReadCsv();

		  ADDRS = Efuse.getFTReadAddr();


		  fr.d2s_LABEL_BEGIN("efuse_ft_post_check",d2s_mode);
		  	  for(size_t i=0;i<ADDRS.size();i++){
		  		int addr=ADDRS[i];
		  		if(debug_mode) cout<<"read addr =0x"<<hex<<addr<<endl;
		  		char tmpchar[256];
		  		sprintf(tmpchar,"0x%x",addr);
		  		myD2S.read(addr,tmpchar);
		  	  }

		  fr.d2s_LABEL_END();

	  ON_FIRST_INVOCATION_END();

	  vector<long> DATAS;
	  DATAS.clear();
	  for(size_t i=0;i<ADDRS.size();i++){
		  int addr=ADDRS[i];
		  char tmpchar[256];
		  sprintf(tmpchar,"0x%x",addr);
		  long tmp_data=myD2S.getReadValue(tmpchar);
		  DATAS.push_back(tmp_data);
//		  if(debug_mode) cout<<"read addr =0x"<<hex<<addr<<",read value =0x"<<tmp_data<<endl;
	  }

	  vector<WRV> word_read_val;
	  word_read_val = Efuse.getFTWordReadValue(DATAS);
	  vector<WEV> word_expect_val;
	  word_expect_val = Efuse.getFTWordExpectValue();

	  int flag_efuse_ft_post_check = Efuse.Match_check(word_read_val,word_expect_val);

	  int Gen_testTable=mGen_testTable;

	  for(size_t i=0;i<word_read_val.size();i++){
		  if("chip_id"==word_read_val[i].word_name){
			  if(Gen_testTable){
				  Efuse.Generate_testTable("chip_id_write_value");
				  Efuse.Generate_testTable("chip_id_read_value");
				  Efuse.Generate_testTable("fabcode_write_value");
				  Efuse.Generate_testTable("fabcode_read_value");
			  }
			  TestSet.cont(true).TEST("",word_expect_val[i].word_name+"_write_value",tmLimits.getLimitObj(word_expect_val[i].word_name+"_write_value"),word_expect_val[i].expect_val&0x0fffffff);
			  TestSet.cont(true).TEST("",word_read_val[i].word_name+"_read_value",tmLimits.getLimitObj(word_read_val[i].word_name+"_read_value"),word_read_val[i].read_val&0x0fffffff);
			  TestSet.cont(true).TEST("","fabcode_write_value",tmLimits.getLimitObj("fabcode_write_value"),word_expect_val[i].expect_val>>28 &0xf);
			  TestSet.cont(true).TEST("","fabcode_read_value",tmLimits.getLimitObj("fabcode_read_value"),word_read_val[i].read_val>>28 &0xf);
		  }
		  else{
			  if(Gen_testTable){
				  Efuse.Generate_testTable(word_expect_val[i].word_name+"_write_value");
				  Efuse.Generate_testTable(word_read_val[i].word_name+"_read_value");
			  }
			  TestSet.cont(true).TEST("",word_expect_val[i].word_name+"_write_value",tmLimits.getLimitObj(word_expect_val[i].word_name+"_write_value"),word_expect_val[i].expect_val);
			  TestSet.cont(true).TEST("",word_read_val[i].word_name+"_read_value",tmLimits.getLimitObj(word_read_val[i].word_name+"_read_value"),word_read_val[i].read_val);
		  }
		  if(debug_mode) {
			  cout<<"the expected value for word \""<<word_expect_val[i].word_name<<"\" is "<<dec<<word_expect_val[i].expect_val<<endl;
			  cout<<"the read value for word \""<<word_read_val[i].word_name<<"\" is "<<dec<<word_read_val[i].read_val<<endl;
		  }
	  }

	  if(Gen_testTable){
		  Efuse.Generate_testTable("flag_efuse_ft_post_check",1,1);
		  Gen_testTable=0;
	  }

	  TestSet.cont(true).TEST("","flag_efuse_ft_post_check",tmLimits.getLimitObj("flag_efuse_ft_post_check"),flag_efuse_ft_post_check);

	 //Add your test code here.
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
     *This function will be invoked once the Select Test Method Dialog is opened
     */
    virtual const string getComment() const
    {
      string comment = " please add your comment for this method.";
      return comment;
    }
  };
REGISTER_TESTMETHOD("bm_efuse_tml.FT.Efuse_FT_Post_Check", Efuse_FT_Post_Check);
//
class Efuse_CP_Post_Check: public testmethod::TestMethod {

 protected:
   /**
	*Initialize the parameter interface to the testflow.
	*This method is called just once after a testsuite is created.
	*/
   int 		debug_mode;
   int 		D2SMode;
   int   	mGen_testTable;
   virtual void initialize()
   {
	 //Add your initialization code here
	 //Note: Test Method API should not be used in this method!
		 addParameter("debug_mode",
					  "int",
					  &debug_mode)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

		 addParameter("D2SMode", "int", &D2SMode)
					   .setDefault("1")
					   .setOptions("1:2")
					   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");
		 addParameter("Gen_testTable",
					  "int",
					  &mGen_testTable)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("Gen_testTable  0:no_need_Gen_testTable, 1:Gen_testTable");
   }

   /**
	*This test is invoked per site.
	*/
   virtual void run()
   {

	  __D2SREADBLOCKCONFIG__

	  d2sFramework fr;
	  fr.registerTransactionPort(myD2S);
	  d2sFrameWorkModeType::Enum d2s_mode;

	  switch ( D2SMode )
	  {
			case 0:
			case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
			case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
	  }

	  static vector<int> ADDRS;

	  static EFUSE Efuse;

	  ON_FIRST_INVOCATION_BEGIN();

		  Efuse.ReadCsv();

		  ADDRS = Efuse.getCPReadAddr();


		  fr.d2s_LABEL_BEGIN("efuse_cp_post_check",d2s_mode);
		  	  for(size_t i=0;i<ADDRS.size();i++){
		  		int addr=ADDRS[i];
		  		if(debug_mode) cout<<"read addr =0x"<<hex<<addr<<endl;
		  		char tmpchar[256];
		  		sprintf(tmpchar,"0x%x",addr);
		  		myD2S.read(addr,tmpchar);
		  	  }

		  fr.d2s_LABEL_END();

	  ON_FIRST_INVOCATION_END();

	  vector<long> DATAS;
	  DATAS.clear();
	  for(size_t i=0;i<ADDRS.size();i++){
		  int addr=ADDRS[i];
		  char tmpchar[256];
		  sprintf(tmpchar,"0x%x",addr);
		  long tmp_data=myD2S.getReadValue(tmpchar);
		  DATAS.push_back(tmp_data);
//		  if(debug_mode) cout<<"read addr =0x"<<hex<<addr<<",read value =0x"<<tmp_data<<endl;
	  }

	  vector<WRV> word_read_val;
	  word_read_val = Efuse.getCPWordReadValue(DATAS);
	  vector<WEV> word_expect_val;
	  word_expect_val = Efuse.getCPWordExpectValue();

	  int flag_efuse_cp_post_check = Efuse.Match_check(word_read_val,word_expect_val);

	  int Gen_testTable=mGen_testTable;

	  for(size_t i=0;i<word_read_val.size();i++){
		  if("chip_id"==word_read_val[i].word_name){
			  if(Gen_testTable){
				  Efuse.Generate_testTable("chip_id_write_value");
				  Efuse.Generate_testTable("chip_id_read_value");
				  Efuse.Generate_testTable("fabcode_write_value");
				  Efuse.Generate_testTable("fabcode_read_value");
			  }
			  TestSet.cont(true).TEST("",word_expect_val[i].word_name+"_write_value",tmLimits.getLimitObj(word_expect_val[i].word_name+"_write_value"),word_expect_val[i].expect_val&0x0fffffff);
			  TestSet.cont(true).TEST("",word_read_val[i].word_name+"_read_value",tmLimits.getLimitObj(word_read_val[i].word_name+"_read_value"),word_read_val[i].read_val&0x0fffffff);
			  TestSet.cont(true).TEST("","fabcode_write_value",tmLimits.getLimitObj("fabcode_write_value"),word_expect_val[i].expect_val>>28 &0xf);
			  TestSet.cont(true).TEST("","fabcode_read_value",tmLimits.getLimitObj("fabcode_read_value"),word_read_val[i].read_val>>28 &0xf);
		  }
		  else if("lot_id"==word_read_val[i].word_name){
			  if(Gen_testTable){
				  Efuse.Generate_testTable("lot_id0_read_value");
				  Efuse.Generate_testTable("lot_id0_write_value");
				  Efuse.Generate_testTable("lot_id1_read_value");
				  Efuse.Generate_testTable("lot_id1_write_value");
				  Efuse.Generate_testTable("lot_id2_read_value");
				  Efuse.Generate_testTable("lot_id2_write_value");
				  Efuse.Generate_testTable("lot_id3_read_value");
				  Efuse.Generate_testTable("lot_id3_write_value");
				  Efuse.Generate_testTable("lot_id4_read_value");
				  Efuse.Generate_testTable("lot_id4_write_value");
				  Efuse.Generate_testTable("lot_id5_read_value");
				  Efuse.Generate_testTable("lot_id5_write_value");
			  }
			  TestSet.cont(true).TEST("","lot_id5_read_value",tmLimits.getLimitObj("lot_id5_read_value"),word_read_val[i].read_val&0xff);
			  TestSet.cont(true).TEST("","lot_id4_read_value",tmLimits.getLimitObj("lot_id4_read_value"),(word_read_val[i].read_val>>8)&0xff);
			  TestSet.cont(true).TEST("","lot_id3_read_value",tmLimits.getLimitObj("lot_id3_read_value"),(word_read_val[i].read_val>>16)&0xff);
			  TestSet.cont(true).TEST("","lot_id2_read_value",tmLimits.getLimitObj("lot_id2_read_value"),(word_read_val[i].read_val>>24)&0xff);
			  TestSet.cont(true).TEST("","lot_id1_read_value",tmLimits.getLimitObj("lot_id1_read_value"),(word_read_val[i].read_val>>32)&0xff);
			  TestSet.cont(true).TEST("","lot_id0_read_value",tmLimits.getLimitObj("lot_id0_read_value"),(word_read_val[i].read_val>>40)&0xff);

			  TestSet.cont(true).TEST("","lot_id5_read_value",tmLimits.getLimitObj("lot_id5_read_value"),word_expect_val[i].expect_val&0xff);
			  TestSet.cont(true).TEST("","lot_id4_write_value",tmLimits.getLimitObj("lot_id4_write_value"),(word_expect_val[i].expect_val>>8)&0xff);
			  TestSet.cont(true).TEST("","lot_id3_write_value",tmLimits.getLimitObj("lot_id3_write_value"),(word_expect_val[i].expect_val>>16)&0xff);
			  TestSet.cont(true).TEST("","lot_id2_write_value",tmLimits.getLimitObj("lot_id2_write_value"),(word_expect_val[i].expect_val>>24)&0xff);
			  TestSet.cont(true).TEST("","lot_id1_write_value",tmLimits.getLimitObj("lot_id1_write_value"),(word_expect_val[i].expect_val>>32)&0xff);
			  TestSet.cont(true).TEST("","lot_id0_write_value",tmLimits.getLimitObj("lot_id0_write_value"),(word_expect_val[i].expect_val>>40)&0xff);
		  }
		  else{
			  if(Gen_testTable){
				  Efuse.Generate_testTable(word_expect_val[i].word_name+"_write_value");
				  Efuse.Generate_testTable(word_read_val[i].word_name+"_read_value");
			  }
			  TestSet.cont(true).TEST("",word_expect_val[i].word_name+"_write_value",tmLimits.getLimitObj(word_expect_val[i].word_name+"_write_value"),word_expect_val[i].expect_val);
			  TestSet.cont(true).TEST("",word_read_val[i].word_name+"_read_value",tmLimits.getLimitObj(word_read_val[i].word_name+"_read_value"),word_read_val[i].read_val);
		  }
	  }

	  if(Gen_testTable){
		  Efuse.Generate_testTable("flag_efuse_cp_post_check",1,1);
		  Gen_testTable=0;
	  }

	  TestSet.cont(true).TEST("","flag_efuse_cp_post_check",tmLimits.getLimitObj("flag_efuse_cp_post_check"),flag_efuse_cp_post_check);

	 //Add your test code here.
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
     *This function will be invoked once the Select Test Method Dialog is opened
     */
    virtual const string getComment() const
    {
      string comment = " please add your comment for this method.";
      return comment;
    }
  };
REGISTER_TESTMETHOD("bm_efuse_tml.CP.Efuse_CP_Post_Check", Efuse_CP_Post_Check);
//
class Efuse_FT_RT_Check: public testmethod::TestMethod {

	protected:
	  /**
	*Initialize the parameter interface to the testflow.
	*This method is called just once after a testsuite is created.
	*/
	   int 		debug_mode;
	   int 		D2SMode;
	   int   	mGen_testTable;
	   virtual void initialize()
	   {
		 //Add your initialization code here
		 //Note: Test Method API should not be used in this method!
			 addParameter("debug_mode",
						  "int",
						  &debug_mode)
						  .setDefault("0")
						  .setOptions("0:1")
						  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

			 addParameter("D2SMode", "int", &D2SMode)
						   .setDefault("1")
						   .setOptions("1:2")
						   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");
			 addParameter("Gen_testTable",
						  "int",
						  &mGen_testTable)
						  .setDefault("0")
						  .setOptions("0:1")
						  .setComment("Gen_testTable  0:no_need_Gen_testTable, 1:Gen_testTable");
	   }

  /**
*This test is invoked per site.
*/
  virtual void run()
  {

	  __D2SREADBLOCKCONFIG__

	  d2sFramework fr;
	  fr.registerTransactionPort(myD2S);
	  d2sFrameWorkModeType::Enum d2s_mode;

	  switch ( D2SMode )
	  {
			case 0:
			case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
			case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
	  }

	  static vector<int> ADDRS;

	  static EFUSE Efuse;

	  ON_FIRST_INVOCATION_BEGIN();

		  Efuse.ReadCsv();

		  ADDRS = Efuse.getFTReadAddr();

		  fr.d2s_LABEL_BEGIN("efuse_ft_rt_check",d2s_mode);
			  for(size_t i=0;i<ADDRS.size();i++){
				int addr=ADDRS[i];
	//		  		if(debug_mode) cout<<"read addr =0x"<<hex<<addr<<endl;
				char tmpchar[256];
				sprintf(tmpchar,"0x%x",addr);
				myD2S.read(addr,tmpchar);
			  }
		  fr.d2s_LABEL_END();

	  ON_FIRST_INVOCATION_END();

	  vector<long> DATAS;
	  DATAS.clear();
	  for(size_t i=0;i<ADDRS.size();i++){
		  int addr=ADDRS[i];
		  char tmpchar[256];
		  sprintf(tmpchar,"0x%x",addr);
		  long tmp_data=myD2S.getReadValue(tmpchar);
		  DATAS.push_back(tmp_data);
//		  if(debug_mode) cout<<"read addr =0x"<<hex<<addr<<",read value =0x"<<tmp_data<<endl;
	  }

	  vector<WRV> word_read_val;
	  word_read_val = Efuse.getFTWordReadValue(DATAS);
	  Efuse.FTReadCRC8Calc(debug_mode);
	  long crc8_calc_val=Efuse.getFTCRC8CalcValue();

	  long crc8_read_val=0;
	  int Gen_testTable=mGen_testTable;

	  for(size_t i=0;i<word_read_val.size();i++){
		  if("chip_id"==word_read_val[i].word_name){
			  if(Gen_testTable){
				  Efuse.Generate_testTable("chip_id_read_value");
				  Efuse.Generate_testTable("fabcode_read_value");
			  }

			  TestSet.cont(true).TEST("","chip_id_read_value",tmLimits.getLimitObj("chip_id_read_value"),word_read_val[i].read_val&0x0fffffff);

			  TestSet.cont(true).TEST("","fabcode_read_value",tmLimits.getLimitObj("fabcode_read_value"),word_read_val[i].read_val>>28 &0xf);
		  }
		  else{
			  if(Gen_testTable){

				  Efuse.Generate_testTable(word_read_val[i].word_name+"_read_value");

			  }
			  if("crc_ate_ft"==word_read_val[i].word_name){

				  if(Gen_testTable) Efuse.Generate_testTable("crc_ate_ft_calc_value");

				  crc8_read_val=word_read_val[i].read_val;

				  TestSet.cont(true).TEST("","crc_ate_ft_calc_value",tmLimits.getLimitObj("crc_ate_ft_calc_value"),crc8_calc_val);
			  }
			  TestSet.cont(true).TEST("",word_read_val[i].word_name+"_read_value",tmLimits.getLimitObj(word_read_val[i].word_name+"_read_value"),word_read_val[i].read_val);
		  }
	  }

//
	  if(Gen_testTable){
		  Efuse.Generate_testTable("flag_efuse_ft_rt_check",1,1);

		  Gen_testTable=0;
	  }
	  int flag_efuse_ft_rt_check=0;
	  if(crc8_calc_val==crc8_read_val){
		  flag_efuse_ft_rt_check=1;
	  }

	  TestSet.cont(true).TEST("","flag_efuse_ft_rt_check",tmLimits.getLimitObj("flag_efuse_ft_rt_check"),flag_efuse_ft_rt_check);


   	 //Add your test code here.
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
	*This function will be invoked once the Select Test Method Dialog is opened
	*/
   virtual const string getComment() const
   {
	 string comment = " please add your comment for this method.";
	 return comment;
   }
 };

REGISTER_TESTMETHOD("bm_efuse_tml.FT.Efuse_FT_RT_Check", Efuse_FT_RT_Check);
//
class Efuse_CP_RT_Check: public testmethod::TestMethod {

	protected:
	  /**
	*Initialize the parameter interface to the testflow.
	*This method is called just once after a testsuite is created.
	*/
	   int 		debug_mode;
	   int 		D2SMode;
	   int   	mGen_testTable;
	   virtual void initialize()
	   {
		 //Add your initialization code here
		 //Note: Test Method API should not be used in this method!
			 addParameter("debug_mode",
						  "int",
						  &debug_mode)
						  .setDefault("0")
						  .setOptions("0:1")
						  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

			 addParameter("D2SMode", "int", &D2SMode)
						   .setDefault("1")
						   .setOptions("1:2")
						   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");
			 addParameter("Gen_testTable",
						  "int",
						  &mGen_testTable)
						  .setDefault("0")
						  .setOptions("0:1")
						  .setComment("Gen_testTable  0:no_need_Gen_testTable, 1:Gen_testTable");
	   }

  /**
*This test is invoked per site.
*/
  virtual void run()
  {

	  __D2SREADBLOCKCONFIG__

	  d2sFramework fr;
	  fr.registerTransactionPort(myD2S);
	  d2sFrameWorkModeType::Enum d2s_mode;

	  switch ( D2SMode )
	  {
			case 0:
			case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
			case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
	  }

	  static vector<int> ADDRS;

	  static EFUSE Efuse;

	  ON_FIRST_INVOCATION_BEGIN();

		  Efuse.ReadCsv();

		  ADDRS = Efuse.getCPReadAddr();

		  fr.d2s_LABEL_BEGIN("efuse_cp_rt_check",d2s_mode);
		  	  for(size_t i=0;i<ADDRS.size();i++){
		  		int addr=ADDRS[i];
//		  		if(debug_mode) cout<<"read addr =0x"<<hex<<addr<<endl;
		  		char tmpchar[256];
		  		sprintf(tmpchar,"0x%x",addr);
		  		myD2S.read(addr,tmpchar);
		  	  }

		  fr.d2s_LABEL_END();

	  ON_FIRST_INVOCATION_END();

	  vector<long> DATAS;
	  DATAS.clear();
	  for(size_t i=0;i<ADDRS.size();i++){
		  int addr=ADDRS[i];
		  char tmpchar[256];
		  sprintf(tmpchar,"0x%x",addr);
		  long tmp_data=myD2S.getReadValue(tmpchar);
		  DATAS.push_back(tmp_data);
//		  if(debug_mode) cout<<"read addr =0x"<<hex<<addr<<",read value =0x"<<tmp_data<<endl;
	  }

	  vector<WRV> word_read_val;
	  word_read_val = Efuse.getCPWordReadValue(DATAS);
	  Efuse.CPReadCRC8Calc(debug_mode);
	  long crc8_calc_val=Efuse.getCPCRC8CalcValue();

	  long crc8_read_val=0;
	  int Gen_testTable=mGen_testTable;

	  for(size_t i=0;i<word_read_val.size();i++){
		  if("chip_id"==word_read_val[i].word_name){
			  if(Gen_testTable){
				  Efuse.Generate_testTable("chip_id_read_value");
				  Efuse.Generate_testTable("fabcode_read_value");
			  }

			  TestSet.cont(true).TEST("","chip_id_read_value",tmLimits.getLimitObj("chip_id_read_value"),word_read_val[i].read_val&0x0fffffff);

			  TestSet.cont(true).TEST("","fabcode_read_value",tmLimits.getLimitObj("fabcode_read_value"),word_read_val[i].read_val>>28 &0xf);
		  }
		  else if("lot_id"==word_read_val[i].word_name){
			  if(Gen_testTable){
				  Efuse.Generate_testTable("lot_id0_read_value");
				  Efuse.Generate_testTable("lot_id1_read_value");
				  Efuse.Generate_testTable("lot_id2_read_value");
				  Efuse.Generate_testTable("lot_id3_read_value");
				  Efuse.Generate_testTable("lot_id4_read_value");
				  Efuse.Generate_testTable("lot_id5_read_value");
			  }
			  TestSet.cont(true).TEST("","lot_id5_read_value",tmLimits.getLimitObj("lot_id5_read_value"),word_read_val[i].read_val&0xff);
			  TestSet.cont(true).TEST("","lot_id4_read_value",tmLimits.getLimitObj("lot_id4_read_value"),(word_read_val[i].read_val>>8)&0xff);
			  TestSet.cont(true).TEST("","lot_id3_read_value",tmLimits.getLimitObj("lot_id3_read_value"),(word_read_val[i].read_val>>16)&0xff);
			  TestSet.cont(true).TEST("","lot_id2_read_value",tmLimits.getLimitObj("lot_id2_read_value"),(word_read_val[i].read_val>>24)&0xff);
			  TestSet.cont(true).TEST("","lot_id1_read_value",tmLimits.getLimitObj("lot_id1_read_value"),(word_read_val[i].read_val>>32)&0xff);
			  TestSet.cont(true).TEST("","lot_id0_read_value",tmLimits.getLimitObj("lot_id0_read_value"),(word_read_val[i].read_val>>40)&0xff);

		  }
		  else{
			  if(Gen_testTable){

				  Efuse.Generate_testTable(word_read_val[i].word_name+"_read_value");
			  }
			  if("crc_ate_cp"==word_read_val[i].word_name){
				  crc8_read_val=word_read_val[i].read_val;
				  TestSet.cont(true).TEST("","crc_ate_cp_calc_value",tmLimits.getLimitObj("crc_ate_cp_calc_value"),crc8_calc_val);
			  }
			  if("x_address"==word_read_val[i].word_name){
				  SET_USER_DOUBLE("x_address",word_read_val[i].read_val);//set back the read value to uservar
			  }
			  if("y_address"==word_read_val[i].word_name){
				  SET_USER_DOUBLE("y_address",word_read_val[i].read_val);//set back the read value to uservar
			  }
			  TestSet.cont(true).TEST("",word_read_val[i].word_name+"_read_value",tmLimits.getLimitObj(word_read_val[i].word_name+"_read_value"),word_read_val[i].read_val);
		  }
	  }

//
	  if(Gen_testTable){
		  Efuse.Generate_testTable("flag_efuse_cp_rt_check",1,1);
		  Efuse.Generate_testTable("crc_ate_cp_calc_value");
		  Gen_testTable=0;
	  }

	  int flag_addr=0;
	  long x_addr=0;
	  long y_addr=0;
	  GetDiePosXYOfSite(CURRENT_SITE_NUMBER(),&x_addr,&y_addr);
	  double x_address_val=0;
	  double y_address_val=0;
	  GET_USER_DOUBLE("x_address",&x_address_val);
	  GET_USER_DOUBLE("y_address",&y_address_val);
	  if((-9999==x_addr && -9999==y_addr) || (x_address_val==x_addr && y_address_val==y_addr)){
		  flag_addr=1;
	  }

	  int flag_efuse_cp_rt_check=0;
	  if(crc8_calc_val==crc8_read_val && 1==flag_addr){
		  flag_efuse_cp_rt_check=1;
	  }

	  TestSet.cont(true).TEST("","flag_efuse_cp_rt_check",tmLimits.getLimitObj("flag_efuse_cp_rt_check"),flag_efuse_cp_rt_check);

   	 //Add your test code here.
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
	*This function will be invoked once the Select Test Method Dialog is opened
	*/
   virtual const string getComment() const
   {
	 string comment = " please add your comment for this method.";
	 return comment;
   }
 };

REGISTER_TESTMETHOD("bm_efuse_tml.CP.Efuse_CP_RT_Check", Efuse_CP_RT_Check);
//
class Efuse_CP_RT_Pre_Check: public testmethod::TestMethod {

	protected:
	  /**
	*Initialize the parameter interface to the testflow.
	*This method is called just once after a testsuite is created.
	*/
	   int 		debug_mode;
	   int 		D2SMode;
	   int   	mGen_testTable;
	   virtual void initialize()
	   {
		 //Add your initialization code here
		 //Note: Test Method API should not be used in this method!
			 addParameter("debug_mode",
						  "int",
						  &debug_mode)
						  .setDefault("0")
						  .setOptions("0:1")
						  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

			 addParameter("D2SMode", "int", &D2SMode)
						   .setDefault("1")
						   .setOptions("1:2")
						   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");
			 addParameter("Gen_testTable",
						  "int",
						  &mGen_testTable)
						  .setDefault("0")
						  .setOptions("0:1")
						  .setComment("Gen_testTable  0:no_need_Gen_testTable, 1:Gen_testTable");
	   }

  /**
*This test is invoked per site.
*/
   virtual void run()
   {

	  __D2SREADBLOCKCONFIG__

	  d2sFramework fr;
	  fr.registerTransactionPort(myD2S);
	  d2sFrameWorkModeType::Enum d2s_mode;

	  switch ( D2SMode )
	  {
			case 0:
			case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
			case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
	  }

	  static vector<int> ADDRS;

	  static EFUSE Efuse;

	  ON_FIRST_INVOCATION_BEGIN();

		  Efuse.ReadCsv();

		  ADDRS = Efuse.getCPReadAddr();

		  fr.d2s_LABEL_BEGIN("efuse_cp_rt_pre_check",d2s_mode);
			  for(size_t i=0;i<ADDRS.size();i++){
				int addr=ADDRS[i];
//				if(debug_mode) cout<<"read addr =0x"<<hex<<addr<<endl;
				char tmpchar[256];
				sprintf(tmpchar,"0x%x",addr);
				myD2S.read(addr,tmpchar);
			  }

		  fr.d2s_LABEL_END();

	  ON_FIRST_INVOCATION_END();

	  vector<long> DATAS;
	  DATAS.clear();
	  for(size_t i=0;i<ADDRS.size();i++){
		  int addr=ADDRS[i];
		  char tmpchar[256];
		  sprintf(tmpchar,"0x%x",addr);
		  long tmp_data=myD2S.getReadValue(tmpchar);
		  DATAS.push_back(tmp_data);
//		  if(debug_mode) cout<<"read addr =0x"<<hex<<addr<<",read value =0x"<<tmp_data<<endl;
	  }

	  vector<WRV> word_read_val;
	  word_read_val = Efuse.getCPWordReadValue(DATAS);
	  Efuse.CPReadCRC8Calc(debug_mode);
	  long crc8_calc_val=Efuse.getCPCRC8CalcValue();

	  long crc8_read_val=0;
	  int Gen_testTable=mGen_testTable;

	  for(size_t i=0;i<word_read_val.size();i++){
		  if("chip_id"==word_read_val[i].word_name){
			  if(Gen_testTable){
				  Efuse.Generate_testTable("chip_id_read_value");
				  Efuse.Generate_testTable("fabcode_read_value");
			  }

			  TestSet.cont(true).TEST("","chip_id_read_value",tmLimits.getLimitObj("chip_id_read_value"),word_read_val[i].read_val&0x0fffffff);

			  TestSet.cont(true).TEST("","fabcode_read_value",tmLimits.getLimitObj("fabcode_read_value"),word_read_val[i].read_val>>28 &0xf);
		  }
		  else if("lot_id"==word_read_val[i].word_name){
			  if(Gen_testTable){
				  Efuse.Generate_testTable("lot_id0_read_value");
				  Efuse.Generate_testTable("lot_id1_read_value");
				  Efuse.Generate_testTable("lot_id2_read_value");
				  Efuse.Generate_testTable("lot_id3_read_value");
				  Efuse.Generate_testTable("lot_id4_read_value");
				  Efuse.Generate_testTable("lot_id5_read_value");
			  }
			  TestSet.cont(true).TEST("","lot_id5_read_value",tmLimits.getLimitObj("lot_id5_read_value"),word_read_val[i].read_val&0xff);
			  TestSet.cont(true).TEST("","lot_id4_read_value",tmLimits.getLimitObj("lot_id4_read_value"),(word_read_val[i].read_val>>8)&0xff);
			  TestSet.cont(true).TEST("","lot_id3_read_value",tmLimits.getLimitObj("lot_id3_read_value"),(word_read_val[i].read_val>>16)&0xff);
			  TestSet.cont(true).TEST("","lot_id2_read_value",tmLimits.getLimitObj("lot_id2_read_value"),(word_read_val[i].read_val>>24)&0xff);
			  TestSet.cont(true).TEST("","lot_id1_read_value",tmLimits.getLimitObj("lot_id1_read_value"),(word_read_val[i].read_val>>32)&0xff);
			  TestSet.cont(true).TEST("","lot_id0_read_value",tmLimits.getLimitObj("lot_id0_read_value"),(word_read_val[i].read_val>>40)&0xff);

		  }
		  else{
			  if(Gen_testTable){

				  Efuse.Generate_testTable(word_read_val[i].word_name+"_read_value");
			  }
			  if("crc_ate_cp"==word_read_val[i].word_name){

				  if(Gen_testTable) Efuse.Generate_testTable("crc_ate_cp_calc_value");

				  crc8_read_val=word_read_val[i].read_val;
				  TestSet.cont(true).TEST("","crc_ate_cp_calc_value",tmLimits.getLimitObj("crc_ate_cp_calc_value"),crc8_calc_val);
			  }
			  TestSet.cont(true).TEST("",word_read_val[i].word_name+"_read_value",tmLimits.getLimitObj(word_read_val[i].word_name+"_read_value"),word_read_val[i].read_val);
		  }
	  }

 //
	  if(Gen_testTable){
		  Efuse.Generate_testTable("flag_efuse_cp_rt_pre_check",1,1);

		  Gen_testTable=0;
	  }
	  int flag_efuse_cp_rt_pre_check=0;
	  if(crc8_calc_val==crc8_read_val){
		  flag_efuse_cp_rt_pre_check=1;
	  }

	  TestSet.cont(true).TEST("","flag_efuse_cp_rt_pre_check",tmLimits.getLimitObj("flag_efuse_cp_rt_pre_check"),flag_efuse_cp_rt_pre_check);

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
	*This function will be invoked once the Select Test Method Dialog is opened
	*/
   virtual const string getComment() const
   {
	 string comment = " please add your comment for this method.";
	 return comment;
   }
 };

REGISTER_TESTMETHOD("bm_efuse_tml.FT.Efuse_CP_RT_Pre_Check", Efuse_CP_RT_Pre_Check);
////
class Efuse_CP_RT_Post_Check: public testmethod::TestMethod {

	protected:
	  /**
	*Initialize the parameter interface to the testflow.
	*This method is called just once after a testsuite is created.
	*/
	   int 		debug_mode;
	   int 		D2SMode;
	   int   	mGen_testTable;
	   virtual void initialize()
	   {
		 //Add your initialization code here
		 //Note: Test Method API should not be used in this method!
			 addParameter("debug_mode",
						  "int",
						  &debug_mode)
						  .setDefault("0")
						  .setOptions("0:1")
						  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

			 addParameter("D2SMode", "int", &D2SMode)
						   .setDefault("1")
						   .setOptions("1:2")
						   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");
			 addParameter("Gen_testTable",
						  "int",
						  &mGen_testTable)
						  .setDefault("0")
						  .setOptions("0:1")
						  .setComment("Gen_testTable  0:no_need_Gen_testTable, 1:Gen_testTable");
	   }

  /**
*This test is invoked per site.
*/
   virtual void run()
   {

	  __D2SREADBLOCKCONFIG__

	  d2sFramework fr;
	  fr.registerTransactionPort(myD2S);
	  d2sFrameWorkModeType::Enum d2s_mode;

	  switch ( D2SMode )
	  {
			case 0:
			case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
			case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
	  }

	  static vector<int> ADDRS;

	  static EFUSE Efuse;

	  ON_FIRST_INVOCATION_BEGIN();

		  Efuse.ReadCsv();

		  ADDRS = Efuse.getCPReadAddr();

		  fr.d2s_LABEL_BEGIN("efuse_cp_rt_post_check",d2s_mode);
			  for(size_t i=0;i<ADDRS.size();i++){
				int addr=ADDRS[i];
//				if(debug_mode) cout<<"read addr =0x"<<hex<<addr<<endl;
				char tmpchar[256];
				sprintf(tmpchar,"0x%x",addr);
				myD2S.read(addr,tmpchar);
			  }

		  fr.d2s_LABEL_END();

	  ON_FIRST_INVOCATION_END();

	  vector<long> DATAS;
	  DATAS.clear();
	  for(size_t i=0;i<ADDRS.size();i++){
		  int addr=ADDRS[i];
		  char tmpchar[256];
		  sprintf(tmpchar,"0x%x",addr);
		  long tmp_data=myD2S.getReadValue(tmpchar);
		  DATAS.push_back(tmp_data);
//		  if(debug_mode) cout<<"read addr =0x"<<hex<<addr<<",read value =0x"<<tmp_data<<endl;
	  }

	  vector<WRV> word_read_val;
	  word_read_val = Efuse.getCPWordReadValue(DATAS);
	  Efuse.CPReadCRC8Calc(debug_mode);
	  long crc8_calc_val=Efuse.getCPCRC8CalcValue();

	  long crc8_read_val=0;
	  int Gen_testTable=mGen_testTable;

	  for(size_t i=0;i<word_read_val.size();i++){
		  if("chip_id"==word_read_val[i].word_name){
			  if(Gen_testTable){
				  Efuse.Generate_testTable("chip_id_read_value");
				  Efuse.Generate_testTable("fabcode_read_value");
			  }

			  TestSet.cont(true).TEST("","chip_id_read_value",tmLimits.getLimitObj("chip_id_read_value"),word_read_val[i].read_val&0x0fffffff);

			  TestSet.cont(true).TEST("","fabcode_read_value",tmLimits.getLimitObj("fabcode_read_value"),word_read_val[i].read_val>>28 &0xf);
		  }
		  else if("lot_id"==word_read_val[i].word_name){
			  if(Gen_testTable){
				  Efuse.Generate_testTable("lot_id0_read_value");
				  Efuse.Generate_testTable("lot_id1_read_value");
				  Efuse.Generate_testTable("lot_id2_read_value");
				  Efuse.Generate_testTable("lot_id3_read_value");
				  Efuse.Generate_testTable("lot_id4_read_value");
				  Efuse.Generate_testTable("lot_id5_read_value");
			  }
			  TestSet.cont(true).TEST("","lot_id5_read_value",tmLimits.getLimitObj("lot_id5_read_value"),word_read_val[i].read_val&0xff);
			  TestSet.cont(true).TEST("","lot_id4_read_value",tmLimits.getLimitObj("lot_id4_read_value"),(word_read_val[i].read_val>>8)&0xff);
			  TestSet.cont(true).TEST("","lot_id3_read_value",tmLimits.getLimitObj("lot_id3_read_value"),(word_read_val[i].read_val>>16)&0xff);
			  TestSet.cont(true).TEST("","lot_id2_read_value",tmLimits.getLimitObj("lot_id2_read_value"),(word_read_val[i].read_val>>24)&0xff);
			  TestSet.cont(true).TEST("","lot_id1_read_value",tmLimits.getLimitObj("lot_id1_read_value"),(word_read_val[i].read_val>>32)&0xff);
			  TestSet.cont(true).TEST("","lot_id0_read_value",tmLimits.getLimitObj("lot_id0_read_value"),(word_read_val[i].read_val>>40)&0xff);

		  }
		  else{
			  if(Gen_testTable){

				  Efuse.Generate_testTable(word_read_val[i].word_name+"_read_value");

			  }
			  if("crc_ate_cp"==word_read_val[i].word_name){

				  if(Gen_testTable) Efuse.Generate_testTable("crc_ate_cp_calc_value");

				  crc8_read_val=word_read_val[i].read_val;
				  TestSet.cont(true).TEST("","crc_ate_cp_calc_value",tmLimits.getLimitObj("crc_ate_cp_calc_value"),crc8_calc_val);
			  }
			  TestSet.cont(true).TEST("",word_read_val[i].word_name+"_read_value",tmLimits.getLimitObj(word_read_val[i].word_name+"_read_value"),word_read_val[i].read_val);
		  }
	  }

 //
	  if(Gen_testTable){
		  Efuse.Generate_testTable("flag_efuse_cp_rt_post_check",1,1);

		  Gen_testTable=0;
	  }
	  int flag_efuse_cp_rt_post_check=0;
	  if(crc8_calc_val==crc8_read_val){
		  flag_efuse_cp_rt_post_check=1;
	  }

	  TestSet.cont(true).TEST("","flag_efuse_cp_rt_post_check",tmLimits.getLimitObj("flag_efuse_cp_rt_post_check"),flag_efuse_cp_rt_post_check);

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
	*This function will be invoked once the Select Test Method Dialog is opened
	*/
   virtual const string getComment() const
   {
	 string comment = " please add your comment for this method.";
	 return comment;
   }
 };

REGISTER_TESTMETHOD("bm_efuse_tml.FT.Efuse_CP_RT_Post_Check", Efuse_CP_RT_Post_Check);

//
class Efuse_Non_CP_Pre_Check: public testmethod::TestMethod {

protected:
/**
*Initialize the parameter interface to the testflow.
*This method is called just once after a testsuite is created.
*/
   int 		debug_mode;
   int 		D2SMode;
   int   	mGen_testTable;
   virtual void initialize()
   {
	 //Add your initialization code here
	 //Note: Test Method API should not be used in this method!
		 addParameter("debug_mode",
					  "int",
					  &debug_mode)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

		 addParameter("D2SMode", "int", &D2SMode)
					   .setDefault("1")
					   .setOptions("1:2")
					   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");
		 addParameter("Gen_testTable",
					  "int",
					  &mGen_testTable)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("Gen_testTable  0:no_need_Gen_testTable, 1:Gen_testTable");
   }

/**
*This test is invoked per site.
*/
virtual void run()
{

	  __D2SREADBLOCKCONFIG__
	  d2sFramework fr;
	  fr.registerTransactionPort(myD2S);
	  d2sFrameWorkModeType::Enum d2s_mode;

	  switch ( D2SMode )
	  {
		case 0:
		case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
		case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
	  }

	  static EFUSE Efuse;

	  ON_FIRST_INVOCATION_BEGIN();

		  Efuse.ReadCsv();

		  vector<int> ADDRS = Efuse.getCPReadAddr();

		  vector<WI> word_info = Efuse.getCPWordInfo();

	  	  myD2S.enablePassFailCheckForNextExecution();

		  fr.d2s_LABEL_BEGIN("efuse_non_cp_pre_check",d2s_mode);

		  // check if cp unused address bits is 0

			double tmp_val;
			int READ_ADDRS;

			GET_USER_DOUBLE("READ_ADDRS",&tmp_val);
			READ_ADDRS = int(tmp_val);

			  for(int i=0;i<(1<<READ_ADDRS);i++){

				  int mask_bit=0;

				  for(size_t j=0;j<ADDRS.size();j++){
					if(i==ADDRS[j]){
					  for(size_t m=0;m<word_info.size();m++){
						  for(int n=0;n<word_info[m].bits_cnt;n++){
							  int read_addr=word_info[m].bit_info[n].read_addr;
							  int bit_index=word_info[m].bit_info[n].bit_index;
							  if(i==read_addr){
								  mask_bit=mask_bit | (1<<bit_index);
								  if(debug_mode) cout<<"used address:0x"<<hex<<i<<", bit:"<<dec<<bit_index<<";"<<endl;
							  }
						  }
					  }
					  break;
				 }
			  }

			  myD2S.expectValue(i,0,mask_bit);//addr,data,maskbit

		  }

		  fr.d2s_LABEL_END();

	  ON_FIRST_INVOCATION_END();

	  int Gen_testTable=mGen_testTable;

	  int efuse_non_cp_pre_check=0;

	 // check if non cp,ft used address bits is 0
	  if(myD2S.hasPassed("efuse_non_cp_pre_check"))  efuse_non_cp_pre_check=1;
	  if(Gen_testTable){
		  Efuse.Generate_testTable("efuse_non_cp_pre_check",1,1);
		  Gen_testTable=0;
	  }
	  TestSet.cont(true).TEST("","efuse_non_cp_pre_check",tmLimits.getLimitObj("efuse_non_cp_pre_check"),efuse_non_cp_pre_check);

	 //Add your test code here.
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
	*This function will be invoked once the Select Test Method Dialog is opened
	*/
   virtual const string getComment() const
   {
	 string comment = " please add your comment for this method.";
	 return comment;
   }
 };

REGISTER_TESTMETHOD("bm_efuse_tml.CP.Efuse_Non_CP_Pre_Check", Efuse_Non_CP_Pre_Check);
//
class Efuse_Non_CP_FT_Pre_Check: public testmethod::TestMethod {

 protected:
 /**
 *Initialize the parameter interface to the testflow.
 *This method is called just once after a testsuite is created.
 */
    int 		debug_mode;
    int 		D2SMode;
    int   	    mGen_testTable;
    virtual void initialize()
    {
 	 //Add your initialization code here
 	 //Note: Test Method API should not be used in this method!
 		 addParameter("debug_mode",
 					  "int",
 					  &debug_mode)
 					  .setDefault("0")
 					  .setOptions("0:1")
 					  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

 		 addParameter("D2SMode", "int", &D2SMode)
 					   .setDefault("1")
 					   .setOptions("1:2")
 					   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");
 		 addParameter("Gen_testTable",
 					  "int",
 					  &mGen_testTable)
 					  .setDefault("0")
 					  .setOptions("0:1")
 					  .setComment("Gen_testTable  0:no_need_Gen_testTable, 1:Gen_testTable");
    }

 /**
 *This test is invoked per site.
 */
 virtual void run()
 {
 	  __D2SREADBLOCKCONFIG__
 	  d2sFramework fr;
 	  fr.registerTransactionPort(myD2S);
 	  d2sFrameWorkModeType::Enum d2s_mode;

 	  switch ( D2SMode )
 	  {
 		case 0:
 		case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
 		case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
 	  }

 	  static EFUSE Efuse;

 	  ON_FIRST_INVOCATION_BEGIN();

 	  	  Efuse.ReadCsv();

		  vector<WI> word_info_ft = Efuse.getFTWordInfo();
		  vector<WI> word_info_cp = Efuse.getCPWordInfo();

	 	  vector<int> ADDRS_FT = Efuse.getFTReadAddr();
	 	  vector<int> ADDRS_CP = Efuse.getCPReadAddr();

		  double tmp_val;
		  int READ_ADDRS;

		  GET_USER_DOUBLE("READ_ADDRS",&tmp_val);
		  READ_ADDRS = int(tmp_val);

 	  	  myD2S.enablePassFailCheckForNextExecution();

 		  fr.d2s_LABEL_BEGIN("efuse_non_cp_ft_pre_check",d2s_mode);

 		  // check if cp unused address bits is 0

 			  for(int i=0;i<(1<<READ_ADDRS);i++){

 				  int mask_bit=0;

 				  for(size_t j=0;j<ADDRS_FT.size();j++){
 					if(i==ADDRS_FT[j]){
 					  for(size_t m=0;m<word_info_ft.size();m++){
 						  for(int n=0;n<word_info_ft[m].bits_cnt;n++){
 							  int read_addr=word_info_ft[m].bit_info[n].read_addr;
 							  int bit_index=word_info_ft[m].bit_info[n].bit_index;
 							  if(i==read_addr){
 								  mask_bit=mask_bit | (1<<bit_index);
 								  if(debug_mode) cout<<"used address:0x"<<hex<<i<<", bit:"<<dec<<bit_index<<";"<<endl;
 							  }
 						  }
 					  }
 					  break;
 					}
 				  }
 				  for(size_t j=0;j<ADDRS_CP.size();j++){
 					if(i==ADDRS_CP[j]){
 					  for(size_t m=0;m<word_info_cp.size();m++){
 						  for(int n=0;n<word_info_cp[m].bits_cnt;n++){
 							  int read_addr=word_info_cp[m].bit_info[n].read_addr;
 							  int bit_index=word_info_cp[m].bit_info[n].bit_index;
 							  if(i==read_addr){
 								 mask_bit=mask_bit | (1<<bit_index);
 								 if(debug_mode) cout<<"used address:0x"<<hex<<i<<", bit:"<<dec<<bit_index<<";"<<endl;
 							  }
 						  }
 					  }
 					  break;
 				 }
 			  }

 			myD2S.expectValue(i,0,mask_bit);//addr,data,maskbit

 		  }

 		  fr.d2s_LABEL_END();

 	  ON_FIRST_INVOCATION_END();

 	  int Gen_testTable=mGen_testTable;

 	  int efuse_non_cp_ft_pre_check=0;

 	 // check if non cp,ft used address bits is 0

 	  if(myD2S.hasPassed("efuse_non_cp_ft_pre_check"))  efuse_non_cp_ft_pre_check=1;
 	  if(Gen_testTable){
 		  Efuse.Generate_testTable("efuse_non_cp_ft_pre_check",1,1);
 		  Gen_testTable=0;
 	  }
 	  TestSet.cont(true).TEST("","efuse_non_cp_ft_pre_check",tmLimits.getLimitObj("efuse_non_cp_ft_pre_check"),efuse_non_cp_ft_pre_check);

 	 //Add your test code here.
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
 	*This function will be invoked once the Select Test Method Dialog is opened
 	*/
    virtual const string getComment() const
    {
 	 string comment = " please add your comment for this method.";
 	 return comment;
    }
  };
REGISTER_TESTMETHOD("bm_efuse_tml.FT.Efuse_Non_CP_FT_Pre_Check", Efuse_Non_CP_FT_Pre_Check);
//
class Efuse_Non_CP_Post_Check: public testmethod::TestMethod {

protected:
/**
*Initialize the parameter interface to the testflow.
*This method is called just once after a testsuite is created.
*/
   int 		debug_mode;
   int 		D2SMode;
   int   	mGen_testTable;
   virtual void initialize()
   {
	 //Add your initialization code here
	 //Note: Test Method API should not be used in this method!
		 addParameter("debug_mode",
					  "int",
					  &debug_mode)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

		 addParameter("D2SMode", "int", &D2SMode)
					   .setDefault("1")
					   .setOptions("1:2")
					   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");
		 addParameter("Gen_testTable",
					  "int",
					  &mGen_testTable)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("Gen_testTable  0:no_need_Gen_testTable, 1:Gen_testTable");
   }

/**
*This test is invoked per site.
*/
virtual void run()
{
	  __D2SREADBLOCKCONFIG__
	  d2sFramework fr;
	  fr.registerTransactionPort(myD2S);
	  d2sFrameWorkModeType::Enum d2s_mode;

	  switch ( D2SMode )
	  {
		case 0:
		case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
		case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
	  }


	  static EFUSE Efuse;

	  ON_FIRST_INVOCATION_BEGIN();

		  Efuse.ReadCsv();

		  vector<int> ADDRS = Efuse.getCPReadAddr();

		  vector<WI> word_info = Efuse.getCPWordInfo();

	  	  myD2S.enablePassFailCheckForNextExecution();

		  fr.d2s_LABEL_BEGIN("efuse_non_cp_post_check",d2s_mode);

		  // check if cp unused address bits is 0
			  double tmp_val;
			  int READ_ADDRS;

			  GET_USER_DOUBLE("READ_ADDRS",&tmp_val);
			  READ_ADDRS = int(tmp_val);

			  for(int i=0;i<(1<<READ_ADDRS);i++){

				  int mask_bit=0;

				  for(size_t j=0;j<ADDRS.size();j++){
					if(i==ADDRS[j]){

					  for(size_t m=0;m<word_info.size();m++){

						  for(int n=0;n<word_info[m].bits_cnt;n++){

							  int read_addr=word_info[m].bit_info[n].read_addr;
							  int bit_index=word_info[m].bit_info[n].bit_index;


							  if(i==read_addr){
								  if(debug_mode) cout<<"used address:0x"<<hex<<i<<", bit:"<<dec<<bit_index<<";"<<endl;

								  mask_bit=mask_bit | (1<<bit_index);
							  }
						  }
					  }

					break;
				 }
			  }

			myD2S.expectValue(i,0,mask_bit);//addr,data,maskbit

		  }

		  fr.d2s_LABEL_END();

	  ON_FIRST_INVOCATION_END();

	  int Gen_testTable=mGen_testTable;

	  int efuse_non_cp_post_check=0;


	  if(myD2S.hasPassed("efuse_non_cp_post_check"))  efuse_non_cp_post_check=1;
	  if(Gen_testTable){
		  Efuse.Generate_testTable("efuse_non_cp_post_check",1,1);
		  Gen_testTable=0;
	  }
	  TestSet.cont(true).TEST("","efuse_non_cp_post_check",tmLimits.getLimitObj("efuse_non_cp_post_check"),efuse_non_cp_post_check);

	 //Add your test code here.
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
	*This function will be invoked once the Select Test Method Dialog is opened
	*/
   virtual const string getComment() const
   {
	 string comment = " please add your comment for this method.";
	 return comment;
   }
 };
REGISTER_TESTMETHOD("bm_efuse_tml.CP.Efuse_Non_CP_Post_Check", Efuse_Non_CP_Post_Check);
//
class Efuse_Non_CP_FT_Post_Check: public testmethod::TestMethod {

 protected:
 /**
 *Initialize the parameter interface to the testflow.
 *This method is called just once after a testsuite is created.
 */
    int 		debug_mode;
    int 		D2SMode;
    int   	    mGen_testTable;
    virtual void initialize()
    {
 	 //Add your initialization code here
 	 //Note: Test Method API should not be used in this method!
 		 addParameter("debug_mode",
 					  "int",
 					  &debug_mode)
 					  .setDefault("0")
 					  .setOptions("0:1")
 					  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

 		 addParameter("D2SMode", "int", &D2SMode)
 					   .setDefault("1")
 					   .setOptions("1:2")
 					   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");
 		 addParameter("Gen_testTable",
 					  "int",
 					  &mGen_testTable)
 					  .setDefault("0")
 					  .setOptions("0:1")
 					  .setComment("Gen_testTable  0:no_need_Gen_testTable, 1:Gen_testTable");
    }

 /**
 *This test is invoked per site.
 */
 virtual void run()
 {
 	  __D2SREADBLOCKCONFIG__
 	  d2sFramework fr;
 	  fr.registerTransactionPort(myD2S);
 	  d2sFrameWorkModeType::Enum d2s_mode;

 	  switch ( D2SMode )
 	  {
 		case 0:
 		case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
 		case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
 	  }

 	  static EFUSE Efuse;

 	  ON_FIRST_INVOCATION_BEGIN();

	  	  Efuse.ReadCsv();

		  vector<WI> word_info_ft = Efuse.getFTWordInfo();
		  vector<WI> word_info_cp = Efuse.getCPWordInfo();
	 	  vector<int> ADDRS_FT = Efuse.getFTReadAddr();
	 	  vector<int> ADDRS_CP = Efuse.getCPReadAddr();

		  double tmp_val;
		  int READ_ADDRS;

		  GET_USER_DOUBLE("READ_ADDRS",&tmp_val);
		  READ_ADDRS = int(tmp_val);

 	  	  myD2S.enablePassFailCheckForNextExecution();

 		  fr.d2s_LABEL_BEGIN("efuse_non_cp_ft_post_check",d2s_mode);

 		  // check if cp unused address bits is 0

 			  for(int i=0;i<(1<<READ_ADDRS);i++){

 				  int mask_bit=0;

 				  for(size_t j=0;j<ADDRS_FT.size();j++){
 					if(i==ADDRS_FT[j]){

 					  for(size_t m=0;m<word_info_ft.size();m++){

 						  for(int n=0;n<word_info_ft[m].bits_cnt;n++){

 							  int read_addr=word_info_ft[m].bit_info[n].read_addr;
 							  int bit_index=word_info_ft[m].bit_info[n].bit_index;


 							  if(i==read_addr){
 								  if(debug_mode) cout<<"used address:0x"<<hex<<i<<", bit:"<<dec<<bit_index<<";"<<endl;

 								  mask_bit=mask_bit | (1<<bit_index);
 							  }
 						  }
 					  }

 					break;
 				 }
 			  }
 				  for(size_t j=0;j<ADDRS_CP.size();j++){
 					if(i==ADDRS_CP[j]){

 					  for(size_t m=0;m<word_info_cp.size();m++){

 						  for(int n=0;n<word_info_cp[m].bits_cnt;n++){

 							  int read_addr=word_info_cp[m].bit_info[n].read_addr;
 							  int bit_index=word_info_cp[m].bit_info[n].bit_index;


 							  if(i==read_addr){
 								  if(debug_mode) cout<<"used address:0x"<<hex<<i<<", bit:"<<dec<<bit_index<<";"<<endl;

 								  mask_bit=mask_bit | (1<<bit_index);
 							  }
 						  }
 					  }

 					break;
 				 }
 			  }

 			myD2S.expectValue(i,0,mask_bit);//addr,data,maskbit

 		  }

 		  fr.d2s_LABEL_END();

 	  ON_FIRST_INVOCATION_END();

 	  int Gen_testTable=mGen_testTable;

 	  int efuse_non_cp_ft_post_check=0;

 	 // check if non cp,ft used address bits is 0

 	  if(myD2S.hasPassed("efuse_non_cp_ft_post_check"))  efuse_non_cp_ft_post_check=1;
 	  if(Gen_testTable){
 		  Efuse.Generate_testTable("efuse_non_cp_ft_post_check",1,1);
 		  Gen_testTable=0;
 	  }
 	  TestSet.cont(true).TEST("","efuse_non_cp_ft_post_check",tmLimits.getLimitObj("efuse_non_cp_ft_post_check"),efuse_non_cp_ft_post_check);

 	 //Add your test code here.
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
 	*This function will be invoked once the Select Test Method Dialog is opened
 	*/
    virtual const string getComment() const
    {
 	 string comment = " please add your comment for this method.";
 	 return comment;
    }
  };
REGISTER_TESTMETHOD("bm_efuse_tml.FT.Efuse_Non_CP_FT_Post_Check", Efuse_Non_CP_FT_Post_Check);
//
class Efuse_CP_Pre_Check: public testmethod::TestMethod {

protected:
/**
*Initialize the parameter interface to the testflow.
*This method is called just once after a testsuite is created.
*/
   int 		debug_mode;
   int 		D2SMode;
   int   	mGen_testTable;
   virtual void initialize()
   {
	 //Add your initialization code here
	 //Note: Test Method API should not be used in this method!
		 addParameter("debug_mode",
					  "int",
					  &debug_mode)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

		 addParameter("D2SMode", "int", &D2SMode)
					   .setDefault("1")
					   .setOptions("1:2")
					   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");
		 addParameter("Gen_testTable",
					  "int",
					  &mGen_testTable)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("Gen_testTable  0:no_need_Gen_testTable, 1:Gen_testTable");
   }

/**
*This test is invoked per site.
*/
virtual void run()
{
	  __D2SREADBLOCKCONFIG__
	  d2sFramework fr;
	  fr.registerTransactionPort(myD2S);
	  d2sFrameWorkModeType::Enum d2s_mode;

	  switch ( D2SMode )
	  {
		case 0:
		case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
		case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
	  }

	  static EFUSE Efuse;

	  ON_FIRST_INVOCATION_BEGIN();

	  	  Efuse.ReadCsv();

		  double tmp_val;
		  int READ_ADDRS;

		  GET_USER_DOUBLE("READ_ADDRS",&tmp_val);
		  READ_ADDRS = int(tmp_val);

		  vector<int> ADDRS = Efuse.getCPReadAddr();
		  vector<WI> word_info = Efuse.getCPWordInfo();

	  	  myD2S.enablePassFailCheckForNextExecution();

		  fr.d2s_LABEL_BEGIN("efuse_cp_pre_check",d2s_mode);

		  // check if cp unused address bits is 0

			  for(int i=0;i<(1<<READ_ADDRS);i++){

				  int mask_bit=0;

				  for(size_t j=0;j<ADDRS.size();j++){
					if(i==ADDRS[j]){
					  for(size_t m=0;m<word_info.size();m++){
						  for(int n=0;n<word_info[m].bits_cnt;n++){
							  int read_addr=word_info[m].bit_info[n].read_addr;
							  int bit_index=word_info[m].bit_info[n].bit_index;
							  if(i==read_addr){
								  mask_bit=mask_bit | (1<<bit_index);
								  if(debug_mode) cout<<"used address:0x"<<hex<<i<<", bit:"<<dec<<bit_index<<";"<<endl;
							  }
						  }
					  }
					  mask_bit = ~mask_bit;
					  myD2S.expectValue(i,0,mask_bit);//addr,data,maskbit
					  break;
				 }
			  }
		  }

		  fr.d2s_LABEL_END();

	  ON_FIRST_INVOCATION_END();

	  int Gen_testTable=mGen_testTable;

	  int efuse_cp_pre_check=0;

	 // check if non cp,ft used address bits is 0

	  if(myD2S.hasPassed("efuse_cp_pre_check"))  efuse_cp_pre_check=1;
	  if(Gen_testTable){
		  Efuse.Generate_testTable("efuse_cp_pre_check",1,1);
		  Gen_testTable=0;
	  }
	  TestSet.cont(true).TEST("","efuse_cp_pre_check",tmLimits.getLimitObj("efuse_cp_pre_check"),efuse_cp_pre_check);

	 //Add your test code here.
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
	*This function will be invoked once the Select Test Method Dialog is opened
	*/
   virtual const string getComment() const
   {
	 string comment = " please add your comment for this method.";
	 return comment;
   }
 };
REGISTER_TESTMETHOD("bm_efuse_tml.CP.Efuse_CP_Pre_Check", Efuse_CP_Pre_Check);
//
class Efuse_FT_Pre_Check: public testmethod::TestMethod {

protected:
/**
*Initialize the parameter interface to the testflow.
*This method is called just once after a testsuite is created.
*/
   int 		debug_mode;
   int 		D2SMode;
   int   	mGen_testTable;
   virtual void initialize()
   {
	 //Add your initialization code here
	 //Note: Test Method API should not be used in this method!
		 addParameter("debug_mode",
					  "int",
					  &debug_mode)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");

		 addParameter("D2SMode", "int", &D2SMode)
					   .setDefault("1")
					   .setOptions("1:2")
					   .setComment("d2s mode  1:LearningMode, 2:ProductionMode");
		 addParameter("Gen_testTable",
					  "int",
					  &mGen_testTable)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("Gen_testTable  0:no_need_Gen_testTable, 1:Gen_testTable");
   }

/**
*This test is invoked per site.
*/
virtual void run()
{
	  __D2SREADBLOCKCONFIG__
	  d2sFramework fr;
	  fr.registerTransactionPort(myD2S);
	  d2sFrameWorkModeType::Enum d2s_mode;

	  switch ( D2SMode )
	  {
		case 0:
		case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
		case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
	  }

	  static EFUSE Efuse;

	  ON_FIRST_INVOCATION_BEGIN();

	  	  Efuse.ReadCsv();
		  vector<int> ADDRS = Efuse.getFTReadAddr();
		  vector<WI> word_info = Efuse.getFTWordInfo();

		  double tmp_val;
		  int READ_ADDRS;

		  GET_USER_DOUBLE("READ_ADDRS",&tmp_val);
		  READ_ADDRS = int(tmp_val);

	  	  myD2S.enablePassFailCheckForNextExecution();

		  fr.d2s_LABEL_BEGIN("efuse_ft_pre_check",d2s_mode);

		  // check if cp unused address bits is 0

			  for(int i=0;i<(1<<READ_ADDRS);i++){

				  int mask_bit=0;

				  for(size_t j=0;j<ADDRS.size();j++){
					if(i==ADDRS[j]){
					  for(size_t m=0;m<word_info.size();m++){
						  for(int n=0;n<word_info[m].bits_cnt;n++){
							  int read_addr=word_info[m].bit_info[n].read_addr;
							  int bit_index=word_info[m].bit_info[n].bit_index;
							  if(i==read_addr){
								  mask_bit=mask_bit | (1<<bit_index);
								  if(debug_mode) cout<<"used address:0x"<<hex<<i<<", bit:"<<dec<<bit_index<<";"<<endl;
							  }
						  }
					  }
					  mask_bit = ~mask_bit;
					  myD2S.expectValue(i,0,mask_bit);//addr,data,maskbit
					  break;
				 }
			  }
		  }

		  fr.d2s_LABEL_END();

	  ON_FIRST_INVOCATION_END();

	  int Gen_testTable=mGen_testTable;

	  int efuse_ft_pre_check=0;

	 // check if non cp,ft used address bits is 0

	  if(myD2S.hasPassed("efuse_ft_pre_check"))  efuse_ft_pre_check=1;
	  if(Gen_testTable){
		  Efuse.Generate_testTable("efuse_ft_pre_check",1,1);
		  Gen_testTable=0;
	  }
	  TestSet.cont(true).TEST("","efuse_ft_pre_check",tmLimits.getLimitObj("efuse_ft_pre_check"),efuse_ft_pre_check);

	 //Add your test code here.
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
	*This function will be invoked once the Select Test Method Dialog is opened
	*/
   virtual const string getComment() const
   {
	 string comment = " please add your comment for this method.";
	 return comment;
   }
 };
REGISTER_TESTMETHOD("bm_efuse_tml.FT.Efuse_FT_Pre_Check", Efuse_FT_Pre_Check);
//
class Efuse_Word_Write: public testmethod::TestMethod {

 protected:
   /**
	*Initialize the parameter interface to the testflow.
	*This method is called just once after a testsuite is created.
	*/

   string	word_name;

   virtual void initialize()
   {
 	 //Add your initialization code here
 	 //Note: Test Method API should not be used in this method!

 		 addParameter("word_name", "string", &word_name)
 					   .setDefault("")
 					   .setComment("word name for write");
    }
   /**
	*This test is invoked per site.
	*/
   virtual void run()
   {

	  ON_FIRST_INVOCATION_BEGIN();

		  __D2SWRITEBLOCKCONFIG__

		  d2sFramework fr;
		  fr.registerTransactionPort(myD2S);


  	  	  char label_name[256];
	  	  sprintf(label_name,"%s_write",word_name.c_str());

	  	  EFUSE Efuse;

	  	  Efuse.ReadCsv();

		  fr.d2s_LABEL_BEGIN(label_name,d2sFrameWorkModeType::LearningMode);

			  FOR_EACH_SITE_BEGIN();

			    cout<<"site:"<<CURRENT_SITE_NUMBER()<<endl;
			  	Efuse.WordEfuseCodeCalc(word_name);

			  	vector<Register> WriteInfo = Efuse.getWordWriteInfo();

				  for(size_t i=0;i<WriteInfo.size();i++){
					  char tmpchar[256];
					  sprintf(tmpchar,"efuse_ft_write_addr_%ld",WriteInfo[i].addr);
					  myD2S.writeDynamic(WriteInfo[i].addr,WriteInfo[i].data,tmpchar);//

					  cout<<"addr:"<<dec<<WriteInfo[i].addr<<","<<"data:"<<WriteInfo[i].data<<";"<<endl;

				  }

			  FOR_EACH_SITE_END();

		  fr.d2s_LABEL_END();

	  ON_FIRST_INVOCATION_END();



	 //Add your test code here.
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
     *This function will be invoked once the Select Test Method Dialog is opened
     */
    virtual const string getComment() const
    {
      string comment = " please add your comment for this method.";
      return comment;
    }
  };
REGISTER_TESTMETHOD("bm_efuse_tml.Debug.Efuse_Word_Write", Efuse_Word_Write);
//
class Efuse_Word_Read: public testmethod::TestMethod {

  protected:
     /**
  	*Initialize the parameter interface to the testflow.
  	*This method is called just once after a testsuite is created.
  	*/
  	string		word_name;
    virtual void initialize()
    {
  	 //Add your initialization code here
  	 //Note: Test Method API should not be used in this method!

  		 addParameter("word_name", "string", &word_name)
  					   .setDefault("")
  					   .setComment("word name for read");
     }

     /**
  	*This test is invoked per site.
  	*/
     virtual void run()
     {

		  __D2SREADBLOCKCONFIG__

		  d2sFramework fr;
		  fr.registerTransactionPort(myD2S);


		  static vector<int> ADDRS;
		  static EFUSE Efuse;

		  ON_FIRST_INVOCATION_BEGIN();

		  	  Efuse.ReadCsv();

		  	  ADDRS = Efuse.getWordReadAddr(word_name);

		  	  char label_name[256];
	  	  	  sprintf(label_name,"%s_read",word_name.c_str());

			  fr.d2s_LABEL_BEGIN(label_name,d2sFrameWorkModeType::LearningMode);
			  	  for(size_t i=0;i<ADDRS.size();i++){
			  		int addr=ADDRS[i];
			  		char tmpchar[256];
			  		sprintf(tmpchar,"0x%x",addr);
			  		myD2S.read(addr,tmpchar);
			  	  }

			  fr.d2s_LABEL_END();

		  ON_FIRST_INVOCATION_END();

		  vector<long> DATAS;
		  DATAS.clear();
		  for(size_t i=0;i<ADDRS.size();i++){
			  int addr=ADDRS[i];
			  char tmpchar[256];
			  sprintf(tmpchar,"0x%x",addr);
			  long tmp_data=myD2S.getReadValue(tmpchar);
			  DATAS.push_back(tmp_data);
		  }

		  long word_value = Efuse.getWordReadValue(word_name,DATAS);

		  cout<<"the read value of word "<<word_name<<"is:0x"<<hex<<word_value<<endl;


  	 //Add your test code here.
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
   *This function will be invoked once the Select Test Method Dialog is opened
   */
  virtual const string getComment() const
  {
	string comment = " please add your comment for this method.";
	return comment;
  }
};
REGISTER_TESTMETHOD("bm_efuse_tml.Debug.Efuse_Word_Read", Efuse_Word_Read);
//
class Efuse_Bit_Write: public testmethod::TestMethod {

 protected:
   /**
	*Initialize the parameter interface to the testflow.
	*This method is called just once after a testsuite is created.
	*/
   int		Bit;

   virtual void initialize()
   {
 	 //Add your initialization code here
 	 //Note: Test Method API should not be used in this method!

 		 addParameter("Bit", "int", &Bit)
 					   .setDefault("")
 					   .setComment("Bit number for write");
    }
   /**
	*This test is invoked per site.
	*/
   virtual void run()
   {

	  ON_FIRST_INVOCATION_BEGIN();

		  __D2SWRITEBLOCKCONFIG__

		  d2sFramework fr;
		  fr.registerTransactionPort(myD2S);

  	  	  char label_name[256];
	  	  sprintf(label_name,"Bit%d_write",Bit);

		  fr.d2s_LABEL_BEGIN(label_name,d2sFrameWorkModeType::LearningMode);

			  FOR_EACH_SITE_BEGIN();

			  	myD2S.writeDynamic(Bit,1,label_name);//

			  	cout<<"site:"<<CURRENT_SITE_NUMBER()<<endl;
				cout<<"write 1 to Bit"<<dec<<Bit<<endl;

			  FOR_EACH_SITE_END();

		  fr.d2s_LABEL_END();

	  ON_FIRST_INVOCATION_END();



	 //Add your test code here.
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
     *This function will be invoked once the Select Test Method Dialog is opened
     */
    virtual const string getComment() const
    {
      string comment = " please add your comment for this method.";
      return comment;
    }
  };
REGISTER_TESTMETHOD("bm_efuse_tml.Debug.Efuse_Bit_Write", Efuse_Bit_Write);
//
class Efuse_Bit_Read: public testmethod::TestMethod {

  protected:
     /**
  	*Initialize the parameter interface to the testflow.
  	*This method is called just once after a testsuite is created.
  	*/
  	int			Bit;
    virtual void initialize()
    {
  	 //Add your initialization code here
  	 //Note: Test Method API should not be used in this method!

 		 addParameter("Bit", "int", &Bit)
 					   .setDefault("")
 					   .setComment("Bit number for read");
     }

     /**
  	*This test is invoked per site.
  	*/
     virtual void run()
     {

		  __D2SREADBLOCKCONFIG__

		  d2sFramework fr;
		  fr.registerTransactionPort(myD2S);


		  int read_addr,bit_index;

		  ON_FIRST_INVOCATION_BEGIN();

			string READ_ADDRS_TYPE;
			double tmp_val;
			int TOTAL_ADDRS;
			int READ_ADDRS;
			GET_USER_STRING("READ_ADDRS_TYPE",READ_ADDRS_TYPE);
			GET_USER_DOUBLE("TOTAL_ADDRS",&tmp_val);
			TOTAL_ADDRS = int(tmp_val);
			GET_USER_DOUBLE("READ_ADDRS",&tmp_val);
			READ_ADDRS = int(tmp_val);

			if("MSB"==READ_ADDRS_TYPE){
				int unmask_bit=0;
				for(int k=0;k<(TOTAL_ADDRS - READ_ADDRS);k++){
					unmask_bit=unmask_bit|1<<k;
				}
				read_addr=(Bit)>>(TOTAL_ADDRS - READ_ADDRS);
				bit_index=(Bit) & unmask_bit;
			}
			else if("LSB"==READ_ADDRS_TYPE){
				int unmask_bit=0;
				for(int k=0;k<READ_ADDRS;k++){
					unmask_bit=unmask_bit|1<<k;
				}
				bit_index=(Bit)>>READ_ADDRS;
				read_addr=(Bit) & unmask_bit;
			}


	  	  	  char label_name[256];
		  	  sprintf(label_name,"Bit%d_read",Bit);

			  fr.d2s_LABEL_BEGIN(label_name,d2sFrameWorkModeType::LearningMode);

				char tmpchar[256];
				sprintf(tmpchar,"0x%x",read_addr);
				myD2S.read(read_addr,tmpchar);

			  fr.d2s_LABEL_END();

		  ON_FIRST_INVOCATION_END();

		  char tmpchar[256];
		  sprintf(tmpchar,"0x%x",read_addr);
		  long tmp_data=myD2S.getReadValue(tmpchar)>>bit_index &0x1;

		  cout<<"read Bit"<<dec<<Bit<<",read value ="<<tmp_data<<endl;

  	 //Add your test code here.
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
   *This function will be invoked once the Select Test Method Dialog is opened
   */
  virtual const string getComment() const
  {
	string comment = " please add your comment for this method.";
	return comment;
  }
};
REGISTER_TESTMETHOD("bm_efuse_tml.Debug.Efuse_Bit_Read", Efuse_Bit_Read);
//
class Efuse_AllBits_Read: public testmethod::TestMethod {

  protected:
     /**
  	*Initialize the parameter interface to the testflow.
  	*This method is called just once after a testsuite is created.
  	*/

    virtual void initialize()
    {
  	 //Add your initialization code here
  	 //Note: Test Method API should not be used in this method!

     }

     /**
  	*This test is invoked per site.
  	*/
     virtual void run()
     {

		  __D2SREADBLOCKCONFIG__

		  d2sFramework fr;
		  fr.registerTransactionPort(myD2S);

		  double tmp_val;
		  int TOTAL_ADDRS;
		  int READ_ADDRS;

		  GET_USER_DOUBLE("TOTAL_ADDRS",&tmp_val);
		  TOTAL_ADDRS = int(tmp_val);
		  GET_USER_DOUBLE("READ_ADDRS",&tmp_val);
		  READ_ADDRS = int(tmp_val);

		  ON_FIRST_INVOCATION_BEGIN();

			  fr.d2s_LABEL_BEGIN("Efuse_AllBit_Read",d2sFrameWorkModeType::LearningMode);

			  	for(int i=0;i<(1<<READ_ADDRS);i++){
					char tmpchar[256];
					sprintf(tmpchar,"0x%x",i);
					myD2S.read(i,tmpchar);
			  	}

			  fr.d2s_LABEL_END();

		  ON_FIRST_INVOCATION_END();

		  cout<<std::left<<setw(10)<<"Bit:";
		  for(int i=0;i<(1<<(TOTAL_ADDRS - READ_ADDRS));i++)
			  cout<<setw(3)<<dec<<i;
		  cout<<endl;
		  for(int i=0;i<(1<<READ_ADDRS);i++){
			  char tmpchar[256];
			  sprintf(tmpchar,"0x%x",i);
			  long tmp_data=myD2S.getReadValue(tmpchar);
			  cout<<std::left<<"Addr"<<setw(6)<<dec<<i;
			  for(int j=0;j<(1<<(TOTAL_ADDRS - READ_ADDRS));j++)
				  cout<<setw(3)<<dec<<(tmp_data>>j&0x1);
			  cout<<endl;
		  }


  	 //Add your test code here.
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
   *This function will be invoked once the Select Test Method Dialog is opened
   */
  virtual const string getComment() const
  {
	string comment = " please add your comment for this method.";
	return comment;
  }
};
REGISTER_TESTMETHOD("bm_efuse_tml.Debug.Efuse_AllBits_Read", Efuse_AllBits_Read);
//
class Efuse_Crc_Calc: public testmethod::TestMethod {

  protected:
     /**
  	*Initialize the parameter interface to the testflow.
  	*This method is called just once after a testsuite is created.
  	*/


    virtual void initialize()
    {
  	 //Add your initialization code here
  	 //Note: Test Method API should not be used in this method!

     }

     /**
  	*This test is invoked per site.
  	*/
     virtual void run()
     {
    	 EFUSE	Efuse;

    	 Efuse.ReadCsv();

    	 Efuse.FTEfuseCodeCalc(1);
    	 Efuse.CPEfuseCodeCalc(1);

  	 //Add your test code here.
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
   *This function will be invoked once the Select Test Method Dialog is opened
   */
  virtual const string getComment() const
  {
	string comment = " please add your comment for this method.";
	return comment;
  }
};
REGISTER_TESTMETHOD("bm_efuse_tml.Debug.Efuse_Crc_Calc", Efuse_Crc_Calc);

class Hardbin_judge: public testmethod::TestMethod {
protected:


  vector< std::string >  mFailHardbinList;

  int 		debug_mode;

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
  virtual void initialize()
  {
    //Add your initialization code here

	    addParameter("FailHBList",
	    		"OptionList",
	    		&mFailHardbinList,
	    		testmethod::TM_PARAMETER_INPUT)
	    		.setOptions("2:3:4:5:6:7:8:9:10:11:12:13:14:15:16")
	    		.setComment("the list of fail hard bin, separated by comma");

		 addParameter("debug_mode",
					  "int",
					  &debug_mode)
					  .setDefault("0")
					  .setOptions("0:1")
					  .setComment("debug mode  0:no_need_ouput, 1:output_to_ui");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
	  	STRING softbin,mutisoftbin;
		int hardbin;
		int efuse_flag=1; //1 means do efuse burn in action//0 means not do efuse burn in action

		GET_BIN(softbin,&hardbin);

		if(debug_mode) cout <<"hardbin is "<<hardbin<<endl;

//		if(-1==hardbin) efuse_flag = 0;//-1 means otherwise bin

	    for(size_t i=0;i<mFailHardbinList.size();i++)
	    {
	    	int tmpHardbin=atoi(mFailHardbinList[i].c_str());
	    	if(tmpHardbin==hardbin)
			{
	    		efuse_flag = 0;    //0 means not do efuse burn in action
				if(debug_mode) cout << "The fail hard bin is:" <<mFailHardbinList[i]<<endl;
				break;
			}
	    }
	    if(debug_mode) cout << "The efuse_flag is:" <<efuse_flag<<endl;
		SET_USER_DOUBLE("Hardbin_judge",efuse_flag);

    //Add your test code here.
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
REGISTER_TESTMETHOD("bm_efuse_tml.Others.Hardbin_judge", Hardbin_judge);


class power_on_per_site: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
  virtual void initialize()
  {
    //Add your initialization code here
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
	  PPMU_RELAY    relay_close;

	  DPS_POWER_STATUS().pin("@").on();//all power on for forcus site but not enable site

	  //all digital pin close for forcus site
	  relay_close. pin("@").status("AC_ON");
	  relay_close. wait(1 mS);
	  relay_close.execute();


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
REGISTER_TESTMETHOD("bm_efuse_tml.Others.power_on_per_site", power_on_per_site);

class power_off_all_sites: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
  virtual void initialize()
  {
    //Add your initialization code here
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {

	  ON_FIRST_INVOCATION_BEGIN();

	  	  CONNECT();
	  	  DISCONNECT();

	  ON_FIRST_INVOCATION_END();

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
REGISTER_TESTMETHOD("bm_efuse_tml.Others.power_off_all_sites", power_off_all_sites);

class init_uservar: public testmethod::TestMethod {

protected:
/**
*Initialize the parameter interface to the testflow.
*This method is called just once after a testsuite is created.
*/
   virtual void initialize()
   {
	 //Add your initialization code here
	 //Note: Test Method API should not be used in this method!
   }

/**
*This test is invoked per site.
*/
virtual void run()
{

	  static EFUSE Efuse;

	  ON_FIRST_INVOCATION_BEGIN();

	  	  Efuse.ReadCsv();

		  FOR_EACH_SITE_BEGIN();

		  	  Efuse.Init_userval();

		  FOR_EACH_SITE_END();

	  ON_FIRST_INVOCATION_END();

	 //Add your test code here.
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
	*This function will be invoked once the Select Test Method Dialog is opened
	*/
   virtual const string getComment() const
   {
	 string comment = " please add your comment for this method.";
	 return comment;
   }
 };

REGISTER_TESTMETHOD("bm_efuse_tml.Others.init_uservar", init_uservar);
