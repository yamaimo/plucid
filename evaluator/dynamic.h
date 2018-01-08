#pragma once

#include "imanifs.h"

void initspace(void);
char* alloc(int size);
void collect(void);
void memusage(void);
void rm_eres(STPPTR stptl);
