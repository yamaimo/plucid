#pragma once

#include "cmanifs.h"

void pass1(EXPRPTR e);
//EXPRPTR stripstring(char* s);
float stripcode(char* s);
float stripn(char* s);
EXPRPTR stripnumb(char* s);
STRING stripname(char* s);

EXPRPTR filenode(int first_line, int cursor_position);
EXPRPTR connode(char* s1, char* s2);
//EXPRPTR lu_connode(STRING s, float f);
EXPRPTR f_connode(float f);
EXPRPTR varnode(char* name, int argcount, EXPRPTR exprlist, EXPRPTR file);
EXPRPTR opnode(char* name, int argcount, EXPRPTR exprlist, EXPRPTR file);
EXPRPTR wherenode(EXPRPTR expr, EXPRPTR exprlist, EXPRPTR file);
EXPRPTR defnode(char* name, int argcount, EXPRPTR argnames, EXPRPTR expr, EXPRPTR file);
EXPRPTR declnode(char* name, EXPRPTR expr, EXPRPTR file);
EXPRPTR identlistnode(EXPRPTR tail, char* name);
EXPRPTR listnode(EXPRPTR tail, EXPRPTR expr);
EXPRPTR bodynode(EXPRPTR expr, EXPRPTR tail);
EXPRPTR exprlist2(EXPRPTR expr1, EXPRPTR expr2);
EXPRPTR exprlist3(EXPRPTR expr1, EXPRPTR expr2, EXPRPTR expr3);

int yyerror(STRING a);
int my_yyerror(STRING a, STRING b);
