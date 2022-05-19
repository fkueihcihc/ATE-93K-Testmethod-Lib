#include "testmethod.hpp"
#include "TMUtils.h"
#include "libcicpi.h"
//for test method API interfaces
#include "mapi.hpp"
using namespace std;

/**
 * Test method class.
 *
 * Set all USERFLAG and USERDOUBLE Variables as 0
 *
 */
class Initial_AllUserVar: public testmethod::TestMethod {

protected:
/**
*Initialize the parameter interface to the testflow.
*This method is called just once after a testsuite is created.
*/
   int debug_mode;

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

   }

/**
*This test is invoked per site.
*/
virtual void run()
{
		// initial USERFLAG to 0
	   CI_CPI_USER_FLAG_LIST user_flag_list;
	   CI_CPI_USER_FLAG *pFlag;
	   if (GetAllTestflowUserFlags(&user_flag_list)==0 )
	   {
		   for (int i=0; i< user_flag_list.number_of_user_flag; i++)
		   {
			   pFlag = & user_flag_list.user_flags[i];
			   SET_USER_FLAG(pFlag->name,0);//set 0
			   if(debug_mode){
				   INT tmp_UFvar;
				   GET_USER_FLAG(pFlag->name,&tmp_UFvar);
				   cout<<"after initial:"<<pFlag->name<<"="<<tmp_UFvar<<endl;
			   }

		   }
	   }

		// initial USERDOUBLE to 0
	   CI_CPI_DOUBLE_TFL_VAR_LIST double_tfl_var_list;
	   CI_CPI_DOUBLE_TFL_VAR *pTflVar;
	   if (GetAllDoubleTestflowVariables(&double_tfl_var_list)==0 )
	   {
		   for (int i=0; i< double_tfl_var_list.number_of_double_tfl_vars; i++)
		   {
			   pTflVar = & double_tfl_var_list.double_tfl_vars[i];
			   SET_USER_DOUBLE(pTflVar->name,0.0); //set 0
			   if(debug_mode){
				   DOUBLE tmp_UDvar;
				   GET_USER_DOUBLE(pTflVar->name,&tmp_UDvar);
				   cout<<"after initial:"<<pTflVar->name<<"="<<tmp_UDvar<<endl;
			   }

		   }
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

REGISTER_TESTMETHOD("bm_miscellaneous_tml.Initial_AllUserVar", Initial_AllUserVar);
