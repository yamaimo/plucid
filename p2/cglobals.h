#pragma once

#include <stdio.h>
#include "cmanifs.h"

extern int l;
extern int c;
extern int peekc;
extern int errcount;
extern int in_index;
extern int true_;
extern int false_;

extern char noclashes;
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
extern struct in_file   in_files[NOOFFILES];

extern STRING s;
extern STRING t;

extern char buffer[500];
