#include "cglobals.h"

int wvr_upon_count;

int l;
int c;
int peekc;
int errcount;
int in_index;
int true_;
int false_;

int cconst;

char* largest;
char linebuf[BUFFERLENGTH];

FILE* lexin;
FILE* outfile;
FILE* savelex;

YYSTYPE yylval;

struct in_file in_files[NOOFFILES];

STRING s;
STRING t;

char buffer[500];
