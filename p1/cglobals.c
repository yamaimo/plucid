#include "cglobals.h"

STRING  incdir;

EXPRPTR expval[NESTINGDEPTH];
int     expvalcnt;

int     exprlevel;
int     exprlistcount[NESTINGDEPTH];

int     c;
int     peekc;

int     newline;

int     startoflex;
int     curr_index;
int     curr_length;
int     in_index;

int     const_list;

int     errcount;
int     false_;
int     true_;

int     l;

int     idcount;

int     linebuf[BUFFERLENGTH];

FILE*   lexin;

YYSTYPE yylval;

struct in_file  in_files[NOOFFILES];

STRING  s;
STRING  t;

char    buffer[2000];
