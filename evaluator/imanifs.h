#pragma once

#include <stdio.h>
//#include <signal.h>
//#include <ctype.h>
//#include <sgtty.h>

#define POP 999
#define UNIX 1000
#define CHILD 0
#define SYSERR -1
#define READ_END 0
#define WRITE_END 1
#define CHILD_IN 0
#define CHILD_OUT 1
#define IO_RECORD 11
#define IOFILTER  100
#define IFILE     101
#define INPUT     102
#define CLOSEDSTREAM -20

#define F_FILE 9
#define F_CONST 7
#define F_FILTER 81
#define F_INPUT 2
#define F_PLUS 94
#define F_MINUS 95
#define F_TIMES 96
#define F_DIV 51
#define F_MOD 52
#define F_RDIV 98
#define F_LE 60
#define F_GT 58
#define F_LT 61
#define F_GE 59

#define swap_t0s0   0
#define swap_t0s1   1
#define swap_t0s2   2
#define swap_t1s1   3
#define swap_t2s3   4
#define swap_t0t1   5
#define swap_t0t2   6
#define swap_t0t3   7
#define swap_t1t2   8
#define swap_t1t3   9
#define swap_t2t3  10
#define swap_s0s1  11
#define swap_s0s2  12
#define swap_s0s3  13
#define swap_s1s2  14
#define swap_s1s3  15
#define swap_s2s3  16

typedef char FUNCTION;
typedef char* STRING;

typedef struct EXPR* EXPRPTR;

typedef struct COMPLEX {
    float r;
    float i;
} COMPLEX;

typedef union X_OR_I {
    int     i;
    COMPLEX c;
    EXPRPTR x;
} X_OR_I;

typedef struct U_EXPR {
    FUNCTION    f;
    int         dim;
    X_OR_I      arg1;
} U_EXPR;

typedef struct B_EXPR {
    FUNCTION    f;
    int         dim;
    X_OR_I      arg1;
    X_OR_I      arg2;
} B_EXPR;

typedef struct T_EXPR {
    FUNCTION    f;
    int         dim;
    X_OR_I      arg1;
    X_OR_I      arg2;
    X_OR_I      arg3;
} T_EXPR;

typedef struct Q_EXPR {
    FUNCTION    f;
    int         dim;
    X_OR_I      arg1;
    X_OR_I      arg2;
    X_OR_I      arg3;
    X_OR_I      arg4;
} Q_EXPR;

typedef struct QU_EXPR {
    FUNCTION    f;
    int         dim;
    X_OR_I      arg1;
    X_OR_I      arg2;
    X_OR_I      arg3;
    X_OR_I      arg4;
    X_OR_I      arg5;
} QU_EXPR;

typedef struct EXPR {
    FUNCTION    f;
    int         dim;
    X_OR_I      arg1;
    X_OR_I      arg2;
    X_OR_I      arg3;
    X_OR_I      arg4;
    X_OR_I      arg5;
} EXPR;

typedef struct {
    STRING  name;
    char    nargs;
    void (*apply)(EXPRPTR);
    char    type;
} FITEM;
typedef FITEM* FPTR;

#define INTERIOR    0
#define TERMINAL    1
#define D(x)    if (tflags[x])
#define E(x)    if (!tflags[x])
#define eval(x) (*ftable[(long)e->x->f].apply)(e->x)

/*
 *      possible values
 */

// v_type
#define UNDEFINED               0
#define DOTTED_PAIR             1
#define WORD                    2
#define SWCHAR                  2
#define NUMERIC                 3
#define QSTRING                 4
#define EOD                     5
#define NIL                     6
#define ERROR                   7       /* now not necc. error is type WORD */

#define SMALL_RECORD            0
#define STRING_RECORD           2
#define OTHER_RECORD            1
#define BIG_RECORD              OTHER_RECORD
#define MAXERRORS               20
#define BYTESPERWORD            4

typedef long   LU_WORD;
typedef long   LU_EOD;
typedef long   LU_ERROR;
typedef long   LU_NIL;

typedef struct D_TAGSTYPE
{
    char    u_count;
    char    g_mark;
    short   n;
    char    d_hd;
    char    d_tl;
    char    d_size;
    char    d_mark;
} D_TAGS;

typedef struct COORDTYPE
{
    short d1;
    short d2;
    short d3;
} COORDS;

typedef union BITSTYPE
{
    COORDS  xyz;
    long    word;
    D_TAGS  bits;
} BITS;

typedef struct NONAME
{
    BITS data;
    char* f_next;
} NAMELESS;

typedef union SPACETYPE
{
    char*       sp;
    NAMELESS*   no;
} SPACEITEM;

typedef struct CELLTYPE*        CELLPTR;
typedef struct VALUETYPE*       VALUEPTR;
typedef struct STPSTACKTYPE*    STPPTR;
typedef struct MEMITEMTYPE*     MEMPTR;
typedef struct STPLISTTYPE*     STPLISTPTR;
typedef struct DISPLAYITEMTYPE* DISPLAYPTR;
typedef struct IOTYPE*          IOPTR;

typedef union CELLUNIONTYPE
{
    char        swch;   /* word & strings */
    COMPLEX     na;     /* numeric atom */
    LU_WORD     wrd;    /* word atom */
    CELLPTR     strg;   /* string atom */
    LU_EOD      eod;    /* end of data marker */
    LU_ERROR    err;    /* type error */
    LU_NIL      nil;    /* the empty list */
    CELLPTR     dp;
} CELLUNION, WORDCELL;

typedef struct CELLTYPE
{
    BITS        data;
    CELLUNION   hd;
    CELLUNION   tl;
} CELL;

typedef struct VALUETYPE
{
//    long     v_type;
    char     v_type;    // maybe char (UNDEFINED, ... , ERROR)
    WORDCELL v_value;
} VALUE;

typedef struct STPSTACKTYPE
{
    BITS    stphd;
    STPPTR  stptl;
    STPPTR  stplink;
} STPSTACK;

typedef struct MEMITEMTYPE
{
    BITS    data;
    STPPTR  s;
    STPPTR  t;
    STPPTR  p;
    VALUE   v;
    MEMPTR  m_next;
} MEMITEM;

typedef struct STPLISTTYPE
{
    BITS        data;
    STPPTR      hd;
    STPLISTPTR  tl;
} STPLIST;

typedef struct DISPLAYITEMTYPE
{
    DISPLAYPTR  d_envg;
    DISPLAYPTR  d_envf;
    STPPTR      d_sg;
    STPPTR      d_sf;
    STPPTR      d_tg;
    STPPTR      d_tf;
    STPPTR      d_pg;
    STPPTR      d_pf;
    long        d_frozen;
} DISPLAYITEM;

typedef struct STPPAIRTYPE
{
    STPPTR stp_s;
    STPPTR stp_t;
    STPPTR stp_p;
} STPPAIR;

typedef struct IOTYPE
{
    long    type;
    EXPRPTR expr;
    STPPTR  s;
    STPPTR  t;
    STPPTR  p;
    FILE*   in;
    FILE*   out;
    long    chin;
    long    chout;
    long    inx;
    long    outx;
    int     inmode;
    int     outmode;
    int     ptwize;
    int     ignore;
    IOPTR   next;
} IO;
