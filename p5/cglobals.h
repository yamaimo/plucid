#pragma once

#include <stdio.h>
#include "cmanifs.h"

#define MAXFUNCS        104
#define HASHSIZE        120
#define MAXSTRINGS      200
#define MAXVARS         400
#define MAXFILES       1000

extern int filecount;
extern int l;
extern int c;
extern int peekc;
extern int errcount;
extern int in_index;
extern int true_;
extern int false_;

extern int cconst;
extern int wordval;

extern char linebuf[BUFFERLENGTH];

extern FILE* lexin;
extern FILE* outfile;
extern FILE* savelex;

extern YYSTYPE yylval;

struct in_file {
    FILE*   in_fdes;
    STRING  in_name;
    int     in_line;
};
extern struct in_file in_files[NOOFFILES];

extern STRING s;
extern STRING t;

extern char buffer[500];

extern F_STACKPTR f_stack;
extern P_STACKPTR p_stack;
extern E_STACKPTR parm_stack;
extern E_LISTPTR  parm_list;

extern int lexlevel;
extern int funclevel;
extern int elemlevel;
extern int identcount;
extern int formalcount;
extern int idusage; /* is ident being declared, used, or what? */

extern SYMPTR hashtable[HASHSIZE + 1];

extern STRING stringtable[MAXSTRINGS];
extern STRING wordtable[MAXSTRINGS];
extern int stringcount;
extern int wordcount;

extern EXPRPTR exprtable[MAXVARS];  /* the expressions trees for each variable */
extern STRING  nametable[MAXVARS];  /* the user names for each variable */
extern EXPRPTR filetable[MAXFILES];

extern FITEM ftable[MAXFUNCS];
