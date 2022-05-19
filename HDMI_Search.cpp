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
class HDMI_Search: public testmethod::TestMethod {

protected:
  /**
   *Initialize the parameter interface to the testflow.
   *This method is called just once after a testsuite is created.
   *
   *Note: Test Method API should not be used in this method.
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

		 		static int HDMI_TX0P_FLAG[4] = {0,0,0,0};
		 		static int HDMI_TX0N_FLAG[4] = {0,0,0,0};
		 		static int HDMI_TX1P_FLAG[4] = {0,0,0,0};
		 		static int HDMI_TX1N_FLAG[4] = {0,0,0,0};
		 		static int HDMI_TX2P_FLAG[4] = {0,0,0,0};
		 		static int HDMI_TX2N_FLAG[4] = {0,0,0,0};
		 		static int HDMI_TXCP_FLAG[4] = {0,0,0,0};
		 		static int HDMI_TXCN_FLAG[4] = {0,0,0,0};

		 		int HDMI_FLAG = 0;


	            SPECIFICATION  Tspec=Primary.getSpecification();
	            double of=-0.5;
	            double Cur_T_value=Tspec.getSpecValue("offset@pHDMI_DataOut_PS1600");


		 	    ON_FIRST_INVOCATION_BEGIN();

		 			CONNECT();

	                for(int i=0;i<10;i++)
	                {
	                	cout<<i<<"Spec"<<endl;

	                	Tspec.change("offset@pHDMI_DataOut_PS1600",of+i*0.067);

	                	FLUSH(TM::APRM);

//	                	for(int l=0;l<10;l++)
//	                	{

	                		FUNCTIONAL_TEST();

							WAIT_TIME(5 ms);

		    	 			FOR_EACH_SITE_BEGIN();

		    	 			    cout<<"site"<<CURRENT_SITE_NUMBER()<<endl;

								HDMI_TX0N_FLAG[CURRENT_SITE_NUMBER()-1] |= GET_FUNCTIONAL_RESULT("HDMI_TX0N");
								HDMI_TX0P_FLAG[CURRENT_SITE_NUMBER()-1] |= GET_FUNCTIONAL_RESULT("HDMI_TX0P");
								HDMI_TX1N_FLAG[CURRENT_SITE_NUMBER()-1] |= GET_FUNCTIONAL_RESULT("HDMI_TX1N");
								HDMI_TX1P_FLAG[CURRENT_SITE_NUMBER()-1] |= GET_FUNCTIONAL_RESULT("HDMI_TX1P");
								HDMI_TX2N_FLAG[CURRENT_SITE_NUMBER()-1] |= GET_FUNCTIONAL_RESULT("HDMI_TX2N");
								HDMI_TX2P_FLAG[CURRENT_SITE_NUMBER()-1] |= GET_FUNCTIONAL_RESULT("HDMI_TX2P");
								HDMI_TXCN_FLAG[CURRENT_SITE_NUMBER()-1] |= GET_FUNCTIONAL_RESULT("HDMI_TXCN");
								HDMI_TXCP_FLAG[CURRENT_SITE_NUMBER()-1] |= GET_FUNCTIONAL_RESULT("HDMI_TXCP");

								cout<<HDMI_TX0N_FLAG[CURRENT_SITE_NUMBER()-1]<<endl;
								cout<<HDMI_TX0P_FLAG[CURRENT_SITE_NUMBER()-1]<<endl;
								cout<<HDMI_TX1N_FLAG[CURRENT_SITE_NUMBER()-1]<<endl;
								cout<<HDMI_TX1P_FLAG[CURRENT_SITE_NUMBER()-1]<<endl;
								cout<<HDMI_TX2N_FLAG[CURRENT_SITE_NUMBER()-1]<<endl;
								cout<<HDMI_TX2P_FLAG[CURRENT_SITE_NUMBER()-1]<<endl;
								cout<<HDMI_TXCN_FLAG[CURRENT_SITE_NUMBER()-1]<<endl;
								cout<<HDMI_TXCP_FLAG[CURRENT_SITE_NUMBER()-1]<<endl;

								cout<<endl<<endl;

		    	 			FOR_EACH_SITE_END();

//	                	}



	                }


		 		    DISCONNECT();

				    Tspec.restore();
				    FLUSH(TM::APRM);

		 	  ON_FIRST_INVOCATION_END();
	//

//              int z =0;
//              z&=1;
//		 	  cout<<z<<endl;

		 	 FOR_EACH_SITE_BEGIN();

		 	 if((HDMI_TX0N_FLAG[CURRENT_SITE_NUMBER()-1]&
				HDMI_TX0P_FLAG[CURRENT_SITE_NUMBER()-1]&
				HDMI_TX1N_FLAG[CURRENT_SITE_NUMBER()-1]&
				HDMI_TX1P_FLAG[CURRENT_SITE_NUMBER()-1]&
				HDMI_TX2N_FLAG[CURRENT_SITE_NUMBER()-1]&
				HDMI_TX2P_FLAG[CURRENT_SITE_NUMBER()-1]&
				HDMI_TXCN_FLAG[CURRENT_SITE_NUMBER()-1]&
				HDMI_TXCP_FLAG[CURRENT_SITE_NUMBER()-1]) == 1)
		 	 {
		 		 HDMI_FLAG =1;
		 		 cout<<"pass"<<endl;
		 		 cout<<(HDMI_TX0N_FLAG[CURRENT_SITE_NUMBER()-1]&
		 				HDMI_TX0P_FLAG[CURRENT_SITE_NUMBER()-1]&
		 				HDMI_TX1N_FLAG[CURRENT_SITE_NUMBER()-1]&
		 				HDMI_TX1P_FLAG[CURRENT_SITE_NUMBER()-1]&
		 				HDMI_TX2N_FLAG[CURRENT_SITE_NUMBER()-1]&
		 				HDMI_TX2P_FLAG[CURRENT_SITE_NUMBER()-1]&
		 				HDMI_TXCN_FLAG[CURRENT_SITE_NUMBER()-1]&
		 				HDMI_TXCP_FLAG[CURRENT_SITE_NUMBER()-1])<<endl;
		 	 }


	 		 //TestSet.cont(true).TEST("","flag",tmLimits,HDMI_FLAG);


			 FOR_EACH_SITE_END();





	    //Add your test code here.
    //Add your test code here.
    return;
  }
  /**
   *This function will be invoked once the specified parameter's value is changed.
   *@param parameterIdentifier
   *
   *Note: Test Method API should not be used in this method.
   */
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
REGISTER_TESTMETHOD("bm_ac_tml.HDMI_Search", HDMI_Search);
