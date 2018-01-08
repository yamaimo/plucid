#pragma once

#include "imanifs.h"

void f_elt(EXPRPTR e);
void f_all(EXPRPTR e);
void f_original(EXPRPTR e);
void f_succ(EXPRPTR e);
void f_nrest(EXPRPTR e);
void f_rest(EXPRPTR e);
void f_aby(EXPRPTR e);
void f_initial(EXPRPTR e);
void f_noriginal(EXPRPTR e);
void f_rshift(EXPRPTR e);
void f_lshift(EXPRPTR e);
void f_swap(EXPRPTR e);
void f_sby(EXPRPTR e);
void f_cby(EXPRPTR e);
void f_atspace(EXPRPTR e);
void f_whr(EXPRPTR e);
void f_here(EXPRPTR e);
void f_pred(EXPRPTR e);

// not used
#if 0
void f_beside(EXPRPTR e);
void f_ncby(EXPRPTR e);
void f_nsby(EXPRPTR e);
void f_even(EXPRPTR e);
void f_odd(EXPRPTR e);
void f_toggle(EXPRPTR e);
#endif
