#pragma once

#include <stdio.h>
#include "cmanifs.h"

#define LIST_SIZE 100

extern int ffcount;
extern int fflevel;
extern int funclevel;
extern int f_level[LIST_SIZE];
extern int valoflevel;
extern int currentlevel;

extern EXPRPTR used_list[LIST_SIZE];
extern EXPRPTR defined_list[LIST_SIZE];
extern EXPRPTR new_defs[LIST_SIZE];
extern EXPRPTR new_decls[LIST_SIZE];

extern EXPRPTR formals_list[LIST_SIZE];

extern FFPTR new_def[LIST_SIZE];

extern int l;
extern int c;
extern int peekc;
extern int errcount;
extern int in_index;
extern int true_;
extern int false_;

extern char cconst;
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
extern STRING largest;

extern char buffer[500];
