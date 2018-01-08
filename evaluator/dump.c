#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/times.h>

#include "imanifs.h"
#include "iglobals.h"
#include "dump.h"

static void dump_uctable(char* s);
static int dump_getc(FILE* readfile, char* filename);
static void dumpstphash(int i, STPPTR table[]);
static void char_to_code(char c);
static void dumpword(int x, FILE* channel);

static char charcode[5];

void bar(int x)
{
    if (x > 80)
    {
        x = 80;
    }
    for (int i = 0; i < x; i++)
    {
        fprintf(stderr, "|");
    }
}

static void dump_uctable(char* s)
{
    FILE* ucout;
    char t[100];

    sprintf(t, "%s.uc", s);
    if ((ucout = fopen(t, "w")) == NULL)
    {
        fprintf(stderr, "cannot write to file %s\n", s);
    }
    else
    {
        for(int i = 0; i < exprquan; i++)
        {
            fprintf(ucout, "%d\n", u_countvec[i]);
        }
    }
}

void my_exit(int n)
{
    if (isatty(fileno(stdout)))
    {
        stats();
    }
    dump_uctable(infilename);
//    exit(2);  // bug?
    exit(n);
}

/*
void dumpspaces(int n)
{
    for (int i = 0; i < n; i++)
    {
        fprintf(stderr, " ");
    }
}
*/

void stats(void)
{
    struct tms buff;
    times(&buff);

    time_t syst = (time_t)(buff.tms_stime / 60);
    time_t ut =   (time_t)(buff.tms_utime / 60);

    fprintf(stderr, "\n\n");
    fprintf(stderr, "Statistical Information about the evaluation\n");
    D(7) {
        fprintf(
            stderr,
            "# of small records recycled    = %10d (%10d bytes)\n",
            srecycle,
            12 * srecycle);
        fprintf(
            stderr,
            "# of large records recycled    = %10d (%10d bytes)\n",
            brecycle,
            24 * brecycle);
        fprintf(
            stderr,
            "Total # of records recycled    = %10d (%10d bytes)\n",
            srecycle + brecycle,
            12 * srecycle + 24 * brecycle);
        fprintf(
            stderr,
            "# of small records used        = %10d (%10d bytes) \n",
            s_count,
            12 * s_count);
        fprintf(
            stderr,
            "# of large records used        = %10d (%10d bytes) \n",
            b_count,
            24 * b_count);
        fprintf(
            stderr,
            "Total # of records used        = %10d (%10d bytes) \n",
            b_count + s_count,
            12 * s_count + 24 * b_count);
        fprintf(
            stderr,
            "# of demands for variables     = %10d\n",
            varcount);
        fprintf(
            stderr,
            "# demands with computation     = %10d\n",
            notfoundcount);
        fprintf(
            stderr,
            "user time                      = %10d secs\n",
            (int)ut);
        fprintf(
            stderr,
            "system time                    = %10d secs\n",
            (int)syst);
        fprintf(
            stderr,
            "total CPU time                 = %10d secs\n",
            (int)(ut + syst));
        fprintf(
            stderr,
            "%f eductions per second including system calls\n",
            (double)varcount / (syst + ut));
        fprintf(
            stderr,
            "%f eductions per second\n",
            (double)varcount / ut);
    }
    fprintf(
        stderr,
        "# of memory buckets created    = %10d\n",
        memcount);
    fprintf(
        stderr,
        "# of space/time/place tags created   = %10d\n",
        stpcount);
}

void dumpfile(char* x, EXPRPTR e)
{
    char* filename = wordtable[e->arg1.i];
    int firstline = e->arg2.i;
    int lastline = e->arg3.i;
    int cursorposition = e->arg4.i;

    fprintf(stderr, "\nEvaluation time error ");
    fprintf(stderr, "on line %d, ", firstline);
    fprintf(stderr, "of file %s\n ", filename);

    FILE* readfile = fopen(filename, "r");
    if (readfile == NULL)
    {
        fprintf(stderr, "\nfile %s cannot be opened to read\n", filename);
        return;
    }

    int lineno = 1;
    while (lineno < firstline)
    {
        char ch = dump_getc(readfile, filename);
        if (ch == '\n')
        {
            lineno++;
        }
    }

    for (char ch = dump_getc(readfile, filename);
        ch != '\n';
        ch = dump_getc(readfile, filename))
    {
        fprintf(stderr, "%c", ch);
    }
    fprintf(stderr, "\n");

    for (int i = 0; i < cursorposition; i++)
    {
        fprintf(stderr, ".");
    }
    fprintf(stderr, "^");
    fprintf(stderr, "\n%s", x);
    fclose(readfile);
}

static int dump_getc(FILE* readfile, char* filename)
{
    int ch = getc(readfile);
    if (ch == EOF)
    {
        fprintf(stderr, "unexpected end to file %s\n", filename);
        my_exit(1); // infinite loop...
    }
    return ch;
}

void dumpstp(STPPTR x)
{
    STPPTR s = x;
    fprintf(stderr, "(");
    while (s != NULL)
    {
        fprintf(stderr, "%ld", s->stphd.word);
        s = s->stptl;
        if (s != NULL)
        {
            fprintf(stderr, ",");
        }
    }
    fprintf(stderr,")");
}

void dumps(STPPTR x)
{
    STPPTR s = x;
    fprintf(stderr, "(");
    while (s != NULL)
    {
        fprintf(
            stderr,
            "<%d,%d,%d>",
            s->stphd.xyz.d1,
            s->stphd.xyz.d2,
            s->stphd.xyz.d3);
        s = s->stptl;
        if (s != NULL)
        {
            fprintf(stderr, ",");
        }
    }
    fprintf(stderr, ")");
}

void dumpt(STPPTR x)
{
    fprintf(stderr, "(");
    fprintf(stderr, "%3ld", x->stphd.word);
    fprintf(stderr, ")");
}

static void dumpstphash(int i, STPPTR table[])
{
    fprintf(stderr, "Bucket %d ", i);
    for (STPPTR p = table[i]; p != NULL; p = p->stplink)
    {
        dumpstp(p);
    }
    fprintf(stderr, "\n");
}

// can't work correctly?
#if 0
static void dumpnhash(int i)
{
    if (nhashtab[i] != NULL)
    {
        fprintf(stderr, "bucket %d\n", i);
        for (MEMPTR s = nhashtab[i]; s != NULL; s = s->m_next)
        {
            dumpmem(s);
        }
    }
}
#endif

void dumpmem(MEMPTR x)
{
    int crcint;
    int icrcint;
    float rlval;
    float ilval;

    fprintf(
        stderr,
        "name: %d %s ",
        x->data.bits.n,
        nametable[x->data.bits.n]);
    fprintf(stderr, "time: ");
    dumpstp(x->t);
    fprintf(stderr, "place: ");
    dumpstp(x->p);
    fprintf(stderr, "value: ");
    switch (x->v.v_type)
    {
    case NUMERIC:
        icrcint = x->v.v_value.na.i;
        crcint = x->v.v_value.na.r;
        ilval = icrcint;
        rlval = crcint;
        if (x->v.v_value.na.i == 0)
        {
            if (rlval == x->v.v_value.na.r)
            {
                if ((int)(x->v.v_value.na.r) < 0)
                {
                    fprintf(
                        stderr,
                        "~%-d ",
                        -(int)(x->v.v_value.na.r));
                }
                else
                {
                    fprintf(
                        stderr,
                        "%-d ",
                        (int)(x->v.v_value.na.r));
                }
            }
            else
            {
                if (x->v.v_value.na.r < 0)
                {
                    fprintf(
                        stderr,
                        "~%-10.5f ",
                        -x->v.v_value.na.r);
                }
                else
                {
                    fprintf(
                        stderr,
                        "%-10.5f ",
                        x->v.v_value.na.r);
                }
            }
        }
        else
        {
            if (x->v.v_value.na.r < 0)
            {
                fprintf(
                    stderr,
                    "~(%-10.5f,%-10.5f)",
                    -x->v.v_value.na.r,
                    x->v.v_value.na.i);
            }
            else
            {
                fprintf(
                    stderr,
                    "(%-10.5f,%-10.5f)",
                    x->v.v_value.na.r,
                    x->v.v_value.na.i);
            }
        }
        break;
    case WORD:
        dumpword(x->v.v_value.wrd, stderr);
        break;
    case QSTRING:
        dumpstring(x->v.v_value.strg, stderr, false_);
        break;
    case NIL:
        fprintf(stderr, "[]\n");
        break;
    case ERROR:
        fprintf(stderr, "?");
        break;
    case DOTTED_PAIR:
        dumplist(x->v.v_value.dp, stderr);
        break;
    case EOD:
        fprintf(stderr, "@ \n");
        break;
    }
}

void dumpmemry(char type, WORDCELL value)
{
    int crcint;
    float rlval;
    switch (type)
    {
    case NUMERIC:
        crcint = value.na.r;
        rlval = crcint;
        if (rlval == value.na.r)
        {
            if ((int)(value.na.r) < 0)
            {
                fprintf(stderr, "~%-d\n", -(int)(value.na.r));
            }
            else
            {
                fprintf(stderr, "%-d\n", (int)(value.na.r));
            }
        }
        else {
            if (value.na.r < 0)
            {
                fprintf(stderr, "~%-10.5f\n", -value.na.r);
            }
            else
            {
                fprintf(stderr, "%-10.5f\n", value.na.r);
            }
        }
        break;
    case WORD:
        dumpword(value.wrd, stderr);
        fprintf(stderr, "\n");
        break;
    case QSTRING:
        dumpstring(value.strg, stderr, false_);
        fprintf(stderr, "\n");
        break;
    case NIL:
        fprintf(stderr, "[]\n");
        break;
    case ERROR:
        fprintf(stderr, "?\n");
        break;
    case DOTTED_PAIR:
        dumplist(value.dp, stderr);
        fprintf(stderr, "\n");
        break;
    }
}

void dumpval(FILE* stream, VALUEPTR x)
{
    int crcint;
    int icrcint;
    float rlval;
    float ilval;

    switch (x->v_type)
    {
    case NUMERIC:
        icrcint = x->v_value.na.i;
        crcint = x->v_value.na.r;
        ilval = icrcint;
        rlval = crcint;
        if (x->v_value.na.i == 0)
        {
            if (rlval == x->v_value.na.r)
            {
                if ((int)(x->v_value.na.r) < 0)
                {
                    E(13) {
                        fprintf(stream, "~%-9d", -(int)(x->v_value.na.r));
                    }
                    else
                    {
                        fprintf(stream, "%d", (int)(x->v_value.na.r));
                    }
                }
                else E(13) {
                    fprintf(stream, "%d", (int)(x->v_value.na.r));
                }
                else
                {
                    fprintf(stream, "%d", (int)(x->v_value.na.r));
                }
            }
            else if (x->v_value.na.r < 0)
            {
                E(13) {
                    fprintf(stream, "~%-9f", -x->v_value.na.r);
                }
                else
                {
                    fprintf(stream, "%10f", x->v_value.na.r);
                }
            }
            else E(13) {
                fprintf(stream, "%-10f", x->v_value.na.r);
            }
            else
            {
                fprintf(stream, "%-10f", x->v_value.na.r);
            }
        }
        else
        {
            E(13) {
                fprintf(
                    stream,
                    "(%-10f,%-10f)",
                    x->v_value.na.r,
                    x->v_value.na.i);
            }
            else
            {
                fprintf(
                    stream,
                    "(%10f,%10f)",
                    x->v_value.na.r,
                    x->v_value.na.i);
            }
        }
        break;
    case WORD:
        dumpword(x->v_value.wrd, stream);
        break;
    case QSTRING:
        dumpstring(x->v_value.strg, stream, sflag);
        break;
    case NIL:
        fprintf(stream, "        []");
        break;
    case ERROR:
        fprintf(stream, "         ?");
        break;
    case DOTTED_PAIR:
        dumplist(x->v_value.dp, stream);
        break;
    case EOD:
        fprintf(stream, "        @");
        break;
    }
    if (!sflag)
    {
        fprintf(stream, " ");
    }
}

STRING code_to_char(STRING s)
{
    char newstring[200];
    int l = 0;
    int i = 0;
    char c = s[i];

    while (c != '\0')
    {
        if (c == '\\')
        {
            i++;
            c = s[i];
            switch (c)
            {
            case 'n':
                newstring[l] = '\n';
                l++;
                break;
            case 't':
                newstring[l] = '\t';
                l++;
                break;
            case 'f':
                newstring[l] = '\f';
                l++;
                break;
            case 'b':
                newstring[l] = '\b';
                l++;
                break;
            case 'r':
                newstring[l] = '\r';
                l++;
                break;
            case '\\':
                newstring[l] = '\\';
                l++;
                break;
            case '\'':
                newstring[l] = '\'';
                l++;
                break;
            default:
                if ('0' <= c && c <= '7')
                {
                    int sum = 0;
                    for (int j = 1; j <= 3 && '0' <= c && c <= '7'; j++)
                    {
                        sum = sum * 8 + c - '0';
                        i++;
                        c = s[i];
                    }
                    newstring[l] = sum;
                    l++;
                }

                newstring[l] = c;
                l++;
            }
        }
        else
        {
            newstring[l] = c;
            l++;
        }
        i++;
        c = s[i];
    }
    newstring[l] = '\0';
    strcpy(s, newstring);
    return s;
}

static void char_to_code(char c)
{
    charcode[0]='\\';
    switch (c)
    {
    case '\n':
        charcode[1] = 'n';
        break;
    case '\t':
        charcode[1] = 't';
        break;
    case '\f':
        charcode[1] = 'f';
        break;
    case '\b':
        charcode[1] = 'b';
        break;
    case '\r':
        charcode[1] = 'r';
        break;
    case '\\':
        charcode[1] = '\\';
        break;
    case '\'':
        charcode[1] = '\'';
        break;
    default:
        if (!isprint(c) && !isspace(c))
        {
            sprintf(charcode, "\\%o", c);
            return;
        }
        else
        {
            charcode[0] = c;
            charcode[1] = '\0';
            return;
        }
    }
    charcode[2] = '\0';
}

/*
void pstring(char* s, FILE* channel)
{
    int i = 0;
    char c = s[i];
    while (c != '\0')
    {
        fprintf(channel, "%c", c);
        i++;
        c = s[i];
    }
}
*/

void dumpval2(FILE* stream, VALUE x)
{
    int crcint;
    float rlval;

    switch (x.v_type)
    {
    case NUMERIC:
        crcint = x.v_value.na.r;
        rlval = crcint;
        if (rlval == x.v_value.na.r)
        {
            if ((int)(x.v_value.na.r) < 0)
            {
                fprintf(stream, "~%-d", -(int)(x.v_value.na.r));
            }
            else
            {
                fprintf(stream, "%-d", (int)(x.v_value.na.r));
            }
        }
        else
        {
            if (x.v_value.na.r < 0)
            {
                fprintf(stream, "~%-10.5f", -x.v_value.na.r);
            }
            else
            {
                fprintf(stream, "%-10.5f", x.v_value.na.r);
            }
        }
        break;
    case WORD:
        dumpword(x.v_value.wrd, stream);
        break;
    case QSTRING:
        dumpstring(x.v_value.strg, stream, current_io->outmode == UNIX);
        break;
    case ERROR:
        fprintf(stream, "?");
        break;
    case NIL:
        fprintf(stream, "[]");
        break;
    case DOTTED_PAIR:
        dumplist(x.v_value.dp, stream);
        break;
    case EOD:
        fprintf(stream, "EOD");
        break;
    }
}

void dumplist(CELLPTR x, FILE* channel)
{
    float rlval;
    int crcint;
    fprintf(channel,"[");
    while (1)
    {
        if (x->data.bits.d_hd == NUMERIC)
        {
            crcint = (int)x->hd.na.r;
            rlval = (float)crcint;
            if (rlval == x->hd.na.r)
            {
                if ((int)(x->hd.na.r) < 0)
                {
                    fprintf(channel, "~%d", (int)(-x->hd.na.r));
                }
                else
                {
                    fprintf(channel, "%d", (int)(x->hd.na.r));
                }
            }
            else
            {
                if (x->hd.na.r < 0)
                {
                    fprintf(channel, "~%10.5f", -x->hd.na.r);
                }
                else
                {
                    fprintf(channel, "%10.5f", x->hd.na.r);
                }
            }
        }
        else if (x->data.bits.d_hd == WORD)
        {
            dumpword(x->hd.wrd, channel);
        }
        else if (x->data.bits.d_hd == QSTRING)
        {
            dumpstring(x->hd.strg, channel, sflag);
        }
        else if (x->data.bits.d_hd == NIL)
        {
            fprintf(channel, "[]");
        }
        else if (x->data.bits.d_hd == DOTTED_PAIR)
        {
            dumplist(x->hd.dp, channel);
        }
        else
        {
            fprintf(stderr, "messed up list");
            break;
        }
        if (x->data.bits.d_tl == NUMERIC)
        {
            crcint = (int)x->tl.na.r;
            rlval = (float)crcint;
            if (rlval == x->tl.na.r)
            {
                if ((int)(x->tl.na.r) < 0)
                {
                    fprintf(channel, "~%d", (int)(-x->tl.na.r));
                }
                else
                {
                    fprintf(channel, "%d", (int)(x->tl.na.r));
                }
            }
            else
            {
                if (x->tl.na.r < 0 )
                {
                    fprintf(channel, "~%10.5f", -x->tl.na.r);
                }
                else
                {
                    fprintf(channel, "%10.5f", x->tl.na.r);
                }
            }
            break;
        }
        else if (x->data.bits.d_tl == NIL)
        {
            break;
        }
        else if (x->data.bits.d_tl == WORD)
        {
            dumpword(x->tl.wrd, channel);
            break;
        }
        else if (x->data.bits.d_tl == QSTRING)
        {
            dumpstring(x->tl.strg, channel, sflag);
            break;
        }
        else if (x->data.bits.d_tl == DOTTED_PAIR)
        {
            fprintf(channel, " ");
            x = x->tl.dp;
        }
        else
        {
            fprintf(stderr, "messed up list two");
            break;
        }
    }
    fprintf(channel, "]");
}

static void dumpword(int x, FILE* channel)
{
    fprintf(channel, "%10s", wordtable[x]);
}

void dumpstring(CELLPTR x, FILE* channel, char flag)
{
    if (!flag)
    {
        fprintf(channel, "`");
    }
    while (1)
    {
        char c = x->hd.swch;
        if (x->data.bits.d_tl == NIL)
        {
            if (!flag)
            {
                fprintf(channel,"\'");
            }
            return;
        }
        if (flag)
        {
            fprintf(channel, "%c", c);
        }
        else
        {
            char_to_code(c);
            fprintf(channel, "%s", charcode);
        }
        x = x->tl.strg;
    }
}

void dumphashes(void)
{
    // can't work correctly?
#if 0
    fprintf(stderr, "\n\n\n\n*****NAMES******\n");
    for(int i = 0; i < NHASHSIZE; i++)
    {
        if (nhashtab[i] != NULL)
        {
            dumpnhash(i);
        }
    }
#endif
    fprintf(stderr, "\n\n\n\n\n*****TIMES******\n");
    for (int i = 0; i < STPHASHSIZE; i++)
    {
        if (thashtab[i] != NULL)
        {
            dumpstphash(i, thashtab);
        }
    }

    fprintf(stderr, "\n\n\n\n****PLACES******\n");
    for (int i = 0; i < STPHASHSIZE; i++)
    {
        if (phashtab[i] != NULL)
        {
            dumpstphash(i, phashtab);
        }
    }
}
