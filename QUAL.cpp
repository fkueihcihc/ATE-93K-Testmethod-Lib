#include "testmethod.hpp"

//for test method API interfaces
#include "mapi.hpp"
using namespace std;
using namespace V93kLimits;


/**
 * Test method class.
 *
 * For each testsuite using this test method, one object of this
 * class is created.
 */
class VDDmin_Search: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
	string V_SPEC;
	double VScale_UL;
	double VScale_LL;
	int V_points;
	string Gen_testTabel;

  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
	  addParameter("V_SPEC", "string", &V_SPEC).setDefault("v_DVDDCORE_0P9").setComment(
				"Core Voltage Spec");
	  addParameter("VScale_UL", "double", &VScale_UL).setDefault("1.2").setComment(
				"Up Limit of Voltage Scale");
	  addParameter("VScale_LL", "double", &VScale_LL).setDefault("0.6").setComment(
				"Low Limit of Voltage Scale");
	  addParameter("V_points", "int", &V_points).setDefault("100").setComment(
				"Voltage Spec search points");
	  addParameter("Gen_testTabel","string",&Gen_testTabel,testmethod::TM_PARAMETER_INPUT)
						   .setOptions("Yes:No")
		  	  	   	   	   .setDefault("No");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
    //Add your test code here.
	LEVEL_SPEC   Vspec=Primary.getLevelSpec();
	double Cur_V_value=Vspec.getSpecValue(V_SPEC);
	//cout << Cur_V_value <<endl;

	int result = 0;
	double mVDDmin=-1;
	INT mGenTestTable;
	string test_name;
	GET_TESTSUITE_NAME(test_name);
	GET_USER_FLAG("GenTestTable",&mGenTestTable);
	if("Yes"==Gen_testTabel||1==mGenTestTable){
		Generate_testTable();
//		SET_USER_FLAG("GenTestTable",0);
		tmLimits.load();
	}
	else{
		for(double i=Cur_V_value*VScale_LL;i<=Cur_V_value*VScale_UL;i=i+Cur_V_value*(VScale_UL-VScale_LL)/V_points)//0.5% step
			{
				 Vspec.change(V_SPEC,i);
				 FLUSH(TM::APRM);
				 CONNECT();
				 FUNCTIONAL_TEST();
				 result = GET_FUNCTIONAL_RESULT();

				 if(result == 0) cout << test_name <<" Fail: ";
				 if(result == 1) cout << test_name <<" Pass: ";

				 cout << "VDD_MIN = " << i << "V"<<endl;

				 if(result == 1)
				 {
					 mVDDmin=i;
					 break;
				 }
			}
			 Vspec.change(V_SPEC,Cur_V_value);
			 FLUSH(TM::APRM);

			 TestSet.cont(true).TEST("","VDDmin",tmLimits, mVDDmin);//log pass
	}



    return;
  }
  /*****************************sub function*************************
   *
   * */
  	  void Generate_testTable()
	  {
  		char temp_test[50];
		string suite;
		GET_TESTSUITE_NAME(suite);
		tmLimits.load();
//
		TestTable* testTable = TestTable::getDefaultInstance();
		testTable->isCsvLoadNeeded();
		testTable->readCsvFile();
//
		int KeyValue[]={1};
		testTable->setLookupKeys(1,KeyValue);

		sprintf(temp_test,"%s",suite.c_str());

		if(testTable->getTestPos(temp_test)<0)
		{
			testTable->addTest(temp_test);

			testTable->setValue(temp_test,"Lsl_typ","NA");
			testTable->setValue(temp_test,"Usl_typ","NA");
			testTable->setValue(temp_test,"Units","V");
			testTable->setValue(temp_test,"Suite name",suite.c_str());
			testTable->setValue(temp_test,"Test name","VDDmin");

			testTable->writeCsvFile();
		}


  		TestTable::clearDefaultInstance();

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
REGISTER_TESTMETHOD("bm_ac_tml.QUAL_TOOL.VDDmin_Search", VDDmin_Search);


class VDDmin_Search_Binary: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
	string V_SPEC;
	double VScale_UL;
	double VScale_LL;
	int V_points;
	string  method;
	double  resolution_V;
	string  Gen_testTabel;

  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
	  addParameter("V_SPEC", "string", &V_SPEC).setDefault("v_DVDDCORE_0P9").setComment(
				"Core Voltage Spec");
	  addParameter("VScale_UL", "double", &VScale_UL).setDefault("1.2").setComment(
				"Up Limit of Voltage Scale");
	  addParameter("VScale_LL", "double", &VScale_LL).setDefault("0.6").setComment(
				"Low Limit of Voltage Scale");
	  addParameter("V_points", "int", &V_points).setDefault("100").setComment(
				"Voltage Spec search points");
	  addParameter("method",
				 "string",
				 &method)
	  .setDefault("binary")
	  .setOptions("linear:binary");
	  addParameter("resolution_V",
	                   "double",
	                   &resolution_V)
	  .setDefault("0.002");
	  addParameter("Gen_testTabel","string",&Gen_testTabel,testmethod::TM_PARAMETER_INPUT)
						   .setOptions("Yes:No")
		  	  	   	   	   .setDefault("No");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
    //Add your test code here.
	LEVEL_SPEC   Vspec=Primary.getLevelSpec();
	double Cur_V_value=Vspec.getSpecValue(V_SPEC);
	SPEC_SEARCH search_lvl(V_SPEC,TM::LEV);

	double mVDDmin=-1;
	INT mGenTestTable;
	string test_name;
	GET_TESTSUITE_NAME(test_name);
	GET_USER_FLAG("GenTestTable",&mGenTestTable);
	if("Yes"==Gen_testTabel||1==mGenTestTable){
		Generate_testTable();
//		SET_USER_FLAG("GenTestTable",0);
		tmLimits.load();
	}
	else{



			ON_FIRST_INVOCATION_BEGIN();

			CONNECT();


			if("linear"==method){
				search_lvl.method(TM::Linear).

				unit("V").start(Cur_V_value*VScale_LL).stop(Cur_V_value*VScale_UL).numberOfSteps(V_points).resolution(resolution_V);
			}
			else if("binary"==method){
				search_lvl.method(TM::Binary).

				unit("V").start(Cur_V_value*VScale_LL).stop(Cur_V_value*VScale_UL).resolution(resolution_V);
			}

			search_lvl.execute();

			ON_FIRST_INVOCATION_END();

			DOUBLE Pval;

			switch(search_lvl.getResultSpec()){
				case TM::TransitionPassFail :
				case TM::TransitionFailPass :
				Pval = search_lvl.getPassVal();
				mVDDmin = Pval;//V
//				cout << "Pval = " << Pval <<endl;
				break;


				case TM::AllPass :
				mVDDmin =-2;
				cout << "No transition found within the search interval" << endl;
				break;
				case TM::AllFail :
				mVDDmin =-1;
				cout << "No transition found within the search interval" << endl;
				break;
			}

			Vspec.change(V_SPEC,Cur_V_value);
			FLUSH(TM::APRM);
			cout << "VDDmin = " << mVDDmin <<endl;
			TestSet.cont(true).TEST("","VDDmin",tmLimits, mVDDmin);//log pass

	}


    return;
  }
  /*****************************sub function*************************
   *
   * */
  	  void Generate_testTable()
	  {
  		char temp_test[50];
		string suite;
		GET_TESTSUITE_NAME(suite);
		tmLimits.load();
//
		TestTable* testTable = TestTable::getDefaultInstance();
		testTable->isCsvLoadNeeded();
		testTable->readCsvFile();
//
		int KeyValue[]={1};
		testTable->setLookupKeys(1,KeyValue);

		sprintf(temp_test,"%s",suite.c_str());

		if(testTable->getTestPos(temp_test)<0)
		{
			testTable->addTest(temp_test);

			testTable->setValue(temp_test,"Lsl_typ","NA");
			testTable->setValue(temp_test,"Usl_typ","NA");
			testTable->setValue(temp_test,"Units","V");
			testTable->setValue(temp_test,"Suite name",suite.c_str());
			testTable->setValue(temp_test,"Test name","VDDmin");

			testTable->writeCsvFile();
		}


  		TestTable::clearDefaultInstance();

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
REGISTER_TESTMETHOD("bm_ac_tml.QUAL_TOOL.VDDmin_Search_Binary", VDDmin_Search_Binary);

class Fmax_Search: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
	string T_SPEC;
	double TScale_UL;
	double TScale_LL;
	int T_points;
	string  method;
	double  resolution_ns;
	string  Gen_testTabel;

  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
	  addParameter("T_SPEC", "string", &T_SPEC).setDefault("per_xxx").setComment(
				" Timing Spec");
	  addParameter("TScale_UL", "double", &TScale_UL).setDefault("1.2").setComment(
				"Up Limit of Timing Scale");
	  addParameter("TScale_LL", "double", &TScale_LL).setDefault("0.6").setComment(
				"Low Limit of Timing Scale");
	  addParameter("T_points", "int", &T_points).setDefault("100").setComment(
				"Timing Spec search points");
	  addParameter("method",
				 "string",
				 &method)
	  .setDefault("linear")
	  .setOptions("linear:binary");
	  addParameter("resolution_ns",
	                   "double",
	                   &resolution_ns)
	  .setDefault("1");
	  addParameter("Gen_testTabel","string",&Gen_testTabel,testmethod::TM_PARAMETER_INPUT)
						   .setOptions("Yes:No")
		  	  	   	   	   .setDefault("No");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
    //Add your test code here.
	TIMING_SPEC  Tspec=Primary.getTimingSpec();
	double Cur_T_value=Tspec.getSpecValue(T_SPEC);
	SPEC_SEARCH search_tim(T_SPEC,TM::TIM);

	double mFmax=-1;
	INT mGenTestTable;
	string test_name;
	GET_TESTSUITE_NAME(test_name);
	GET_USER_FLAG("GenTestTable",&mGenTestTable);
	if("Yes"==Gen_testTabel||1==mGenTestTable){
		Generate_testTable();
//		SET_USER_FLAG("GenTestTable",0);
		tmLimits.load();
	}
	else{

			ON_FIRST_INVOCATION_BEGIN();

			CONNECT();


			if("linear"==method){
				search_tim.method(TM::Linear).

				unit("ns").start(Cur_T_value*TScale_UL).stop(Cur_T_value*TScale_LL).numberOfSteps(T_points).resolution(resolution_ns);
			}
			else if("binary"==method){
				search_tim.method(TM::Binary).

				unit("ns").start(Cur_T_value*TScale_UL).stop(Cur_T_value*TScale_LL).resolution(resolution_ns);
			}

			search_tim.execute();

			ON_FIRST_INVOCATION_END();

			DOUBLE Pval;

			switch(search_tim.getResultSpec()){
				case TM::TransitionPassFail :
				Pval = search_tim.getPassVal();
				mFmax = 1e3/Pval;//MHz
				break;

				case TM::TransitionFailPass :
				case TM::AllPass :
				case TM::AllFail :
				mFmax =-1;
				cout << "No transition found within the search interval" << endl;
				break;
			}

			Tspec.change(T_SPEC,Cur_T_value);
			FLUSH(TM::APRM);
			cout << "Pval = " << Pval << "ns"<<endl;
			cout << "Fmax = " << mFmax << "MHz"<<endl;
			TestSet.cont(true).TEST("","Fmax",tmLimits, mFmax);//log pass

	}


    return;
  }
  /*****************************sub function*************************
   *
   * */
  	  void Generate_testTable()
	  {
  		char temp_test[50];
		string suite;
		GET_TESTSUITE_NAME(suite);
		tmLimits.load();
//
		TestTable* testTable = TestTable::getDefaultInstance();
		testTable->isCsvLoadNeeded();
		testTable->readCsvFile();
//
		int KeyValue[]={1};
		testTable->setLookupKeys(1,KeyValue);

		sprintf(temp_test,"%s",suite.c_str());

		if(testTable->getTestPos(temp_test)<0)
		{
			testTable->addTest(temp_test);

			testTable->setValue(temp_test,"Lsl_typ","NA");
			testTable->setValue(temp_test,"Usl_typ","NA");
			testTable->setValue(temp_test,"Units","MHz");
			testTable->setValue(temp_test,"Suite name",suite.c_str());
			testTable->setValue(temp_test,"Test name","Fmax");

			testTable->writeCsvFile();
		}


  		TestTable::clearDefaultInstance();

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
REGISTER_TESTMETHOD("bm_ac_tml.QUAL_TOOL.Fmax_Search", Fmax_Search);

class Fmax_Search_MP: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   */
	string T_SPEC;
	double TScale_UL;
	double TScale_LL;
	int T_points;
	string  method;
	double  resolution_ns;
	string  Gen_testTabel;

  virtual void initialize()
  {
    //Add your initialization code here
    //Note: Test Method API should not be used in this method!
	  addParameter("T_SPEC", "string", &T_SPEC).setDefault("per_xxx@portName").setComment(
				" Timing Spec");
	  addParameter("TScale_UL", "double", &TScale_UL).setDefault("1.2").setComment(
				"Up Limit of Timing Scale");
	  addParameter("TScale_LL", "double", &TScale_LL).setDefault("0.6").setComment(
				"Low Limit of Timing Scale");
	  addParameter("T_points", "int", &T_points).setDefault("100").setComment(
				"Timing Spec search points");
	  addParameter("method",
				 "string",
				 &method)
	  .setDefault("linear")
	  .setOptions("linear:binary");
	  addParameter("resolution_ns",
	                   "double",
	                   &resolution_ns)
	  .setDefault("1");
	  addParameter("Gen_testTabel","string",&Gen_testTabel,testmethod::TM_PARAMETER_INPUT)
						   .setOptions("Yes:No")
		  	  	   	   	   .setDefault("No");
  }

  /**
   *This test is invoked per site.
   */
  virtual void run()
  {
    //Add your test code here.
	SPECIFICATION&  Tspec=Primary.getSpecification();
	double Cur_T_value=Tspec.getSpecValue(T_SPEC);

	double mFmax=-1;
	INT mGenTestTable;
	string test_name;
	GET_TESTSUITE_NAME(test_name);
	GET_USER_FLAG("GenTestTable",&mGenTestTable);
	if("Yes"==Gen_testTabel||1==mGenTestTable){
		Generate_testTable();
//		SET_USER_FLAG("GenTestTable",0);
		tmLimits.load();
	}
	else{

			CONNECT();

			SPEC_SEARCH search_tim(T_SPEC,TM::TIM);
			if("linear"==method){
				search_tim.method(TM::Linear).

				unit("ns").start(Cur_T_value*TScale_UL).stop(Cur_T_value*TScale_LL).numberOfSteps(T_points).resolution(resolution_ns);
			}
			else if("binary"==method){
				search_tim.method(TM::Binary).

				unit("ns").start(Cur_T_value*TScale_UL).stop(Cur_T_value*TScale_LL).resolution(resolution_ns);
			}

			search_tim.execute();

			DOUBLE Pval;

			switch(search_tim.getResultSpec()){
				case TM::TransitionPassFail :
				Pval = search_tim.getPassVal();
				cout<<"Pval="<<Pval<<"ns"<<endl;
				mFmax = 1e3/Pval;//MHz
				break;

				case TM::TransitionFailPass :
					// add code here
				case TM::AllPass :
				mFmax =-2;
				cout << "No transition found within the search interval" << endl;
				break;
				case TM::AllFail :
				mFmax =-1;
				cout << "No transition found within the search interval" << endl;
				break;
			}

		    Tspec.restore();
		    FLUSH(TM::APRM);
		    cout<<"Fmax="<<mFmax<<"MHz"<<endl;
			TestSet.cont(true).TEST("","Fmax",tmLimits, mFmax);//log pass

	}


    return;
  }
  /*****************************sub function*************************
   *
   * */
  	  void Generate_testTable()
	  {
  		char temp_test[50];
		string suite;
		GET_TESTSUITE_NAME(suite);
		tmLimits.load();
//
		TestTable* testTable = TestTable::getDefaultInstance();
		testTable->isCsvLoadNeeded();
		testTable->readCsvFile();
//
		int KeyValue[]={1};
		testTable->setLookupKeys(1,KeyValue);

		sprintf(temp_test,"%s",suite.c_str());

		if(testTable->getTestPos(temp_test)<0)
		{
			testTable->addTest(temp_test);

			testTable->setValue(temp_test,"Lsl_typ","NA");
			testTable->setValue(temp_test,"Usl_typ","NA");
			testTable->setValue(temp_test,"Units","MHz");
			testTable->setValue(temp_test,"Suite name",suite.c_str());
			testTable->setValue(temp_test,"Test name","Fmax");

			testTable->writeCsvFile();
		}


  		TestTable::clearDefaultInstance();

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
REGISTER_TESTMETHOD("bm_ac_tml.QUAL_TOOL.Fmax_Search_MP", Fmax_Search_MP);
