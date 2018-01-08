#pragma once

#include <stdio.h>

#include "imanifs.h"

void bar(int x);
void my_exit(int n);
void stats(void);
void dumpfile(char* x, EXPRPTR e);
void dumpstp(STPPTR x);
void dumps(STPPTR x);
void dumpt(STPPTR x);
void dumpmem(MEMPTR x);
void dumpmemry(char type, WORDCELL value);
void dumpval(FILE* stream, VALUEPTR x);
STRING code_to_char(STRING s);
void dumpval2(FILE* stream, VALUE x);
void dumplist(CELLPTR x, FILE* channel);
void dumpstring(CELLPTR x, FILE* channel, char flag);
void dumphashes(void);
