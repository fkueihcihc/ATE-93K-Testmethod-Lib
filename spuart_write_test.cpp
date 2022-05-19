#include "testmethod.hpp"
#include "mapi.hpp"
#include "libcicpi.h"

#include "d2sFramework.h"
#include "d2s_SPUART_w.h"
#include "d2s_SPUART_r.h"

#define __D2SREADBLOCKCONFIG__      d2s_SPUART_r &myD2S = d2s_SPUART_r::Instance();

#define __D2SWRITEBLOCKCONFIG__     d2s_SPUART_w &myD2S = d2s_SPUART_w::Instance();

 using namespace std;
 using namespace V93kLimits;
 /**
  * Test method class.
  *
  * For each testsuite using this test method, one object of this
  * class is created.
  */

class spuart_write_test: public testmethod::TestMethod {

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

		  fr.d2s_LABEL_BEGIN("spuart_write_try",d2s_mode);
			  	  myD2S.write(0x0,0xa5);//
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
REGISTER_TESTMETHOD("bm_ac_tml.spuart_write_test", spuart_write_test);

class spuart_read_test: public testmethod::TestMethod {

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

	   d2s_SPUART_r &myD2S = d2s_SPUART_r::Instance();

			  d2sFramework fr;
			  fr.registerTransactionPort(myD2S);
			  d2sFrameWorkModeType::Enum d2s_mode;

			  switch ( D2SMode )
			  {
					case 0:
					case 1: d2s_mode = d2sFrameWorkModeType::LearningMode;break;
					case 2: d2s_mode = d2sFrameWorkModeType::ProductionMode;break;
			  }

	  ON_FIRST_INVOCATION_BEGIN();
	  fr.d2s_LABEL_BEGIN("efuse_ft_post_check",d2s_mode);

	  		 myD2S.read(0x1f,"0x1f_read");//


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
REGISTER_TESTMETHOD("bm_ac_tml.spuart_read_test", spuart_read_test);
