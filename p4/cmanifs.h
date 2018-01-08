#pragma once

#define BUFFERLENGTH  200
#define NOOFFILES      12

#define F_CONST         1
#define F_VAR           2
#define F_OP            3
#define F_WHERE         4
#define F_DEFN          5
#define F_DECL          6
#define F_IDENTLISTNODE 7
#define F_LISTNODE      8
#define F_FILE          9
#define F_VALOF        10
#define F_EGLOBALS     11
#define F_NGLOBALS     12
#define F_EVALOF       13

typedef char* STRING;
typedef struct EXPR* EXPRPTR;

typedef union X_OR_I {
    int     i;
    float   r;
    STRING  s;
    EXPRPTR x;
} X_OR_I;

typedef struct EXPR {
    int     f;
    X_OR_I  arg1;
    X_OR_I  arg2;
    X_OR_I  arg3;
    X_OR_I  arg4;
    X_OR_I  arg5;
} EXPR;

typedef struct EXPR2 {
    int     f;
    X_OR_I  arg1;
    X_OR_I  arg2;
} EXPR2;

typedef struct EXPR3 {
    int     f;
    X_OR_I  arg1;
    X_OR_I  arg2;
    X_OR_I  arg3;
} EXPR3;

typedef struct EXPR4 {
    int     f;
    X_OR_I  arg1;
    X_OR_I  arg2;
    X_OR_I  arg3;
    X_OR_I  arg4;
} EXPR4;

typedef struct EXPR5 {
    int     f;
    X_OR_I  arg1;
    X_OR_I  arg2;
    X_OR_I  arg3;
    X_OR_I  arg4;
    X_OR_I  arg5;
} EXPR5;

typedef struct FFTYPE* FFPTR;

typedef struct FFTYPE {
    STRING  name;
    EXPRPTR rhs;
    FFPTR   next_d;
} FFITEM;

typedef union stacktype {
    float   numb;   /* for numbers returned by yylex */
    STRING  strg;   /* for symbol table entries returned by yylex */
    EXPRPTR eptr;   /* for expressions pointers returned by yacc actions */
} YYSTYPE;
#define YYSTYPE_IS_DECLARED 1
#include "y.tab.h"
