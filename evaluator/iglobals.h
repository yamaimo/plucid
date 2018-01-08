#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "imanifs.h"

extern int false_;
extern int true_;

#define copy(x,y) x = y


/*
 * io stuff, and trace flags
 */
#define MAXWORDS        400
#define MAXSTRINGS      400
#define MAXOPENFILES    20
#define MAXVARS         1000
#define AVGWRDLEN       100
#define MAXFILTERSPACE  400
#define TRACEQUAN       100

extern char* infilename;
extern FILE* infile;
extern FILE* current_out;
extern FILE* current_in;

extern int s_count;
extern int b_count;
extern int filterlevel;
extern int active_filters;

extern int dc1;
extern int dc2;
extern int dc3;
extern int c1;
extern int c2;
extern int c3;

extern int brecycle;
extern int srecycle;
extern int varcount;
extern int stpcount;
extern int memcount;

extern int eodch;
extern int current_var;

extern char evalerr;
extern char newout;

extern char fnl;
extern char sflag;
extern char cflag;
extern char iflag;
extern char fflag;
extern char nflag;
extern char dflag;
extern char pflag;
extern char tflags[TRACEQUAN];

extern char filterp[30];

extern int errcount;
extern int notfoundcount;

extern int open_file;
extern int file_var;
extern int current_strg;
extern int u_countvec[MAXVARS];
extern int retvec[MAXVARS];

extern int b_recquan;
extern int s_recquan;
extern int ddepth;
extern int current_inx;
extern int current_outx;

extern int curr_in_mode;
extern int curr_out_mode;
extern int curr_point;
extern int curr_ign;
extern int conc;

extern IOPTR io_info[MAXVARS];
extern IOPTR current_io;

extern EXPRPTR filter_e[100];

#define STPHASHSIZE 1113
#define PHASHSIZE 1113
#define THASHSIZE 1113
#define NHASHSIZE 1113
#define SHASHSIZE 1113
extern STPPTR phashtab[PHASHSIZE];
extern STPPTR thashtab[THASHSIZE];
extern STPPTR shashtab[SHASHSIZE];
extern MEMPTR nhashtab[NHASHSIZE];

extern int memargc;
extern char** memargv;
extern STPPTR outside; /* the time in the outside environment */


/*
 * run time stacks:
 * - v_stack:   value stack
 * - d_stack:   display stack
 * - stp_stack: space-time-place stack
 */

#define VSIZE   500
#define VStop   v_top
#define VSpush  do {                                \
        if (v_top + 1 >= &v_stack[0] + 2 * VSIZE)   \
        {                                           \
            printf("v_stack overflow\n");           \
            exit(1);                                \
        }                                           \
        v_top++;                                    \
    } while (0)
#define VSpop   v_top--
#define VStype  v_top->v_type
#define VSvalue v_top->v_value
extern VALUE v_stack[VSIZE];
extern VALUEPTR v_top;

#define DSIZE   2000
extern DISPLAYITEM d_stack[DSIZE];
extern DISPLAYPTR d_top;

#define STPSIZE  6000
#define STPStop  stp_top
#define STPSpush do {                                   \
        if (stp_top + 1 > &stp_stack[0] + 2 * STPSIZE)  \
        {                                               \
            printf("stp_stack overflow\n");             \
            my_exit(1);                                 \
        }                                               \
        stp_top++;                                      \
    } while (0)
#define STPSpop  stp_top--
#define STPSt    stp_top->stp_t
#define STPSp    stp_top->stp_p
#define STPSs    stp_top->stp_s
#define STPSpushval(s,t,p)  do {    \
        STPSpush;                   \
        STPSs = (s);                \
        STPSt = (t);                \
        STPSp = (p);                \
    } while (0)
extern STPPAIR stp_stack[STPSIZE];
extern STPPAIR* stp_top;

extern unsigned int filequan;
extern unsigned int exprquan;
extern unsigned int wordquan;

extern STRING* nametable;
extern STRING* wordtable;

extern EXPRPTR* filetable;
extern EXPRPTR* exprtable;

extern char* ngc_allocbuf;
extern char* ngc_allocp;

extern int ngc_allocsize;

/* STPPTR safe;  safe is used to hold a time in case */
extern int dynasizes[2];
extern int pid;
extern int ch;


/* function table */
#define MAXFUNCS    105
extern FITEM ftable[MAXFUNCS];
