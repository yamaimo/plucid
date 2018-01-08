#pragma once

#include "imanifs.h"

void formstring(char* s, CELLPTR sl);
CELLPTR findstring(char* s);
int strglen(char type, WORDCELL val);

void f_substr(EXPRPTR e);
void f_scons(EXPRPTR e);
void f_strconc(EXPRPTR e);
void f_isstring(EXPRPTR e);
void f_chr(EXPRPTR e);
void f_ord(EXPRPTR e);
