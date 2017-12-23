#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmanifs.h"
#include "cglobals.h"

STRING fname;

int yyparse(void);  /* y.tab.c */

void initialise(void);
void connect_file(int argc, char** argv);
void output(EXPRPTR p);
STRING strsave(char* s);
int eqstring(STRING a, STRING b);
void my_exit(int n);

int main(int argc, char** argv)
{
    int i;
    int temp;

    initialise();
    connect_file(argc, argv);
    fprintf(stderr, "Compilation begins ......\n");
    i = 0;
    c = getc(lexin);
    while (c != '\n' && c != EOF)
    {
        linebuf[i] = c;
        c = getc(lexin);
        i++;
    }
    linebuf[i] = c;
    curr_index = 0;
    curr_length = i;
    temp = yyparse();
    if (temp || errcount)
    {
        fprintf(stderr, "Fatal errors: no expression file written.\n");
        my_exit(1);
    }

    return 0;
}

/*
void accept(void)
{
    if (fopen(fname, "w") == NULL)
    {
        fprintf(stderr, "cannot create %s\n", fname);
        my_exit(1);
    }
}
*/

void initialise(void)
{
    int i;

    /* name the directory containing the pLucid library */
    incdir = "/usr/local/lib";

    /* initialise the simple variables */

    const_list = 0;
    errcount = 0;
    peekc = 0;
    expvalcnt=0;
    startoflex =0;

    for (i = 0; i < NESTINGDEPTH; i++)
    {
        exprlistcount[i] = 0;
        expval[i] = NULL;
    }

    /* predefined symbolic words */

    false_ = 0;
    true_ = 1;

    newline = false_;
    /* No newline character has been encountered */
}

void connect_file(int argc, char** argv)
{
    /* connect to source file */
    if (argc > 1)
    {
        fname = argv[1];
        in_index = 0;
        for (int j = 0; fname[j]; j++) ;
        if ((lexin = fopen(fname, "r")) == NULL)
        {
            fprintf(stderr, "cannot open %s\n", fname);
            my_exit(1);
        }
        in_files[in_index].in_name = strsave(fname);
        sprintf(fname, "%s", argv[1]);
    }
    else
    {
        in_files[in_index].in_name = "stdin";
        fname = "?.1";
        lexin = stdin;
    }
    in_files[in_index].in_line = 1;
    in_files[in_index].in_fdes = lexin;
}

void output(EXPRPTR p)
{
    EXPRPTR tmp;

    switch (p->f)
    {
    case F_CONST:
        if (eqstring(p->arg1.s, "string"))
        {
            fprintf(
                stdout,
                " [ const [ string `%s' ] ",
                p->arg2.s);
            fprintf(stdout, " ] ");
            return;
        }
        if (eqstring(p->arg1.s, "word"))
        {
            fprintf(
                stdout,
                " [ const [ word `%s' ] ",
                p->arg2.s);
            fprintf(stdout, " ] ");
            return;
        }
        if (eqstring(p->arg1.s, "numb"))
        {
            if (p->arg2.r < 0)
            {
                fprintf(
                    stdout,
                    " [ const [ numb ~%-10.5f ] ",
                    -p->arg2.r);
            }
            else
            {
                fprintf(
                    stdout,
                    " [ const [ numb %-10.5f ] ",
                    p->arg2.r);
            }
        }
        else
        {
            fprintf(
                stdout,
                " [ const [ special `%s' ] ",
                p->arg2.s);
        }
        fprintf(stdout, " ] ");
        return;
    case F_VAR:
        fprintf(
            stdout,
            " [ var %s %d ",
            p->arg1.s,
            p->arg2.i);
        if (p->arg3.x != NULL)
        {
            output(p->arg3.x);
        }
        output(p->arg4.x);
        fprintf(stdout, " ] ");
        return;
    case F_OP:
        fprintf(
            stdout,
            "[ op %s %d ",
            p->arg1.s,
            p->arg2.i);
        if (p->arg3.x != NULL)
        {
            output(p->arg3.x);
        }
        output(p->arg4.x);
        fprintf(stdout," ] ");
        return;
    case F_WHERE:
        fprintf(stdout, " [ where ");
        output(p->arg1.x);
        output(p->arg2.x);
        output(p->arg3.x);
        fprintf(stdout," ] ");
        return;
    case F_DEFN:
        fprintf(
            stdout,
            " [ defn %s %d ",
            p->arg1.s,
            p->arg2.i);
        if (p->arg2.i > 0)
        {
            output(p->arg3.x);
        }
        output(p->arg4.x);
        output(p->arg5.x);
        fprintf(stdout, " ] ");
        return;
    case F_DECL:
        fprintf(stdout, " [ decl %s ", p->arg1.s);
        output(p->arg2.x);
        output(p->arg3.x);
        fprintf(stdout, " ] ");
        return;
    case F_IDENTLISTNODE:
        if (p->arg1.x==NULL)
        {
            fprintf(stdout, " %s ", p->arg2.s);
            return;
        }
        output(p->arg1.x);
        fprintf(stdout, " %s ", p->arg2.s);
        return;
    case F_LISTNODE:
        if (p->arg1.x == NULL)
        {
            output(p->arg2.x);
            return;
        }
        output(p->arg1.x);
        output(p->arg2.x);
        return;
    case F_BODY:
        if (p->arg1.x == NULL)
        {
            output(p->arg2.x);
            return;
        }
        output(p->arg1.x);
        output(p->arg2.x);
        return;
    case F_FILE:
        fprintf(
            stdout,
            " [ file `%s' %d %d %d ] ",
            p->arg1.s,
            p->arg2.i,
            p->arg3.i,
            p->arg4.i);
        return;
    default:
        fprintf(stderr, "UNKNOWN NODE IN PARSE TREE\n");
        return;
    }
}

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

int eqstring(STRING a, STRING b)
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

void my_exit(int n)
{
    fprintf(stdout, "%c\n", '\032');
    exit(n);
}
