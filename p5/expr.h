#pragma once

#include "cmanifs.h"

EXPRPTR filenode(int filename, int first_line, int last_line, int cursor_position);
EXPRPTR connode(int f, int n);
EXPRPTR f_connode(float n);
EXPRPTR varnode(int type, SYMPTR v);

EXPRPTR sconsnode(char* s, EXPRPTR file);
EXPRPTR unode(int f, int n, EXPRPTR a, EXPRPTR file);
EXPRPTR binnode(int f, int n, EXPRPTR a, EXPRPTR b, EXPRPTR file);
EXPRPTR ternode(int f, int n, EXPRPTR b, EXPRPTR c, EXPRPTR d, EXPRPTR file);

void pushexpr(void);
void appeexpr(EXPRPTR e);
void define(SYMPTR s, EXPRPTR e);

int yyerror(STRING a);
int my_yyerror(STRING a, STRING b);
