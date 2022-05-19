//////////////////////////////////////////////////////////////////
/// This regular expression support class has been modified from
/// the Fox Tollkit library http://www.foxtoolkit.org
///
/// I've added some interface to make it more friendly to
/// 93K test application developers
///
/// Brian Buras
/// Agilent Technologies
/// Application Development Center
/// 10/21/2005
////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <assert.h>
#include <vector>
#include "TMRex.h"

using std::vector;
using std::string;

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
*********************************************************************************
* $Id: TMRex.cpp 4057 2007-09-14 17:12:55 +0200 (Fri, 14 Sep 2007) sledford $                           *
********************************************************************************/


/*
  The Story:
  ==========

  As with most regex implementations, this one is inspired by Henry Spencer's
  original implementation.

  This is however an ab-initio implementation, with the following goals:

        o Full C++ implementation, no simple C++ wapper.
        o Trade speed and memory in favor of speed, but keep it compact where
          possible.
        o Thread-safe:
            - No global variables used during parsing or execution.
            - Multiple threads could use same single FXRex at the same time.
        o Perl-like syntax for character classes.
        o Additional features such as lazy/greedy closures, counting repeats,
          back references, trailing context.
        o Forward and backward subject string scanning mode.
        o Single line/multi line matching modes.
        o 8-bit safe (you can use it to grep binary data!).
        o No arbitrary limitations.  Patterns can be as long as you need them
          to be, memory allowing.  This is possible due to representing the
          progam as 32-bit integers.
        o When parsing fails, or when created with default constructor, FXRex
          is initialized to a "fallback" program; its thus safe to invoke match
          at any time.
        o The default fallback program will fail to match anything.
        o Observe current locale if your "ctype.h" functions support it, by calling
          all locale-sensitive functions during the match.
        o Convenient feature: disallow empty string matches; this is nice as
          it prevents a common problem, for example searching for "a*" in "bbba";
          without the REX_NOT_EMPTY option, this matches "" and not the "a".
        o Another convenient feature is the ability to compile verbatim strings.
          This is practical as it allows FXRex to be populated with a simple
          string with no interpretation of special characters ^*?+{}()\$.[].
        o Note that FXRex's implementation would naturally move to wide
          character support...

  Because this is completely new implementation of regular expressions, and
  not merely an extension of a previous implementation, some people may want to
  adapt it for use outside of FOX.  This is perfectly OK with me.

  However:

        o The Author is not responsible for the consequences of using this
          software.

        o Recipient should be clearly informed of the origin of the software;
          and if alterations have been made, this must be stated clearly.

        o Software will continue to fall under Lesser GPL, unless specific
          permission from the Author has been obtained.


  Implementation notes:
  =====================

  This implementation does not use "nodes" with "next" pointers; instead, the "next"
  opcode is located implicitly by simple sequentiality.  This has beneficial effect
  on speed, as one can simply add to the program counter instead of performing a
  memory reference.

  Sometimes one needs to jump out of sequence; this is implemented by an
  explicit jump instruction.  Because it works with 32-bit offsets, there
  is no need to distinguish between forward and backward jumps.

  Henry Spencer implemented a trick to "prefix" simple single-character matching
  opcodes with a closure operator, by shifting down already generated code and
  inserting the closure operator in front of it.

  FXRex uses the same trick of shifting down code; but this same trick is also
  useful for branches!

  FXRex only generates a branch node when an alternative has in fact been seen;
  if no alternative is there, we've saved ourselves both a branch node and a
  jump node!

  This has interesting side-effects:

        o The common case of 1 single branch now no longer needs a branch opcode
          and corresponding jump opcode at all!

        o When an alternative is found, we insert a branch node in front, and a jump
          node behind the already generated code.  This can be done easily as
          branches and jumps within the shifted block of code are relative, and have
          already been resolved!

        o The matching algorithm for a branch opcode simplifies as well:- either
          it matches the first branch, or it continues after the jump.

        o Its easier to dig out some info from the program, and in fact so easy
          that this digging can be moved to the execute phase.

   When a repeat is prefixed in front of a simple single-character match, counted
   repeats are simplified: {1}, {1,1} is eliminated, {}, {0,} becomes *, {1,} becomes
   +, and {0,1} becomes ?.

   Because single-character repeats are prefixed with a repeat instruction, there is
   no recursion needed; single character repeats are therefore very fast.

   Complex repeats are implemented using branches loop constructs and may involve
   recursions (in fact only the fixed repeat is non-recursive!).  Hence complex repeats
   should be avoided when single-character repeats will suffice.

   OP_BRANCH and OP_BRANCHREV implement alternatives. For OP_BRANCH, first the inline
   code immediately following the offset is executed; if the inline code fails, OP_BRANCH
   takes a jump to the new location and tries the alternative.

   For OP_BRANCHREV, it works the opposite way: OP_BRANCHREV takes the alternative
   first, before trying the inline code.

   Having both OP_BRANCH and OP_BRANCHREV substantially simplifies the design of
   complex, greedy or lazy matches:- the greedy and lazy match turn out to have the
   same code structure, except we're using OP_BRANCHREV for the lazy matches and
   OP_BRANCH for the greedy ones.

   OP_JUMP is an unconditional jump to a new location. OP_JUMPLT and OP_JUMPGT jump
   when the loop counter is less than or greater than the given value, respectively.



  Syntax:
  =======

      ^         Match begin of line [if at begin of pattern]
      $         Match end of line [if at end of pattern]
      .         Match any character
      |         Alternation
      ( ... )   Grouping sub pattern
      (?i ... ) Match sub pattern case insensitive
      (?I ... ) Match sub pattern case sensitive
      (?n ... ) Match sub pattern with newlines
      (?N ... ) Match sub pattern with no newlines
      (?: ... ) Non-capturing parentheses
      (?= ... ) Zero width positive lookahead
      (?! ... ) Zero width negative lookahead
      []        Character class

      *         Match 0 or more [greedy]
      +         Match 1 or more [greedy]
      ?         Match 0 or 1 [greedy]
      {}        Match 0 or more [greedy]
      {n}       Match n times [greedy]
      {,m}      Match no more than m times [greedy]
      {n,}      Match n or more [greedy]
      {n,m}     Match at least n but no more than m times [greedy]

      *?        Match 0 or more [lazy]
      +?        Match 1 or more [lazy]
      ??        Match 0 or 1 [lazy]
      {}?       Match 0 or more times [lazy]
      {n}?      Match n times [lazy]
      {,m}?     Match no more than m times [lazy]
      {n,}?     Match n or more [lazy]
      {n,m}?    Match at least n but no more than m times [lazy]

      \a        Alarm, bell
      \e        Escape character
      \t        Tab
      \f        Form feed
      \n        Newline
      \r        Return
      \v        Vertical tab
      \cx       Control character
      \033      Octal
      \x1b      Hex
      \w        Word character [a-zA-Z_0-9]
      \W        Non-word character
      \l        Letter [a-zA-Z]
      \L        Non-letter
      \s        Space
      \S        Non-space
      \d        Digit [0-9]
      \D        Non-digit
      \h        Hex digit [0-9a-fA-F]
      \H        Non-hex digit
      \b        Word boundary
      \B        Word interior
      \u        Single uppercase character
      \U        Single lowercase character
      \p        Punctuation (not including '_')
      \P        Non punctuation
      \<        Begin of word
      \>        End of word
      \A        Match only beginning of string
      \Z        Match only and end of string
      \1 ... \9 Back reference


  Grammar:
  ========

      exp        ::= branch { "|" branch }*
      branch     ::= { piece }*
      piece      ::= atom [ rep ]
      rep        ::= ( "*" | "+" | "?" | counts ) [ "?" ]
      counts     ::= "{" digits ["," [ digits] ] "}"
      atom       ::= "(" exp ")" | "[" [^] range "]" | characters
      range      ::= { character | character "-" character } +
      characters ::= { character }*
      digits     ::= { digit }*

  To do:
  ======

  - Look into optimizing character class when possible (e.g.
    collapse [0-9] to OP_DIGIT and [^A] into OP_NOT_CHAR).
  - Should \D, \W, \L match newline?
  - Look behind would be nice...
  - Repeating back references, only possible if capturing parentheses
    are known NOT to match "".
  - Note the \uXXXX is going to be used for UNICODE perhaps:
    See: http://www.unicode.org/unicode/reports/tr18.
*/


// As close to infinity as we're going to get; this seems big
// enough.  We can not make it MAX_INT as this may wrap around when
// added to something else!
#define ONEINDIG 1000000

// Number of capturing sub-expressions allowed
#define NSUBEXP  10

// Size of string buffer
#define MAXCHARS 512

typedef unsigned char Uchar;

// Set operations
#define EXCL(set,ch) (set[((Uchar)(ch))>>5]&=~(1<<(((Uchar)(ch))&31)))
#define INCL(set,ch) (set[((Uchar)(ch))>>5]|=(1<<(((Uchar)(ch))&31)))
#define ISIN(set,ch) (set[((Uchar)(ch))>>5]&(1<<(((Uchar)(ch))&31)))
#define CLEAR(set)   (set[0]=set[1]=set[2]=set[3]=set[4]=set[5]=set[6]=set[7]=0)

#define FXMEMDUP(ptr,src,type,no) (fxmemdup((void **)(ptr),(const void*)(src),sizeof(type)*(no)))
#define FXFREE(ptr)               (fxfree((void **)(ptr)))
#define FXMALLOC(ptr,type,no)     (fxmalloc((void **)(ptr),sizeof(type)*(no)))
#define FXCALLOC(ptr,type,no)     (fxcalloc((void **)(ptr),sizeof(type)*(no)))
#define FXRESIZE(ptr,type,no)     (fxresize((void **)(ptr),sizeof(type)*(no)))

#define TRUE 1
#define FALSE 0

/*******************************************************************************/

///@cond

// Opcodes of the engine
enum {
  OP_END           =   0,           // End of pattern reached
  OP_FAIL          =   1,           // Always fail
  OP_SUCCEED       =   2,           // Always succeed
  OP_LINE_BEG      =   3,           // Beginning of line
  OP_LINE_END      =   4,           // End of line
  OP_WORD_BEG      =   5,           // Beginning of word
  OP_WORD_END      =   6,           // End of word
  OP_WORD_BND      =   7,           // Word boundary
  OP_WORD_INT      =   8,           // Word interior
  OP_STR_BEG       =   9,           // Beginning of string
  OP_STR_END       =  10,           // End of string
  OP_ANY_OF        =  11,           // Any character in set
  OP_ANY_BUT       =  12,           // Any character not in set
  OP_ANY           =  13,           // Any character but no newline
  OP_ANY_NL        =  14,           // Any character including newline
  OP_SPACE         =  15,           // White space
  OP_SPACE_NL      =  16,           // White space including newline
  OP_NOT_SPACE     =  17,           // Non-white space
  OP_DIGIT         =  18,           // Digit
  OP_NOT_DIGIT     =  19,           // Non-digit
  OP_NOT_DIGIT_NL  =  20,           // Non-digit including newline
  OP_LETTER        =  21,           // Letter
  OP_NOT_LETTER    =  22,           // Non-letter
  OP_NOT_LETTER_NL =  23,           // Non-letter including newline
  OP_WORD          =  24,           // Word character
  OP_NOT_WORD      =  25,           // Non-word character
  OP_NOT_WORD_NL   =  26,           // Non-word character including newline
  OP_HEX           =  27,           // Hex digit
  OP_NOT_HEX       =  28,           // Non hex digit
  OP_NOT_HEX_NL    =  29,           // Non hex digit including newline
  OP_PUNCT         =  30,           // Punctuation
  OP_NOT_PUNCT     =  31,           // Non punctuation
  OP_NOT_PUNCT_NL  =  32,           // Non punctuation including newline
  OP_CHARS         =  33,           // Match literal string
  OP_CHARS_CI      =  34,           // Match literal string, case insensitive
  OP_CHAR          =  35,           // Single character
  OP_CHAR_CI       =  36,           // Single character, case insensitive
  OP_JUMP          =  37,           // Jump to another location
  OP_BRANCH        =  38,           // Branch: jump after trying following code
  OP_BRANCHREV     =  39,           // Branch: jump before trying following code
  OP_STAR          =  40,           // Greedy * (simple)
  OP_MIN_STAR      =  41,           // Lazy * (simple)
  OP_PLUS          =  42,           // Greedy + (simple)
  OP_MIN_PLUS      =  43,           // Lazy + (simple)
  OP_QUEST         =  44,           // Greedy ? (simple)
  OP_MIN_QUEST     =  45,           // Lazy ? (simple)
  OP_REP           =  46,           // Greedy counted repeat (simple)
  OP_MIN_REP       =  47,           // Lazy counted repeat (simple)
  OP_LOOK_NEG      =  48,           // Negative look ahead
  OP_LOOK_POS      =  49,           // Positive look ahead
  OP_UPPER         =  50,           // Match upper case
  OP_LOWER         =  51,           // Match lower case
  OP_SUB_BEG       =  52,           // Start of substring i
  OP_SUB_END       =  62,           // End of substring i
  OP_REF           =  72,           // Back reference to substring i
  OP_REF_CI        =  82,           // Match substring i case insensitive
  OP_ZERO          =  92,           // Zero count i
  OP_INCR          = 102,           // Increment count i
  OP_JUMPLT        = 112,           // Jump if count i less than value
  OP_JUMPGT        = 122            // JUmp if count i greater than value
};


// Flags
enum {
	FLG_WORST  = 0,           // Worst case
	FLG_WIDTH  = 1,           // Matches >=1 character
	FLG_SIMPLE = 2            // Simple
};

// Allocate memory
int fxmalloc(void** ptr,unsigned long size){
	*ptr=NULL;
	if(size!=0){
		if((*ptr=malloc(size))==NULL) return FALSE;
	}
	return TRUE;
}


// Allocate cleaned memory
int fxcalloc(void** ptr,unsigned long size){
	*ptr=NULL;
	if(size!=0){
		if((*ptr=calloc(size,1))==NULL) return FALSE;
		}
	return TRUE;
}


// Resize memory
int fxresize(void** ptr,unsigned long size){
	register void *p=NULL;
	if(size!=0){
		if((p=realloc(*ptr,size))==NULL) return FALSE;
	}
	else{
		if(*ptr) free(*ptr);
	}
	*ptr=p;
	return TRUE;
}
	
// Allocate and initialize memory
int fxmemdup(void** ptr,const void* src,unsigned long size){
	*ptr=NULL;
	if(size!=0 && src!=NULL){
		if((*ptr=malloc(size))==NULL) return FALSE;
		memcpy(*ptr,src,size);
	}
	return TRUE;
}
// Free memory, resets ptr to NULL afterward
void fxfree(void** ptr){
	if(*ptr){
		free(*ptr);
		*ptr=NULL;
	}
}


void fxmessage(const char* format,...){
	va_list arguments;
	va_start(arguments,format);
	vfprintf(stderr,format,arguments);
	va_end(arguments);
}

void fxerror(const char* format,...){
	va_list arguments;
	va_start(arguments,format);
	vfprintf(stderr,format,arguments);
	va_end(arguments);
}

// Structure used during matching
struct FXExecute {
	const char  *str;               // String
	const char  *str_beg;           // Begin of string
	const char  *str_end;           // End of string
	int         *sub_beg;           // Begin of substring i
	int         *sub_end;           // End of substring i
	const int   *code;              // Program code
	int          npar;              // Number of capturing parentheses
	int          count[10];         // Counters for counted repeats
	int          mode;              // Match mode
	
	// Attempt to match
	bool attempt(const char* string);
	
	// Match at current string position
	bool match(const int* prog);
	
	// Execute
	bool execute(const char* fm,const char* to);
};


// Structure used during compiling
struct FXCompile {
	const char  *pat;               // Pattern string pointer
	int         *code;              // Program code
	int         *pc;                // Program counter
	int          mode;              // Compile mode
	int          nbra;              // Number of counting braces
	int          npar;              // Number of capturing parentheses
	
	// Code generation
	int* append(int op);
	int* append(int op,int arg);
	int* append(int op,int arg1,int arg2);
	int* append(int op,int set[]);
	int* append(int op,int len,int *data);
	int* insert(int *ptr,int op);
	int* insert(int *ptr,int op,int arg);
	int* insert(int *ptr,int op,int arg1,int arg2);
	
	// Patch branches
	void patch(int *fm,int *to);
	
	// Parsing
	FXRexError compile(int& flags);
	FXRexError expression(int& flags);
	FXRexError verbatim(int& flags);
	FXRexError alternative(int& flags);
	FXRexError piece(int& flags);
	FXRexError atom(int& flags);
	FXRexError charset();
};


/*******************************************************************************/

#ifndef NDEBUG

// Dump program
void dump(int *prog){
	int op,min,max,no,val;
	fxmessage("\n");
	fxmessage("Program:\n");
	fxmessage("%-10p SIZE %d\n",prog,*prog);
	prog++;
	while(1){
		fxmessage("%-10p ",prog);
		op=*prog++;
		switch(op){
			case OP_END:
				fxmessage("OP_END\n");
				goto x;
			case OP_FAIL:
				fxmessage("OP_FAIL\n");
				break;
			case OP_SUCCEED:
				fxmessage("OP_SUCCEED\n");
				break;
			case OP_LINE_BEG:
				fxmessage("OP_LINE_BEG\n");
				break;
			case OP_LINE_END:
				fxmessage("OP_LINE_END\n");
				break;
			case OP_WORD_BEG:
				fxmessage("OP_WORD_BEG\n");
				break;
			case OP_WORD_END:
				fxmessage("OP_WORD_END\n");
				break;
			case OP_WORD_BND:
				fxmessage("OP_WORD_BND\n");
				break;
			case OP_WORD_INT:
				fxmessage("OP_WORD_INT\n");
				break;
			case OP_STR_BEG:
				fxmessage("OP_STR_BEG\n");
				break;
			case OP_STR_END:
				fxmessage("OP_STR_END\n");
				break;
			case OP_ANY_OF:
				fxmessage("OP_ANY_OF   \"");
				for(no=0; no<256; no++){
					if(ISIN(prog,no)){
						if(' '<=no){ fxmessage("%c",no); } else { fxmessage("\\x%02x",no); }
					}
				}
				fxmessage("\"\n");
				prog+=8;
				break;
			case OP_ANY_BUT:
				fxmessage("OP_ANY_BUT  \"");
				for(no=0; no<256; no++){
					if(ISIN(prog,no)){
						if(' '<=no){ fxmessage("%c",no); } else { fxmessage("\\x%02x",no); }
					}
				}
				fxmessage("\"\n");
				prog+=8;
				break;
			case OP_ANY:
				fxmessage("OP_ANY\n");
				break;
			case OP_ANY_NL:
				fxmessage("OP_ANY_NL\n");
				break;
			case OP_SPACE:
				fxmessage("OP_SPACE\n");
				break;
			case OP_SPACE_NL:
				fxmessage("OP_SPACE_NL\n");
				break;
			case OP_NOT_SPACE:
				fxmessage("OP_NOT_SPACE\n");
				break;
			case OP_DIGIT:
				fxmessage("OP_DIGIT\n");
				break;
			case OP_NOT_DIGIT:
				fxmessage("OP_NOT_DIGIT\n");
				break;
			case OP_NOT_DIGIT_NL:
				fxmessage("OP_NOT_DIGIT_NL\n");
				break;
			case OP_LETTER:
				fxmessage("OP_LETTER\n");
				break;
			case OP_NOT_LETTER:
				fxmessage("OP_NOT_LETTER\n");
				break;
			case OP_NOT_LETTER_NL:
				fxmessage("OP_NOT_LETTER_NL\n");
				break;
			case OP_WORD:
				fxmessage("OP_WORD\n");
				break;
			case OP_NOT_WORD:
				fxmessage("OP_NOT_WORD\n");
				break;
			case OP_NOT_WORD_NL:
				fxmessage("OP_NOT_WORD_NL\n");
				break;
			case OP_HEX:
				fxmessage("OP_HEX\n");
				break;
			case OP_NOT_HEX:
				fxmessage("OP_NOT_HEX\n");
				break;
			case OP_NOT_HEX_NL:
				fxmessage("OP_NOT_HEX_NL\n");
				break;
			case OP_PUNCT:
				fxmessage("OP_PUNCT\n");
				break;
			case OP_NOT_PUNCT:
				fxmessage("OP_NOT_PUNCT\n");
				break;
			case OP_NOT_PUNCT_NL:
				fxmessage("OP_NOT_PUNCT_NL\n");
				break;
			case OP_UPPER:
				fxmessage("OP_UPPER\n");
				break;
			case OP_LOWER:
				fxmessage("OP_LOWER\n");
				break;
			case OP_CHARS:
				no=*prog++;
				fxmessage("OP_CHARS     %d,\"",no);
				while(no>0){
					if(' '<=*prog){ fxmessage("%c",*prog); } else { fxmessage("\\x%02x",*prog); }
					prog++;
					no--;
				}
				fxmessage("\"\n");
				break;
			case OP_CHARS_CI:
				no=*prog++;
				fxmessage("OP_CHARS_CI  %d,\"",no);
				while(no>0){
					if(' '<=*prog){ fxmessage("%c",*prog); } else { fxmessage("\\x%02x",*prog); }
					prog++;
					no--;
				}
				fxmessage("\"\n");
				break;
			case OP_CHAR:
				fxmessage("OP_CHAR      \"");
				if(' '<=*prog){ fxmessage("%c",*prog); } else { fxmessage("\\x%02x",*prog); }
				fxmessage("\"\n");
				prog++;
				break;
			case OP_CHAR_CI:
				fxmessage("OP_CHAR_CI   \"");
				if(' '<=*prog){ fxmessage("%c",*prog); } else { fxmessage("\\x%02x",*prog); }
				fxmessage("\"\n");
				prog++;
				break;
			case OP_JUMP:
				fxmessage("OP_JUMP      %-10p\n",*prog ? prog+*prog : 0);
				prog++;
				break;
			case OP_BRANCH:
				fxmessage("OP_BRANCH    %-10p\n",*prog ? prog+*prog : 0);
				prog++;
				break;
			case OP_BRANCHREV:
				fxmessage("OP_BRANCHREV %-10p\n",*prog ? prog+*prog : 0);
				prog++;
				break;
			case OP_STAR:
				fxmessage("OP_STAR\n");
				break;
			case OP_MIN_STAR:
				fxmessage("OP_MIN_STAR\n");
				break;
			case OP_PLUS:
				fxmessage("OP_PLUS\n");
				break;
			case OP_MIN_PLUS:
				fxmessage("OP_MIN_PLUS\n");
				break;
			case OP_QUEST:
				fxmessage("OP_QUEST\n");
				break;
			case OP_MIN_QUEST:
				fxmessage("OP_MIN_QUEST\n");
				break;
			case OP_REP:
				min=*prog++;
				max=*prog++;
				fxmessage("OP_REP       {%d,%d}\n",min,max);
				break;
			case OP_MIN_REP:
				min=*prog++;
				max=*prog++;
				fxmessage("OP_MIN_REP   {%d,%d}\n",min,max);
				break;
			case OP_LOOK_NEG:
				fxmessage("OP_LOOK_NEG  %-10p\n",*prog ? prog+*prog : 0);
				prog++;
				break;
			case OP_LOOK_POS:
				fxmessage("OP_LOOK_POS  %-10p\n",*prog ? prog+*prog : 0);
				prog++;
				break;
			case OP_SUB_BEG+0:
			case OP_SUB_BEG+1:
			case OP_SUB_BEG+2:
			case OP_SUB_BEG+3:
			case OP_SUB_BEG+4:
			case OP_SUB_BEG+5:
			case OP_SUB_BEG+6:
			case OP_SUB_BEG+7:
			case OP_SUB_BEG+8:
			case OP_SUB_BEG+9:
				fxmessage("OP_SUB_BEG%d\n",op-OP_SUB_BEG);
				break;
			case OP_SUB_END+0:
			case OP_SUB_END+1:
			case OP_SUB_END+2:
			case OP_SUB_END+3:
			case OP_SUB_END+4:
			case OP_SUB_END+5:
			case OP_SUB_END+6:
			case OP_SUB_END+7:
			case OP_SUB_END+8:
			case OP_SUB_END+9:
				fxmessage("OP_SUB_END%d\n",op-OP_SUB_END);
				break;
			case OP_REF+0:
			case OP_REF+1:
			case OP_REF+2:
			case OP_REF+3:
			case OP_REF+4:
			case OP_REF+5:
			case OP_REF+6:
			case OP_REF+7:
			case OP_REF+8:
			case OP_REF+9:
				fxmessage("OP_REF%d\n",op-OP_REF);
				break;
			case OP_REF_CI+0:
			case OP_REF_CI+1:
			case OP_REF_CI+2:
			case OP_REF_CI+3:
			case OP_REF_CI+4:
			case OP_REF_CI+5:
			case OP_REF_CI+6:
			case OP_REF_CI+7:
			case OP_REF_CI+8:
			case OP_REF_CI+9:
				fxmessage("OP_REF_CI%d\n",op-OP_REF_CI);
				break;
			case OP_ZERO+0:
			case OP_ZERO+1:
			case OP_ZERO+2:
			case OP_ZERO+3:
			case OP_ZERO+4:
			case OP_ZERO+5:
			case OP_ZERO+6:
			case OP_ZERO+7:
			case OP_ZERO+8:
			case OP_ZERO+9:
				fxmessage("OP_ZERO%d\n",op-OP_ZERO);
				break;
			case OP_INCR+0:
			case OP_INCR+1:
			case OP_INCR+2:
			case OP_INCR+3:
			case OP_INCR+4:
			case OP_INCR+5:
			case OP_INCR+6:
			case OP_INCR+7:
			case OP_INCR+8:
			case OP_INCR+9:
				fxmessage("OP_INCR%d\n",op-OP_INCR);
				break;
			case OP_JUMPLT+0:
			case OP_JUMPLT+1:
			case OP_JUMPLT+2:
			case OP_JUMPLT+3:
			case OP_JUMPLT+4:
			case OP_JUMPLT+5:
			case OP_JUMPLT+6:
			case OP_JUMPLT+7:
			case OP_JUMPLT+8:
			case OP_JUMPLT+9:
				val=*prog++;
				fxmessage("OP_JUMPLT%d   %d,%-10p\n",op-OP_JUMPLT,val,*prog ? prog+*prog : 0);
				prog++;
				break;
			case OP_JUMPGT+0:
			case OP_JUMPGT+1:
			case OP_JUMPGT+2:
			case OP_JUMPGT+3:
			case OP_JUMPGT+4:
			case OP_JUMPGT+5:
			case OP_JUMPGT+6:
			case OP_JUMPGT+7:
			case OP_JUMPGT+8:
			case OP_JUMPGT+9:
				val=*prog++;
				fxmessage("OP_JUMPGT%d   %d,%-10p\n",op-OP_JUMPGT,val,*prog ? prog+*prog : 0);
				prog++;
				break;
			default:
				fxmessage("OP_%d: error\n",op);
				goto x;
      }
	}
x:fxmessage("end\n");
}

#endif


/*******************************************************************************/

// FXCompile members

// Parse hex escape code
int hex(const char*& pat){
	register int ch,n,c;
	for(ch=0,n=2; isxdigit((Uchar)*pat) && n; n--){
		c=(Uchar)toupper((Uchar)*pat++);
		ch=(ch<<4)+(('A'<=c)?c-'A'+10:c-'0');
	}
	return ch;
}


// Parse octal escape code
int oct(const char*& pat){
	register int ch,n;
	for(ch=0,n=3; '0'<=((Uchar)*pat) && ((Uchar)*pat)<='7' && n; n--){
		ch=(ch<<3)+(*pat++-'0');
	}
	return ch;
}


// Compiler main
FXRexError FXCompile::compile(int& flags){
	FXRexError err;
	if(*pat=='\0') return REGERR_EMPTY;
	if(mode&REX_VERBATIM)
		err=verbatim(flags);
	else
		err=expression(flags);
	if(err!=REGERR_OK) return err;
	if(*pat!='\0') return REGERR_PAREN;
	append(OP_END);
	return REGERR_OK;
}


// Parse without interpretation of magic characters
FXRexError FXCompile::verbatim(int& flags){
	int buf[MAXCHARS],ch,len;
	flags=FLG_WIDTH;
	while(*pat!='\0'){
		len=0;
		do{
			ch=*pat++;
			if(mode&REX_ICASE) ch=tolower((Uchar)ch);
			buf[len++]=ch;
      }
		while(*pat!='\0' && len<MAXCHARS);
		if(len==1){
			flags|=FLG_SIMPLE;
			append((mode&REX_ICASE)?OP_CHAR_CI:OP_CHAR,buf[0]);
      }
		else{
			append((mode&REX_ICASE)?OP_CHARS_CI:OP_CHARS,len,buf);
      }
	}
	return REGERR_OK;
}


// Parse expression
FXRexError FXCompile::expression(int& flags){
  FXRexError err;
  int *at,*jp,flg;
  flags=FLG_WIDTH;
  at=pc;
  jp=NULL;
  err=alternative(flg);
  if(err!=REGERR_OK) return err;
  if(!(flg&FLG_WIDTH)) flags&=~FLG_WIDTH;
  while(*pat=='|'){
    pat++;
    insert(at,OP_BRANCH,pc-at+3);
    append(OP_JUMP,jp?jp-pc-1:0);
    jp=pc-1;
    at=pc;
    err=alternative(flg);
    if(err!=REGERR_OK) return err;
    if(!(flg&FLG_WIDTH)) flags&=~FLG_WIDTH;
    }
  patch(jp,pc);
  return REGERR_OK;
  }


// Parse branch
FXRexError FXCompile::alternative(int& flags){
  FXRexError err;
  int flg;
  flags=FLG_WORST;
  while(*pat!='\0' && *pat!='|' && *pat!=')'){
    err=piece(flg);
    if(err!=REGERR_OK) return err;
    flags|=flg;
    }
  return REGERR_OK;
  }


// Parse piece
FXRexError FXCompile::piece(int& flags){
  int ch,rep_min,rep_max,lazy,flg,*ptr;
  FXRexError err;
  ptr=pc;
  err=atom(flg);
  if(err!=REGERR_OK) return err;
  if((ch=*pat)=='*' || ch=='+' || ch=='?' || ch=='{'){    // Followed by repetition
    if(!(flg&FLG_WIDTH)) return REGERR_NOATOM;            // Repeats may not match empty
    pat++;
    lazy=0;
    rep_min=1;
    rep_max=1;
    switch(ch){
      case '*':                                           // Repeat 0-INF
        rep_min=0;
        rep_max=ONEINDIG;
        break;
      case '+':                                           // Repeat 1-INF
        rep_min=1;
        rep_max=ONEINDIG;
        break;
      case '?':                                           // Repeat 0-1
        rep_min=0;
        rep_max=1;
        break;
      case '{':                                           // Repeat n-m
        rep_min=0;
        rep_max=ONEINDIG;
        if(*pat!='}'){
          while(isdigit((Uchar)*pat)){
            rep_min=10*rep_min+(*pat-'0');
            pat++;
            }
          rep_max=rep_min;
          if(*pat==','){
            pat++;
            rep_max=ONEINDIG;
            if(*pat!='}'){
              rep_max=0;
              while(isdigit((Uchar)*pat)){
                rep_max=10*rep_max+(*pat-'0');
                pat++;
                }
              }
            }
          if(rep_min>rep_max) return REGERR_RANGE;              // Illegal range
          if(rep_min==0 && rep_max==0) return REGERR_COUNT;     // Bad count
          }
        if(*pat!='}') return REGERR_BRACE;                      // Unmatched brace
        pat++;
        break;
      }
    if(*pat=='?'){ pat++; lazy=1; }   // Lazy
    if(rep_min==0) flg&=~FLG_WIDTH;   // If zero repetitions are allowed, then may have no width

    // Handle only non-trivial cases
    if(!(rep_min==1 && rep_max==1)){

      // For simple repeats we prefix the last operation
      if(flg&FLG_SIMPLE){
        if(rep_min==0 && rep_max==ONEINDIG){
          insert(ptr,OP_STAR+lazy);
          }
        else if(rep_min==1 && rep_max==ONEINDIG){
          insert(ptr,OP_PLUS+lazy);
          }
        else if(rep_min==0 && rep_max==1){
          insert(ptr,OP_QUEST+lazy);
          }
        else{
          insert(ptr,OP_REP+lazy,rep_min,rep_max);
          }
        }

      // For complex repeats we build loop constructs
      else{
        if(rep_min==0 && rep_max==ONEINDIG){
          /*    ________
          **   |        \
          ** --B--(...)--J--+--                 (...){0,ONEINDIG}
          **    \___________|
          */
          insert(ptr,lazy?OP_BRANCHREV:OP_BRANCH,pc-ptr+3);
          append(OP_JUMP,ptr-pc-1);
          }
        else if(rep_min==1 && rep_max==ONEINDIG){
          /*    ________
          **   |        \
          ** --+--(...)--B--                    (...){1,ONEINDIG}
          **
          */
          append(lazy?OP_BRANCH:OP_BRANCHREV,ptr-pc-1);
          }
        else if(rep_min==0 && rep_max==1){
          /*
          **
          ** --B--(...)--+--                    (...){0,1}
          **    \________|
          */
          insert(ptr,lazy?OP_BRANCHREV:OP_BRANCH,pc-ptr+1);
          }
        else if(0<rep_min && rep_min==rep_max){
          /*       ___________
          **      |           \
          ** --Z--+--(...)--I--L--              (...){n,n}
          **
          */
          if(nbra>=NSUBEXP) return REGERR_COMPLEX;
          insert(ptr,OP_ZERO+nbra);
          append(OP_INCR+nbra);
          append(OP_JUMPLT+nbra,rep_min,ptr-pc-1);
          nbra++;
          }
        else if(rep_min==0 && rep_max<ONEINDIG){
          /*       ___________
          **      |           \
          ** --Z--B--(...)--I--L--+--           (...){0,n}
          **       \______________|
          */
          if(nbra>=NSUBEXP) return REGERR_COMPLEX;
          insert(ptr,OP_ZERO+nbra);
          insert(ptr+1,lazy?OP_BRANCHREV:OP_BRANCH,pc-ptr+4);
          append(OP_INCR+nbra);
          append(OP_JUMPLT+nbra,rep_max,ptr-pc-1);
          nbra++;
          }
        else if(0<rep_min && rep_max==ONEINDIG){
          /*       ________________
          **      |   ___________  \
          **      |  |           \  \
          ** --Z--+--+--(...)--I--L--B--        (...){n,ONEINDIG}
          */
          if(nbra>=NSUBEXP) return REGERR_COMPLEX;
          insert(ptr,OP_ZERO+nbra);
          append(OP_INCR+nbra);
          append(OP_JUMPLT+nbra,rep_min,ptr-pc-1);
          append(lazy?OP_BRANCH:OP_BRANCHREV,ptr-pc);
          nbra++;
          }
        else{
          /*       ___________________
          **      |   ___________     \
          **      |  |           \     \
          ** --Z--+--+--(...)--I--L--G--B--+--  (...){n,m}
          **                          \____|
          */
          if(nbra>=NSUBEXP) return REGERR_COMPLEX;
          insert(ptr,OP_ZERO+nbra);
          append(OP_INCR+nbra);
          append(OP_JUMPLT+nbra,rep_min,ptr-pc-1);
          append(OP_JUMPGT+nbra,rep_max,3);
          append(lazy?OP_BRANCH:OP_BRANCHREV,ptr-pc);
          nbra++;
          }
        }
      }
    }
  flags=flg&FLG_WIDTH;
  return REGERR_OK;
  }


// Parse atom
FXRexError FXCompile::atom(int& flags){
  int buf[MAXCHARS],level,save,ch,len,flg,*ptr;
  const char *p;
  FXRexError err;
  flags=FLG_WORST;                                // Assume the worst
  switch(*pat){
    case '(':                                     // Subexpression grouping
      pat++;
      if(*pat=='?'){
        pat++;
        ch=*pat++;
        if(ch==':'){                              // Non capturing parentheses
          err=expression(flg);
          if(err!=REGERR_OK) return err;          // Propagate error
          }
        else if(ch=='=' || ch=='!'){              // Positive or negative look ahead
          append((ch=='=')?OP_LOOK_POS:OP_LOOK_NEG);
          ptr=append(0);
          err=expression(flg);
          if(err!=REGERR_OK) return err;          // Propagate error
          append(OP_SUCCEED);
          patch(ptr,pc);                          // If trailing context matches (fails), go here!
          flg=FLG_WORST;                          // Look ahead has no width!
          }
        else if(ch=='i' || ch=='I' || ch=='n' || ch=='N'){
          save=mode;                              // Save flags
          if(ch=='i') mode|=REX_ICASE;
          if(ch=='I') mode&=~REX_ICASE;
          if(ch=='n') mode|=REX_NEWLINE;
          if(ch=='N') mode&=~REX_NEWLINE;
          err=expression(flg);
          if(err!=REGERR_OK) return err;          // Propagate error
          mode=save;                              // Restore flags
          }
        else{
          return REGERR_TOKEN;
          }
        }
      else if(mode&REX_CAPTURE){                  // Capturing
        level=++npar;
        if(level>=NSUBEXP) return REGERR_COMPLEX; // Expression too complex
        append(OP_SUB_BEG+level);
        err=expression(flg);
        if(err!=REGERR_OK) return err;            // Propagate error
        append(OP_SUB_END+level);
        }
      else{                                       // Normal
        err=expression(flg);
        if(err!=REGERR_OK) return err;            // Propagate error
        }
      if(*pat!=')') return REGERR_PAREN;          // Unmatched parenthesis
      pat++;
      flags=flg&~FLG_SIMPLE;
      break;
    case '.':                                     // Any character
      pat++;
      append((mode&REX_NEWLINE)?OP_ANY_NL:OP_ANY);
      flags=FLG_WIDTH|FLG_SIMPLE;
      break;
    case '^':                                     // Begin of line
      pat++;
      append(OP_LINE_BEG);
      break;
    case '$':                                     // End of line
      pat++;
      append(OP_LINE_END);
      break;
    case '*':                                     // No preceding atom
    case '+':
    case '?':
    case '{':
      return REGERR_NOATOM;
    case '\0':                                    // Technically, this can not happen!
    case '|':
    case ')':
      return REGERR_NOATOM;
    case '}':                                     // Unmatched brace
      return REGERR_BRACE;
    case '[':
      pat++;
      err=charset();
      if(err!=REGERR_OK) return err;              // Bad character class
      if(*pat!=']') return REGERR_BRACK;          // Unmatched bracket
      pat++;
      flags=FLG_WIDTH|FLG_SIMPLE;
      break;
    case ']':                                     // Unmatched bracket
      return REGERR_BRACK;
    case '\\':                                    // Escape sequences which are NOT part of simple character-run
      ch=*(pat+1);
      switch(ch){
        case '\0':                                // Unexpected pattern end
          return REGERR_NOATOM;
        case 'w':                                 // Word character
          append(OP_WORD);
          pat+=2;
          flags=FLG_WIDTH|FLG_SIMPLE;
          return REGERR_OK;
        case 'W':                                 // Non-word character
          append((mode&REX_NEWLINE)?OP_NOT_WORD_NL:OP_NOT_WORD);
          pat+=2;
          flags=FLG_WIDTH|FLG_SIMPLE;
          return REGERR_OK;
        case 's':                                 // Space
          append((mode&REX_NEWLINE)?OP_SPACE_NL:OP_SPACE);
          pat+=2;
          flags=FLG_WIDTH|FLG_SIMPLE;
          return REGERR_OK;
        case 'S':                                 // Non-space
          append(OP_NOT_SPACE);
          pat+=2;
          flags=FLG_WIDTH|FLG_SIMPLE;
          return REGERR_OK;
        case 'd':                                 // Digit
          append(OP_DIGIT);
          pat+=2;
          flags=FLG_WIDTH|FLG_SIMPLE;
          return REGERR_OK;
        case 'D':                                 // Non-digit
          append((mode&REX_NEWLINE)?OP_NOT_DIGIT_NL:OP_NOT_DIGIT);
          pat+=2;
          flags=FLG_WIDTH|FLG_SIMPLE;
          return REGERR_OK;
        case 'h':                                 // Hex digit
          append(OP_HEX);
          pat+=2;
          flags=FLG_WIDTH|FLG_SIMPLE;
          return REGERR_OK;
        case 'H':                                 // Non-hex digit
          append((mode&REX_NEWLINE)?OP_NOT_HEX_NL:OP_NOT_HEX);
          pat+=2;
          flags=FLG_WIDTH|FLG_SIMPLE;
          return REGERR_OK;
        case 'p':                                 // Punctuation
          append(OP_PUNCT);
          pat+=2;
          flags=FLG_WIDTH|FLG_SIMPLE;
          return REGERR_OK;
        case 'P':                                 // Non-punctuation
          append((mode&REX_NEWLINE)?OP_NOT_PUNCT_NL:OP_NOT_PUNCT);
          pat+=2;
          flags=FLG_WIDTH|FLG_SIMPLE;
          return REGERR_OK;
        case 'l':                                 // Letter
          append(OP_LETTER);
          pat+=2;
          flags=FLG_WIDTH|FLG_SIMPLE;
          return REGERR_OK;
        case 'L':                                 // Non-letter
          append((mode&REX_NEWLINE)?OP_NOT_LETTER_NL:OP_NOT_LETTER);
          pat+=2;
          flags=FLG_WIDTH|FLG_SIMPLE;
          return REGERR_OK;
        case 'u':                                 // Upper case
          append(OP_UPPER);
          pat+=2;
          flags=FLG_WIDTH|FLG_SIMPLE;
          return REGERR_OK;
        case 'U':                                 // Lower case
          append(OP_LOWER);
          pat+=2;
          flags=FLG_WIDTH|FLG_SIMPLE;
          return REGERR_OK;
        case 'b':                                 // Word boundary
          append(OP_WORD_BND);
          pat+=2;
          return REGERR_OK;
        case 'B':                                 // Word interior
          append(OP_WORD_INT);
          pat+=2;
          return REGERR_OK;
        case 'A':                                 // Match only beginning of string
          append(OP_STR_BEG);
          pat+=2;
          return REGERR_OK;
        case 'Z':                                 // Match only and end of string
          append(OP_STR_END);
          pat+=2;
          return REGERR_OK;
        case '<':                                 // Begin of word
          append(OP_WORD_BEG);
          pat+=2;
          return REGERR_OK;
        case '>':                                 // End of word
          append(OP_WORD_END);
          pat+=2;
          return REGERR_OK;
        case '1':                                 // Back reference to previously matched subexpression
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          if(!(mode&REX_CAPTURE)) return REGERR_BACKREF;  // Can't do backreferences
          level=ch-'0';
          if(level>npar) return REGERR_BACKREF;           // Back reference out of range
          append((mode&REX_ICASE)?(OP_REF_CI+level):(OP_REF+level));
          pat+=2;
          return REGERR_OK;
        }
      /*fall*/
    default:
      len=0;
      do{
        p=pat;                                    // In case we need to back up...
        ch=*pat;
        switch(ch){
          case '^':                               // Bail out on magic characters
          case '$':
          case '.':
          case '(':
          case ')':
          case '[':
          case ']':
          case '|':
            goto x;
          case '\\':
            ch=*(pat+1);
            switch(ch){
              case 'w':                           // Bail out on special matching constructs
              case 'W':
              case 's':
              case 'S':
              case 'd':
              case 'D':
              case 'h':
              case 'H':
              case 'p':
              case 'P':
              case 'l':
              case 'L':
              case 'u':
              case 'U':
              case 'b':
              case 'B':
              case 'A':
              case 'Z':
              case '<':
              case '>':
              case '1':
              case '2':
              case '3':
              case '4':
              case '5':
              case '6':
              case '7':
              case '8':
              case '9':
                goto x;
              case 'a':                           // Bell
                pat+=2;
                ch='\a';
                break;
              case 'e':                           // Escape
                pat+=2;
                ch='\033';
                break;
              case 'f':                           // Form feed
                pat+=2;
                ch='\f';
                break;
              case 'n':                           // Newline
                pat+=2;
                ch='\n';
                break;
              case 'r':                           // Return
                pat+=2;
                ch='\r';
                break;
              case 't':                           // Tab
                pat+=2;
                ch='\t';
                break;
              case 'v':                           // Vertical tab
                pat+=2;
                ch='\v';
                break;
              case 'c':                           // Control character
                pat+=2;
                ch=*pat++;
                if(ch=='\0') return REGERR_NOATOM;// Unexpected pattern end
                ch=toupper(ch)-'@';
                break;
              case '0':                           // Octal digit
                pat+=2;
                ch=oct(pat);
                if(ch>256) return REGERR_TOKEN;   // Characters should be 0..255
                break;
              case 'x':                           // Hex digit
                pat+=2;
                ch=hex(pat);
                if(ch>256) return REGERR_TOKEN;   // Characters should be 0..255
                break;
              case '\0':                          // Unexpected pattern end
                return REGERR_NOATOM;
              default:
                pat+=2;
                break;
              }
            break;
          case '\0':                              // Unexpected pattern end
            return REGERR_NOATOM;
          default:
            pat++;
            break;
          }

        // Make lower case?
        if(mode&REX_ICASE) ch=tolower((Uchar)ch);

        // Add to buffer
        buf[len++]=ch;
        }
      while(*pat!='\0' && *pat!='*' && *pat!='+' && *pat!='?' && *pat!='{' && len<MAXCHARS);

      // Back up one character if followed by a repetition: aaa* is interpreted as (aa)a*
x:    if(1<len && (*pat=='*' || *pat=='+' || *pat=='?' || *pat=='{')){
        pat=p;
        len--;
        }

      assert(1<=len);

      // Had at least 1 character
      flags=FLG_WIDTH;

      // Simple only if length is 1
      if(len==1){
        flags|=FLG_SIMPLE;
        append((mode&REX_ICASE)?OP_CHAR_CI:OP_CHAR,buf[0]);
        }

      // Array of characters
      else{
        append((mode&REX_ICASE)?OP_CHARS_CI:OP_CHARS,len,buf);
        }
      break;
    }
  return REGERR_OK;
  }


// TRUE if character is a word character
inline int isword(int ch){
  return isalnum((Uchar)ch) || ch=='_';
  }


// TRUE if character is punctuation (delimiter) character
inline int isdelim(int ch){
  return ispunct(ch) && ch!='_';
  }


// Parse character class
FXRexError FXCompile::charset(){
  register int first,last,op,i;
  int set[8];
  CLEAR(set);
  first=-1;
  if(*pat=='^'){                                  // Negated character class
    op=OP_ANY_BUT;
    pat++;
    }
  else{
    op=OP_ANY_OF;
    }
  if(*pat=='-' || *pat==']') goto in;             // '-' and ']' are literal at begin
  while(*pat!='\0' && *pat!=']'){
in: last=*pat++;
    if(last=='\\'){
      last=*pat++;
      switch(last){
        case 'w':
          for(i=0; i<256; i++) {if(isword(i)) INCL(set,i); }
          first=-1;
          continue;
        case 'W':
          for(i=0; i<256; i++){ if(!isword(i)) INCL(set,i); }
          first=-1;
          continue;
        case 's':
          for(i=0; i<256; i++){ if(isspace(i)) INCL(set,i); }
          first=-1;
          continue;
        case 'S':
          for(i=0; i<256; i++){ if(!isspace(i)) INCL(set,i); }
          first=-1;
          continue;
        case 'd':
          for(i=0; i<256; i++){ if(isdigit(i)) INCL(set,i); }
          first=-1;
          continue;
        case 'D':
          for(i=0; i<256; i++){ if(!isdigit(i)) INCL(set,i); }
          first=-1;
          continue;
        case 'h':
          for(i=0; i<256; i++){ if(isxdigit(i)) INCL(set,i); }
          first=-1;
          continue;
        case 'H':
          for(i=0; i<256; i++){ if(!isxdigit(i)) INCL(set,i); }
          first=-1;
          continue;
        case 'p':
          for(i=0; i<256; i++){ if(isdelim(i)) INCL(set,i); }
          first=-1;
          continue;
        case 'P':
          for(i=0; i<256; i++){ if(!isdelim(i)) INCL(set,i); }
          first=-1;
          continue;
        case 'l':
          for(i=0; i<256; i++){ if(isalpha((Uchar)i)) INCL(set,i); }
          first=-1;
          continue;
        case 'L':
          for(i=0; i<256; i++){ if(!isalpha(i)) INCL(set,i); }
          first=-1;
          continue;
        case 'u':
          for(i=0; i<256; i++){ if(isupper(i)) INCL(set,i); }
          first=-1;
          continue;
        case 'U':
          for(i=0; i<256; i++){ if(islower(i)) INCL(set,i); }
          first=-1;
          continue;
        case 'a':                             // Bell
          last='\a';
          break;
        case 'e':                             // Escape
          last='\033';
          break;
        case 'b':                             // Backspace
          last='\b';
          break;
        case 'f':                             // Form feed
          last='\f';
          break;
        case 'n':                             // Newline
          last='\n';
          break;
        case 'r':                             // Return
          last='\r';
          break;
        case 't':                             // Tab
          last='\t';
          break;
        case 'v':                             // Vertical tab
          last='\v';
          break;
        case 'c':                             // Control character
          last=*pat++;
          if(last=='\0') return REGERR_NOATOM;// Unexpected pattern end
          last=toupper(last)-'@';
          break;
        case '0':                             // Octal digit
          last=oct(pat);
          break;
        case 'x':                             // Hex digit
          last=hex(pat);
          break;
        case '\0':
          return REGERR_NOATOM;               // Unexpected pattern end
        }
      }
    if(first==-1){
      if(mode&REX_ICASE){
        INCL(set,tolower(last));
        INCL(set,toupper(last));
        }
      else{
        INCL(set,last);
        }
      if(*pat=='-' && *(pat+1)!='\0' && *(pat+1)!=']'){
        first=last;
        pat++;
        }
      }
    else{
      if(first>=last) return REGERR_RANGE;    // Bad range
      if(mode&REX_ICASE){
        for(i=first; i<=last; i++){
          INCL(set,tolower(i));
          INCL(set,toupper(i));
          }
        }
      else{
        for(i=first; i<=last; i++){
          INCL(set,i);
          }
        }
      first=-1;
      }
    }

  // Are we matching newlines
  if((op==OP_ANY_BUT) && !(mode&REX_NEWLINE) && !ISIN(set,'\n')){
    INCL(set,'\n');
    }

  // Emit opcode
  append(op,set);
  return REGERR_OK;
  }


// Append opcode
int* FXCompile::append(int op){
  register int *val=pc;
  if(code){
    pc[0]=op;
    }
  pc++;
  return val;
  }


// Append one-argument opcode
int* FXCompile::append(int op,int arg){
  register int *val=pc;
  if(code){
    pc[0]=op;
    pc[1]=arg;
    }
  pc+=2;
  return val;
  }


// Append two-argument opcode
int* FXCompile::append(int op,int arg1,int arg2){
  register int *val=pc;
  if(code){
    pc[0]=op;
    pc[1]=arg1;
    pc[2]=arg2;
    }
  pc+=3;
  return val;
  }


// Append character class opcode
int* FXCompile::append(int op,int set[]){
  register int *val=pc;
  if(code){
    pc[0]=op;
    pc[1]=set[0];
    pc[2]=set[1];
    pc[3]=set[2];
    pc[4]=set[3];
    pc[5]=set[4];
    pc[6]=set[5];
    pc[7]=set[6];
    pc[8]=set[7];
    }
  pc+=9;
  return val;
  }


// Append character array
int* FXCompile::append(int op,int len,int *data){
  register int *val=pc;
  if(code){
    pc[0]=op;
    pc[1]=len;
    memcpy(pc+2,data,sizeof(int)*len);
    }
  pc+=len+2;
  return val;
  }


// Insert opcode at ptr
int* FXCompile::insert(int *ptr,int op){
  if(code){
    memmove(ptr+1,ptr,sizeof(int)*(pc-ptr));
    ptr[0]=op;
    }
  pc+=1;
  return ptr;
  }


// Insert one-argument opcode at ptr
int* FXCompile::insert(int *ptr,int op,int arg){
  if(code){
    memmove(ptr+2,ptr,sizeof(int)*(pc-ptr));
    ptr[0]=op;
    ptr[1]=arg;
    }
  pc+=2;
  return ptr;
  }


// Insert two-argument opcode at ptr
int* FXCompile::insert(int *ptr,int op,int arg1,int arg2){
  if(code){
    memmove(ptr+3,ptr,sizeof(int)*(pc-ptr));
    ptr[0]=op;
    ptr[1]=arg1;
    ptr[2]=arg2;
    }
  pc+=3;
  return ptr;
  }



// Patch linked set of branches or jumps
// Example:
//
//      Before:        After:
//      ==========================
//      0:  OP_JUMP    0:  OP_JUMP
//      1:  0          1:  9
//      2:  ....       2:  ....
//      3:  OP_JUMP    3:  OP_JUMP
//      4:  -3         4:  6
//      5:  ....       5:  ....
//      6:  ....       6:  ....
//      7:  OP_JUMP    7:  OP_JUMP
// fm-> 8:  -4         8:  2
//      9:  ....       9:  ....
// to->10:  ....      10:  ....
//
void FXCompile::patch(int *fm,int *to){
  register int delta;
  if(code && fm){
    do{
      delta=*fm;
      *fm=to-fm;
      fm+=delta;
      }
    while(delta);
    }
  }


/*******************************************************************************/

// FXExecute members

// The workhorse
bool FXExecute::match(const int* prog){
  register int no,keep,rep_min,rep_max,greed,op;
  register const char *save,*beg,*end;
  register char ch;
  for(;;){
    op=*prog++;
    switch(op){
      case OP_END:
        return TRUE;
      case OP_FAIL:           // Fail (sub) pattern
        return FALSE;
      case OP_SUCCEED:        // Succeed (sub) pattern
        return TRUE;
      case OP_JUMP:
        prog+=*prog;
        break;
      case OP_BRANCH:         // Jump after trying following code
        save=str;
        if(match(prog+1)) return TRUE;
        str=save;
        prog+=*prog;
        break;
      case OP_BRANCHREV:      // Jump before trying following code
        save=str;
        if(match(prog+*prog)) return TRUE;
        str=save;
        prog++;
        break;
      case OP_LINE_BEG:       // Must be at begin of line
        if((str==str_beg && (mode&REX_NOT_BOL)) || (str_beg<str && *(str-1)!='\n')) return FALSE;
        break;
      case OP_LINE_END:       // Must be at end of line
        if((str==str_end && (mode&REX_NOT_EOL)) || (str<str_end && *str!='\n')) return FALSE;
        break;
      case OP_WORD_BEG:       // Must be at begin of word
        if(str_beg<str && isword((Uchar) *(str-1))) return FALSE;
        if(str_end<=str || !isword((Uchar) *str)) return FALSE;
        break;
      case OP_WORD_END:       // Must be at end of word
        if(str<str_end && isword((Uchar) *str)) return FALSE;
        if(str<=str_beg || !isword((Uchar) *(str-1))) return FALSE;
        break;
      case OP_WORD_BND:       // Must be at word boundary
        if(!(((str==str_beg || !isword((Uchar) *(str-1))) && (str<str_end && isword((Uchar) *str))) || ((str==str_end || !isword((Uchar) *str)) && (str_beg<str && isword((Uchar) *(str-1)))))) return FALSE;
        break;
      case OP_WORD_INT:       // Must be inside a word
        if(str==str_beg || !isword((Uchar) *(str-1))) return FALSE;
        if(str==str_end || !isword((Uchar) *str)) return FALSE;
        break;
      case OP_STR_BEG:        // Must be at begin of entire string
        if(str!=str_beg) return FALSE;
        break;
      case OP_STR_END:        // Must be at end of entire string
        if(str!=str_end) return FALSE;
        break;
      case OP_ANY_OF:         // Match a character in a set
        if(str==str_end || !ISIN(prog,*str)) return FALSE;
        prog+=8;
        str++;
        break;
      case OP_ANY_BUT:        // Match a character NOT in a set
        if(str==str_end || ISIN(prog,*str)) return FALSE;
        prog+=8;
        str++;
        break;
      case OP_CHAR:           // Match single character
        if(str==str_end || *prog != *str) return FALSE;
        prog++;
        str++;
        break;
      case OP_CHAR_CI:        // Match single character, disregard case
        if(str==str_end || *prog != tolower((Uchar)*str)) return FALSE;
        prog++;
        str++;
        break;
      case OP_CHARS:          // Match a run of 1 or more characters
        no=*prog++;
        if(str+no>str_end) return FALSE;
        do{
          if(*prog++ != (Uchar)*str++) return FALSE;
          }
        while(--no);
        break;
      case OP_CHARS_CI:       // Match a run of 1 or more characters, disregard case
        no=*prog++;
        if(str+no>str_end) return FALSE;
        do{
          if(*prog++ != tolower((Uchar)*str++)) return FALSE;
          }
        while(--no);
        break;
      case OP_SPACE:          // Match space
        if(str==str_end || *str=='\n' || !isspace((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_SPACE_NL:       // Match space including newline
        if(str==str_end || !isspace((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_NOT_SPACE:      // Match non-space
        if(str==str_end || isspace((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_DIGIT:          // Match a digit 0..9
        if(str==str_end || !isdigit((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_NOT_DIGIT:      // Match a non-digit
        if(str==str_end || *str=='\n' || isdigit((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_NOT_DIGIT_NL:   // Match a non-digit including newline
        if(str==str_end || isdigit((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_HEX:            // Match a hex digit 0..9A-Fa-f
        if(str==str_end || !isxdigit((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_NOT_HEX:        // Match a non-hex digit
        if(str==str_end || *str=='\n' || isxdigit((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_NOT_HEX_NL:     // Match a non-hex digit including newline
        if(str==str_end || isxdigit((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_PUNCT:          // Match a punctuation
        if(str==str_end || !isdelim((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_NOT_PUNCT:      // Match a non-punctuation
        if(str==str_end || *str=='\n' || isdelim((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_NOT_PUNCT_NL:   // Match a non-punctuation including newline
        if(str==str_end || isdelim((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_LETTER:         // Match a letter a..z, A..Z
        if(str==str_end || !isalpha((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_NOT_LETTER:     // Match a non-letter
        if(str==str_end || *str=='\n' || isalpha((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_NOT_LETTER_NL:  // Match a non-letter including newline
        if(str==str_end || isalpha((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_WORD:           // Match a word character a..z,A..Z,0..9,_
        if(str==str_end || !isword((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_NOT_WORD:       // Match a non-word character
        if(str==str_end || *str=='\n' || isword((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_NOT_WORD_NL:    // Match a non-word character including newline
        if(str==str_end || isword((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_UPPER:          // Match if uppercase
        if(str==str_end || !isupper((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_LOWER:          // Match if lowercase
        if(str==str_end || !islower((Uchar) *str)) return FALSE;
        str++;
        break;
      case OP_ANY:            // Match any character
        if(str==str_end || *str=='\n') return FALSE;
        str++;
        break;
      case OP_ANY_NL:          // Matches any character including newline
        if(str==str_end) return FALSE;
        str++;
        break;
      case OP_PLUS:         // Simple repetitions of one character
        rep_min=1;
        rep_max=ONEINDIG;
        greed=1;
        goto rep;
      case OP_MIN_PLUS:
        rep_min=1;
        rep_max=ONEINDIG;
        greed=0;
        goto rep;
      case OP_QUEST:
        rep_min=0;
        rep_max=1;
        greed=1;
        goto rep;
      case OP_MIN_QUEST:
        rep_min=0;
        rep_max=1;
        greed=0;
        goto rep;
      case OP_REP:
        rep_min=*prog++;
        rep_max=*prog++;
        greed=1;
        goto rep;
      case OP_MIN_REP:
        rep_min=*prog++;
        rep_max=*prog++;
        greed=0;
        goto rep;
      case OP_MIN_STAR:
        rep_min=0;
        rep_max=ONEINDIG;
        greed=0;
        goto rep;
      case OP_STAR:         // Star is the most popular so make that the fall-through case...
        rep_min=0;
        rep_max=ONEINDIG;
        greed=1;

        // We need to match more characters than are available
rep:    if(str+rep_min>str_end) return FALSE;
        beg=str;
        end=beg+rep_max;
        if(end>str_end) end=str_end;
        save=beg;

        // Find out how much could be matched
        op=*prog++;
        switch(op){
          case OP_CHAR:
            ch=*prog++;
            while(save<end && *save==ch) save++;
            break;
          case OP_CHAR_CI:
            ch=*prog++;
            while(save<end && tolower((Uchar) *save)==ch && *save!='\n') save++;
            break;
          case OP_CHARS:
            ch=*++prog;
            while(save<end && *save==ch) save++;
            prog+=3;
            break;
          case OP_CHARS_CI:
            ch=*++prog;
            while(save<end && tolower((Uchar) *save)==ch) save++;
            prog+=3;
            break;
          case OP_ANY_OF:
            while(save<end && ISIN(prog,*save)) save++;
            prog+=8;
            break;
          case OP_ANY_BUT:
            while(save<end && !ISIN(prog,*save)) save++;
            prog+=8;
            break;
          case OP_SPACE:
            while(save<end && *save!='\n' && isspace((Uchar) *save)) save++;
            break;
          case OP_SPACE_NL:
            while(save<end && isspace((Uchar) *save)) save++;
            break;
          case OP_NOT_SPACE:
            while(save<end && !isspace((Uchar) *save)) save++;
            break;
          case OP_DIGIT:
            while(save<end && isdigit((Uchar) *save)) save++;
            break;
          case OP_NOT_DIGIT:
            while(save<end && *save!='\n' && !isdigit((Uchar) *save)) save++;
            break;
          case OP_NOT_DIGIT_NL:
            while(save<end && !isdigit((Uchar) *save)) save++;
            break;
          case OP_HEX:
            while(save<end && isxdigit((Uchar) *save)) save++;
            break;
          case OP_NOT_HEX:
            while(save<end && *save!='\n' && !isxdigit((Uchar) *save)) save++;
            break;
          case OP_NOT_HEX_NL:
            while(save<end && !isxdigit((Uchar) *save)) save++;
            break;
          case OP_PUNCT:
            while(save<end && isdelim((Uchar) *save)) save++;
            break;
          case OP_NOT_PUNCT:
            while(save<end && *save!='\n' && !isdelim((Uchar) *save)) save++;
            break;
          case OP_NOT_PUNCT_NL:
            while(save<end && !isdelim((Uchar) *save)) save++;
            break;
          case OP_LETTER:
            while(save<end && isalpha((Uchar) *save)) save++;
            break;
          case OP_NOT_LETTER:
            while(save<end && *save!='\n' && !isalpha((Uchar) *save)) save++;
            break;
          case OP_NOT_LETTER_NL:
            while(save<end && !isalpha((Uchar) *save)) save++;
            break;
          case OP_WORD:
            while(save<end && isword((Uchar) *save)) save++;
            break;
          case OP_NOT_WORD:
            while(save<end && *save!='\n' && !isword((Uchar) *save)) save++;
            break;
          case OP_NOT_WORD_NL:
            while(save<end && !isword((Uchar) *save)) save++;
            break;
          case OP_UPPER:
            while(save<end && isupper((Uchar) *save)) save++;
            break;
          case OP_LOWER:
            while(save<end && islower((Uchar) *save)) save++;
            break;
          case OP_ANY:
            while(save<end && *save!='\n') save++;
            break;
          case OP_ANY_NL:
            save=end; // Big byte
            break;
          default:
            fxerror("FXRex::match: bad opcode (%d) at: %p on line: %d\n",op,prog-1,__LINE__);
            break;
          }

        // Matched fewer than the minimum desired so bail out
        if(save<beg+rep_min) return FALSE;

        // We must match between beg and end characters
        beg+=rep_min;
        end=save;

        // Greedily match the most characters
        if(greed){
          while(beg<=end){
            str=end;
            if(match(prog)) return TRUE;
            end--;
            }
          }

        // Lazily match the fewest characters
        else{
          while(beg<=end){
            str=beg;
            if(match(prog)) return TRUE;
            beg++;
            }
          }
        return FALSE;
      case OP_SUB_BEG+0:              // Capturing open parentheses
      case OP_SUB_BEG+1:
      case OP_SUB_BEG+2:
      case OP_SUB_BEG+3:
      case OP_SUB_BEG+4:
      case OP_SUB_BEG+5:
      case OP_SUB_BEG+6:
      case OP_SUB_BEG+7:
      case OP_SUB_BEG+8:
      case OP_SUB_BEG+9:
        no=op-OP_SUB_BEG;
        if(no>=npar) break;           // Match w/o capture if array too small
        keep=sub_beg[no];             // Keep old value
        sub_beg[no]=str-str_beg;      // Tentatively set new value
        if(match(prog)) return TRUE;  // Match the rest
        sub_beg[no]=keep;             // Restore old value
        return FALSE;
      case OP_SUB_END+0:              // Capturing close parentheses
      case OP_SUB_END+1:
      case OP_SUB_END+2:
      case OP_SUB_END+3:
      case OP_SUB_END+4:
      case OP_SUB_END+5:
      case OP_SUB_END+6:
      case OP_SUB_END+7:
      case OP_SUB_END+8:
      case OP_SUB_END+9:
        no=op-OP_SUB_END;
        if(no>=npar) break;           // Match w/o capture if array too small
        keep=sub_end[no];
        sub_end[no]=str-str_beg;      // Remember capture end for future back reference
        if(match(prog)) return TRUE;
        sub_end[no]=keep;             // Restore old value
        return FALSE;
      case OP_REF+0:                  // Back reference to capturing parentheses
      case OP_REF+1:
      case OP_REF+2:
      case OP_REF+3:
      case OP_REF+4:
      case OP_REF+5:
      case OP_REF+6:
      case OP_REF+7:
      case OP_REF+8:
      case OP_REF+9:
        no=op-OP_REF;
        if(no>=npar) return FALSE;                    // Arrays were too small
        if(sub_beg[no]<0) return FALSE;               // Not captured yet
        if(sub_end[no]<0) return FALSE;               // Not captured yet
        beg=str_beg+sub_beg[no];
        end=str_beg+sub_end[no];
        if(beg<end){                                  // Empty capture matches!
          if(str+(end-beg)>str_end) return FALSE;     // Not enough characters left
          do{
            if(*beg != *str) return FALSE;            // No match
            beg++;
            str++;
            }
          while(beg<end);
          }
        break;
      case OP_REF_CI+0:               // Back reference to capturing parentheses
      case OP_REF_CI+1:
      case OP_REF_CI+2:
      case OP_REF_CI+3:
      case OP_REF_CI+4:
      case OP_REF_CI+5:
      case OP_REF_CI+6:
      case OP_REF_CI+7:
      case OP_REF_CI+8:
      case OP_REF_CI+9:
        no=op-OP_REF_CI;
        if(no>=npar) return FALSE;                    // Arrays were too small
        if(sub_beg[no]<0) return FALSE;               // Not captured yet
        if(sub_end[no]<0) return FALSE;               // Not captured yet
        beg=str_beg+sub_beg[no];
        end=str_beg+sub_end[no];
        if(beg<end){                                  // Empty capture matches!
          if(str+(end-beg)>str_end) return FALSE;     // Not enough characters left
          do{
            if(*beg != tolower((Uchar)*str)) return FALSE;            // No match
            beg++;
            str++;
            }
          while(beg<end);
          }
        break;
      case OP_LOOK_NEG:               // Positive or negative look ahead
      case OP_LOOK_POS:
        save=str;
        keep=match(prog+1);
        str=save;
        if((op-OP_LOOK_NEG)!=keep) return FALSE;      // Didn't get what we expected
        prog=prog+*prog;              // Jump to code after OP_SUCCEED
        break;
      case OP_ZERO+0:                 // Initialize counter for counting repeat
      case OP_ZERO+1:
      case OP_ZERO+2:
      case OP_ZERO+3:
      case OP_ZERO+4:
      case OP_ZERO+5:
      case OP_ZERO+6:
      case OP_ZERO+7:
      case OP_ZERO+8:
      case OP_ZERO+9:
        count[op-OP_ZERO]=0;
        break;
      case OP_INCR+0:                 // Increment counter for counting repeat
      case OP_INCR+1:
      case OP_INCR+2:
      case OP_INCR+3:
      case OP_INCR+4:
      case OP_INCR+5:
      case OP_INCR+6:
      case OP_INCR+7:
      case OP_INCR+8:
      case OP_INCR+9:
        count[op-OP_INCR]++;
        break;
      case OP_JUMPLT+0:               // Jump if counter less than value
      case OP_JUMPLT+1:
      case OP_JUMPLT+2:
      case OP_JUMPLT+3:
      case OP_JUMPLT+4:
      case OP_JUMPLT+5:
      case OP_JUMPLT+6:
      case OP_JUMPLT+7:
      case OP_JUMPLT+8:
      case OP_JUMPLT+9:
        if(count[op-OP_JUMPLT] < *prog++)   // Compare with value
          prog+=*prog;
        else
          prog++;
        break;
      case OP_JUMPGT+0:               // Jump if counter greater than value
      case OP_JUMPGT+1:
      case OP_JUMPGT+2:
      case OP_JUMPGT+3:
      case OP_JUMPGT+4:
      case OP_JUMPGT+5:
      case OP_JUMPGT+6:
      case OP_JUMPGT+7:
      case OP_JUMPGT+8:
      case OP_JUMPGT+9:
        if(count[op-OP_JUMPGT] > *prog++)   // Compare with value
          prog+=*prog;
        else
          prog++;
        break;
      default:
        fxerror("FXRex::match: bad opcode (%d) at: %p on line: %d\n",op,prog-1,__LINE__);
        break;
      }
    }
  return FALSE;
  }


// regtry - try match at specific point; 0 failure, 1 success
bool FXExecute::attempt(const char* string){
  register int i=npar;
  str=string;
  do{--i;sub_beg[i]=sub_end[i]=-1;}while(i);          // Possibly move this to FXExecute::execute?
  if(match(code+1)){
    if(string!=str || !(mode&REX_NOT_EMPTY)){         // Match if non-empty or empty is allowed!
      sub_beg[0]=string-str_beg;
      sub_end[0]=str-str_beg;
      return TRUE;
      }
    }
  return FALSE;
  }


// Match subject string, returning number of matches found
bool FXExecute::execute(const char* fm,const char* to){
	register char ch;
	
	// Simple case
	if(fm==to) return attempt(fm);
	
	// Match backwards
	if(mode&REX_BACKWARD){
		if(code[1]==OP_STR_BEG){                          // Anchored at string start
			return (fm==str_beg) && attempt(str_beg);
      }
		if(code[1]==OP_LINE_BEG){                         // Anchored at BOL
			while(fm<=to){
				if(((to==str_beg)||(*(to-1)=='\n')) && attempt(to)) return TRUE;
				to--;
			}
			return FALSE;
      }
		if(code[1]==OP_CHAR || code[1]==OP_CHARS){        // Known starting character
			ch=(code[1]==OP_CHAR)?code[2]:code[3];
			if(to==str_end) to--;
			while(fm<=to){
				if(*to==ch && attempt(to)) return TRUE;
				to--;
			}
			return FALSE;
      }
		if(code[1]==OP_CHAR_CI || code[1]==OP_CHARS_CI){  // Known starting character, ignoring case
			ch=(code[1]==OP_CHAR_CI)?code[2]:code[3];
			if(to==str_end) to--;
			while(fm<=to){
				if(tolower((Uchar)*to)==ch && attempt(to)) return TRUE;
				to--;
			}
			return FALSE;
      }
		while(fm<=to){                                    // General case
			if(attempt(to)) return TRUE;
			to--;
      }
	}
	
	// Match forwards
	else{
    if(code[1]==OP_STR_BEG){                          // Anchored at string start
		 return (fm==str_beg) && attempt(str_beg);
	 }
    if(code[1]==OP_LINE_BEG){                         // Anchored at BOL
		 while(fm<=to){
			 if(((fm==str_beg)||(*(fm-1)=='\n')) && attempt(fm)) return TRUE;
        fm++;
		 }
		 return FALSE;
	 }
    if(code[1]==OP_CHAR || code[1]==OP_CHARS){        // Known starting character
		 ch=(code[1]==OP_CHAR)?code[2]:code[3];
		 if(to==str_end) to--;
		 while(fm<=to){
			 if(*fm==ch && attempt(fm)) return TRUE;
			 fm++;
		 }
		 return FALSE;
	 }
    if(code[1]==OP_CHAR_CI || code[1]==OP_CHARS_CI){  // Known starting character, ignoring case
		 ch=(code[1]==OP_CHAR_CI)?code[2]:code[3];
		 if(to==str_end) to--;
		 while(fm<=to){
			 if(tolower((Uchar)*fm)==ch && attempt(fm)) return TRUE;
			 fm++;
		 }
		 return FALSE;
	 }
    while(fm<=to){                                   // General case
		 if(attempt(fm)) return TRUE;
		 fm++;
      }
	}
	return FALSE;
}



/*******************************************************************************/


// Table of error messages
const char *const FXRex::errors[]={
	"OK",
	"Empty pattern",
	"Unmatched parenthesis",
	"Unmatched bracket",
	"Unmatched brace",
	"Bad character range",
	"Bad escape sequence",
	"Bad counted repeat",
	"No atom preceding repetition",
	"Repeat following repeat",
	"Bad backward reference",
	"Bad character class",
	"Expression too complex",
	"Out of memory",
	"Illegal token"
};


// Default program always fails
const int FXRex::fallback[]={2,OP_FAIL};


// Copy regex object
FXRex::FXRex(const FXRex& orig){
	code=(int*)fallback;
	if(orig.code!=fallback){
		FXMEMDUP(&code,orig.code,int,orig.code[0]);
	}
}


// Compile expression from pattern; fail if error
FXRex::FXRex(const char* pattern,int mode,FXRexError* error):code((int*)fallback){
	FXRexError err=parse(pattern,mode);
	if(error){ *error=err; }
}


// Compile expression from pattern; fail if error
FXRex::FXRex(const string& pattern,int mode,FXRexError* error):code((int*)fallback){
	FXRexError err=parse(pattern.c_str(),mode);
	if(error){ *error=err; }
}


// Assignment
FXRex& FXRex::operator=(const FXRex& orig){
	if(code!=orig.code){
		if(code!=fallback) FXFREE(&code);
		code=(int*)fallback;
		if(orig.code!=fallback){
			FXMEMDUP(&code,orig.code,int,orig.code[0]);
      }
	}
	return *this;
}


// Parse pattern
FXRexError FXRex::parse(const char* pattern,int mode){
	FXRexError err=REGERR_EMPTY;
	FXCompile cs;
	int flags,size;
	
	// Free old code, if any
	if(code!=fallback) FXFREE(&code);
	code=(int*)fallback;
	
	// Check
	if(pattern){

    // Fill in compile data
    cs.code=NULL;
    cs.pc=NULL;
    cs.pat=pattern;
    cs.mode=mode;
    cs.nbra=0;
    cs.npar=0;

    // Unknown size
    cs.append(0);

    // Check syntax and amount of memory needed
    err=cs.compile(flags);
    if(err==REGERR_OK){

      // Compile code unless only syntax checking
      if(!(mode&REX_SYNTAX)){

        // Allocate new code
        size=cs.pc-((int*)NULL);
        if(!FXMALLOC(&code,int,size)){
			  code=(int*)fallback;
			  return REGERR_MEMORY;
		  }

        // Fill in compile data
        cs.code=code;
        cs.pc=code;
        cs.pat=pattern;
        cs.mode=mode;
        cs.nbra=0;
        cs.npar=0;

        // Size of program
        cs.append(size);

        // Generate program
        err=cs.compile(flags);

        }
      }
    }
  return err;
  }


// Parse pattern, return error code if syntax error is found
FXRexError FXRex::parse(const string& pattern,int mode){
  return parse(pattern.c_str(),mode);
  }


/*******************************************************************************/



// Match subject string, returning number of matches found
bool FXRex::match(const char* estring,int len,int* beg,int* end,int mode,int npar,int fm,int to) const {
	if(!estring || len<0 || npar<1 || NSUBEXP<npar){ fxerror("FXRex::match: bad argument.\n"); }
	if(fm<0) fm=0;
	if(to>len) to=len;
	if(fm<=to){
		int abeg[NSUBEXP];
		int aend[NSUBEXP];
		FXExecute ms;
		if(!beg) beg=abeg;
		if(!end) end=aend;
		ms.str_beg=estring;
		ms.str_end=estring+len;
		ms.sub_beg=beg;
		ms.sub_end=end;
		ms.code=code;
		ms.npar=npar;
		ms.mode=mode;
		return ms.execute(estring+fm,estring+to);
	}
	return FALSE;
}


// Search for match in string
bool FXRex::match(const string& estring,int* beg,int* end,int mode,int npar,int fm,int to) const {
	return match(estring.c_str(),estring.length(),beg,end,mode,npar,fm,to);
}


// Return substitution string
string FXRex::substitute(const char* estring,int len,int* beg,int* end,const string& replace,int npar){
  register int ch,n,i=0;
  string result;
  if(!estring || len<0 || !beg || !end || npar<1 || NSUBEXP<npar){ fxerror("FXRex::substitute: bad argument.\n"); }
  while((ch=replace[i++])!='\0'){
    if(ch=='&'){
      if(0<=beg[0] && end[0]<=len){result.append(&estring[beg[0]],end[0]-beg[0]);}
      }
    else if(ch=='\\' && '0'<=replace[i] && replace[i]<='9'){
      n=replace[i++]-'0';
      if(n<npar && 0<=beg[n] && end[n]<=len){result.append(&estring[beg[n]],end[n]-beg[n]);}
      }
    else{
		 if(ch=='\\' && (replace[i]=='\\' || replace[i]=='&')){ch=replace[i++];}
		 result += ch;
	 }
  }
  return result;
}


// Return substitution string
string FXRex::substitute(const string& estring,int* beg,int* end,const string& replace,int npar){
  return substitute(estring.c_str(),estring.length(),beg,end,replace,npar);
  }


// Equality
bool operator==(const FXRex &r1,const FXRex &r2){
  return r1.code==r2.code || (r1.code[0]==r2.code[0] && memcmp(r1.code,r2.code,sizeof(int)*r1.code[0])==0);
  }


// Inequality
bool operator!=(const FXRex &r1,const FXRex &r2){
  return !(r1==r2);
  }


/*
// Save
ostream& operator<<(ostream& store,const FXRex& s){
	int size=s.code[0];
	store << size;
	store.save(s.code+1,size-1);
	return store;
}


// Load
istream& operator>>(istream& store,FXRex& s){
	int size;
	store >> size;
	FXMALLOC(&s.code,int,size);
	store.load(s.code+1,size-1);
	return store;
}
*/

// Clean up
FXRex::~FXRex(){
if(code!=fallback) FXFREE(&code);
}


//************************ wrappers **********************
const int MAX_MATCH=10;
TMRex::TMRex() {}
TMRex::TMRex(const string& pattern) {
	parse(pattern);
}
int TMRex::parse(const string& pattern) {
	return rex.parse(pattern,REX_CAPTURE);
}
string TMRex::substitute(const string& replace, int flag) {
	string res = estring;
	int max = 2;
	if(flag == GLOBAL) { max = MAX_MATCH; }
	for(int i=1;i<max;i++) {
		if(end[i] != -1) {
			res.replace(res.begin()+beg[i],res.begin()+end[i],replace);
		}
		else { i = max; }
	}
	return res;
}

bool TMRex::match(const string& target, vector<string>& result) {
	int m=0;
	estring = target;
	if(rex.match(target,beg,end,REX_FORWARD,MAX_MATCH)) {
		m=1;
		for(int i=1;i<MAX_MATCH;i++) {
			if(end[i] != -1) {
				result.push_back(target.substr(beg[i],end[i]-beg[i]));
			}
			else { i = MAX_MATCH; }
		}
	}
	return m;
}

///@endcond

