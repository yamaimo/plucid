#pragma once

#include "cmanifs.h"

EXPRPTR filenode(char* filename, int first_line, int last_line, int cursor_position);
EXPRPTR connode(char* s1, char* s2);
EXPRPTR f_connode(float n);
EXPRPTR varnode(char* name, int argcount, EXPRPTR exprlist);
EXPRPTR opnode(char* name, int argcount, EXPRPTR exprlist, EXPRPTR file);
EXPRPTR wherenode(EXPRPTR expr, EXPRPTR exprlist);
EXPRPTR defnode(char* name, int argcount, EXPRPTR argnames, EXPRPTR expr);
EXPRPTR declnode(char* name, EXPRPTR expr);
EXPRPTR identlistnode(EXPRPTR tail, char* name);
EXPRPTR listnode(EXPRPTR tail, EXPRPTR expr);
EXPRPTR exprlist2(EXPRPTR expr1, EXPRPTR expr2);
EXPRPTR exprlist3(EXPRPTR expr1, EXPRPTR expr2, EXPRPTR expr3);

EXPRPTR expand_wvr_upon(EXPRPTR arg);

int yyerror(STRING a);
