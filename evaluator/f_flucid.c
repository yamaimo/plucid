#include <stdio.h>
#include <stdlib.h>

#include "imanifs.h"
#include "iglobals.h"
#include "memory.h"
#include "dump.h"
#include "util.h"
#include "f_flucid.h"

void f_elt(EXPRPTR e)
{
    int dim = e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    int time = t->stphd.word;
    COORDS xyz = s->stphd.xyz;
    switch (dim)
    {
    case 0:
        xyz.d2 = xyz.d1;
        xyz.d3 = xyz.d2;
        xyz.d1 = time;
        break;
    case 1:
        xyz.d3 = xyz.d2;
        xyz.d2 = time;
        break;
    case 2:
        xyz.d3 = time;
        break;
    default:
        fprintf(stderr, "illegal # dimensions\n");
        my_exit(1);
    }
    s = ssearch(xyz);
    STPSpushval(s, t, p);
    eval(arg1.x);
    STPSpop;
}

void f_all(EXPRPTR e)
{
    int dim = e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
    switch (dim)
    {
    case 0:
        t = stpsearch((long)xyz.d1, t->stptl, thashtab);
        xyz.d1 = xyz.d2;
        xyz.d2 = xyz.d3;
        xyz.d3 = 0;
        break;
    case 1:
        t = stpsearch((long)xyz.d2, t->stptl, thashtab);
        xyz.d2 = xyz.d3;
        xyz.d3 = 0;
        break;
    case 2:
        t = stpsearch((long)xyz.d3, t->stptl, thashtab);
        xyz.d3 = 0;
        break;
    default:
        fprintf(stderr, "illegal # dimensions\n");
        my_exit(1);
    }
    s = ssearch(xyz);
    STPSpushval(s, t, p);
    eval(arg1.x);
    STPSpop;
}

void f_original(EXPRPTR e)
{
    int dim = e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
    switch (dim)
    {
    case 0:
        xyz.d3 = xyz.d2;
        xyz.d2 = xyz.d1;
        xyz.d1 = 0;
        break;
    case 1:
        xyz.d3 = xyz.d2;
        xyz.d2 = 0;
        break;
    case 2:
        xyz.d3 = 0;
        break;
    }
    s = ssearch(xyz);
    STPSpushval(s, t, p);
    eval(arg1.x);
    STPSpop;
}

void f_succ(EXPRPTR e)
{
    int dim = e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
    switch (dim)
    {
    case 0:
        xyz.d1 = xyz.d1 + 1;
        break;
    case 1:
        xyz.d2 = xyz.d2 + 1;
        break;
    case 2:
        xyz.d3 = xyz.d3 + 1;
        break;
    default:
        fprintf(stderr, "illegal # dimensions\n");
        my_exit(1);
    }
    s = ssearch(xyz);
    STPSpushval(s, t, p);
    eval(arg1.x);
    STPSpop;
}

void f_nrest(EXPRPTR e)
{
    int dim=e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
    switch (dim)
    {
    case 0:
        xyz.d1 = xyz.d1 - 1;
        break;
    case 1:
        xyz.d2 = xyz.d2 - 1;
        break;
    case 2:
        xyz.d3 = xyz.d3 - 1;
        break;
    default:
        fprintf(stderr, "illegal # dimensions\n");
        my_exit(1);
    }
    s = ssearch(xyz);
    STPSpushval(s, t, p);
    eval(arg1.x);
    STPSpop;
}

void f_rest(EXPRPTR e)
{
    int dim = e -> dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
    xyz.d1 = xyz.d1 + 1;
    s = ssearch(xyz);
    STPSpushval(s, t, p);
    eval(arg1.x);
    STPSpop;
}

void f_aby(EXPRPTR e)
{
}

void f_initial(EXPRPTR e)
{
    int dim = e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
    switch (dim)
    {
    case 0:
        xyz.d1 = 0;
        break;
    case 1:
        xyz.d2 = 0;
        break;
    case 2:
        xyz.d3 = 0;
        break;
    default:
        fprintf(stderr, "illegal # dimensions\n");
        my_exit(1);
    }
    s = ssearch( xyz);
    STPSpushval(s, t, p);
    eval(arg1.x);
    STPSpop;
}

void f_noriginal(EXPRPTR e)
{
    int dim = e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
    switch (dim)
    {
    case 0:
        xyz.d3 = xyz.d2;
        xyz.d2 = xyz.d1;
        xyz.d1 = 0;
        break;
    case 1:
        xyz.d3 = xyz.d2;
        xyz.d2 = 0;
        break;
    case 2:
        xyz.d3 = 0;
        break;
    }
    s = ssearch( xyz);
    STPSpushval(s, t, p);
    eval(arg1.x);
    STPSpop;
}

void f_rshift(EXPRPTR e)
{
    int dim = e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
    xyz.d1 = 0;
    xyz.d3 = xyz.d2;
    s = ssearch(xyz);
    STPSpushval(s, t, p);
    eval(arg1.x);
    STPSpop;
}

void f_lshift(EXPRPTR e)
{
    int dim = e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
    xyz.d1 = xyz.d2;
    xyz.d2 = xyz.d3;
    xyz.d3 = 0;
    s = ssearch(xyz);
    STPSpushval(s, t, p);
    eval(arg1.x);
    STPSpop;
}

void f_swap(EXPRPTR e)
{
    int tmp;
    int dim = e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
/*
    switch (dim)
    {
    case swap_t0s0:
*/
        tmp = t->stphd.word;
        t = stpsearch((long)xyz.d1, t->stptl, thashtab);
        xyz.d1 = tmp;
/*
        break;
    case swap_t0s1:
        tmp = t->stphd.word;
        t->stphd.word = xyz.d2;
        xyz.d2 = t->stphd.word;
        break;
    case swap_t0s2:
        tmp = t->stphd.word;
        t->stphd.word = xyz.d3;
        xyz.d3 = t->stphd.word;
        break;
    case swap_t2s3:
    case swap_t0t1:
    case swap_t0t2:
    case swap_t1t2:
        break;
    case swap_s0s1:
        tmp = xyz.d1;
        xyz.d1 = xyz.d2;
        xyz.d2 = tmp;
        break;
    case swap_s0s2:
        tmp = xyz.d3;
        xyz.d1 = xyz.d3;
        xyz.d3 = tmp;
        break;
    case swap_s1s2:
        tmp = xyz.d2;
        xyz.d2 = xyz.d3;
        xyz.d3 = tmp;
        break;
    }
*/
    s = ssearch(xyz);
    STPSpushval(s, t, p);
    eval(arg1.x);
    STPSpop;
}

void f_sby(EXPRPTR e)
{
    int dim = e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
    switch (dim)
    {
    case 0:
        if (xyz.d1 <= 0)
        {
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg1.x);
            STPSpop;
        }
        else
        {
            xyz.d1 = xyz.d1 - 1;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg2.x);
            STPSpop;
        }
        break;
    case 1:
        if (xyz.d2 <= 0)
        {
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg1.x);
            STPSpop;
        }
        else
        {
            xyz.d2 = xyz.d2 - 1;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg2.x);
            STPSpop;
        }
        break;
    case 2:
        if (xyz.d3 <= 0)
        {
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg1.x);
            STPSpop;
        }
        else
        {
            xyz.d3 = xyz.d3 - 1;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg2.x);
            STPSpop;
        }
        break;
    default:
        fprintf(stderr, "illegal # dimensions\n");
        my_exit(1);
    }
}

void f_cby(EXPRPTR e)
{
    int dim = e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
    switch (dim)
    {
    case 0:
        if (xyz.d1 <= 0)
        {
            xyz.d1 = xyz.d2;
            xyz.d2 = xyz.d3;
            xyz.d3 = 0;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg1.x);
            STPSpop;
        }
        else
        {
            xyz.d1 = xyz.d1 - 1;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg2.x);
            STPSpop;
        }
        break;
    case 2:
        if (xyz.d2 <= 0)
        {
            xyz.d2 = xyz.d3;
            xyz.d3 = 0;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg1.x);
            STPSpop;
        }
        else
        {
            xyz.d2 = xyz.d2 - 1;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg2.x);
            STPSpop;
        }
        break;
    case 3:
        if (xyz.d3 <= 0)
        {
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg1.x);
            STPSpop;
        }
        else
        {
            xyz.d3 = xyz.d3 - 1;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg2.x);
            STPSpop;
        }
        break;
    }
}

void f_atspace(EXPRPTR e)
{
    int dim = e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;

    eval(arg2.x);

    D(30) {
        fprintf(stderr, "atspace \n");
    }

    int tmp;
    char type;
    CELLUNION value;
    switch (VStype)
    {
    case NUMERIC:
        tmp= (int)VSvalue.na.r;
        switch (dim)
        {
        case 0:
            xyz.d1 = tmp;
            break;
        case 1:
            xyz.d2 = tmp;
            break;
        case 2:
            xyz.d3 = tmp;
            break;
        default:
            fprintf(stderr, "illegal # dimensions\n");
            my_exit(1);
        }
        s = ssearch(xyz);
        STPSpushval(s, t, p);
        eval(arg1.x);
        type = VStype;
        value = VSvalue;
        STPSpop;
        VSpop;
        VStype = type;
        VSvalue = value;
        return;
    case EOD:
        VStype = EOD;
        return;
    case ERROR:
        error("right arg of @s is", e->arg3.x, ERROR, VSvalue);
        return;
    default:
        error("right arg of @s must be numeric, not ", e->arg3.x, VStype, VSvalue);
        VStype = ERROR;
        return;
    }
}

void f_whr(EXPRPTR e)
{
    STPPTR s = STPSs;
    STPPTR p = STPSp;
    STPPTR t = STPSt;
    int dim = e->dim;
    COORDS xyz = s->stphd.xyz;
    STPSpush;
    STPSt = t;
    STPSp = p;
    for (int i = 0; ; i++)
    {
        switch (dim)
        {
        case 0:
            xyz.d1 = i;
            break;
        case 1:
            xyz.d2 = i;
            break;
        case 2:
            xyz.d3 = i;
            break;
        default:
            fprintf(stderr, "illegal # dimensions\n");
            my_exit(1);
        }
        STPSs = ssearch(xyz);

        eval(arg2.x);

        D(30) {
            fprintf(stderr, " whr\n");
        }

        switch (VStype)
        {
        case WORD:
            if (VSvalue.wrd == true_)
            {
                VSpop;
                eval(arg1.x);
                STPSpop;
                return;
            }
            VSpop;
            break;
        case ERROR:
            error("right arg of whr is ", e->arg3.x, ERROR, VSvalue);
            VStype = ERROR;
            STPSpop;
            return;
        case EOD:
            VStype = EOD;
            STPSpop;
            return;
        default:
            error("right arg of whr must be logical, not ", e->arg3.x, VStype, VSvalue);
            STPSpop;
            VStype = ERROR;
            return;
        }
    }
}

void f_here(EXPRPTR e)
{
    int dim = e->dim;
    COORDS space = STPSs->stphd.xyz;
    VSpush;
    VStype = NUMERIC;
    switch (dim)
    {
    case 0:
        VSvalue.na.r = space.d1;
        break;
    case 1:
        VSvalue.na.r = space.d2;
        break;
    case 2:
        VSvalue.na.r = space.d3;
        break;
    default:
        fprintf(stderr, "illegal # dimensions\n");
        my_exit(1);
    }
}

void f_pred(EXPRPTR e)
{
    int dim=e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
    switch (dim)
    {
    case 0:
        xyz.d1 = xyz.d1 - 1;
        break;
    case 1:
        xyz.d2 = xyz.d2 - 1;
        break;
    case 2:
        xyz.d3 = xyz.d3 - 1;
        break;
    default:
        fprintf(stderr, "illegal # dimensions\n");
        my_exit(1);
    }
    s = ssearch(xyz);
    STPSpushval(s, t, p);
    eval(arg1.x);
    STPSpop;
}

// not used
#if 0
void f_beside(EXPRPTR e)
{
    int dim = e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
    switch (dim)
    {
    case 0:
        if (xyz.d1 <= 0)
        {
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg1.x);
            STPSpop;
        }
        else
        {
            xyz.d1 = xyz.d1 + 1;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg2.x);
            STPSpop;
        }
        break;
    case 1:
        if (xyz.d2 <= 0)
        {
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg1.x);
            STPSpop;
        }
        else
        {
            xyz.d2 = xyz.d2 + 1;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg2.x);
            STPSpop;
        }
        break;
    case 2:
        if (xyz.d3 <= 0)
        {
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg1.x);
            STPSpop;
        }
        else
        {
            xyz.d3 = xyz.d3 + 1;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg2.x);
            STPSpop;
        }
        break;
    default:
        fprintf(stderr, "illegal # dimensions\n");
        my_exit(1);
    }
}

void f_ncby(EXPRPTR e)
{
    int dim = e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
    switch (dim)
    {
    case 0:
        if (xyz.d1 >= 0)
        {
            xyz.d1 = xyz.d2;
            xyz.d2 = xyz.d3;
            xyz.d3 = xyz.d3;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg1.x);
            STPSpop;
        }
        else
        {
            xyz.d1 = xyz.d1 + 1;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg2.x);
            STPSpop;
        }
        break;
    case 1:
        if (xyz.d2 >= 0)
        {
            xyz.d2 = xyz.d3;
            xyz.d3 = xyz.d3;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg1.x);
            STPSpop;
        }
        else
        {
            xyz.d2 = xyz.d2 + 1;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg2.x);
            STPSpop;
        }
        break;
    case 2:
        if (xyz.d3 >= 0)
        {
            xyz.d3 = 0;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg1.x);
            STPSpop;
        }
        else
        {
            xyz.d3 = xyz.d3 + 1;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg2.x);
            STPSpop;
        }
        break;
    default:
        fprintf(stderr, "illegal # dimensions\n");
        my_exit(1);
    }
}

void f_nsby(EXPRPTR e)
{
    int dim = e->dim;
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    COORDS xyz = s->stphd.xyz;
    switch (dim)
    {
    case 0:
        if (xyz.d1 >= 0)
        {
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg2.x);
            STPSpop;
        }
        else
        {
            xyz.d1 = xyz.d1 + 1;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg1.x);
            STPSpop;
        }
        break;
    case 1:
        if (xyz.d2 >= 0)
        {
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg2.x);
            STPSpop;
        }
        else
        {
            xyz.d2 = xyz.d2 + 1;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg1.x);
            STPSpop;
        }
        break;
    case 2:
        if (xyz.d3 >= 0)
        {
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg2.x);
            STPSpop;
        }
        else
        {
            xyz.d3 = xyz.d3 + 1;
            s = ssearch(xyz);
            STPSpushval(s, t, p);
            eval(arg1.x);
            STPSpop;
        }
        break;
    default:
        fprintf(stderr, "illegal # dimensions\n");
        my_exit(1);
    }
}

void f_even(EXPRPTR e)
{
    int dim = e->dim;
    COORDS space = STPSs->stphd.xyz;
    VSpush;
    VStype = NUMERIC;
    switch (dim)
    {
    case 0:
        VSvalue.na.r = space.d1 * 2;
        break;
    case 1:
        VSvalue.na.r = space.d2 * 2;
        break;
    case 2:
        VSvalue.na.r = space.d3 * 2;
        break;
    default:
        fprintf(stderr, "illegal # dimensions\n");
        my_exit(1);
    }
}

void f_odd(EXPRPTR e)
{
    int dim = e->dim;
    COORDS space = STPSs->stphd.xyz;
    VSpush;
    VStype = NUMERIC;
    switch (dim)
    {
    case 0:
        VSvalue.na.r = space.d1 * 2 + 1;
        break;
    case 1:
        VSvalue.na.r = space.d2 * 2 + 1;
        break;
    case 2:
        VSvalue.na.r = space.d3 * 2 + 1;
        break;
    default:
        fprintf(stderr, "illegal # dimensions\n");
        my_exit(1);
    }
}

void f_toggle(EXPRPTR e)
{
    int dim = e->dim;
    COORDS space = STPSs->stphd.xyz;
    VSpush;
    VStype = NUMERIC;
    switch (dim)
    {
    case 0:
        VSvalue.na.r = space.d1;
        break;
    case 1:
        VSvalue.na.r = space.d2;
        break;
    case 2:
        VSvalue.na.r = space.d3;
        break;
    default:
        fprintf(stderr, "illegal # dimensions\n");
        my_exit(1);
    }
}
#endif
