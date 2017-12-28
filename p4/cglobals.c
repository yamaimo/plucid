#include "cglobals.h"

int ffcount;
int fflevel;
int funclevel;
int f_level[LIST_SIZE];
int valoflevel;
int currentlevel;

EXPRPTR used_list[LIST_SIZE];
EXPRPTR defined_list[LIST_SIZE];
EXPRPTR new_defs[LIST_SIZE];
EXPRPTR new_decls[LIST_SIZE];

EXPRPTR formals_list[LIST_SIZE];

FFPTR new_def[LIST_SIZE];

int l;
int c;
int peekc;
int errcount;
int in_index;
int true_;
int false_;

char cconst;
char linebuf[BUFFERLENGTH];

FILE* lexin;
FILE* outfile;
FILE* savelex;

YYSTYPE yylval;

struct in_file in_files[NOOFFILES];

STRING s;
STRING t;
STRING largest;

char buffer[500];
