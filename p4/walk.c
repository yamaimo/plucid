#include <stdlib.h>
#include <stdio.h>

#include "cmanifs.h"
#include "cglobals.h"
#include "expr.h"
#include "walk.h"

STRING strsave(char* s);            /* main.c */
int eqstring(STRING a, STRING b);   /* main.c */
void output(EXPRPTR p);             /* main.c */

static EXPRPTR append(EXPRPTR e1, EXPRPTR e2);
static EXPRPTR extract_file_filter(EXPRPTR arg);
static EXPRPTR remove_evalofs(EXPRPTR arg);
static EXPRPTR new_valof(EXPRPTR e);
static EXPRPTR replace_ngs(EXPRPTR idlist, EXPRPTR e);
static EXPRPTR rremove(EXPRPTR e, EXPRPTR idlist);
static EXPRPTR remove_decls(EXPRPTR arg);
static EXPRPTR new_evalof(EXPRPTR e);
static EXPRPTR get_globals(EXPRPTR arg);
static EXPRPTR new_ng_nglobals(int type, EXPRPTR e);
static void export_nglobals(void);
static void add_used_list(EXPRPTR e);
static int is_defined(char* s);
static void add_defined_list(EXPRPTR e);
static void remove_formals(void);
static void add_formals(EXPRPTR e);
static void add_used(char* s);
static void add_defined(char* s);
static EXPRPTR mkwhere(EXPRPTR e);
static EXPRPTR where_to_valof(EXPRPTR arg);
static EXPRPTR add_valof(EXPRPTR e);
static int is_decl(EXPRPTR e);

static int varcount = 0;

void pass4(EXPRPTR x)
{
    output(
        extract_file_filter(
            remove_evalofs(
                get_globals(
                    remove_decls(
                        add_valof(
                            where_to_valof(x)))))));
}

static EXPRPTR append(EXPRPTR e1, EXPRPTR e2)
{
    if (e2 == NULL)
    {
        return e1;
    }
    if (e1 == NULL)
    {
        return e2;
    }

    EXPRPTR tmp = listnode(e1, e2->arg2.x);
    while (e2->arg1.x != NULL)
    {
        e2 = e2->arg1.x;
        tmp = listnode(tmp, e2->arg2.x);
    }
    return tmp;
}

static EXPRPTR extract_file_filter(EXPRPTR arg)
{
    EXPRPTR file = NULL;

    switch (arg->f)
    {
    case F_OP:
        file = arg->arg4.x;
        if (eqstring(arg->arg1.s, "file") || eqstring(arg->arg1.s, "filter"))
        {
            char name[40];
            FFPTR s = (FFPTR)calloc(1, sizeof(FFITEM));
//            sprintf(name, "_%d", arg->arg1.s, 1000 + varcount++); // bug?
            sprintf(name, "_%d", 1000 + varcount++);
            STRING sv = strsave(name);
            s->name = sv;
            s->rhs = opnode(
                arg->arg1.s,
                arg->arg2.i,
                extract_file_filter(arg->arg3.x),
                file);
            s->next_d = new_def[fflevel];
            new_def[fflevel] = s;
            return varnode(sv, 0, NULL);
        }
        return opnode(
            arg->arg1.s,
            arg->arg2.i,
            extract_file_filter(arg->arg3.x),
            file);
    case F_VAR:
        if (arg->arg2.i == 0)
        {
            return arg;
        }
        return varnode(
            arg->arg1.s,
            arg->arg2.i,
            extract_file_filter(arg->arg3.x));
    case F_DEFN:
        return defnode(
            arg->arg1.s,
            arg->arg2.i,
            arg->arg3.x,
            extract_file_filter(arg->arg4.x));
    case F_NGLOBALS:
        return arg;
    case F_EGLOBALS:
        return arg;
    case F_LISTNODE:
        if (arg->arg1.x == NULL)
        {
            return listnode(
                NULL,
                extract_file_filter(arg->arg2.x));
        }
        return listnode(
            extract_file_filter(arg->arg1.x),
            extract_file_filter(arg->arg2.x));
    case F_CONST:
        return arg;
    case F_VALOF:
        fflevel++;
        EXPRPTR tmp = extract_file_filter(arg->arg1.x);
        EXPRPTR deflist = NULL;
        for (FFPTR s = new_def[fflevel]; s != NULL; s = s->next_d)
        {
            deflist = listnode(
                deflist,
                defnode(s->name, 0, NULL, s->rhs));
        }
        fflevel--;
        return valofnode(append(tmp, deflist));
    default:
        fprintf(stderr, "error in parse tree\n");
        return arg;
    }
}

static EXPRPTR remove_evalofs(EXPRPTR arg)
{
    EXPRPTR file = NULL;

    switch (arg->f)
    {
    case F_OP:
        file = arg->arg4.x;
        return opnode(
            arg->arg1.s,
            arg->arg2.i,
            remove_evalofs(arg->arg3.x),
            file);
    case F_VAR:
        if (arg->arg2.i == 0)
        {
            return arg;
        }
        return varnode(
            arg->arg1.s,
            arg->arg2.i,
            remove_evalofs(arg->arg3.x));
    case F_DEFN:
        return defnode(
            arg->arg1.s,
            arg->arg2.i,
            arg->arg3.x,
            remove_evalofs(arg->arg4.x));
    case F_NGLOBALS:
        return arg;
    case F_LISTNODE:
        if (arg->arg1.x == NULL)
        {
            return listnode(
                NULL,
                remove_evalofs(arg->arg2.x));
        }
        return listnode(
            remove_evalofs(arg->arg1.x),
            remove_evalofs(arg->arg2.x));
    case F_CONST:
        return arg;
    case F_EVALOF:
        return new_valof(remove_evalofs(arg->arg1.x));
    case F_VALOF:
        return valofnode(remove_evalofs(arg->arg1.x));
    default:
        fprintf(stderr, "error in parse tree\n");
        return arg;
    }
}

static EXPRPTR new_valof(EXPRPTR e)
{
    EXPRPTR tmp = e;
    EXPRPTR idlist = NULL;
    EXPRPTR glbs = NULL;
    EXPRPTR result = NULL;
    EXPRPTR dec = e->arg2.x;
    EXPRPTR save = NULL;

    while (tmp != NULL)
    {
        if (dec->f == F_NGLOBALS)
        {
            glbs = listnode(glbs, dec);
        }
        else
        {
            if (dec->f == F_DEFN && !eqstring("_result", dec->arg1.s))
            {
                idlist = identlistnode(idlist, dec->arg1.s);
            }
            if (dec->f == F_DEFN && eqstring("_result", dec->arg1.s))
            {
                save = dec->arg4.x ;
            }
            else
            {
                result = listnode(result, dec);
            }
        }
        tmp = tmp->arg1.x;
        if (tmp != NULL)
        {
            dec = tmp->arg2.x;
        }
    }
    result = append(
        glbs,
        listnode(
            result,
            replace_ngs(idlist, save)));
    return valofnode(result);
}

static EXPRPTR replace_ngs(EXPRPTR idlist, EXPRPTR e)
{
    EXPRPTR result = NULL;
    EXPRPTR tmp = e->arg1.x;
    EXPRPTR dec = tmp->arg2.x;
    EXPRPTR glbs = listnode(NULL, eglobnode(idlist));
    EXPRPTR rem_t = NULL;

    while (tmp != NULL)
    {
        if (dec->f == F_NGLOBALS)
        {
            rem_t = rremove(dec, idlist);
            if (rem_t != NULL)
            {
                glbs = listnode(glbs,rem_t);
            }
        }
        else
        {
            result = listnode(result,dec);
        }
        tmp = tmp->arg1.x;
        if (tmp != NULL)
        {
            dec = tmp->arg2.x;
        }
    }

    return defnode(
        "_result",
        0,
        NULL,
        valofnode(append(glbs, result)));
}

static EXPRPTR rremove(EXPRPTR e, EXPRPTR idlist)
{
    char found = 0;
    char* s = NULL;

    EXPRPTR newlist = NULL;
    EXPRPTR oldlist = e->arg1.x;
    EXPRPTR remlist = NULL;

    while (oldlist != NULL)
    {
        s = oldlist -> arg2.s;
        remlist = idlist;
        found = false_;
        while (remlist != NULL)
        {
            if (eqstring(remlist->arg2.s, s))
            {
                found = true_;
            }
            remlist = remlist->arg1.x;
        }
        if (!found)
        {
            newlist = identlistnode(newlist,s);
        }
        oldlist = oldlist->arg1.x;
    }
    if (newlist == NULL)
    {
        return NULL;
    }
    return nglobnode(newlist);
}

static EXPRPTR remove_decls(EXPRPTR arg)
{
    EXPRPTR file = NULL;

    switch (arg->f)
    {
    case F_OP:
        file = arg->arg4.x;
        return opnode(
            arg->arg1.s,
            arg->arg2.i,
            remove_decls(arg->arg3.x),
            file);
    case F_VAR:
        if (arg->arg2.i == 0)
        {
            return arg;
        }
        return varnode(
            arg->arg1.s,
            arg->arg2.i,
            remove_decls(arg->arg3.x));
    case F_DEFN:
        return defnode(
            arg->arg1.s,
            arg->arg2.i,
            arg->arg3.x,
            remove_decls(arg->arg4.x));
    case F_LISTNODE:
        if (arg->arg1.x == NULL)
        {
            return listnode(
                NULL,
                remove_decls(arg->arg2.x));
        }
        return listnode(
            remove_decls(arg->arg1.x),
            remove_decls(arg->arg2.x));
    case F_CONST:
        return arg;
    case F_VALOF:
        if (is_decl(arg->arg1.x))
        {
            return new_evalof(arg->arg1.x);
        }
        return valofnode(remove_decls(arg->arg1.x));
    default:
        fprintf(stderr, "error in parse tree\n");
        return arg;
    }
}

static EXPRPTR new_evalof(EXPRPTR e)
{
    EXPRPTR tmp = e;
    EXPRPTR ev_list = NULL;
    EXPRPTR v_list = NULL;
    EXPRPTR dec = e->arg2.x;
    EXPRPTR v_def = NULL;
    EXPRPTR ev_def = NULL;

    while (tmp != NULL)
    {
        if (dec->f == F_DECL)
        {
            char name[30];
            sprintf(name, "_%d", 1000 + varcount++);
            char* s = strsave(name);
            ev_def = defnode(
                s,
                0,
                NULL,
                remove_decls(dec->arg2.x));
            v_def = defnode(
                dec->arg1.s,
                0,
                NULL,
                varnode(s, 0, NULL));
            ev_list = listnode(ev_list, ev_def);
        }
        else
        {
            v_def = remove_decls(dec);
        }
        v_list = listnode(v_list, v_def);
        tmp = tmp->arg1.x;
        if (tmp != NULL)
        {
            dec = tmp->arg2.x;
        }
    }
    ev_def = defnode("_result", 0, NULL, valofnode(v_list));
    return evalofnode(listnode(ev_list, ev_def));
}

static EXPRPTR get_globals(EXPRPTR arg)
{
    EXPRPTR tmp = NULL;
    EXPRPTR file = NULL;

    switch (arg->f)
    {
    case F_OP:
        file = arg->arg4.x;
        return opnode(
            arg->arg1.s,
            arg->arg2.i,
            get_globals(arg->arg3.x),
            file);
    case F_VAR:
//        add_used(arg->arg1.s, funclevel); // bug?
        add_used(arg->arg1.s);
        if (arg->arg2.i == 0)
        {
            return arg;
        }
        return varnode(
            arg->arg1.s,
            arg->arg2.i,
            get_globals(arg->arg3.x));
    case F_DEFN:
        add_defined(arg->arg1.s);
        if (arg->arg2.i > 0)
        {
            funclevel++;
            add_formals(arg->arg3.x);
        }
        tmp = defnode(
            arg->arg1.s,
            arg->arg2.i,
            arg->arg3.x,
            get_globals(arg->arg4.x));
        if (arg->arg2.i > 0)
        {
            remove_formals();
            funclevel--;
        }
        return tmp;
    case F_NGLOBALS:
    case F_EGLOBALS:
        add_defined_list(arg->arg1.x);
        return arg;
    case F_LISTNODE:
        if (arg->arg1.x == NULL)
        {
            return listnode(
                NULL,
                get_globals(arg->arg2.x));
        }
        return listnode(
            get_globals(arg->arg1.x),
            get_globals(arg->arg2.x));
    case F_CONST:
        return arg;
    case F_EVALOF:
        valoflevel++;
        formals_list[valoflevel] = NULL;
        used_list[valoflevel] = NULL;
        defined_list[valoflevel] = NULL;
        tmp = new_ng_nglobals(
            F_EVALOF,
            get_globals(arg->arg1.x));
        valoflevel--;
        return tmp;
    case F_VALOF:
        valoflevel++;
        formals_list[valoflevel] = NULL;
        used_list[valoflevel] = NULL;
        defined_list[valoflevel] = NULL;
        tmp = new_ng_nglobals(
            F_VALOF,
            get_globals(arg->arg1.x));
        valoflevel--;
        return tmp;
    default:
        fprintf(stderr, "error in add_globals()\n");
        return arg;
    }
}

static EXPRPTR new_ng_nglobals(int type, EXPRPTR e)
{
    if (valoflevel == 1)
    {
        return valofnode(e);
    }
    EXPRPTR tmp = used_list[valoflevel];
    new_decls[valoflevel] = NULL;
    if (tmp != NULL)
    {
        if (!is_defined(tmp->arg2.s))
        {
            new_decls[valoflevel] = identlistnode(new_decls[valoflevel], tmp->arg2.s);
        }
        while (tmp->arg1.x != NULL)
        {
            tmp = tmp->arg1.x;
            if (!is_defined(tmp->arg2.s))
            {
                new_decls[valoflevel] = identlistnode(new_decls[valoflevel], tmp->arg2.s);
            }
        }
    }
    if (new_decls[valoflevel] != NULL)
    {
        tmp = append(
            listnode(
                NULL,
                nglobnode(new_decls[valoflevel])),
            e);
    }
    else
    {
        tmp = e;
    }
    if (new_decls[valoflevel] != NULL)
    {
        export_nglobals();
    }
    new_decls[valoflevel] = NULL;
    if (type == F_EVALOF)
    {
        return evalofnode(tmp);
    }
    return valofnode(tmp);
}

static void export_nglobals(void)
{
    valoflevel--;
    add_used_list(new_decls[valoflevel + 1]);
    valoflevel++;
}

static void add_used_list(EXPRPTR e)
{
    EXPRPTR tmp = e;
    if (tmp == NULL)
    {
        return;
    }
    add_used(tmp->arg2.s);
    while (tmp->arg1.x != NULL)
    {
        tmp = tmp->arg1.x;
        add_used(tmp->arg2.s);
    }
}

static int is_defined(char* s)
{
    EXPRPTR tmp = defined_list[valoflevel];
    if (tmp == NULL)
    {
        return false_;
    }
    if (eqstring(s, tmp->arg2.s))
    {
        return true_;
    }
    while (tmp->arg1.x != NULL)
    {
        tmp = tmp->arg1.x;
        if (eqstring(s, tmp->arg2.s))
        {
            return true_;
        }
    }
    return false_;
}

static void add_defined_list(EXPRPTR e)
{
    EXPRPTR tmp = e;
    if (tmp == NULL)
    {
        return;
    }
    add_defined(tmp->arg2.s);
    while (tmp->arg1.x != NULL)
    {
        tmp = tmp->arg1.x;
        add_defined(tmp->arg2.s);
    }
}

static void remove_formals(void)
{
    formals_list[valoflevel] = NULL;
}

static void add_formals(EXPRPTR e)
{
    formals_list[valoflevel] = e;
}

static void add_used(char* s)
{
    EXPRPTR tmp = formals_list[valoflevel];

    if (tmp != NULL)
    {
        if (eqstring(s, tmp->arg2.s))
        {
            return;
        }
        while (tmp->arg1.x != NULL)
        {
            tmp = tmp->arg1.x;
            if (eqstring(s, tmp->arg2.s))
            {
                return;
            }
        }
    }
    tmp = used_list[valoflevel];
    if (tmp == NULL)
    {
        used_list[valoflevel] = identlistnode(NULL, s);
        return;
    }
    if (eqstring(s, tmp->arg2.s))
    {
        return;
    }
    while (tmp->arg1.x != NULL)
    {
        tmp = tmp->arg1.x;
        if (eqstring(s, tmp->arg2.s))
        {
            return;
        }
    }
    used_list[valoflevel] = identlistnode(used_list[valoflevel], s);
}

static void add_defined(char* s)
{
    EXPRPTR tmp = defined_list[valoflevel];
    if (tmp == NULL)
    {
        defined_list[valoflevel] = identlistnode(NULL, s);
        return;
    }
    if (eqstring(s, tmp->arg2.s))
    {
        return;
    }
    while (tmp->arg1.x != NULL)
    {
        tmp = tmp->arg1.x;
        if (eqstring(s, tmp->arg2.s))
        {
            return;
        }
    }
    defined_list[valoflevel] = identlistnode(defined_list[valoflevel], s);
}

static EXPRPTR mkwhere(EXPRPTR e)
{
    char name[50];
    sprintf(name, "_%d", 1000 + varcount++);
    return wherenode(
        varnode(name, 0, NULL),
        listnode(
            NULL,
            defnode(name, 0, NULL, e)));
}

static EXPRPTR where_to_valof(EXPRPTR arg)
{
    EXPRPTR file = NULL;
    char* res = NULL;

    switch (arg->f)
    {
    case F_OP:
        file = arg->arg4.x;
        return opnode(
            arg->arg1.s,
            arg->arg2.i,
            where_to_valof(arg->arg3.x),
            file);
    case F_VAR:
        if (arg->arg2.i == 0)
        {
            return arg;
        }
        return varnode(
            arg->arg1.s,
            arg->arg2.i,
            where_to_valof(arg->arg3.x));
    case F_DECL:
        return declnode(
            arg->arg1.s,
            where_to_valof(arg->arg2.x));
    case F_DEFN:
        if (arg->arg2.i > 0 && arg->arg4.x->f != F_WHERE)
        {
            arg->arg4.x = mkwhere(arg->arg4.x);
        }
        return defnode(
            arg->arg1.s,
            arg->arg2.i,
            arg->arg3.x,
            where_to_valof(arg->arg4.x));
    case F_LISTNODE:
        if (arg->arg1.x == NULL)
        {
            return listnode(
                NULL,
                where_to_valof(arg->arg2.x));
        }
        return listnode(
            where_to_valof(arg->arg1.x),
            where_to_valof(arg->arg2.x));
    case F_CONST:
        return arg;
    case F_WHERE:
        res = strsave("_result");
        return valofnode(
            listnode(
                where_to_valof(arg->arg2.x),
                defnode(
                    res,
                    0,
                    NULL,
                    where_to_valof(arg->arg1.x))));
    default:
        fprintf(stderr, "error in parse tree\n");
        return arg;
    }
}

static EXPRPTR add_valof(EXPRPTR e)
{
    EXPRPTR file = NULL;
    char* res = NULL;

    switch (e->f)
    {
    case F_VAR:
        break;
    case F_OP:
        file = e->arg4.x;   // correct?
        break;
    case F_CONST:
        break;
    case F_VALOF:
        if (is_decl(e->arg1.x))
        {
            res = strsave("_result");
            return valofnode(
                listnode(
                    NULL,
                    defnode(res, 0, NULL, e)));
        }
        return e;
    default:
        fprintf(stderr, "error in add_valof():%d:\n", e->f);
        return e;
    }
    res = strsave("_result");
    return valofnode(
        listnode(
            NULL,
            defnode(res, 0, NULL, e)));
}

static int is_decl(EXPRPTR e)
{
    if (e->arg1.x == NULL)
    {
        return (e->arg2.x->f == F_DECL);
    }
    return (is_decl(e->arg1.x) || e->arg2.x->f == F_DECL);
}
