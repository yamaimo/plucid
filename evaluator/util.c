#include <stdio.h>

#include "imanifs.h"
#include "iglobals.h"
#include "dump.h"
#include "util.h"

void error(STRING x, EXPRPTR y, char type, CELLUNION val)
{
    if (!tflags[9])
    {
        evalerr = true_;
        if (newout)
        {
            for (int i = 0; i < 80; i++)
            {
                fprintf(stderr, "-");
            }
            newout = false_;
        }
        dumpfile(x, y);
        dumpmemry(type, val);
    }
    errcount++;
    if (errcount > 2)
    {
        my_exit(1);
    }
}
