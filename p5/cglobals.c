#include "cglobals.h"

int filecount;
int l;
int c;
int peekc;
int errcount;
int in_index;
int true_;
int false_;

int cconst;
int wordval;

char linebuf[BUFFERLENGTH];

FILE* lexin;
FILE* outfile;
FILE* savelex;

YYSTYPE yylval;

struct in_file in_files[NOOFFILES];

STRING s;
STRING t;

char buffer[500];

F_STACKPTR f_stack;
P_STACKPTR p_stack;
E_STACKPTR parm_stack;
E_LISTPTR  parm_list;

int lexlevel;
int funclevel;
int elemlevel;
int identcount;
int formalcount;
int idusage;    /* is ident being declared, used, or what? */

SYMPTR hashtable[HASHSIZE + 1];

STRING stringtable[MAXSTRINGS];
STRING wordtable[MAXSTRINGS];
int stringcount;
int wordcount;

EXPRPTR exprtable[MAXVARS]; /* the expressions trees for each variable */
STRING  nametable[MAXVARS]; /* the user names for each variable */
EXPRPTR filetable[MAXFILES];

FITEM ftable[MAXFUNCS] = {
    {"eres",        1, TERMINAL }, /*   0 */
    {"var",         3, TERMINAL }, /*   1 */
    {"input",       1, TERMINAL }, /*   2 */
    {"local",       1, TERMINAL }, /*   3 */
    {"word",        1, TERMINAL }, /*   4 */
    {"fcall",       4, TERMINAL }, /*   5 */
    {"swchar",      1, TERMINAL }, /*   6 */
    {"const",       1, CONST    }, /*   7 */
    {"nil",         1, SPL      }, /*   8 */
    {"cxfile",      4, CXFILE   }, /*   9 */
    {"scons",       3, INTERIOR }, /*  10 */
    {"wrd",         0, WRD      }, /*  11 */
    {"valof",       0, VALOF    }, /*  12 */
    {"decl",        0, DECL     }, /*  13 */
    {"defn",        0, DEFN     }, /*  14 */
    {"strg",        0, STRG     }, /*  15 */
    {"numb",        0, NMB      }, /*  16 */
    {"nullry",      0, NULLRY   }, /*  17 */
    {"nonnullry",   0, NONNULLRY}, /*  18 */
    {"constant",    0, CONST    }, /*  19 */
    {"eglobal",     0, EGLOB    }, /*  20 */
    {"nglobal",     0, NGLOB    }, /*  21 */
    {"frmls",       0, FRMLS    }, /*  22 */
    {"op",          0, OP       }, /*  23 */
    {"first",       2, INTERIOR }, /*  24 */
    {"next",        2, INTERIOR }, /*  25 */
    {"pred",        2, INTERIOR }, /*  26 */
    {"prev",        2, INTERIOR }, /*  27 */
    {"fby",         3, INTERIOR }, /*  28 */
    {"before",      3, INTERIOR }, /*  29 */
    {"asa",         3, INTERIOR }, /*  30 */
    {"now",         1, INTERIOR }, /*  31 */
    {"@t",          3, INTERIOR }, /*  32 */
    {"initial",     2, INTERIOR }, /*  33 */
    {"succ",        2, INTERIOR }, /*  34 */
    {"sby",         3, INTERIOR }, /*  35 */
    {"whr",         3, INTERIOR }, /*  36 */
    {"here",        1, INTERIOR }, /*  37 */
    {"@s",          3, INTERIOR }, /*  38 */
    {"original",    2, INTERIOR }, /*  39 */
    {"noriginal",   2, INTERIOR }, /*  40 */
    {"nrest",       2, INTERIOR }, /*  41 */
    {"rest",        2, INTERIOR }, /*  42 */
    {"aby",         3, INTERIOR }, /*  43 */
    {"cby",         3, INTERIOR }, /*  44 */
    {"swap",        3, INTERIOR }, /*  45 */
    {"rshift",      2, INTERIOR }, /*  46 */
    {"lshift",      2, INTERIOR }, /*  47 */
    {"all",         2, INTERIOR }, /*  48 */
    {"elt",         2, INTERIOR }, /*  49 */
    {"isnumber",    2, INTERIOR }, /*  50 */
    {"div",         3, INTERIOR }, /*  51 */
    {"mod",         3, INTERIOR }, /*  52 */
    {"and",         3, INTERIOR }, /*  53 */
    {"not",         2, INTERIOR }, /*  54 */
    {"or",          3, INTERIOR }, /*  55 */
    {"eq",          3, INTERIOR }, /*  56 */
    {"ne",          3, INTERIOR }, /*  57 */
    {"gt",          3, INTERIOR }, /*  58 */
    {"ge",          3, INTERIOR }, /*  59 */
    {"le",          3, INTERIOR }, /*  60 */
    {"lt",          3, INTERIOR }, /*  61 */
    {"sin",         2, INTERIOR }, /*  62 */
    {"cos",         2, INTERIOR }, /*  63 */
    {"log",         2, INTERIOR }, /*  64 */
    {"iseod",       2, INTERIOR }, /*  65 */
    {"isstring",    2, INTERIOR }, /*  66 */
    {"isword",      2, INTERIOR }, /*  67 */
    {"substr",      4, INTERIOR }, /*  68 */
    {"eod",         1, SPL      }, /*  69 */
    {"arg",         2, INTERIOR }, /*  70 */
    {"mkword",      2, INTERIOR }, /*  71 */
    {"mkstring",    2, INTERIOR }, /*  72 */
    {"error",       1, SPL      }, /*  73 */
    {"iserror",     2, INTERIOR }, /*  74 */
    {"length",      2, INTERIOR }, /*  75 */
    {"mknumber",    2, INTERIOR }, /*  76 */
    {"tan",         2, INTERIOR }, /*  77 */
    {"log10",       2, INTERIOR }, /*  78 */
    {"abs",         2, INTERIOR }, /*  79 */
    {"sqrt",        2, INTERIOR }, /*  80 */
    {"filter",      4, INTERIOR }, /*  81 */
    {"cons",        3, INTERIOR }, /*  82 */
    {"islist",      2, INTERIOR }, /*  83 */
    {"isatom",      2, INTERIOR }, /*  84 */
    {"hd",          2, INTERIOR }, /*  85 */
    {"tl",          2, INTERIOR }, /*  86 */
    {"ord",         2, INTERIOR }, /*  87 */
    {"chr",         2, INTERIOR }, /*  88 */
    {"isnil",       2, INTERIOR }, /*  89 */
    {"complex",     2, INTERIOR }, /*  90 */
    {"real",        2, INTERIOR }, /*  91 */
    {"imag",        2, INTERIOR }, /*  92 */
    {"uminus",      2, INTERIOR }, /*  93 */
    {"plus",        3, INTERIOR }, /*  94 */
    {"minus",       3, INTERIOR }, /*  95 */
    {"times",       3, INTERIOR }, /*  96 */
    {"if",          4, INTERIOR }, /*  97 */
    {"fdiv",        3, INTERIOR }, /*  98 */
    {"strconc",     3, INTERIOR }, /*  99 */
    {"append",      3, INTERIOR }, /* 100 */
    {"arg",         2, INTERIOR }, /* 101 */
    {"exp",         3, INTERIOR }, /* 102 */
    {"spl",         0, INTERIOR }, /* 103 */
};
