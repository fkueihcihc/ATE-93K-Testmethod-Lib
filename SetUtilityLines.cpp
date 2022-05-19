// SetUtilityLines.cpp


//for testmethod framework interfaces
#include "testmethod.hpp"
//for testmethod API interfaces
#include "mapi.hpp"
#include "tml_def.h"


#if __GNUC__ >= 3
using namespace std;
#endif

static void PerformSingleUtilityBlock(const int UtilityBlockNumber, const string UT);

/**
 *Testmethod class.
 *
 *For each testsuite using this testmethod, one object of this
 *class is created.
 */
class SetUtilityLines: public testmethod::TestMethod
{
protected:
  string  UT1;
  string  UT2;
  string  UT3;
  string  UT4;
  string  UT5;
  string  UT6;
  string  UT7;
  string  UT8;

protected:
/**
 *Initialize the parameter interface to the testflow.
 *This method is called just once after a testsuite is created.
 *
 *Note: TestMethod API should not be used in this method.
 */
virtual void initialize()
{
  addParameter("UT1",
               "string",
               &UT1)
    .setDefault("0000 0000 0000 0000")
    .setComment("a string of 0 or 1, 16 bits in total. Leave it in blank if no action need to perform.");
  addParameter("UT2",
               "string",
               &UT2)
    .setDefault("0000 0000 0000 0000")
    .setComment("a string of 0 or 1, 16 bits in total. Leave it in blank if no action need to perform.");
  addParameter("UT3",
               "string",
               &UT3)
    .setDefault("0000 0000 0000 0000")
    .setComment("a string of 0 or 1, 16 bits in total. Leave it in blank if no action need to perform.");
  addParameter("UT4",
               "string",
               &UT4)
    .setDefault("0000 0000 0000 0000")
    .setComment("a string of 0 or 1, 16 bits in total. Leave it in blank if no action need to perform.");
  addParameter("UT5",
               "string",
               &UT5)
    .setDefault("0000 0000 0000 0000")
    .setComment("a string of 0 or 1, 16 bits in total. Leave it in blank if no action need to perform.");
  addParameter("UT6",
               "string",
               &UT6)
    .setDefault("0000 0000 0000 0000")
    .setComment("a string of 0 or 1, 16 bits in total. Leave it in blank if no action need to perform.");
  addParameter("UT7",
               "string",
               &UT7)
    .setDefault("0000 0000 0000 0000")
    .setComment("a string of 0 or 1, 16 bits in total. Leave it in blank if no action need to perform.");
  addParameter("UT8",
               "string",
               &UT8)
    .setDefault("0000 0000 0000 0000")
    .setComment("a string of 0 or 1, 16 bits in total. Leave it in blank if no action need to perform.");
}

/**
 *This test is invoked per site.
 */
virtual void run()
{
    ON_FIRST_INVOCATION_BEGIN();

      PerformSingleUtilityBlock(1, UT1);
      PerformSingleUtilityBlock(2, UT2);
      PerformSingleUtilityBlock(3, UT3);
      PerformSingleUtilityBlock(4, UT4);
      PerformSingleUtilityBlock(5, UT5);
      PerformSingleUtilityBlock(6, UT6);
      PerformSingleUtilityBlock(7, UT7);
      PerformSingleUtilityBlock(8, UT8);

    ON_FIRST_INVOCATION_END();

    return;
}

/**
 *This function will be invoked once the specified parameter's value is changed.
 *@param parameterIdentifier
 *
 *Note: TestMethod API should not be used in this method.
 */
static void PerformSingleUtilityBlock(const int UtilityBlockNumber, const string UT) {

    int debug;
    GET_TESTFLOW_FLAG ("debug_analog",&debug);


    int UTbits = 0;

    if(UT.size() == 19 /*16+3 spaces*/) {
        for(int i = 0; i < UT.size(); i++) {
            if(UT[i] == ' ') {
                //do nothing
            } else {
               UTbits = UTbits << 1;
               //cout << "UT[" << i << "] = " << UT[i] << endl;
               if(UT[i] == '1') {
                    UTbits = UTbits | 1;
               }
            }
            //cout << hex << "UTbits" " = " << UTbits << endl;
        }
        stringstream command;
        command << "UTOT " << UtilityBlockNumber << ", 65535, \"" << hex << UTbits << "\";";
        if(debug) cout << command.str() << endl;

        string answer;
        FW_TASK(command.str(), answer);
    } else {
        if(debug) cerr << "Control bit length is not 16 bit! Ignore Utility Block " << UtilityBlockNumber <<"!" << endl;

    }

}
virtual void postParameterChange(const string& parameterIdentifier)
{
  //add your code
}

};
REGISTER_TESTMETHOD("bm_miscellaneous_tml.SetUtilityLines",SetUtilityLines);




