#pragma once

#include <stdio.h>
#include "cmanifs.h"

extern STRING   incdir;

/*
 * Used to keep track of the current expression part
 * of a case statement i.e. case <expr> of ... end
 */
extern EXPRPTR  expval[NESTINGDEPTH];
extern int      expvalcnt;

/*
 * Used to keep count of the number of arguments applied
 * to a function
 */
extern int      exprlevel;
extern int      exprlistcount[NESTINGDEPTH];

/*
 * c     - used to hold current character
 * peekc - used by input routine to allow a one character
 *         lookahead in yylex.c
 */
extern int      c;
extern int      peekc;

extern int      newline;

/*
 * Used to store information about the current line and
 * cursor postion. Used to give  pLucid runtime error
 * messages
 */
extern int      startoflex;
extern int      curr_index;
extern int      curr_length;
extern int      in_index;

/*
 * used throughout yylex.c to indicate where or not a list
 * constant is being read or not
 */
extern int      const_list;

extern int      errcount;
extern int      false_;
extern int      true_;

/*    */
extern int      l;

/*
 * Keeps count of the number of parameters a function is defined
 * to have.
 */
extern int      idcount;

/*
 * Used to buffer the current line of characters being
 * lexically analysed, from current input file.
 */
extern int      linebuf[BUFFERLENGTH];

/* used to hold the value of the current input file */
extern FILE*    lexin;

extern YYSTYPE  yylval;

struct in_file {
    FILE*   in_fdes;
	STRING  in_name;
	int     in_line;
};
extern struct in_file   in_files[NOOFFILES];

extern STRING   s;
extern STRING   t;

extern char     buffer[2000];
