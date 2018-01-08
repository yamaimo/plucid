#pragma once

#include "imanifs.h"

void hiaton(int signum);
int read_c_item(CELLUNION* v);
int read_p_item(CELLUNION* v);
int findword(STRING s);
STRING strsave(char* s);
char* ngc_calloc(int n);

void f_arg(EXPRPTR e);
