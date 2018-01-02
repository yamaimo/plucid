#include <stdlib.h>

#include "cmanifs.h"
#include "cglobals.h"
#include "expr.h"
#include "ident.h"

static SYMPTR find_ident(STRING name, int length);
static SYMPTR glo_ident(STRING name, int length, char attr);
static SYMPTR for_ident(STRING name, int length);
static int eqstring(STRING a, STRING b);
static int hash(STRING s);
static void free_ident(SYMPTR p);

// for debug?
#if 0
static void dumpsym(SYMPTR p);
static void dumpattr(SYMPTR x);
#endif

STRING strsave(char* s);            /* main.c */

SYMPTR handle_ident(STRING name, int length)
{
    switch (idusage)
    {
    case U_NORMAL:
        return find_ident(name, length);
    case U_FORMAL:
        return for_ident(name, length);
    case U_EGLOBAL:
        return glo_ident(name, length, (GLOB_A | ELEM_A));
    case U_NGLOBAL:
        return glo_ident(name, length, GLOB_A);
    default:
        return (SYMPTR)-1;
    }
}

static SYMPTR find_ident(STRING name, int length)
{
    int i = hash(name);
    int found = 0;
    SYMPTR scan = hashtable[i];

    while (scan && scan->lexlevel == lexlevel)
    {
        if (eqstring(scan->name, name))
        {
            found = 1;
            break;
        }
        else
        {
            scan = scan->next;
        }
    }
    if (found)
    {
        return scan;
    }
    else
    {
        SYMPTR new = (SYMPTR)calloc(1, sizeof(SYMBOL));
        new->name = strsave(name);
        new->next = hashtable[i];
        new->lexlevel = lexlevel;
        new->fldef = (f_stack->f_ll == lexlevel) ? (funclevel - 1) : funclevel;
        new->elemdef = elemlevel;
        new->idno = identcount++;
        new->attr = LOC_A;
        hashtable[i] = new;
        return new;
    }
}

static SYMPTR glo_ident(STRING name, int length, char attr)
{
    int i = hash(name);
    int found = 0;
    SYMPTR scan = hashtable[i];

    while (scan)
    {
        if (eqstring(scan->name, name))
        {
            if (scan->lexlevel == lexlevel)
            {
                my_yyerror(scan->name, " already declared in this scope");
                return scan;
            }
            else
            {
                found = 1;
            }
            break;
        }
        else
        {
            scan = scan->next;
        }
    }
    if (!found)
    {
        if (lexlevel == 1)
        {
            my_yyerror("no containing scope for ", name);
            return scan;
        }
        SYMPTR temp = (SYMPTR)calloc(1, sizeof(SYMBOL));
        temp->idno = identcount++;
        temp->name = strsave(name);
        temp->lexlevel = lexlevel-1;
        temp->elemdef = elemlevel - p_stack->p_iselem;
        temp->fldef = (f_stack->f_ll == (lexlevel - 1)) ? (funclevel - 1) : funclevel;
        temp->attr = LOC_A;
#if 0
        temp->next = hashtable[i];
        hashtable[i] = temp;
#endif
        scan = hashtable[i];
        if (scan == NULL)
        {
            hashtable[i] = temp;
            temp->next = NULL;
        }
        else
        {
            if (scan->lexlevel != lexlevel)
            {
               hashtable[i] = temp;
            }
            else
            {
                SYMPTR prev = scan;
                while (scan && scan->lexlevel == lexlevel)
                {
                    prev = scan;
                    scan = scan->next;
                }
                prev->next = temp;
            }
            temp->next = scan;
        }
        scan = temp;
    }

    /*
     * scan points to the record for the variable in the
     * previous scope.
     *
     * if the global variable we have inherited has been
     * inherited in the past, we must make sure that it is being
     * inherited in the same way this time. if it is not, we
     * have an error.
     */
    if (IS_USED(scan))
    {
        if (((scan->attr & UELEM_A) == UELEM_A) != ((attr & ELEM_A) == ELEM_A))
        {
            my_yyerror(name, " inherited inconsistently");
        }
        /*printf("%o, %o\n",scan->attr&UELEM_A,attr&ELEM_A); */
    }
    else
    {
        scan->attr |= USED_A; /* now its used */
        if (attr & ELEM_A)
        {
            scan->attr |= UELEM_A;
        }
    }
    SYMPTR new = (SYMPTR)calloc(1, sizeof(SYMBOL));
    new->next = hashtable[i];
    new->name = scan->name;
    new->idno = scan->idno;
    new->attr = attr | scan->attr;
    new->lexlevel = lexlevel;
    new->elemdef = scan->elemdef; /* elem level where ident is defined */
    new->fldef = scan->fldef;
    hashtable[i] = new;
    return new;
}

static SYMPTR for_ident(STRING name, int length)
{
    int i = hash(name);
    int found = 0;
    SYMPTR scan = hashtable[i];

    while (scan && scan->lexlevel == lexlevel)
    {
        if (eqstring(scan->name, name) && IS_FORM(scan) && !IS_GLOB(scan))
        {
            found = 1;
            break;
        }
        else
        {
            scan = scan->next;
        }
    }
    if (found)
    {
        yyerror("more than one formal of the same name");
        return scan;
    }
    else
    {
        SYMPTR new = (SYMPTR)calloc(1, sizeof(SYMBOL));
        new->name = strsave(name);
        new->idno = formalcount++;
        new->attr = FORM_A;
        new->lexlevel = lexlevel;
        new->elemdef = elemlevel;
        /* strictly speaking, formal is declared outside
         * the funtion def, and then inherited (in practice, as
         * an actual parameter
         */
        new->fldef = funclevel - 1;
        new->next = hashtable[i];
        hashtable[i] = new;
        return new;
    }
}

static int eqstring(STRING a, STRING b)
{
    while (*a++ == *b++)
    {
        if (*a == '\0' && *b == '\0')
        {
            return 1;
        }
        else if (*a == '\0' || *b == '\0')
        {
            break;
        }
    }
    return 0;
}

static int hash(STRING s)
{
    int hashval = *s;
    /*fprintf(stderr,"####%s####",s); */
    while (*s)
    {
        hashval += (*s & 0x7f);
        s++;
    }
    /*fprintf(stderr,"####%d####\n",hashval%HASHSIZE); */
    return (hashval % HASHSIZE);
}

void enter_phrase(void)
{
    lexlevel++;

    /* push the new phrase environment onto the phrase stack */
    P_STACKPTR temp = (P_STACKPTR)calloc(1, sizeof(P_STACK));
    temp->p_tl = p_stack;
    temp->p_idno = identcount++;
    temp->p_iselem = 0;
    p_stack = temp;

    /* enter the RESULT, which must occur in each scope */
    int i = hash("_result");
    SYMPTR new = (SYMPTR)calloc(1, sizeof(SYMBOL));
    new->name = strsave("_result");
    new->lexlevel = lexlevel;
    new->idno = p_stack->p_idno;
    new->attr = LOC_A;
    new->next = hashtable[i];
    hashtable[i] = new;
}

EXPRPTR exit_phrase(void)
{
    EXPRPTR new;

    for (int i = 0; i < HASHSIZE; i++)
    {
        SYMPTR scan = hashtable[i];
        while (scan && (scan->lexlevel == lexlevel))
        {
            SYMPTR temp = scan;
            scan = scan->next;
            if (IS_LOCAL(temp) && !IS_DEF(temp))
            {
                /* local is not defined. define it to be
                 * input variable.
                 */
                EXPRPTR temp2 = (EXPRPTR)calloc(1, sizeof(U_EXPR));
                temp2->dim = 0;
                temp2->f = F_INPUT;
                temp2->arg1.i = temp->idno;
                exprtable[temp->idno] = temp2;
                nametable[temp->idno] = strsave(temp->name);
            }
            free_ident(temp);   // correct?
        }
        hashtable[i] = scan;
    }
    /* p_stack has at the top the id number of the result variable
     * for this phrase. this enables us to build an expression which
     * is the value of the phrase
     */
    if (p_stack->p_iselem == 1)
    {
        new = (EXPRPTR)calloc(1, sizeof(U_EXPR));
        new->dim = 0;
        new->f = F_ERES;
        elemlevel--;
    }
    else
    {
        new = (EXPRPTR)calloc(1, sizeof(T_EXPR));
        new->dim = 0;
        new->f = F_VAR;
        new->arg2.i = 0; /* dont pop off anything */
        new->arg3.i = 0; /* place stack remains unchanged */
    }
    new->arg1.i = p_stack->p_idno;
    lexlevel--;
    P_STACKPTR old = p_stack;
    p_stack = p_stack->p_tl;
    free(old);
    return new;
}

void enter_function(void)
{
    idusage = U_FORMAL;
    formalcount = 0;
    funclevel++;

    F_STACKPTR new = (F_STACKPTR)calloc(1, sizeof(F_STACK));
    new->f_ll = lexlevel;
    new->f_el = elemlevel;
    new->f_tl = f_stack;
    f_stack = new;
}

void exit_function(void)
{
    int i;
    SYMPTR temp, scan;
    F_STACKPTR stemp;

    for (int i = 0; i < HASHSIZE; i++)
    {
        scan = hashtable[i];
        /* remove pure formals. this means that if the
         * function is defined in an outer function, we
         * do not remove its formals too, since they
         * must have been inherited into the phrase that
         * the present function is declared in, and we
         * are still in that phrase, so the earlier functions'
         * formals are globals in this scope.
         */
        while (scan && IS_FORM(scan) && (!IS_GLOB(scan)) && scan->lexlevel == lexlevel)
        {
            temp = scan;
            scan = scan->next;
            free_ident(temp);
        }
        /* scan is now the first element in the linked list
         * that is not a formal parameter.
         */
        hashtable[i] = scan;
    }
    funclevel--;
    stemp = f_stack;
    f_stack = f_stack->f_tl;
    free(stemp);
}

/* free the identifier symbol pointed to by p */
static void free_ident(SYMPTR p)
{
    free(p);
}

// for debug?
#if 0
/* dump an identifiers information to the terminal */
static void dumpsym(SYMPTR p)
{
    printf(
        "name:%s, lexlevel:%d, idno:%d, \n",
        p->name,
        p->lexlevel,
        p->idno);
    dumpattr(p);
}

static void dumpattr(SYMPTR x)
{
    if (x == NONE_A)
    {
        printf("No attributes\n");
        return;
    }
    if (IS_DEF(x))   printf("Defined ");
    if (IS_GLOB(x))  printf("Global ");
    if (IS_LOCAL(x)) printf("Local ");
    if (IS_FORM(x))  printf("Formal ");
    if (IS_ELEM(x))  printf("Elementary ");
    if (IS_USED(x))  printf("Used Before as Glob");
    printf("\n");
}
#endif
