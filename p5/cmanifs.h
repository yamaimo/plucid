#pragma once

#define BUFFERLENGTH  200
#define NOOFFILES      12

#define F_ERES      0
#define F_VAR       1
#define F_INPUT     2
#define F_LOCAL     3
#define F_WORD      4
#define F_FCALL     5
#define F_SWCHAR    6
#define F_CONST     7
#define F_NIL       8
#define F_CXFILE    9
#define F_SCONS    10

typedef int FUNCTION;

typedef char* STRING;
typedef struct EXPR* EXPRPTR;

typedef union X_OR_I {
    int     i;
    float   r;
    EXPRPTR x;
} X_OR_I;

typedef struct EXPR {
    FUNCTION    f;
    int         dim;
    X_OR_I      arg1;
    X_OR_I      arg2;
    X_OR_I      arg3;
    X_OR_I      arg4;
    X_OR_I      arg5;
} EXPR;

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

typedef struct {
    STRING  name;
    int     nargs;
    int     type;
} FITEM;

typedef FITEM* FPTR;

#define U_EGLOBAL   0
#define U_NGLOBAL   1
#define U_FORMAL    2
#define U_NORMAL    3

#define NULLARY     0
#define NONNULLARY  1

/*
 * various bits of information are stored in the symbol table
 * entries for variable names. these are all stored in the word
 * attr, and this is what they mean:
 *
 * ELEM_A - variable is inherited as an elementary variable
 *         in the present scope.
 * FORM_A - variable is ultimately defined as a formal
 *         parameter. (even though it may be inherited into
 *         the present scope as a global)
 * GLOB_A - variable has been inherited into this scope
 *         as a global variable.
 *         a global can be an ordinary global (NGLOBAL)
 *         or an elemntary global (EGLOBAL) in which case,
 *         the ELEM_A flag is also set.
 * LOC_A - variable is ultimately defined as a local. note that
 *         GLOB_A may still be on, if it is inherited as a global
 *         in this scope. thus, to see if a variable is locally
 *         defined in the present scope, one must see that GLOB_A
 *         and FORM_A are not set. checking for LOC_A is not correct.
 */
#define NONE_A  0000
#define ELEM_A  0001
#define FORM_A  0002
#define GLOB_A  0010
#define LOC_A   0020
#define DEF_A   0040
#define USED_A  0100
#define UELEM_A 0200

/*
 * various information getters about symbols.
 * note that IS_LOCAL tests to see if x is declared as a LOCAL
 * in the present state, NOT to see if it is local anywhere else.
 */
#define IS_USED(x)      (x->attr & USED_A)
#define IS_FORM(x)      (x->attr & FORM_A)
#define IS_GLOB(x)      (x->attr & GLOB_A)
#define IS_LOCAL(x)     ((!IS_FORM(x)) && (!IS_GLOB(x)))
#define IS_ELEM(x)      (x->attr & ELEM_A)
#define IS_DEF(x)       (x->attr & DEF_A)
#define ZERO_GLOB(x)    (x->attr & ~GLOB_A)

typedef struct SYMBOL* SYMPTR;

typedef struct SYMBOL {
    STRING  name;
    int     lexlevel;
    int     fldef;
    int     elemdef;
    int     idno;   /* idno name for variable */
    char    attr;   /* various information about variable usage */
    SYMPTR  next;   /* the next symbol in hash chain */
} SYMBOL;

typedef struct F_STACK* F_STACKPTR;
typedef struct P_STACK* P_STACKPTR;
typedef struct E_STACK* E_STACKPTR;
typedef struct E_LIST*  E_LISTPTR;

typedef struct F_STACK {
    int         f_ll;
    int         f_el;
    F_STACKPTR  f_tl;
} F_STACK;

typedef struct P_STACK {
    int         p_idno;
    int         p_iselem;
    P_STACKPTR  p_tl;
} P_STACK;

typedef struct E_STACK {
    E_LISTPTR   es_list;
    E_STACKPTR  es_tl;
} E_STACK;

typedef struct E_LIST {
    EXPRPTR     el_expr;
    E_LISTPTR   el_tl;
} E_LIST;

typedef union stacktype {
    int     value;  /* for constants returned by yylex */
    float   numb;
    SYMPTR  ident;  /* for symbol table entries returned by yylex */
    EXPRPTR eptr;   /* for expressions pointers returned by yacc actions */
} YYSTYPE;
#define YYSTYPE_IS_DECLARED 1
#include "y.tab.h"
