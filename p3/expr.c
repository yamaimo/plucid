#include <stdlib.h>

#include "cmanifs.h"
#include "cglobals.h"

#include "expr.h"

STRING strsave(char* s);            /* main.c */
int eqstring(STRING a, STRING b);   /* main.c */
void my_exit(int n);                /* main.c */

static int varcount = 0;

EXPRPTR filenode(char* filename, int first_line, int last_line, int cursor_position)
{
    EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR4));
    p->f = F_FILE;
    p->arg1.s = strsave(filename);
    p->arg2.i = first_line;
    p->arg3.i = last_line;
    p->arg4.i = cursor_position;
    return p;
}

EXPRPTR connode(char* s1, char* s2)
{
    EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR2));
    p->f = F_CONST;
    p->arg1.s = strsave(s1);
    p->arg2.s = strsave(s2);
    return p;
}

EXPRPTR f_connode(float n)
{
    EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR2));
    p->f = F_CONST;
    p->arg1.s = strsave("numb");
    p->arg2.r = n;
    return p;
}

EXPRPTR varnode(char* name, int argcount, EXPRPTR exprlist)
{
    EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR3));
    p->f = F_VAR;
    p->arg1.s = strsave(name);
    p->arg2.i = argcount;
    p->arg3.x = exprlist;
    return p;
}

EXPRPTR opnode(char* name, int argcount, EXPRPTR exprlist, EXPRPTR file)
{
    EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR4));
    p->f = F_OP;
    p->arg1.s = strsave(name);
    p->arg2.i = argcount;
    p->arg3.x = exprlist;
    p->arg4.x = file;
    return p;
}

EXPRPTR wherenode(EXPRPTR expr, EXPRPTR exprlist)
{
    EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR2));
    p->f = F_WHERE;
    p->arg1.x = expr;
    p->arg2.x = exprlist;
    return p;
}

EXPRPTR defnode(char* name, int argcount, EXPRPTR argnames, EXPRPTR expr)
{
    EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR4));
    p->f = F_DEFN;
    p->arg1.s = strsave(name);
    p->arg2.i = argcount;
    p->arg3.x = argnames;
    p->arg4.x = expr;
    return p;
}

EXPRPTR declnode(char* name, EXPRPTR expr)
{
    EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR2));
    p->f = F_DECL;
    p->arg1.s = strsave(name);
    p->arg2.x = expr;
    return p;
}

EXPRPTR identlistnode(EXPRPTR tail, char* name)
{
    EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR2));
    p->f = F_IDENTLISTNODE;
    p->arg1.x = tail;
    p->arg2.s = name;
    return p;
}

EXPRPTR listnode(EXPRPTR tail, EXPRPTR expr)
{
    EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR2));
    p->f = F_LISTNODE;
    p->arg1.x = tail;
    p->arg2.x = expr;
    return p;
}

EXPRPTR exprlist2(EXPRPTR expr1, EXPRPTR expr2)
{
    EXPRPTR p1 = (EXPRPTR)calloc(1, sizeof(EXPR2));
    EXPRPTR p2 = (EXPRPTR)calloc(1, sizeof(EXPR2));

    p1->f = F_LISTNODE;
    p1->arg1.x = NULL;
    p1->arg2.x = expr1;

    p2->f = F_LISTNODE;
    p2->arg1.x = p1;
    p2->arg2.x = expr2;

    return p2;
}

EXPRPTR exprlist3(EXPRPTR expr1, EXPRPTR expr2, EXPRPTR expr3)
{
    EXPRPTR p1 = (EXPRPTR)calloc(1, sizeof(EXPR2));
    EXPRPTR p2 = (EXPRPTR)calloc(1, sizeof(EXPR2));
    EXPRPTR p3 = (EXPRPTR)calloc(1, sizeof(EXPR2));

    p1->f = F_LISTNODE;
    p1->arg1.x = NULL;
    p1->arg2.x = expr1;

    p2->f = F_LISTNODE;
    p2->arg1.x = p1;
    p2->arg2.x = expr2;

    p3->f = F_LISTNODE;
    p3->arg1.x = p2;
    p3->arg2.x = expr3;

    return p3;
}

EXPRPTR expand_wvr_upon(EXPRPTR arg)
{
    int dimension;
    char name[30];
    char* s1;
    char* s2;
    EXPRPTR e;
    EXPRPTR e1;
    EXPRPTR e2;
    EXPRPTR file;

    switch (arg->f)
    {
    case F_OP:
        file = arg->arg4.x;
        dimension = arg->arg2.i;
        if (eqstring(arg->arg1.s, "wvr"))
        {
            e = arg->arg3.x;
            e1 = expand_wvr_upon(e->arg1.x->arg2.x);
            e2 = expand_wvr_upon(e->arg2.x);
            sprintf(name, "_%d", varcount++);
            s1 = strsave(name);
            sprintf(name, "_%d", varcount++);
            s2 = strsave(name);
            // FIXME: too ugly...
            return
                wherenode(
                    opnode(
                        "@t",
                        dimension,
                        exprlist2(e1, varnode(s2, 0, NULL)),
                        file),
                    exprlist2(
                        defnode(
                            s1,
                            0,
                            NULL,
                            opnode(
                                "if",
                                0,
                                exprlist3(
                                    e2,
                                    opnode(
                                        "now",
                                        dimension,
                                        listnode(NULL, f_connode(1.0)),
                                        file),
                                    opnode(
                                        "next",
                                        dimension,
                                        listnode(NULL, varnode(s1, 0, NULL)),
                                        file)),
                                file)),
                            defnode(
                                s2,
                                0,
                                NULL,
                                opnode(
                                    "fby",
                                    dimension,
                                    exprlist2(
                                        varnode(s1, 0, NULL),
                                        opnode(
                                            "@t",
                                            dimension,
                                            exprlist2(
                                                varnode(s1, 0, NULL),
                                                opnode(
                                                    "plus",
                                                    0,
                                                    exprlist2(
                                                        varnode(s2, 0, NULL),
                                                        f_connode(1.00)),
                                                    file)),
                                            file)),
                                    file))));
        }
        if (eqstring(arg->arg1.s, "whrvr"))
        {
            e = arg->arg3.x;
            e1 = expand_wvr_upon(e->arg1.x->arg2.x);
            e2 = expand_wvr_upon(e->arg2.x);
            sprintf(name, "_%d", varcount++);
            s1 = strsave(name);
            sprintf(name, "_%d", varcount++);
            s2 = strsave(name);
            // FIXME: too ugly...
            return
                wherenode(
                    opnode(
                        "@s",
                        dimension,
                        exprlist2(e1, varnode(s2, 0, NULL)),
                        file),
                    exprlist2(
                        defnode(
                            s1,
                            0,
                            NULL,
                            opnode(
                                "if",
                                0,
                                exprlist3(
                                    e2,
                                    opnode(
                                        "here",
                                        dimension,
                                        listnode(NULL, f_connode(1.0)),
                                        file),
                                    opnode(
                                        "succ",
                                        dimension,
                                        listnode(NULL, varnode(s1, 0, NULL)),
                                        file)),
                                file)),
                        defnode(
                            s2,
                            0,
                            NULL,
                            opnode(
                                "sby",
                                dimension,
                                exprlist2(
                                    varnode(s1, 0, NULL),
                                    opnode(
                                        "@s",
                                        dimension,
                                        exprlist2(
                                            varnode(s1, 0, NULL),
                                            opnode(
                                                "plus",
                                                0,
                                                exprlist2(
                                                    varnode(s2, 0, NULL),
                                                    f_connode(1.00)),
                                                file)),
                                        file)),
                                file))));
        }
        if (eqstring(arg->arg1.s, "stretch"))
        {
            e = arg->arg3.x;
            e1 = expand_wvr_upon(e->arg1.x->arg2.x);
            e2 = expand_wvr_upon(e->arg2.x);
            sprintf(name, "_%d", varcount++);
            s1 = strsave(name);
            // FIXME: too ugly...
            return
                wherenode(
                    opnode(
                        "@s",
                        dimension,
                        exprlist2(e1, varnode(s1, 0, NULL)),
                        file),
                    listnode(
                        NULL,
                        defnode(
                            s1,
                            0,
                            NULL,
                            opnode(
                                "sby",
                                dimension,
                                exprlist2(
                                    f_connode(0),
                                    opnode(
                                        "if",
                                        0,
                                        exprlist3(
                                            e2,
                                            opnode(
                                                "plus",
                                                0,
                                                exprlist2(
                                                    varnode(s1, 0, NULL),
                                                    f_connode(1.00)),
                                                file),
                                            varnode(s1, 0, NULL)),
                                        file)),
                                file))));
        }
        if (eqstring(arg->arg1.s, "delay"))
        {
            e = arg->arg3.x;
            e1 = expand_wvr_upon(e->arg1.x->arg2.x);
            e2 = expand_wvr_upon(e->arg2.x);
            sprintf(name, "_%d", varcount++);
            s1 = strsave(name);
            // FIXME: too ugly...
            return
                wherenode(
                    opnode(
                        "@t",
                        dimension,
                        exprlist2(e1, varnode(s1, 0, NULL)),
                        file),
                    listnode(
                        NULL,
                        defnode(
                            s1,
                            0,
                            NULL,
                            opnode(
                                "fby",
                                dimension,
                                exprlist2(
                                    f_connode(0),
                                    opnode(
                                        "if",
                                        0,
                                        exprlist3(
                                            e2,
                                            opnode(
                                                "plus",
                                                0,
                                                exprlist2(
                                                    varnode(s1, 0, NULL),
                                                    f_connode(1.00)),
                                                file),
                                            varnode(s1, 0, NULL)),
                                        file)),
                                file))));
        }
        return opnode(
            arg->arg1.s,
            arg->arg2.i,
            expand_wvr_upon(arg->arg3.x),
            file);
    case F_VAR:
        if (arg->arg2.i == 0)
        {
            return arg;
        }
        return varnode(
            arg->arg1.s,
            arg->arg2.i,
            expand_wvr_upon(arg->arg3.x));
    case F_DECL:
        return declnode(
            arg->arg1.s,
            expand_wvr_upon(arg->arg2.x));
    case F_DEFN:
        return defnode(
            arg->arg1.s,
            arg->arg2.i,
            arg->arg3.x,
            expand_wvr_upon(arg->arg4.x));
    case F_LISTNODE:
        if (arg->arg1.x == NULL)
        {
            return listnode(
                NULL,
                expand_wvr_upon(arg->arg2.x));
        }
        return listnode(
            expand_wvr_upon(arg->arg1.x),
            expand_wvr_upon(arg->arg2.x));
    case F_CONST:
        return arg;
    case F_WHERE:
        return wherenode(
            expand_wvr_upon(arg->arg1.x),
            expand_wvr_upon(arg->arg2.x));
    default:
        fprintf(stderr, "error in parse tree\n");
        my_exit(1); // no return
        return 0;
    }
}

int yyerror(STRING a)
{
    fprintf(stderr, "%s\n", a);
    return 0;
}
