#include <string.h>
#include <stdlib.h>

#include "imanifs.h"
#include "iglobals.h"
#include "util.h"
#include "dynamic.h"
#include "string.h"
#include "list.h"

static CELLPTR strgapp(int a_type, CELLPTR a, int b_type, CELLPTR b);

void formstring(char* s, CELLPTR sl)
{
    int i = 0;
    CELLPTR temp = sl;
    while (temp->hd.swch != '\0')
    {
        s[i] = temp->hd.swch;
        i++;
        temp = temp->tl.strg;
    }
    s[i] = '\0';
}

CELLPTR findstring(char* s)
{
    CELLPTR temp = (CELLPTR)alloc(SMALL_RECORD);
    char ch = s[0];

    temp->data.bits.d_hd = SWCHAR;
    temp->hd.swch = ch;
    temp->data.bits.d_tl = NIL;
    VSpush;
    VStype = QSTRING;
    VSvalue.strg = temp;
    if (s[0] != '\0')
    {
        temp->tl.strg = findstring(&s[1]);
        temp->data.bits.d_tl = QSTRING;
    }
    VSpop;
    return temp;
}

void f_substr(EXPRPTR e)
{
    eval(arg1.x);

    char type1 = VStype;
    CELLUNION val1 = VSvalue;
    switch (type1)
    {
    case QSTRING:
        break;
    case ERROR:
        error("1st arg of substrg is ", e->arg4.x, ERROR, val1);
        return;
    case EOD:
        return;
    default:
        error("1st arg of substrg is ", e->arg4.x, type1, val1);
        VStype = ERROR;
        return;
    }

    eval(arg2.x);

    char type2 = VStype;
    CELLUNION val2  = VSvalue;
    switch (type2)
    {
    case EOD:
        VSpop;
        VStype = EOD;
        return;
    case ERROR:
        error("2nd arg of substrg is ", e->arg4.x, ERROR, val2);
        VSpop;
        VStype = ERROR;
        return;
    case NUMERIC:
        break;
    default:
        error("2nd arg of substrg is ", e->arg4.x, type2, val2);
        VSpop;
        VStype = ERROR;
        return;
    }

    eval(arg3.x);

    switch (VStype)
    {
    case EOD:
        VSpop;
        VSpop;
        VStype = EOD;
        return;
    case ERROR:
        error("3rd arg of substrg is ", e->arg4.x, ERROR, val2);    // val3?
        VSpop;
        VSpop;
        VStype = ERROR;
        return;
    case NUMERIC:
        break;
    default:
        error("3rd arg of substrg is ", e->arg4.x, VStype, val2);   // val3?
        VSpop;
        VSpop;
        VStype = ERROR;
        return;
    }

    char s[200];
    formstring(s, val1.strg);
    char* temps = s;
    if (val2.na.r <= 0)
    {
        VSpop;
        VSpop;
        VStype = ERROR;
        error(
            "incorrect range for substrg range1 is ",
            e->arg4.x,
            NUMERIC,
            val2);
        return;
    }
    if (VSvalue.na.r > strlen(s))
    {
        VSpop;
        VSpop;
        VStype = ERROR;
        error(
            "incorrect ranges in substr range2 is ",
            e->arg4.x,
            NUMERIC,
            VSvalue);
        return;
    }

    char buff[200];
    char* p = buff;
    for (int i = 1; i <= (VSvalue.na.r - val2.na.r) + 1; i++)
    {
        *p++ = temps[((int)val2.na.r) + i - 2];
    }
    *p = '\0';

    CELLPTR temp = findstring(buff);
    VSpop;
    VSpop;
    VSvalue.strg = temp;
    VStype = QSTRING;
    return;
}

int strglen(char type, WORDCELL val)
{
    if (val.strg->data.bits.d_tl == NIL)
    {
        return 0;
    }
    return (1 + strglen(val.strg->data.bits.d_tl, val.strg->tl));
}

void f_scons(EXPRPTR e)
{
    eval(arg1.x);

    char a_type = VStype;
    WORDCELL a = VSvalue;
    switch (a_type)
    {
    case EOD:
        return;
    case SWCHAR:
        break;
    default:
        error("in string ", e->arg3.x, a_type, a);
        VStype = ERROR;
        return;
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
    case QSTRING:
        break;
    case NIL:
        break;
    default:
        error("in string ", e->arg3.x, b_type, b);
        VSpop;
        VStype = ERROR;
        return;
    }

    CELLPTR temp = cns(a_type, a, b_type, b);
    VSpop;
    VStype = QSTRING;
    VSvalue.strg = temp;
}

void f_strconc(EXPRPTR e)
{
    eval(arg1.x);

    char t1_type = VStype;
    CELLUNION t1_val = VSvalue;
    switch (t1_type)
    {
    case EOD:
        return;
    case ERROR:
        error("left arg of ^ is ", e->arg3.x, ERROR, t1_val);
        return;
    case QSTRING:
        break;
    default:
        error("left arg of ^ is ", e->arg3.x, t1_type, t1_val);
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
    case ERROR:
        error("right arg of ^ is ", e->arg3.x, ERROR, t2_val);
        VSpop;
        VStype = ERROR;
        return;
    case QSTRING:
        break;
    default:
        error("right arg of ^ is ", e->arg3.x, t2_type, t2_val);
        VSpop;
        VStype = ERROR;
        return;
    }

    CELLPTR temp = strgapp(t1_type,t1_val.dp,t2_type,t2_val.dp);
    VSpop;
    VStype = QSTRING;
    VSvalue.strg = temp;
}

static CELLPTR strgapp(int a_type, CELLPTR a, int b_type, CELLPTR b)
{
    if (a->data.bits.d_tl == QSTRING)
    {
        CELLPTR temp = (CELLPTR)alloc(SMALL_RECORD);
        temp->data.bits.d_hd = SWCHAR;
        temp->hd.swch = a->hd.swch;
        temp->data.bits.d_tl = NIL;
        if (v_top + 2 >= &v_stack[0] + 100)
        {
            printf("vstack overflow\n");
            exit(1);
        }
        VSpush;
        VStype = QSTRING;
        VSvalue.strg = temp;
        temp->tl.strg = strgapp(a->data.bits.d_tl, a->tl.strg, b_type, b);
        temp->data.bits.d_tl = QSTRING;
        VSpop;
        return temp;
    }
    return b;
}

void f_isstring(EXPRPTR e)
{
    eval(arg1.x);

    switch (VStype)
    {
    case QSTRING:
        VSvalue.wrd = true_;
        VStype = WORD;
        break;
    case EOD:
        break;
    case ERROR:
        error("arg of isstring is ", e->arg2.x, VStype, VSvalue);
        return;
    default:
        VSvalue.wrd = false_;
        VStype = WORD;
        break;
    }
}

void f_chr(EXPRPTR e)
{
    char s[2];

    eval(arg1.x);

    switch (VStype)
    {
    case NUMERIC:
        if (VSvalue.na.r < 0 || VSvalue.na.r > 128)
        {
            error(
                "integer arg of chr non-ascii ",
                e->arg2.x,
                VStype,
                VSvalue);
            VStype = ERROR;
            break;
        }
        s[0] = VSvalue.na.r;
        s[1] = '\0';
        VSvalue.strg = findstring(s);
        VStype = QSTRING;
        break;
    case EOD:
        break;
    case ERROR:
        error("arg of chr is ? ", e->arg2.x, VStype, VSvalue);
        return;
    default:
        error(
            "arg of chr must be of type integer, not ",
            e->arg2.x,
            VStype,
            VSvalue);
        VStype = ERROR;
    }
}

void f_ord(EXPRPTR e)
{
    eval(arg1.x);

    switch (VStype)
    {
    case QSTRING:
        if (VSvalue.strg->hd.swch < 0 || VSvalue.strg->hd.swch > 127)
        {
            error("invalid char in ord", e->arg2.x, VStype, VSvalue);
            VStype = ERROR;
            return;
        }
        VSvalue.na.r = VSvalue.strg->hd.swch;
        VStype = NUMERIC;
        break;

    case EOD:
        break;
    case ERROR:
        error("arg of ord is  ?", e->arg2.x, VStype, VSvalue);
        return;
    default:
        error(
            "arg of ord must be of type string, not ",
            e->arg2.x,
            VStype,
            VSvalue);
        VStype = ERROR;
    }
}
