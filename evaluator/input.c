#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "imanifs.h"
#include "iglobals.h"
#include "dynamic.h"
#include "dump.h"
#include "string.h"
#include "util.h"
#include "rupture.h"
#include "memory.h"
#include "input.h"

/*
 * all code for input to the interpreter is local to this file
 */

static int mygetchar(void);
static CELLPTR read_c_string(char c);
static CELLPTR readstring(int c);
static int is_word(int c);
static int getword2(int c);
static char is_sign(char c);
static CELLPTR readlist(void);
static float readnumber(void);
static void killspace(void);
static STRING ngc_strsave(char* s);

static int inlist = DOTTED_PAIR;
static int read_err;
#if 0
int readerr;
#endif

void f_arg(EXPRPTR e)
{
    int val;

    eval(arg1.x);

    switch (VStype)
    {
    case NUMERIC:
        val = (int)VSvalue.na.r;
        if (val <= 0)
        {
            error("arg of the function arg is out of range ", e->arg2.x, VStype, VSvalue);
            VStype = ERROR;
            return;
        }
        if (val > memargc - active_filters)
        {
            VSvalue.strg = findstring("");
        }
        else
        {
            VSvalue.strg = findstring(memargv[active_filters + val]);
        }
        VStype = QSTRING;
        return;
    case EOD:
        return;
    case ERROR:
        error("argument of function arg is ", e->arg2.x, VStype, VSvalue);
        return;
    default:
        error("argument of function arg is NUMERIC not ", e->arg2.x, VStype, VSvalue);
        VStype = ERROR;
        return;
    }
}

// old implementation?
#if 0
hiaton()
{   extern rupture();
    if (isatty(fileno(stdout))) signal(SIGINT,rupture);
    readerr=true_;
    signal(SIGALRM,hiaton);
}
#endif

void hiaton(int signum)
{
    VALUE x;
    (void)signum;

    signal(SIGALRM, hiaton);    // FIXME use sigaction

    alarm(1);

    STPPTR t = STPSt;
    STPPTR p = STPSp;
    STPPTR s = STPSs;
    EXPRPTR e = current_io->expr;
    STPSpush;
    STPSp = p;
    STPSs = s;
    if (isatty(fileno(stdout)))
    {
        signal(SIGINT, rupture);
    }
    STPSt = stpsearch((long)current_io->outx++, t->stptl, thashtab);
    IOPTR save_io = current_io;

    eval(arg2.x);

    current_io = save_io;
    x.v_type = VStype;
    x.v_value = VSvalue;
    dumpval2(current_io->out, x);
    if (current_io->outmode == POP)
    {
        fprintf(current_io->out, "\n");
    }
    VSpop;
    STPSpop;
}

int mygetchar(void)
{
    E(13) {
        if (current_io->chin == EOF)
        {
            return(EOF);
        }
    }

    if (filterp[filterlevel]
        && !current_io->ignore
        && !current_io->ptwize)
    {
        alarm(1);
        int ch = getc(current_io->in);
        alarm(0);
        if (ch == EOF || (ch == '@' && !cflag))
        {
            ch = EOF;
            fclose(current_io->in);
            fclose(current_io->out);
        }
        current_io->chin = ch;
        return ch;
    }
    int ch = getc(current_io->in);
    if (ch == '@' && !cflag)
    {
        ch = EOF;
    }
    current_io->chin = ch;  /* remove this if we have problems with eod
                               and filter  */
    if (ch == EOF
        && current_io->in == stdin
        && feof(current_io->in) == 0)
    {
        ch = mygetchar();
    }
    if (ch == EOF && current_io->in == stdin)
    {
        eodch = ch;
        current_io->chin = EOF;
    }
    if (ch == EOF && !isatty(fileno(current_io->in)))
    {
        fclose(current_io->in);
    }
    current_io->chin = ch;

    return ch ;
}

// old implementation?
#if 0
mygetchar()
{ int ch,i;
  EXPRPTR e;
  STPPTR s, t,p;
  IOPTR save_io;
  STPPTR stpsearch();
  VALUE x;
  if (filterp[filterlevel]&&!current_io->ignore&&!current_io->ptwize)
       {if (current_io->chin==EOF) return(EOF);
                  s = STPSs;
          t = STPSt;
          p = STPSp;
          i = current_io->outx;
          e = current_io->expr;
          STPSpush;
          STPSp=p;
          readerr=false_;
                  STPSs=s;
          alarm(1);
          ch = getc(current_io->in);
          alarm(0);
          while (readerr ) {  clearerr(current_io->in);
                      readerr = false_;
                      if (current_io->chout!=EOF)
                       { STPSt = stpsearch((long)i,
                                                  t->stptl,
                          thashtab);
                     save_io = current_io;
                     eval(arg2.x);
                     current_io = save_io;
                     x.v_type = VStype;
                     x.v_value = VSvalue;
                     dumpval2(current_io->out,x);
                     if ( current_io->outmode==POP)
                      fprintf(current_io->out,"\n");
                     i++;VSpop;
                       }
                      readerr = false_;
                      alarm(1);
                      ch = getc(current_io->in);
                      alarm(0);
                      if (!readerr) break;
                   }
        if (ch == EOF) fclose(current_io->in);
        current_io->outx=i;
        current_io->chin = ch;
        STPSpop;
        return(ch);
          }
  if (current_io->chin == EOF) return(EOF);
  ch = getc(current_io->in);
  if (ch=='@' && !cflag) ch = EOF;
  if (ch==EOF && current_io->in==stdin
          && feof(current_io->in)==0) ch = mygetchar();
  if (ch == EOF && current_io->in == stdin)
                 { eodch = ch;
                   current_io->chin = EOF; }
  if (ch == EOF && !isatty(fileno(current_io->in))) fclose(current_io->in);
  current_io->chin = ch;
  return(ch) ;
}
#endif

int read_c_item(CELLUNION* v)
{
    int ch = mygetchar();
    if (ch == EOF)
    {
        return EOD;
    }
    v->strg = read_c_string(ch);
    return QSTRING;
}

static CELLPTR read_c_string(char c)
{
    char buff[5];
    STRING p = buff;
    *p++ = c;
    *p = '\0';
    return findstring(buff);
}

int read_p_item(CELLUNION* v)
{
    if (ch == '[')  // NOTE: ch is global...
    {
        ch = mygetchar();
        if (ch == '%')
        {
            char buf[5];
            buf[0] = '[';
            buf[1] = '%';
            buf[2] = '\0';
            ch = mygetchar();
            v->wrd = findword(buf);
            return WORD;
        }
        /*get rid of [ */
        killspace();
        inlist = DOTTED_PAIR;
        if (ch != ']')
        {
            v->dp = readlist();
            if (v->dp == NULL)
            {
                return inlist;
            }
            if (inlist == ERROR)
            {
                return ERROR;
            }
            return DOTTED_PAIR;
        }
        else
        {
            ch = mygetchar();
            return NIL;
        }
    }
    else if (ch == EOF)
    {
        return EOD;
    }
    else if (isdigit(ch))
    {
        v->na.r = readnumber();
        if (read_err)
        {
            return ERROR;
        }
        return NUMERIC;
    }
    else if (ch == '?')
    {
        ch = mygetchar();
        return ERROR;
    }
    else if (ch == '@')
    {
        while ((ch = mygetchar()) == ' ') ;
        fclose(current_io->in);
        current_io->chin = EOF;
        return EOD;
    }
    else if (ch == '~')
    {
        ch = mygetchar();
        if (isdigit(ch))
        {
            v->na.r = -readnumber();
            if (read_err)
            {
                return ERROR;
            }
            return NUMERIC;
        }
        return ERROR;
    }
    else if (is_word(ch))
    {
        int tmp = getword2(ch);
        if (tmp == -1)
        {
            return EOD;
        }
        if (tmp == -2)
        {
            return ERROR;
        }
        v->wrd = tmp;
        return WORD;
    }
    else if (ch == '`')
    {
        v->strg = readstring(mygetchar());
        return QSTRING;
    }
    else
    {
        E(30) {
            fprintf(
                stderr,
                "\na constant cannot begin with \"%c\"\n",
                ch);
        }
        ch = mygetchar();
        return ERROR;
    }
}

int findword(STRING s)
{
    int i;

    for (i = 0; i < wordquan; i++)
    {
        if (!strcmp(s, wordtable[i]))
        {
            break;
        }
    }
    if (i == wordquan)
    {
        wordquan++;
        if (wordquan > MAXWORDS)
        {
            fprintf(stderr, "too many words in program\n");
            return -2;
        }
        wordtable[i] = ngc_strsave(s);
        if (wordtable[i] == NULL)
        {
            fprintf(stderr, "exceeded total dynamic storage for words\n");
            return -2;
        }
    }

    return i;
}

// old implementation?
#if 0
CELLPTR
findstring(s)
STRING s;
{       CELLPTR temp;
    char ch,*alloc();
    temp = (CELLPTR) alloc(SMALL_RECORD);
    VSpush;
    VStype = QSTRING;
    VSvalue.strg = temp;
    ch = s[0];
    temp->data.bits.d_hd = SWCHAR;
    temp->hd.swch = ch;
    temp->data.bits.d_tl = NIL;
    if ( s[0] != '\0' ) {
     temp->data.bits.d_tl = QSTRING;
     temp->tl.strg = findstring(&s[1]); }
       VSpop;
       return(temp);
}
#endif

static CELLPTR readstring(int c)
{
    char buffer[800];
    STRING p = buffer;
    int l = 0;

    while (c !='\'')
    {
        switch (c)
        {
        case '\\':
            c = mygetchar();
            switch (c)
            {
            case EOF:
                E(9) {
                    fprintf(
                        stderr,
                        "\nEOD in middle reading string\n");
                }
                ch = ' ';
                my_exit(1);
            case '\n':
                c = mygetchar();
                if (c == '\n')
                {
                    E(9) {
                        fprintf(
                            stderr,
                            "\nunexpected newline\n");
                    }
                    ch = ' ';
                    my_exit(1);
                }
                *p++ = c;
                l++;
                break;
            case '\'':
                *p++ = c;
                l++;
                break;
            default:
                *p++ = '\\';
                l++;
                *p++ = c;
                l++;
                break;
            }
            break;
        default:
            *p++ = c;
            l++;
        }
        if (l > 120)
        {
            E(9) {
                fprintf(
                    stderr,
                    "\nstring constant is too long. Max. length 120 chars\n");
            }
            ch = ' ';
            my_exit(1);
        }
        c  =  mygetchar();
        if (c == '\n')
        {
            E(9) {
                fprintf(
                    stderr,
                    "\nunexpected newline char in string constant\n");
                fprintf(
                    stderr,
                    "\nin this context newline should be preceeded by \\\n");
            }
            ch = ' ';
            my_exit(1);
        }
        if (c == EOF)
        {
            my_exit(1);
        }
    }
    /* we are now at the end of the string */
    ch = mygetchar();
    *p = '\0';
    return findstring(code_to_char(&buffer[0]));
}

static int is_word(int c)
{
    switch (c)
    {
    case ';':
    case ',':
    case '.':
    case ')':
    case '"':
    case '%':
    case '(':
    case '[':
    case '+':
    case '-':
    case '/':
    case '<':
    case '=':
    case '>':
    case '*':
    case '$':
    case '&':
    case ':':
    case '^':
    case '#':
        return true_;
    default:
        if (isalpha(c))
        {
            return true_;
        }
        return false_;
    }
}

static int getword2(int c)
{
    char buffer[120];
    int l = 0;
    STRING p = buffer;

    switch (c)
    {
    case ';':
    case ',':
    case '.':
    case ')':
    case '"':
        *p++ = c;
        l++;
        ch = mygetchar();
        break;
    case '%':
        *p++ = c;
        l++;
        ch = mygetchar();
        if (ch == ']' || ch == ')')
        {
            *p++ = ch;
            l++;
            ch = mygetchar();
            break;
        }
        if (ch == EOF)
        {
            return -1;
        }
        E(30) {
            fprintf(stderr, "%% not a word constant\n");
        }
        return -2;
    case '(':
        *p++ = c;
        l++;
        ch = mygetchar();
        if (ch == '%')
        {
            *p++ = ch;
            l++;
            ch = mygetchar();
        }
        break;
    case '[':
        *p++ = c;
        l++;
        ch = mygetchar();
        if (ch == '%')
        {
            *p++ = ch;
            l++;
            ch = mygetchar();
            break;
        }
        if (ch == EOF) {
            return -1;
        }
        E(30) {
            fprintf(stderr, "[ not a word constant\n");
        }
        return -2;
        break;
    case '+':
    case '-':
    case '/':
    case '<':
    case '=':
    case '>':
    case '*':
    case '$':
    case '&':
    case ':':
    case '^':
    case '#':
        *p++ = c;
        l++;
        while (is_sign(ch = mygetchar()))
        {
            *p++ = ch;
            l++;
        }
        break;
    case ']':
        E(30) {
            fprintf(stderr, "] not a word constant\n");
        }
        return -2;
    default:
        /* NOTE isalpha may not be portable */
        if (isalpha(c))
        {
            *p++ = ch;
            l++;
            ch = mygetchar();
            while (isalpha(ch) || isdigit(ch))
            {
                *p++ = ch;
                l++;
                ch = mygetchar();
            }
        }
        else
        {
            if (c == EOF)
            {
                return -1;
            }
            E(30) {
                fprintf(stderr, "error in list constant\n");
            }
            return -2;
        }
        break;
    }
    /* now at end of word */
    *p = '\0';
    return findword(buffer);
}

// not used
#if 0
int makeword(char* s)
{
    char buffer[200];
    int i;

    int l = 8;
    for (i = 0; i < 9; i++)
    {
        buffer[i] = s[i];
        if (s[i] == '\0')
        {
            l = i;
            break;
        }
    }

    switch (buffer[0])
    {
    case ';':
    case ',':
    case '.':
    case ')':
    case '"':
        if (l == 1)
        {
            break;
        }
        return -2;
    case '%':
        if ((buffer[1] == ']' || buffer[1] == ')')
            && l == 2)
        {
            break;
        }
        return -2;
    case '(':
        if (buffer[1] == '%' && l == 2)
        {
            break;
        }
        return -2;
    case '[':
        if (buffer[1] == '%' && l == 2)
        {
            break;
        }
        return -2;
    case '+':
    case '-':
    case '/':
    case '<':
    case '=':
    case '>':
    case '*':
    case '$':
    case '&':
    case ':':
    case '^':
    case '#':
        i = 1;
        while (is_sign(buffer[i]))
        {
            i++;
            if (i == 8)
            {
                break;
            }
        }
        if (i < l)
        {
            return -2;
        }
        break;
    default:
        i = 1;
        if (isalpha(buffer[0]))
        {
            while (isalpha(buffer[i]) || isdigit(buffer[i]))
            {
                i++;
                if (i == 8)
                {
                    break;
                }
            }
        }
        else
        {
            return -2;
        }
        if (i < l)
        {
            return -2;
        }
        break;
    }
    /* now at end of word */
    return findword(buffer);
}
#endif

static char is_sign(char c)
{
    switch (c)
    {
    case ':':
    case '^':
    case '+':
    case '-':
    case '/':
    case '<':
    case '=':
    case '>':
    case '*':
    case '$':
    case '&':
    case '#':
        return true_;
    default:
        return false_;
    }
}

static CELLPTR readlist(void)
{
    int listerr = false_;
    CELLPTR v = (CELLPTR)alloc(SMALL_RECORD);
    v->data.bits.d_mark = 1;
    v->data.bits.d_hd = read_p_item(&v->hd);
    if (v->data.bits.d_hd == EOD)
    {
        inlist = EOD;
        return NULL;
    }
    if (v->data.bits.d_hd == ERROR)
    {
        listerr = true_;
    }
    killspace();
    if (ch == ']')
    {
        v->data.bits.d_tl = NIL;
        ch = mygetchar();
    }
    else
    {
        v->tl.dp = readlist();
        if (v->tl.dp == NULL)
        {
            return NULL;
        }
        v->data.bits.d_tl = DOTTED_PAIR;
    }
    if (listerr)
    {
        inlist = ERROR;
    }
    return v;
}

static float readnumber(void)
{
    int digitcount = 1;
    int sum = ch - '0';

    while (isdigit(ch = mygetchar()))
    {
        digitcount++;
        if (digitcount >= 8)
        {
            E(9) {
                fprintf(
                    stderr,
                    "the number input has too many decimal digits. Max is 7\n");
            }
            read_err = true_;
            while (isdigit(ch))
            {
                ch = mygetchar();
            }
            if (ch == '.')
            {
                ch = mygetchar();
                while (isdigit(ch))
                {
                    ch = mygetchar();
                }
            }
            return 0;
        }
        sum = sum * 10 + ch - '0';
    }
    if (ch != '.')
    {
        return (float)sum;
    }
    float cnt = 10;
    ch = mygetchar();
    float rsum = ch - '0';
    if (isdigit(ch))
    {
        while (isdigit(ch = mygetchar()))
        {
            rsum = rsum * 10 + ch - '0';
            cnt *=  10;
        }
    }
    else
    {
        return (float)sum;
    }
    return (sum + (rsum / cnt));
}

// not used
#if 0
int readword()
{
    char buffer[120];

    char* p = buffer;
    for (*p++ = ch; isalnum(*p = mygetchar()); p++) ;
    ch = *p;
    *p = '\0';

    int i;
    for (i = 0; i < wordquan; i++)
    {
        if (!strcmp(wordtable[i], buffer))
        {
            break;
        }
    }
    if (i == wordquan)
    {
        wordtable[i] = ngc_strsave(buffer);
        wordquan++;
    }
    return i;
}
#endif

static void killspace(void)
{
    if (ch == EOF)
    {
        return;
    }
    while (isspace(ch))
    {
        ch = mygetchar();
        if (ch == EOF)
        {
            return;
        }
    }
}

STRING strsave(char* s)
{
    int n = strlen(s);
    char *p;
    if ((p = calloc(1, n + 1)) != NULL)
    {
        strcpy(p, s);
    }
    return p;
}

static STRING ngc_strsave(char* s)
{
    int n = strlen(s);
    char *p;
    if ((p = ngc_calloc(n + 1)) != NULL)
    {
        strcpy(p, s);
    }
    return p;
}

char* ngc_calloc(int n)
{
    if (ngc_allocp + n <= ngc_allocbuf + ngc_allocsize)
    {
        ngc_allocp += n;
        return (ngc_allocp - n);
    }
    else
    {
        return NULL;
    }
}
