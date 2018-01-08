#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "cmanifs.h"
#include "cglobals.h"

#include "expr.h"

int output(EXPRPTR p);      /* main.c */
STRING strsave(char* s);    /* main.c */

static float mknumber(char* s);

void pass1(EXPRPTR e)
{
    output(e);
}

/*
EXPRPTR stripstring(char* s)
{
    while (isalpha(*s) || *s == '@') s++;
    return connode(strsave("string"), s + 1);
}
*/

float stripcode(char* s)
{
    if (strcmp(s, "swap_t0s0")) return  1.0f;
        /* is this correct? if (strcmp(s, "swap_t0s0") == 0) ... */
    if (strcmp(s, "swap_t0s1")) return  2.0f;
    if (strcmp(s, "swap_t0s2")) return  3.0f;
    if (strcmp(s, "swap_t1s1")) return  4.0f;
    if (strcmp(s, "swap_t1s2")) return  5.0f;
    if (strcmp(s, "swap_t2s3")) return  6.0f;
    if (strcmp(s, "swap_t0t1")) return  7.0f;
    if (strcmp(s, "swap_t0t2")) return  8.0f;
    if (strcmp(s, "swap_t0t3")) return  9.0f;
    if (strcmp(s, "swap_t1t2")) return 10.0f;
    if (strcmp(s, "swap_t1t3")) return 11.0f;
    if (strcmp(s, "swap_t2t3")) return 12.0f;
    if (strcmp(s, "swap_s0s1")) return 13.0f;
    if (strcmp(s, "swap_s0s2")) return 14.0f;
    if (strcmp(s, "swap_s0s3")) return 15.0f;
    if (strcmp(s, "swap_s1s2")) return 16.0f;
    if (strcmp(s, "swap_s1s3")) return 17.0f;
    if (strcmp(s, "swap_s2s3")) return 18.0f;
    return 999.0f;
}

float stripn(char* s)
{
    while (isalpha(*s)|| *s == '@') s++;
    return mknumber(s);
}

EXPRPTR stripnumb(char* s)
{
    while (isalpha(*s) || *s == '@') s++;
    return f_connode(mknumber(s));
}

static float mknumber(char* s)
{
    int n = 0;

    while (('0' <= *s) && (*s <= '9'))
    {
        n = n * 10 + *s - '0';
        s++;
    }

    return (float)n;
}

STRING stripname(char* s)
{
    char buff[20];  /* FIXME: max length of name is 19 */
    int n;
    char* p = buff;

    while (isalpha(*s) || *s == '@')
    {
        *p++ = *s++;
    }
    *p = '\0';

    return strsave(buff);
}

EXPRPTR filenode(int first_line, int cursor_position)
{
	EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR4));

	p->f = F_FILE;
	p->arg1.s = in_files[in_index].in_name;
	p->arg2.i = first_line;
	p->arg3.i = in_files[in_index].in_line;
	p->arg4.i = cursor_position;

	return p;
}

EXPRPTR connode(char* s1, char* s2)
{
	EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR2));

	p->f = F_CONST;
	p->arg1.s = s1;
	p->arg2.s = s2;

	return p;
}

/*
EXPRPTR lu_connode(STRING s, float f)
{
	EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR2));

	p->f = F_CONST;
	p->arg1.s = s;
	p->arg2.r = f;

	return p;
}
*/

EXPRPTR f_connode(float f)
{
	EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR2));

	p->f = F_CONST;
	p->arg1.s = "numb";
	p->arg2.r = f;

	return p;
}

EXPRPTR varnode(char* name, int argcount, EXPRPTR exprlist, EXPRPTR file)
{
	EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR4));

	p->f = F_VAR;
	p->arg1.s = name;
	p->arg2.i = argcount;
	p->arg3.x = exprlist;
	p->arg4.x = file;

	return p;
}

EXPRPTR opnode(char* name, int argcount, EXPRPTR exprlist, EXPRPTR file)
{
	EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR4));

	p->f = F_OP;
	p->arg1.s = name;
	p->arg2.i = argcount;
	p->arg3.x = exprlist;
	p->arg4.x = file;

	return p;
}

EXPRPTR wherenode(EXPRPTR expr, EXPRPTR exprlist, EXPRPTR file)
{
	EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR3));

	p->f = F_WHERE;
	p->arg1.x = expr;
	p->arg2.x = exprlist;
	p->arg3.x = file;

	return p;
}

EXPRPTR defnode(char* name, int argcount, EXPRPTR argnames, EXPRPTR expr, EXPRPTR file)
{
	EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR5));

	p->f = F_DEFN;
	p->arg1.s = name;
	p->arg2.i = argcount;
	p->arg3.x = argnames;
	p->arg4.x = expr;
	p->arg5.x = file;

	return p;
}

EXPRPTR declnode(char* name, EXPRPTR expr, EXPRPTR file)
{
	EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR3));

	p->f = F_DECL;
	p->arg1.s = name;
	p->arg2.x = expr;
	p->arg3.x = file;

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

EXPRPTR bodynode(EXPRPTR expr, EXPRPTR tail)
{
	EXPRPTR p = (EXPRPTR)calloc(1, sizeof(EXPR2));

	p->f = F_BODY;
	p->arg1.x = expr;
	p->arg2.x = tail;

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

int yyerror(STRING a)
{
    fprintf(stderr, "%s\n", a);
    return 0;
}

int my_yyerror(STRING a, STRING b)
{
	fprintf(
        stderr,
        "on line %d of file %s\n",
        in_files[in_index].in_line,
        in_files[in_index].in_name);
	for (int i = 0; i <= curr_length; i++)
    {
        fprintf(stderr, "%c", linebuf[i]);
    }
    for (int i = 0; i < curr_index - 1; i++)
    {
        putc('.', stderr);
    }
    fprintf(stderr, "^\n%s %s\n", a, b);

	errcount++;

    return 0;
}
