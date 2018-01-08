#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "imanifs.h"
#include "iglobals.h"
#include "util.h"
#include "dynamic.h"
#include "string.h"
#include "list.h"
#include "input.h"

#define ERRCASE do { VStype = ERROR; return; } while (0)
#define EODCASE do { VStype = EOD; return; } while (0)

static CELLPTR app(char a_type, CELLPTR a, char b_type, WORDCELL b);    // FIXME: correct?

void f_append(EXPRPTR e)
{
/* printf("ENtering append\n"); */

    eval(arg1.x);
/* return 0; */

    char t1_type = VStype;
    CELLUNION t1_val = VSvalue;
    switch (t1_type)
    {
    case EOD:
        return;
    case NIL:
    case DOTTED_PAIR:
        break;
    default:
        error("1st arg of <> is ", e->arg3.x, t1_type, t1_val);
        VStype = ERROR;
        return;
    }

    eval(arg2.x);

    char t2_type = VStype;
    CELLUNION t2_val = VSvalue;
    switch (t2_type)
    {
    case EOD:
        VSpop;
        VStype = EOD;
        return;
    case NIL:
        VSpop;
        return;
    case DOTTED_PAIR:
        if (t1_type == NIL)
        {
            VSpop;
            VStype = t2_type;
            VSvalue = t2_val;
            return;
        }
        break;
    default:
        error("2nd arg of <> is ", e->arg3.x, t2_type, t2_val);
        VSpop;
        VStype = ERROR;
        return;
    }

    CELLPTR temp = app(t1_type, t1_val.dp, t2_type, t2_val);
    VSpop;
    VStype = DOTTED_PAIR;
    VSvalue.dp = temp;
}

static CELLPTR app(char a_type, CELLPTR a, char b_type, WORDCELL b)
{
    if (a->data.bits.d_tl == NIL)
    {
        return cns(a->data.bits.d_hd, a->hd, b_type, b);
    }
    CELLPTR temp = (CELLPTR)alloc(SMALL_RECORD);
    temp->data.bits.d_mark =  0;
    temp->data.bits.d_hd = a->data.bits.d_hd;
    temp->hd.dp = a->hd.dp;
    temp->data.bits.d_tl = NIL;
    VSpush;
    VStype = DOTTED_PAIR;
    VSvalue.dp = temp;
printf("Recursing...\n");
    temp->tl.dp = app(a->data.bits.d_tl, a->tl.dp, b_type, b);
    temp->data.bits.d_tl = DOTTED_PAIR;
    VSpop;
    return temp;
}

int listlen(char type, WORDCELL val)
{
    if (type == NIL)
    {
        return 0;
    }
    if (val.dp->data.bits.d_tl == NIL)
    {
        return 1;
    }
    return (1 + listlen(val.dp->data.bits.d_tl, val.dp->tl));
}

void f_cons(EXPRPTR e)
{
    eval(arg1.x);

    char a_type = VStype;
    WORDCELL a = VSvalue;
    switch (a_type)
    {
    case EOD:
        return;
    case ERROR:
        error("1st arg of :: or arg of list is ", e->arg3.x, a_type, a);
        return;
    default:
        break;
    }

    eval(arg2.x);

    char b_type = VStype;
    WORDCELL b = VSvalue;
    switch (b_type)
    {
    case EOD:
        VSpop;
        VStype = EOD;
        return;
    case ERROR:
        error("2nd arg of :: or arg of list is ", e->arg3.x, b_type, b);
        VSpop;
        VStype = ERROR;
        return;
    case DOTTED_PAIR:
    case NIL:
        break;
    default:
        error("2nd arg of :: or arg of list is ", e->arg3.x, b_type, b);
        VSpop;
        VStype = ERROR;
        return;
    }

    CELLPTR temp = cns(a_type, a, b_type, b);
    VSpop;
    VStype = DOTTED_PAIR;
    VSvalue.dp = temp;
}

CELLPTR cns(char a_type, WORDCELL a, char b_type, WORDCELL b)
{
    CELLPTR temp = (CELLPTR)alloc(SMALL_RECORD);
    temp->data.bits.d_tl = b_type;
    temp->tl = b;
    temp->data.bits.d_hd = a_type;
    temp->hd = a;
    return temp;
}

void f_hd(EXPRPTR e)
{
    eval(arg1.x);
    switch (VStype)
    {
    case DOTTED_PAIR:
        VStype = VSvalue.dp->data.bits.d_hd;
        VSvalue = VSvalue.dp->hd;
        break;
    case EOD:
        VSvalue.eod = EOD;
        break;
    case NIL:
        error("arg of hd is ", e->arg2.x, VStype, VSvalue);
        VStype = ERROR;
        return;
    case ERROR:
        error("arg of hd is ", e->arg2.x, VStype, VSvalue);
        return;
    default:
        error("arg of hd must be a list, not  ", e->arg2.x, VStype, VSvalue);
        VStype = ERROR;
        break;
    }
}

void f_tl(EXPRPTR e)
{
    eval(arg1.x);
    switch (VStype)
    {
    case DOTTED_PAIR:
        VStype = VSvalue.dp->data.bits.d_tl;
        VSvalue = VSvalue.dp->tl;
        break;
    case EOD:
        VSvalue.eod = EOD;
        break;
    case ERROR:
        error("arg of tl is ", e->arg2.x, VStype, VSvalue);
        return;
    case NIL:
        error("arg of tl is ", e->arg2.x, VStype, VSvalue);
        VStype = ERROR;
        return;
    default:
        error("arg of tl must be a list, not  ", e->arg2.x, VStype, VSvalue);
        VStype = ERROR;
        break;
    }
}

void f_islist(EXPRPTR e)
{
    eval(arg1.x);
    switch (VStype)
    {
    case NIL:
    case DOTTED_PAIR:
        VStype = WORD;
        VSvalue.wrd = true_;
        return;
    case EOD:
        return;
    case ERROR:
        return;
    default:
        VStype = WORD;
        VSvalue.wrd = false_;
        return;
    }
}

void f_mknumber(EXPRPTR e)
{
    char s[30];

    eval(arg1.x);
    switch (VStype)
    {
    case QSTRING:
        formstring(&s[0], VSvalue.strg);
        VStype = NUMERIC;
        VSvalue.na.r  = atof(s);
        if (fabsf(VSvalue.na.r) >= 10000000.0)
        {
            error(
                "numeric value return by mknumber is out of the numeric range.\n"
                "i.e it should be between +100000000 and -1000000000\n",
                e->arg2.x,
                VStype,
                VSvalue);
            ERRCASE;
        }
        break;
    case EOD:
        EODCASE;
    default:
        error("arg of mknumber should a string, not  ", e->arg2.x, VStype, VSvalue);
        ERRCASE;
    }
}

void f_mkstring(EXPRPTR e)
{
    eval(arg1.x);
    switch (VStype)
    {
    case WORD:
        VSvalue.strg  = findstring(wordtable[VSvalue.wrd]);
        VStype = QSTRING;
        break;
    case EOD:
        EODCASE;
    default:
        error(
            "arg of mkstring should be a word, not  ", e->arg2.x, VStype, VSvalue);
        ERRCASE;
    }
}

void f_mkword(EXPRPTR e)
{
    char s[100];

    eval(arg1.x);

    CELLUNION val = VSvalue;
    switch (VStype)
    {
    case QSTRING:
        VStype = WORD;
        formstring(&s[0], VSvalue.strg);
        VSvalue.wrd = findword(s);
        if (VSvalue.wrd < 0)
        {
            error("cannot convert this string into a word : ", e->arg2.x, QSTRING, val);
        }
        break;
    default:
        error("arg of mkword should be a string, not  ", e->arg2.x, VStype, VSvalue);
        VStype = ERROR;
        break;
    case EOD:
        VSvalue.eod = EOD;
        break;
    }
}
