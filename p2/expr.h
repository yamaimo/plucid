#pragma once

#include "cmanifs.h"

void pass2(EXPRPTR e);
EXPRPTR filenode(char* filename, int first_line, int last_line, int cursor_position);
EXPRPTR connode(char* s1, char* s2);
EXPRPTR f_connode(float n);
EXPRPTR varnode(char* name, int argcount, EXPRPTR exprlist, EXPRPTR file);
EXPRPTR opnode(char* name, int argcount, EXPRPTR exprlist, EXPRPTR file);
EXPRPTR wherenode(EXPRPTR expr, EXPRPTR exprlist, EXPRPTR file);
EXPRPTR defnode(char* name, int argcount, EXPRPTR argnames, EXPRPTR expr, EXPRPTR file);
EXPRPTR declnode(char* name, EXPRPTR expr, EXPRPTR file);
EXPRPTR identlistnode(EXPRPTR tail, char* name);
EXPRPTR listnode(EXPRPTR tail, EXPRPTR expr);
//EXPRPTR exprlist2(EXPRPTR expr1, EXPRPTR expr2);

int yyerror(STRING a);
