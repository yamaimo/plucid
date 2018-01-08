#pragma once

#include "imanifs.h"

void f_cxfile(EXPRPTR e);
void f_eod(EXPRPTR e);
void f_error(EXPRPTR e);
void f_nil(EXPRPTR e);
void f_length(EXPRPTR e);
void f_swchar(EXPRPTR e);
void f_isword(EXPRPTR e);
void f_iseod(EXPRPTR e);
void f_iserror(EXPRPTR e);
void f_isatom(EXPRPTR e);
void f_isnumber(EXPRPTR e);
void f_isnil(EXPRPTR e);
void f_uminus(EXPRPTR e);
void f_not(EXPRPTR e);
void f_and(EXPRPTR e);
void f_or(EXPRPTR e);
void f_num2(EXPRPTR e);
void f_log2(EXPRPTR e);
void f_eq(EXPRPTR e);
void f_ne(EXPRPTR e);
void f_const(EXPRPTR e);
void f_word(EXPRPTR e);
void f_if(EXPRPTR e);
