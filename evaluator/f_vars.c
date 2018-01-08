#include <stdio.h>

#include "imanifs.h"
#include "iglobals.h"
#include "dump.h"
#include "dynamic.h"
#include "memory.h"
#include "f_vars.h"

#define evalname(x) (*ftable[exprtable[x]->f].apply)(exprtable[x])
#define INMEMORY(x) memsearch(&memplace,x,s,t,p)

static void dumpname(MEMPTR m, int n);
static void f_var1(EXPRPTR e);
static void f_var2(EXPRPTR e);
static void f_var3(EXPRPTR e);
static void display(void);

static void dumpname(MEMPTR m, int n)
{
    ddepth--;
    bar(ddepth);
    fprintf(stderr, "=%s",nametable[n]);
    dumps(m->s);
    dumpstp(m->t);
    dumpstp(m->p);
    fprintf(stderr, " ");
    dumpval2(stderr, m->v);
    fprintf(stderr, "\n");
}

/*
 * given a var node, evalaute the proper variable.
 * until it is determined what types of things must be
 * done in different situations, most variable
 * requests are handled with this one node.
 * Later they may be broken up for efficiency
 */
void f_var(EXPRPTR e)
{
    varcount++;
    if (e->arg3.i == 0)
    {
        /* thru no functions */
        f_var1(e);
    }
    else if (e->arg1.i < 0)
    {
        /* thru functions to a formal */
        f_var2(e);
    }
    else
    {
        /* thru functions to a local */
        f_var3(e);
    }
}

/*
 * the simple case where the variable being requested is defined
 * in the same function environment that it is being requested from
 */
static void f_var1(EXPRPTR e)
{
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;

    if (e->arg2.i < 0)
    {
        int toptime = t->stphd.word;
        COORDS topspace = s->stphd.xyz;
        for (int i = 1; i < -e->arg2.i; i++)
        {
            t = t->stptl;
        }
        t = stpsearch((long)toptime, t->stptl, thashtab);
        s = ssearch(topspace);
    }
    else
    {
        for(int i = 1; i < e->arg2.i; i++)
        {
            t = t->stptl;
        }
    }

    STPSpushval(s, t, p);
    int name = e->arg1.i;
    MEMPTR memplace;
    if (!INMEMORY(name))    // NOTE: memplace is set here.
    {
        evalname(name);
        notfoundcount++;
        if (VStype == UNDEFINED)
        {
            fprintf(stderr, "undefined\n");
        }
        copy(memplace->v, *VStop);
        D(30) {
            dumpname(memplace, name);
        }
        D(6) {
            fprintf(stderr, "Defining ");
            dumpmem(memplace);
        }
    }
    else
    {
        VSpush;
        copy(*VStop, memplace->v);
    }
    STPSpop;
}

/*
 * the variable being accessed is the formal parameter of
 * a function that is either the function we are in now, or
 * is a function containing the function we are in now
 */
static void f_var2(EXPRPTR e)
{
    int toptime = STPSt->stphd.word;
    COORDS topspace = STPSs->stphd.xyz;

    DISPLAYPTR env = d_top;
    for (int i = 1; i < e->arg3.i; i++)
    {
        env = env->d_envg;
    }

    /*
     * "name" is offset in "exprtable" where expression for
     * function is stored.
     */
    int name = env->d_pf->stphd.word - e->arg1.i - 1;

    if (env->d_frozen >= 0)
    {
        toptime = env->d_frozen;
    }
    STPPTR s = env->d_sf;
    STPPTR t = env->d_tf;
    STPPTR p = env->d_pf->stptl;
    env = env->d_envf;
    t = (e->arg2.i > 0) ? t : stpsearch((long)toptime, t->stptl, thashtab);
    s = (e->arg2.i > 0) ? s : ssearch(topspace);
    STPSpushval(s, t, p);
    if (d_top + 1 >= &d_stack[0] + 4 * DSIZE)
    {
        fprintf(stderr, "d_stack overflow\n");
        my_exit(1);
    }
    else
    {
        d_top++;
    }
    copy(*d_top, *env);

    MEMPTR memplace;
    if (!INMEMORY(name))    // NOTE: memplace is set here.
    {
        evalname(name);
        notfoundcount++;
        if (VStype == UNDEFINED)
        {
            fprintf(stderr, "undefined\n");
        }
        copy(memplace->v, *VStop);
        D(30) {
            dumpname(memplace, name);
        }
        D(6) {
            fprintf(stderr, "Defining ");
            dumpmem(memplace);
        }
    }
    else
    {
        VSpush;
        copy(*VStop, memplace->v);
    }
    stp_top--;
    d_top--;
}

/*
 * the variable being accessed is a defined as a local variable in
 * some function environment containing the one we are presently in
 */
static void f_var3(EXPRPTR e)
{
    int toptime = stp_top->stp_t->stphd.word;
    COORDS topspace = stp_top->stp_s->stphd.xyz;

    DISPLAYPTR env = d_top;
    for (int i = 1; i < e->arg3.i; i++)
    {
        env = env->d_envg;
    }
    if (d_top + 1 >= &d_stack[0] + 4 * DSIZE)
    {
        fprintf(stderr, "d_stack overflow\n");
        my_exit(1);
    }
    else
    {
        d_top++;
    }
    copy(*d_top, *env->d_envg);

    STPPTR s = env->d_sg;
    STPPTR p = env->d_pg;
    STPPTR t = env->d_tg;
    if (e->arg2.i < 0)
    {
        for(int i = 1; i < -e->arg2.i; i++)
        {
            t = t->stptl;
        }
        t = stpsearch((long) toptime, t->stptl, thashtab);
        s = ssearch(topspace);
    }
    else
    {
        for (int i = 1; i < e->arg2.i; i++)
        {
            t = t->stptl;
        }
    }
    if (stp_top + 1 >= &stp_stack[0] + STPSIZE)
    {
        fprintf(stderr, "stp_stack overflow\n");
        my_exit(1);
    }
    else
    {
        stp_top++;
    }
    stp_top->stp_t = t;
    stp_top->stp_p = p;
    stp_top->stp_s = s;
    int name = e->arg1.i;

    MEMPTR  memplace;
    if (!INMEMORY(name))    // NOTE: memplace is set here.
    {
        evalname(name);
        notfoundcount++;
        if (VStype == UNDEFINED)
        {
            fprintf(stderr, "undefined\n");
        }
        copy(memplace->v, *VStop);
        D(30) {
            dumpname(memplace, name);
        }
        D(6) {
            fprintf(stderr, "Defining ");
            dumpmem(memplace);
        }
    }
    else
    {
        VSpush;
        copy(*VStop, memplace->v);
    }
    stp_top--;
    d_top--;
}

/*
 * a variable of the same time and place as the current environment
 * is being requested. this case require no changes on the stacks
 */
void f_local(EXPRPTR e)
{
    varcount++;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    STPPTR s = STPSs;

    MEMPTR memplace;
    if (!INMEMORY(e->arg1.i))   // NOTE: memplace is set here.
    {
        evalname(e->arg1.i);
        notfoundcount++;
        copy(memplace->v, *VStop);
        D(30) {
            dumpname(memplace, e->arg1.i);
        }
        D(6) {
            fprintf(stderr, "Defining ");
            dumpmem(memplace);
        }
    }
    else
    {
        VSpush;
        copy(*VStop, memplace->v);
    }
}

/*
 * here is what the parameters mean:
 * arg1    name of the function invoked
 * arg2    number of time levels to pop.
 *         (times -1 if nonelementary function)
 * arg3    number of place levels to pop
 * arg4    address of actual parameters to substitute
 * arg5    # of parameters
 */
void f_fcall(EXPRPTR e)
{
    varcount++;
    STPPTR t = stp_top->stp_t;
    STPPTR s = stp_top->stp_s;
    STPPTR p = stpsearch((long)e->arg4.i, stp_top->stp_p, phashtab);
    if (stp_top + 1 >= &stp_stack[0] + STPSIZE)
    {
        fprintf(stderr, "stp_stack overflow\n");
        my_exit(1);
    }
    else
    {
        stp_top++;
    }
    stp_top->stp_t = t;
    stp_top->stp_s = s;
    stp_top->stp_p = p;

    DISPLAYPTR env = d_top;
    if (d_top + 1 >= &d_stack[0] + 4 * DSIZE)
    {
        fprintf(stderr, "d_stack overflow\n");
        my_exit(1);
    }
    else
    {
        d_top++;
    }
    /*
     * first, where are the parameters of F defined?
     */
    d_top->d_tf = t; /* stp_top->stp_t; */
    d_top->d_sf = s; /* stp_top->stp_s; */
    d_top->d_pf = p;
    d_top->d_envf = d_top - 1;

    /*
     * now, where is F itself defined
     * it is inherited thru N levels, so we
     * go back N envirnoments and where the first function
     * it was inherited into was defined.
     * We then pop off a few times, as required
     */
    if (e->arg3.i == 0)
    {
        /*
         * function not inherited at all!!
         */
        d_top->d_envg = env;
        d_top->d_pg = stp_top->stp_p->stptl;
        t = stp_top->stp_t;
        s = stp_top->stp_s;
    }
    else
    {
        for(int i = 1; i < e->arg3.i; i++)
        {
            env = env->d_envg;
        }
        d_top->d_envg = env->d_envg;
        d_top->d_pg = env->d_pg;
        t = env->d_tg;
        s = env->d_sg;
    }
    if (e->arg2.i > 0)
    {
        /* elementary */
        for (int i = 1; i < e->arg2.i; i++)
        {
            t = t->stptl;
        }
        d_top->d_frozen = t->stphd.word; /* time frozen at */
        stp_top->stp_t
            = stpsearch((long)t->stphd.word, stp_top->stp_t->stptl, thashtab);
    }
    else
    {
        /* nonelemntary */
        d_top->d_frozen = -1;
        for (int i = 1; i < -e->arg2.i; i++)
        {
            t = t->stptl;
        }
    }
    d_top->d_tg = t;
    d_top->d_sg = s;

    /*
     * call the function
     */
    D(5) {
        display();
    }
    e = exprtable[e->arg1.i];
    (*ftable[e->f].apply)(e);

    /*
     * restore old environment
     */
    D(5) {
        fprintf(stderr, "Pop Display\n");
    }
    stp_top--;
    d_top--;
}

void f_eres(EXPRPTR e)
{
    varcount++;
    STPPTR t = stpsearch((long)STPSt->stphd.word, STPSt, thashtab);
    STPPTR p = STPSp;
    STPPTR s = STPSs;
    STPSpushval(s, t, p);

    MEMPTR memplace;
    if (!INMEMORY(e->arg1.i))   // NOTE: memplace is set here.
    {
        evalname(e->arg1.i);
        notfoundcount++;
        copy(memplace->v, *VStop);
        D(30) {
            dumpname(memplace, e->arg1.i);
        }
        D(6) {
            fprintf(stderr, "Defining ");
            dumpmem(memplace);
        }
    }
    else
    {
        VSpush;
        copy(*VStop, memplace->v);
    }
    /*now, get rid of all shit that isnt needed */
    D(26) {
        rm_eres(t->stptl);
    }

    STPSpop;
}

static void display(void)
{
    fprintf(stderr, "Push Display:\n");
    fprintf(stderr, "tf:");
    dumpstp(d_top->d_tf);
    fprintf(stderr, "pf:");
    dumpstp(d_top->d_pf);
    fprintf(stderr, "\n");
    fprintf(stderr, "tg:");
    dumpstp(d_top->d_tg);
    fprintf(stderr, "pg:");
    dumpstp(d_top->d_pg);
    fprintf(stderr, "\n");
}
