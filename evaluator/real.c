#include <math.h>

#include "imanifs.h"
#include "iglobals.h"
#include "util.h"
#include "real.h"

void f_real(EXPRPTR e)
{
    eval(arg1.x);
    switch (VStype)
    {
    case NUMERIC:
        VSvalue.na.r = VSvalue.na.r;
        break;
    case EOD:
        VSvalue.eod = EOD;
        break;
    case ERROR:
        error("arg of real is ", e->arg2.x, VStype, VSvalue);
        return;
    default:
        error("arg of real must be numeric, not  ", e->arg2.x, VStype, VSvalue);
        VStype = ERROR;
        break;
    }
}

void f_imag(EXPRPTR e)
{
    eval(arg1.x);
    switch (VStype)
    {
    case NUMERIC:
        VSvalue.na.r = VSvalue.na.i;
        break;
    case EOD:
        VSvalue.eod = EOD;
        break;
    case ERROR:
        error("arg of imag is ", e->arg2.x, VStype, VSvalue);
        return;
    default:
        error("arg of imag must be numeric, not  ", e->arg2.x, VStype, VSvalue);
        VStype = ERROR;
        break;
    }
}

void f_cos(EXPRPTR e)
{
    eval(arg1.x);
    switch (VStype)
    {
    case NUMERIC:
        VSvalue.na.r = cos(VSvalue.na.r);
        break;
    case EOD:
        VSvalue.eod = EOD;
        break;
    case ERROR:
        error("arg of cos is ", e->arg2.x, VStype, VSvalue);
        return;
    default:
        error("arg of cos must be numeric, not  ", e->arg2.x, VStype, VSvalue);
        VStype = ERROR;
        break;
    }
}

void f_sin(EXPRPTR e)
{
    eval(arg1.x);
    switch (VStype)
    {
    case NUMERIC:
        VSvalue.na.r = sin(VSvalue.na.r);
        break;
    case EOD:
        VSvalue.eod = EOD;
        break;
    case ERROR:
        error("arg of sin is ", e->arg2.x, VStype, VSvalue);
        return;
    default:
        error("arg of sin must be numeric, not  ", e->arg2.x, VStype, VSvalue);
        VStype = ERROR;
        break;
    }
}

void f_log(EXPRPTR e)
{
    eval(arg1.x);
    switch (VStype)
    {
    case NUMERIC:
        VSvalue.na.r = log(VSvalue.na.r);
        if (fabs(VSvalue.na.r) >= 10000000.0)
        {
            error(
                "numeric value return by log is out of the numeric range.\n"
                "i.e it should be between +10000000 and -10000000, not ",
                e->arg2.x,
                VStype,
                VSvalue);
            VStype = ERROR;
        }
        break;
    case EOD:
        VSvalue.eod = EOD;
        break;
    case ERROR:
        error("arg of log is", e->arg2.x, VStype, VSvalue);
        return;
    default:
        error("arg of log must be numeric, not  ", e->arg2.x, VStype, VSvalue);
        VStype = ERROR;
        break;
    }
}

void f_complex(EXPRPTR e)
{
    eval(arg1.x);

    CELLUNION val = VSvalue;
    char type = VStype;
    switch (VStype)
    {
    case NUMERIC:
        VSpop;
        eval(arg2.x);
        switch (VStype)
        {
        case NUMERIC:
            VSvalue.na.i = VSvalue.na.r;
            VSvalue.na.r = val.na.r;
            if (fabs(VSvalue.na.r) >= 10000000.0
                || fabs(VSvalue.na.i) >= 10000000.0)
            {
                error(
                    "numeric value return by complex is out of the numeric range.\n"
                    "i.e it should be between +10000000 and -10000000 not ",
                    e->arg3.x,
                    VStype,
                    VSvalue);
                VStype = ERROR;
            }
            break;
        case ERROR:
            error("2nd arg of complex is ", e->arg3.x, VStype, VSvalue);
            break;
        case EOD:
            break;
        default:
            error("2nd arg of complex should be numeric, not", e->arg3.x, ERROR, VSvalue);
            VStype = ERROR;
        }
        break;
    case EOD:
        VSvalue.eod = EOD;
        break;
    case ERROR:
        error("1st arg of complex is", e->arg3.x, VStype, VSvalue);
        return;
    default:
        error("1st arg of complex must be numeric, not  ", e->arg3.x, VStype, VSvalue);
        VStype = ERROR;
        break;
    }
}

void f_exp(EXPRPTR e)
{
    eval(arg1.x);

    CELLUNION val = VSvalue;
    char type = VStype;
    switch (VStype)
    {
    case NUMERIC:
        VSpop;
        eval(arg2.x);
        switch (VStype)
        {
        case NUMERIC:
            VSvalue.na.r = pow(val.na.r,VSvalue.na.r);
            if (fabs(VSvalue.na.r) >= 10000000.0)
            {
                error(
                    "numeric value return by ** is out of the numeric range.\n"
                    "i.e it should be between +10000000 and -10000000 not ",
                    e->arg3.x,
                    VStype,
                    VSvalue);
                VStype = ERROR;
            }
            break;
        case ERROR:
            error("2nd arg of ** is ", e->arg3.x, VStype, VSvalue);
            break;
        case EOD:
            break;
        default:
            error("2nd arg of ** should be numeric, not", e->arg3.x, ERROR, VSvalue);
            VStype = ERROR;
        }
        break;
    case EOD:
        VSvalue.eod = EOD;
        break;
    case ERROR:
        error("1st arg of ** is", e->arg3.x, VStype, VSvalue);
        return;
    default:
        error("1st arg of ** must be numeric, not  ", e->arg3.x, VStype, VSvalue);
        VStype = ERROR;
        break;
    }
}

void f_sqrt(EXPRPTR e)
{
    eval(arg1.x);
    switch (VStype)
    {
    case NUMERIC:
        VSvalue.na.r = sqrt(VSvalue.na.r);
        if (fabs(VSvalue.na.r) >= 10000000.0)
        {
            error(
                "numeric value return by mknumber is out of the numeric range.\n"
                "i.e it should be between +100000000 and -1000000000 not ",
                e->arg2.x,
                VStype,
                VSvalue);
            VStype = ERROR;
        }
        break;
    case EOD:
        VSvalue.eod = EOD;
        break;
    case ERROR:
        error("arg of sqrt is ", e->arg2.x, VStype, VSvalue);
        return;
    default:
        error("arg of sqrt must be numeric, not  ", e->arg2.x, VStype, VSvalue);
        VStype = ERROR;
        break;
    }
}

void f_tan(EXPRPTR e)
{
    eval(arg1.x);
    switch (VStype)
    {
    case NUMERIC:
        VSvalue.na.r = tan(VSvalue.na.r);
        break;
    case EOD:
        VSvalue.eod = EOD;
        break;
    case ERROR:
        error("arg of tan is ", e->arg2.x, VStype, VSvalue);
        return;
    default:
        error("arg of tan must be numeric, not  ", e->arg2.x, VStype, VSvalue);
        VStype = ERROR;
        break;
    }
}



void f_log10(EXPRPTR e)
{
    eval(arg1.x);
    switch (VStype)
    {
    case NUMERIC:
        VSvalue.na.r = log10(VSvalue.na.r);
        if (fabs(VSvalue.na.r) >= 10000000.0)
        {
            error(
                "numeric value return by log10 is out of the numeric range.\n"
                "i.e it should be between +100000000 and -1000000000 not ",
                e->arg2.x,
                VStype,
                VSvalue);
            VStype = ERROR;
        }
        break;
    case EOD:
        VSvalue.eod = EOD;
        break;
    case ERROR:
        error("arg of log10 is ", e->arg2.x, VStype, VSvalue);
        return;
    default:
        error("arg of log10 must be numeric, not  ", e->arg2.x, VStype, VSvalue);
        VStype = ERROR;
        break;
    }
}

void f_abs(EXPRPTR e)
{
    eval(arg1.x);
    switch (VStype)
    {
    case NUMERIC:
        VSvalue.na.r = fabs(VSvalue.na.r);
        break;
    case EOD:
        VSvalue.eod = EOD;
        break;
    case ERROR:
        error("arg of abs is ", e->arg2.x, VStype, VSvalue);
        return;
    default:
        error("arg of abs must be numeric, not  ", e->arg2.x, VStype, VSvalue);
        VStype = ERROR;
        break;
    }
}
