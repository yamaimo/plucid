#include <stdio.h>

#include "imanifs.h"
#include "iglobals.h"

#include "string.h"
#include "list.h"
#include "input.h"
#include "real.h"
#include "f_alg.h"
#include "f_flucid.h"
#include "f_lucid.h"
#include "f_vars.h"

int false_;
int true_;

char* infilename;
FILE* infile;
FILE* current_out;
FILE* current_in;

int s_count;
int b_count;
int filterlevel;
int active_filters;

int dc1;
int dc2;
int dc3;
int c1;
int c2;
int c3;

int brecycle;
int srecycle;
int varcount;
int stpcount;
int memcount;

int eodch;
int current_var;

char evalerr;
char newout;

char fnl;
char sflag;
char cflag;
char iflag;
char fflag;
char nflag;
char dflag;
char pflag;
char tflags[TRACEQUAN];

char filterp[30];

int errcount;
int notfoundcount;

int open_file;
int file_var;
int current_strg;
int u_countvec[MAXVARS];
int retvec[MAXVARS];

int b_recquan;
int s_recquan;
int ddepth;
int current_inx;
int current_outx;

int curr_in_mode;
int curr_out_mode;
int curr_point;
int curr_ign;
int conc;

IOPTR io_info[MAXVARS];
IOPTR current_io;

EXPRPTR filter_e[100];

STPPTR phashtab[PHASHSIZE];
STPPTR thashtab[THASHSIZE];
STPPTR shashtab[SHASHSIZE];
MEMPTR nhashtab[NHASHSIZE];

int memargc;
char** memargv;
STPPTR outside; /* the time in the outside environment */


/*
 * run time stacks:
 * - v_stack:   value stack
 * - d_stack:   display stack
 * - stp_stack: space-time-place stack
 */

VALUE v_stack[VSIZE];
VALUEPTR v_top;

DISPLAYITEM d_stack[DSIZE];
DISPLAYPTR d_top;

STPPAIR stp_stack[STPSIZE];
STPPAIR* stp_top;

unsigned int filequan;
unsigned int exprquan;
unsigned int wordquan;

STRING* nametable;
STRING* wordtable;

EXPRPTR* filetable;
EXPRPTR* exprtable;

char* ngc_allocbuf;
char* ngc_allocp;

int ngc_allocsize;

/* STPPTR safe;  safe is used to hold a time in case */
int dynasizes[2];
int pid;
int ch;

/* function table */
FITEM ftable[MAXFUNCS] = {
    {"eres",        1,  f_eres,      TERMINAL}, /*   0 */
    {"var",         3,  f_var,       TERMINAL}, /*   1 */
    {"input",       1,  f_input,     TERMINAL}, /*   2 */
    {"local",       1,  f_local,     TERMINAL}, /*   3 */
    {"word",        1,  f_word,      TERMINAL}, /*   4 */
    {"fcall",       4,  f_fcall,     TERMINAL}, /*   5 */
    {"swchar",      1,  f_swchar,    TERMINAL}, /*   6 */
    {"const",       1,  f_const,     TERMINAL}, /*   7 */
    {"nil",         1,  f_nil,       TERMINAL}, /*   8 */
    {"cxfile",      4,  f_cxfile,    TERMINAL}, /*   9 */
    {"scons",       3,  f_scons,     INTERIOR}, /*  10 */
    {"wrd",         1,  f_error,     TERMINAL}, /*  11 */
    {"valof",       1,  f_error,     TERMINAL}, /*  12 */
    {"decl",        1,  f_error,     TERMINAL}, /*  13 */
    {"defn",        1,  f_error,     TERMINAL}, /*  14 */
    {"strg",        1,  f_error,     TERMINAL}, /*  15 */
    {"numb",        1,  f_error,     TERMINAL}, /*  16 */
    {"nullry",      1,  f_error,     TERMINAL}, /*  17 */
    {"nonnullry",   1,  f_error,     TERMINAL}, /*  18 */
    {"constant",    1,  f_error,     TERMINAL}, /*  19 */
    {"eglobal",     1,  f_error,     TERMINAL}, /*  20 */
    {"nglobal",     1,  f_error,     TERMINAL}, /*  21 */
    {"frmls",       1,  f_error,     TERMINAL}, /*  22 */
    {"op",          1,  f_error,     TERMINAL}, /*  23 */
    {"first",       2,  f_first,     INTERIOR}, /*  24 */
    {"next",        2,  f_next,      INTERIOR}, /*  25 */
    {"pred",        2,  f_pred,      INTERIOR}, /*  26 */
    {"prev",        2,  f_prev,      INTERIOR}, /*  27 */
    {"fby",         3,  f_fby,       INTERIOR}, /*  28 */
    {"before",      3,  f_before,    INTERIOR}, /*  29 */
    {"asa",         3,  f_asa,       INTERIOR}, /*  30 */
    {"now",         1,  f_now,       INTERIOR}, /*  31 */
    {"@t",          3,  f_attime,    INTERIOR}, /*  32 */
    {"initial",     2,  f_initial,   INTERIOR}, /*  33 */
    {"succ",        2,  f_succ,      INTERIOR}, /*  34 */
    {"sby",         3,  f_sby,       INTERIOR}, /*  35 */
    {"whr",         3,  f_whr,       INTERIOR}, /*  36 */
    {"here",        1,  f_here,      INTERIOR}, /*  37 */
    {"@s",          3,  f_atspace,   INTERIOR}, /*  38 */
    {"original",    2,  f_original,  INTERIOR}, /*  39 */
    {"noriginal",   2,  f_noriginal, INTERIOR}, /*  40 */
    {"nrest",       2,  f_nrest,     INTERIOR}, /*  41 */
    {"rest",        2,  f_rest,      INTERIOR}, /*  42 */
    {"aby",         3,  f_aby,       INTERIOR}, /*  43 */
    {"cby",         3,  f_cby,       INTERIOR}, /*  44 */
    {"swap",        3,  f_swap,      INTERIOR}, /*  45 */
    {"rshift",      2,  f_rshift,    INTERIOR}, /*  46 */
    {"lshift",      2,  f_lshift,    INTERIOR}, /*  47 */
    {"all",         2,  f_all,       INTERIOR}, /*  48 */
    {"elt",         2,  f_elt,       INTERIOR}, /*  49 */
    {"isnumber",    2,  f_isnumber,  INTERIOR}, /*  50 */
    {"div",         3,  f_num2,      INTERIOR}, /*  51 */
    {"mod",         3,  f_num2,      INTERIOR}, /*  52 */
    {"and",         3,  f_and,       INTERIOR}, /*  53 */
    {"not",         2,  f_not,       INTERIOR}, /*  54 */
    {"or",          3,  f_or,        INTERIOR}, /*  55 */
    {"eq",          3,  f_eq,        INTERIOR}, /*  56 */
    {"ne",          3,  f_ne,        INTERIOR}, /*  57 */
    {"gt",          3,  f_log2,      INTERIOR}, /*  58 */
    {"ge",          3,  f_log2,      INTERIOR}, /*  59 */
    {"le",          3,  f_log2,      INTERIOR}, /*  60 */
    {"lt",          3,  f_log2,      INTERIOR}, /*  61 */
    {"sin",         2,  f_sin,       INTERIOR}, /*  62 */
    {"cos",         2,  f_sin,       INTERIOR}, /*  63 */
    {"log",         2,  f_log,       INTERIOR}, /*  64 */
    {"iseod",       2,  f_iseod,     INTERIOR}, /*  65 */
    {"isstring",    2,  f_isstring,  INTERIOR}, /*  66 */
    {"isword",      2,  f_isword,    INTERIOR}, /*  67 */
    {"substr",      4,  f_substr,    INTERIOR}, /*  68 */
    {"eod",         1,  f_eod,       TERMINAL}, /*  69 */
    {"arg",         2,  f_arg,       INTERIOR}, /*  70 */
    {"mkword",      2,  f_mkword,    INTERIOR}, /*  71 */
    {"mkstring",    2,  f_mkstring,  INTERIOR}, /*  72 */
    {"error",       1,  f_error,     TERMINAL}, /*  73 */
    {"iserror",     2,  f_iserror,   INTERIOR}, /*  74 */
    {"length",      2,  f_length,    INTERIOR}, /*  75 */
    {"mknumber",    2,  f_mknumber,  INTERIOR}, /*  76 */
    {"tan",         2,  f_tan,       INTERIOR}, /*  77 */
    {"log10",       2,  f_log10,     INTERIOR}, /*  78 */
    {"abs",         2,  f_abs,       INTERIOR}, /*  79 */
    {"sqrt",        2,  f_sqrt,      INTERIOR}, /*  80 */
    {"filter",      4,  f_filter,    INTERIOR}, /*  81 */
    {"cons",        3,  f_cons,      INTERIOR}, /*  82 */
    {"islist",      2,  f_islist,    INTERIOR}, /*  83 */
    {"isatom",      2,  f_isatom,    INTERIOR}, /*  84 */
    {"hd",          2,  f_hd,        INTERIOR}, /*  85 */
    {"tl",          2,  f_tl,        INTERIOR}, /*  86 */
    {"ord",         2,  f_ord,       INTERIOR}, /*  87 */
    {"chr",         2,  f_chr,       INTERIOR}, /*  88 */
    {"isnil",       2,  f_isnil,     INTERIOR}, /*  89 */
    {"complex",     2,  f_complex,   INTERIOR}, /*  90 */
    {"real",        2,  f_real,      INTERIOR}, /*  91 */
    {"imag",        2,  f_imag,      INTERIOR}, /*  92 */
    {"uminus",      2,  f_uminus,    INTERIOR}, /*  93 */
    {"plus",        3,  f_num2,      INTERIOR}, /*  94 */
    {"minus",       3,  f_num2,      INTERIOR}, /*  95 */
    {"times",       3,  f_num2,      INTERIOR}, /*  96 */
    {"if",          4,  f_if,        INTERIOR}, /*  97 */
    {"fdiv",        3,  f_num2,      INTERIOR}, /*  98 */
    {"strconc",     3,  f_strconc,   INTERIOR}, /*  99 */
    {"append",      3,  f_append,    INTERIOR}, /* 100 */
    {"arg",         2,  f_arg,       INTERIOR}, /* 101 */
    {"exp",         3,  f_exp,       INTERIOR}, /* 102 */
};
