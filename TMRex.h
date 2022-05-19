/********************************************************************************
*                                                                               *
*                 R e g u l a r   E x p r e s s i o n   C l a s s               *
*                                                                               *
*********************************************************************************
* Copyright (C) 1999,2005 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*********************************************************************************/
///@cond
#ifndef FXREX_H

#define FXREX_H

#include <string>
#include <vector>
#include <iostream>
using std::string;
using std::vector;
using std::ostream;
using std::istream;

/// Regular expression error codes
enum FXRexError {
  REGERR_OK,
  REGERR_EMPTY,             /// Empty pattern
  REGERR_PAREN,             /// Unmatched parenthesis
  REGERR_BRACK,             /// Unmatched bracket
  REGERR_BRACE,             /// Unmatched brace
  REGERR_RANGE,             /// Bad character range
  REGERR_ESC,               /// Bad escape sequence
  REGERR_COUNT,             /// Bad counted repeat
  REGERR_NOATOM,            /// No atom preceding repetition
  REGERR_REPEAT,            /// Repeat following repeat
  REGERR_BACKREF,           /// Bad backward reference
  REGERR_CLASS,             /// Bad character class
  REGERR_COMPLEX,           /// Expression too complex
  REGERR_MEMORY,            /// Out of memory
  REGERR_TOKEN              /// Illegal token
  };


/// Regular expression parse flags
enum {
  REX_NORMAL    = 0,        /// Normal mode
  REX_CAPTURE   = 1,        /// Perform capturing parentheses
  REX_ICASE     = 2,        /// Case independent matching
  REX_NEWLINE   = 4,        /// Match-any operators match newline too
  REX_VERBATIM  = 8,        /// Disable interpretation of magic characters
  REX_SYNTAX    = 16        /// Perform syntax check only
  };


/// Regular expression match flags
enum {
  REX_FORWARD   = 0,        /// Match scanning forward from offset
  REX_BACKWARD  = 32,       /// Match scanning backward from offset
  REX_NOT_BOL   = 64,       /// Start of string is NOT begin of line
  REX_NOT_EOL   = 128,      /// End of string is NOT end of line
  REX_NOT_EMPTY = 256       /// Do not match empty
  };


/**
* FXRex is a regular expression class implementing a NFA matcher.
* It supports capturing parentheses, non-capturing parentheses,
* positive or negative lookahead, backreferences, case-insensitive
* matching, counted repetitions, lazy or greedy matches, and
* PERL-like matching operators.
* The subject string may be scanned forwards or backwards, and may
* contain any of 256 possible character values.
*
* When parsing a regular expression pattern, the mode parameter is
* the bitwise OR of a set of flags and affects the match algorithm.
* Passing the flag REX_CAPTURE enables capturing parentheses
* and back references. The flag REX_ICASE enables case-insensitive
* matching. When the flag REX_NEWLINE is passed, newlines are treated
* like normal characters; otherwise, newline is NOT matched
* except when explicitly part of a character class. The flag
* REX_VERBATIM disables all special character interpretation.
*
* When matching a compiled pattern, the mode parameter is the
* bitwise OR of a set of flags that affects how the match is
* performed.  Passing the flag REX_BACKWARD causes the match
* to proceed backwards through the subject string.  Passing the
* flags REX_NOT_BOL and/or REX_NOT_EOL causes the begin and
* end of the subject string NOT to be considered a line start
* or line end. The flag REX_NOT_EMPTY causes a match to fail if
* the empty string was matched.
*/
class FXRex {
private:
  int *code;
private:
  static const char *const errors[];
  static const int fallback[];
public:

  /// Construct empty regular expression object
  FXRex():code((int*)fallback){}

  /// Copy regular expression object
  FXRex(const FXRex& orig);

  /// Compile expression from pattern; if error is not NULL, error code is returned
  FXRex(const char* pattern,int mode=REX_NORMAL,FXRexError* error=NULL);

  /// Compile expression from pattern; if error is not NULL, error code is returned
  FXRex(const string& pattern,int mode=REX_NORMAL,FXRexError* error=NULL);

  /// Assign another regular expression to this one
  FXRex& operator=(const FXRex& orig);

  /**
  * See if regular expression is empty; the regular expression
  * will be empty when it is unable to parse a pattern due to
  * a syntax error.
  */
  bool empty() const { return (code==fallback); }

  /// Parse pattern, return error code if syntax error is found
  FXRexError parse(const char* pattern,int mode=REX_NORMAL);

  /// Parse pattern, return error code if syntax error is found
  FXRexError parse(const string& pattern,int mode=REX_NORMAL);

  /**
  * Match a subject string of length len, returning TRUE if a match is found
  * and FALSE otherwise.  The entire pattern is captured in beg[0] and end[0],
  * where beg[0] refers to the position of the first matched character and end[0]
  * refers to the position after the last matched character.
  * Sub expressions from capturing parenthesis i are returned in beg[i] and end[i].
  */
  bool match(const char* estring,int len,int* beg=NULL,int* end=NULL,int mode=REX_FORWARD,int npar=1,int fm=0,int to=2147483647) const;

  /// Search for match in a string
  bool match(const string& estring,int* beg=NULL,int* end=NULL,int mode=REX_FORWARD,int npar=1,int fm=0,int to=2147483647) const;

  /**
  * After performing a regular expression match with capturing parentheses,
  * a substitution string is build from the replace string, where where "&"
  * is replaced by the entire matched pattern, and "\1" through "\9" are
  * replaced by captured expressions.  The original source string and its
  * length, and the match arrays beg and end must be passed.
  */
  static string substitute(const char* estring,int len,int* beg,int* end,const string& replace,int npar=1);

  /// Return substitution string
  static string substitute(const string& estring,int* beg,int* end,const string& replace,int npar=1);

  /// Returns error code for given error
  static const char* getError(FXRexError err){ return errors[err]; }

  /// Comparison operators
  friend bool operator==(const FXRex &r1,const FXRex &r2);
  friend bool operator!=(const FXRex &r1,const FXRex &r2);

  /// Saving and loading
  //  friend ostream& operator<<(ostream& store,const FXRex& s);
  //friend istream& operator>>(istream& store,FXRex& s);

  /// Delete
 ~FXRex();
};
///@endcond

/************** convenience wrappers ********************/

/**
  \brief  A regular expression parser class based on the well known FXRex class
	  used widely in the Open Source development world. Also, if you're a fan of PERL then
	  you'll feel right at home with this class.

  \remarks <b>Getting Started:</b>

Make sure your Test Method project is properly configured:
- \subpage integration "Integration into an application"
- \subpage makefile "Makefile specific details"

You need to insert the following lines into your source code:
\code
#include "TMRex.h"
\endcode

<b>Match Example</b>

\code
// Parse firmware id string (fetched using fw command "*IDN?"
string match_string = "*IDN Firmware   s/w rev. 6.2.0_65x1, 14-Aug-07"
		
// create container to hold our matches
vector<string> match_results;
	
// define a regular expression object - parameter is a standard regular expression
TMRex verstring("Firmware\\s+s/w\\s+rev\\. (\\d+)\\.(\\d+)\\.(\\d+)_(\\w+)");
	
// match and place results in our vector container
verstring.match(match_string,match_results);

// print results
cerr << "major rev # : " << match_results[0] << endl;
cerr << "minor rev # : " << match_results[1] << endl;
cerr << "release #   : " << match_results[2] << endl;
cerr << "patch       : " << match_results[3] << endl;
\endcode

<b>Substitution Example</b>

\code
// Parse firmware id string (fetched using fw command "*IDN?"
string match_string = "*IDN Firmware   s/w rev. 6.2.0_65x1, 14-Aug-07"
		
// create container to hold our matches
vector<string> match_results;
	
// define a regular expression object - parameter is a standard regular expression
TMRex verstring("Firmware\\s+s/w\\s+rev\\. (\\d+)\\.(\\d+)\\.(\\d+)_(\\w+)");
	
// match and place results in our vector container
if(verstring.match(match_string,match_results)) [
    // print results
    cerr << "major rev # : " << match_results[0] << endl;
    cerr << "minor rev # : " << match_results[1] << endl;
    cerr << "release #   : " << match_results[2] << endl;
    cerr << "patch       : " << match_results[3] << endl;
}
else {
    cerr << "invalid id string" << endl;
}
\endcode
*/
class TMRex {
	static const int MAX_MATCH=10;
public:
	enum {GLOBAL=1};
	TMRex();
	/**
	  Construct a regular expression object
	*/
	TMRex(const string&);
	/**
	  Parse accessor method
	*/
	int parse(const string&);
	/**
	  Match accessor method
	*/
	bool match(const string&, vector<string>& );
	/**
	  Substitution accessor method
	*/
	string substitute(const string&, int flag=GLOBAL);
private:
	FXRex rex;
	int beg[MAX_MATCH+1];
	int end[MAX_MATCH+1];
	string estring;
};

#define MAX_CAPT 8;

#endif
