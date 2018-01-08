#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmanifs.h"
#include "cglobals.h"

int yyparse(void);      /* y.tab.c */
int findword(STRING s); /* yylex.c */

STRING strsave(char* s);
void echoexpr(EXPRPTR p);
void my_exit(int n);

static void initialise(int argc, char** argv);
static void accept(void);
static void writename(STRING s);
static void writeexpr(EXPRPTR p);

STRING fname;

STRING strsave(char* s)
{
    char* p;
    if ((p = calloc(1, strlen(s) + 1)) == NULL)
    {
        fprintf(stderr, "ran out of space\n");
    }
    else
    {
        strcpy(p, s);
    }
    return p;
}

int main(int argc, char** argv)
{
    int temp;

    initialise(argc, argv);
    temp = yyparse();
    if (temp || errcount)
    {
        fprintf(stderr, "Fatal errors: no expression file written.\n");
        my_exit(1);
    }
    else
    {
        accept();
    }
    return 0;
}

static void initialise(int argc, char** argv)
{
    /* connect to source file */
    if (argc > 1)
    {
        int j;
        fname = argv[1];
        in_index = 0;
        for (j = 0; fname[j]; j++) ;
// can't work correctly?
#if 0
        if (j < 2 || fname[j - 1] != 'h' || fname[j-2] != '.')
        {
            fprintf(
                stderr,
                "%s: not a legal luthid source file\n",
                fname);
            my_exit(1);
        }
#endif
        /* sprintf(fname, "%s.i", fname); */
        fname = malloc(strlen(fname) + 3);
        sprintf(fname, "%s.i", argv[1]);
// can't work correctly?
#if 0
        if ((lexin = fopen(fname, "r")) == NULL)
        {
            fprintf(stderr, "cannot open %s\n", fname);
            my_exit(1);
        }
#endif
        lexin = stdin;
        in_files[in_index].in_name = strsave(fname);
        /*fname[j-1] = 'i'; */
    }
    else
    {
        in_files[in_index].in_name = "stdin";
        fname = "stdin.i";
        lexin = stdin;
    }
    savelex = lexin;
    in_files[in_index].in_line = 0;
    in_files[in_index].in_fdes = lexin;

    /* hashtable is all NULLS */
    for (int i = 0; i < HASHSIZE; i++)
    {
        hashtable[i] = NULL;
    }

    /* predefined symbolic atoms */
    wordcount = 0;
    false_ = findword("false"); // false_ must be 0
    true_ = findword("true");   // true_ must be 1

    /* initialise various things */
    cconst = false_;
    wordval = false_;
    stringcount = 0;
    filecount = 0;
    errcount = 0;
    lexlevel = 0;
    funclevel = 0;
    identcount = 0;
    idusage = U_NORMAL;
    peekc = 0;

    p_stack = (P_STACKPTR)calloc(1, sizeof(P_STACK));
    p_stack->p_idno = -1;
    p_stack->p_iselem = 0;
    p_stack->p_tl = NULL;

    parm_stack = (E_STACKPTR)calloc(1, sizeof(E_STACK));
    parm_stack->es_list = NULL;
    parm_stack->es_tl = NULL;

    f_stack = (F_STACKPTR)calloc(1, sizeof(F_STACK));
    f_stack->f_ll = -1;
    f_stack->f_el = -1;
    f_stack->f_tl = NULL;

    parm_list = (E_LISTPTR)calloc(1, sizeof(E_LIST));
    parm_list->el_expr = NULL;
    parm_list->el_tl = NULL;
}

static void accept(void)
{
    int flag = false_;  // never become true_...

    if ((outfile = fopen(fname, "w")) == NULL)
    {
        fprintf(stderr, "cannot create %s\n", fname);
        my_exit(1);
    }
    putw(identcount, outfile);
    if (flag)
    {
        fprintf(stderr, "number of expressions is %d\n", identcount);
    }
    for (int i = 0; i < identcount; i++)
    {
        writename(nametable[i]);
        if (flag)
        {
            fprintf(stderr, "%d : %s :",  i, nametable[i]);
            echoexpr(exprtable[i]);
            fprintf(stderr, "\n");
        }
        writeexpr(exprtable[i]);
    }
    putw(wordcount, outfile);
    if (flag)
    {
        fprintf(stderr, "number of words is %d\n", wordcount);
    }
    for (int i = 0; i < wordcount; i++)
    {
        putw(strlen(wordtable[i]), outfile);
        writename(wordtable[i]);
        if (flag)
        {
            fprintf(stderr, "%d : `%s\'\n", i, wordtable[i]);
        }
    }
    if (flag)
    {
        fprintf(stderr, "Compilation complete\n");
    }
}

static void writename(STRING s)
{
    for (char* p = s; *p != '\0'; p++)
    {
        fputc(*p, outfile);
    }
    fputc('\n', outfile);
}

static void writeexpr(EXPRPTR p)
{
    if (p == NULL)
    {
        return;
    }
    putw(p->f, outfile);

    FPTR q = &ftable[p->f];
    int n = q->nargs;
    int dim;
    switch (q->type)
    {
    case INTERIOR:
        dim = p->dim;
        putw(dim, outfile);
        writeexpr(p->arg1.x);
        if (n > 1)
        {
            writeexpr(p->arg2.x);
        }
        if (n > 2)
        {
            writeexpr(p->arg3.x);
        }
        if (n > 3)
        {
            writeexpr(p->arg4.x);
        }
        if (n > 4)
        {
            writeexpr(p->arg5.x);
        }
        break;
    default:
        putw(p->arg1.i,outfile);
        if (n > 1)
        {
            putw(p->arg2.i, outfile);
        }
        if (n > 2)
        {
            putw(p->arg3.i, outfile);
        }
        if (n > 3)
        {
            putw(p->arg4.i, outfile);
        }
        if (n > 4)
        {
            putw(p->arg5.i, outfile);
        }
    }
}

void echoexpr(EXPRPTR p)
{
    if (p == NULL)
    {
        return;
    }

    FPTR q = &ftable[p->f];
    int n = q->nargs;
    int dim;
    switch (q->type)
    {
    case INTERIOR:
        dim = p->dim;
        if (dim == 0)
        {
            fprintf(stderr, "%s(", q->name);
        }
        else
        {
            fprintf(stderr, "%s%d(,", q->name, dim);
        }
        echoexpr(p->arg1.x);
        if (n > 1)
		{
            fprintf(stderr, ", ");
            echoexpr(p->arg2.x);
        }
        if (n > 2)
		{
            fprintf(stderr, ", ");
            echoexpr(p->arg3.x);
        }
        if (n > 3)
		{
            fprintf(stderr, ", ");
            echoexpr(p->arg4.x);
        }
        if (n > 4)
		{
            fprintf(stderr, ", ");
            echoexpr(p->arg5.x);
        }
        fprintf(stderr, ")");
        break;
    default:
        fprintf(stderr, "%s( ", q->name);
        if (p->f == F_CONST)
        {
            fprintf(stderr,"%f ",p->arg1.r);
        }
        else
        {
            fprintf(stderr,"%d ",p->arg1.i);
        }
        if (n > 1)
        {
            fprintf(stderr, ", %d", p->arg2.i);
        }
        if (n > 2)
        {
            fprintf(stderr, ", %d", p->arg3.i);
        }
        if (n > 3)
        {
            fprintf(stderr, ", %d", p->arg4.i);
        }
        if (n > 4)
        {
            fprintf(stderr, ", %d", p->arg5.i);
        }
        fprintf(stderr, ")");
    }
}

void my_exit(int n)
{
    exit(n);
}
