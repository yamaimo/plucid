#pragma once

#include "cmanifs.h"

SYMPTR handle_ident(STRING name, int length);

void enter_phrase(void);
EXPRPTR exit_phrase(void);

void enter_function(void);
void exit_function(void);
