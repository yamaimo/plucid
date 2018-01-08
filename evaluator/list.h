#pragma once

#include "imanifs.h"

int listlen(char type, WORDCELL val);
CELLPTR cns(char a_type, WORDCELL a, char b_type, WORDCELL b);

void f_append(EXPRPTR e);
void f_cons(EXPRPTR e);
void f_hd(EXPRPTR e);
void f_tl(EXPRPTR e);
void f_islist(EXPRPTR e);
void f_mknumber(EXPRPTR e);
void f_mkstring(EXPRPTR e);
void f_mkword(EXPRPTR e);
