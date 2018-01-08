#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "imanifs.h"
#include "iglobals.h"
#include "dump.h"
#include "input.h"
#include "memory.h"
#include "string.h"
#include "util.h"
#include "f_lucid.h"

static IOPTR get_io_item(int type, EXPRPTR e, STPPTR space, STPPTR time, STPPTR place);

static IOPTR get_io_item(int type, EXPRPTR e, STPPTR space, STPPTR time, STPPTR place)
{
    IOPTR s = (IOPTR)ngc_calloc(16 * 4);
    if (s == NULL)
    {
        fprintf(stderr, "\nngc space exhausted\n");
        my_exit(1);
    }
    s->type = type;
    s->expr = e;
    s->s = space;
    s->t = time;
    s->p = place;
    s->out = NULL;
    s->in = NULL;
    if (type == INPUT)
    {
        s->in = stdin;
    }
    s->chin = ' ';
    s->chout = ' ';
    s->inx = 0;
    s->outx = 0;
    s->inmode = POP;
    s->outmode = POP;
    s->ignore = false_;
    s->ptwize = false_;
    s->next = NULL;
    return s;
}

void f_input(EXPRPTR e)
{
    int savefilter = filterp[filterlevel];
    filterp[filterlevel] = false_;
    if (io_info[e->arg1.i] == NULL)
    {
        io_info[e->arg1.i] = get_io_item(INPUT, e, NULL, NULL, NULL);
    }
    current_var = e->arg1.i;
    current_io = io_info[e->arg1.i];
    ch = current_io->chin;
    varcount++;

    MEMPTR memplace;
    if (isatty(fileno(current_io->in)))
    {
        (void)memsearch(&memplace, e->arg1.i, STPSs, STPSt, STPSp);
        if (memplace->v.v_type == UNDEFINED)
        {
            if (!sflag)
            {
                fprintf(stderr, "\n%7s", nametable[e->arg1.i]);
                D(13) {
                    dumps(STPSs);
                }
                dumpt(STPSt);
                fprintf(stderr, " : ");
            }
            if (cflag)
            {
                memplace->v.v_type = read_c_item(&memplace->v.v_value);
            }
            else
            {
                memplace->v.v_type = read_p_item(&memplace->v.v_value);
            }
        }
    }
    else
    {
        for (int i = current_io->inx; i <= STPSt->stphd.word; i++)
        {
            STPPTR t1 = stpsearch(i, STPSt->stptl, thashtab);
            (void)memsearch(&memplace, e->arg1.i, STPSs, t1, STPSp);
            if (memplace->v.v_type == UNDEFINED)
            {
                notfoundcount++;
                if (ch != EOF)
                {
                    if (cflag)
                    {
                        memplace->v.v_type = read_c_item(&memplace->v.v_value);
                    }
                    else
                    {
                        memplace->v.v_type = read_p_item(&memplace->v.v_value);
                    }
                }
                else
                {
                    memplace->v.v_type = EOD;
                }
            }
        }
    }

    current_io->inx      = STPSt->stphd.word + 1;
    current_io->chin     = ch;
    filterp[filterlevel] = savefilter;

    VSpush;
    copy(*VStop, memplace->v);
}

void f_filter(EXPRPTR e)
{
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    filterlevel++;
    varcount++;
    filterp[filterlevel] = true_;

    IOPTR m;
    int found = 0;
    for (m = io_info[e->arg5.i]; m != NULL; m = m->next)
    {
        if (p == m->p)
        {
            found = 1;
            break;
        }
    }
    if (!found)
    {
        char sx[100];
        char ops[20];
        int pipe_a[2];
        int pipe_b[2];

        eval(arg1.x);

        switch (VStype)
        {
        case QSTRING:
            formstring(&sx[0], VSvalue.strg);
            VSpop;
            m = get_io_item(IOFILTER, e, s, t->stptl, p);
            m->next = io_info[e->arg5.i];
            io_info[e->arg5.i] = m;

            eval(arg3.x);
            switch(VStype)
            {
            case QSTRING:
                formstring(&ops[0], VSvalue.strg);
                for (char* iostring = ops; *iostring != '\0'; iostring++)
                {
                    switch (iostring[0])
                    {
                    case 'i':
                        m->ignore = true_;
                        break;
                    case 'p':
                        m->ptwize = true_;
                        break;
                    case 'c':
                        m->inmode = UNIX;
                        break;
                    case 's':
                        m->outmode = UNIX;
                        break;
                    }
                }
                VSpop;
                break;
            case EOD:
                m->chin = EOF;
                VStype = EOD;
                return;
            default:
                error("3rd arg of filter must be a string not ", e->arg4.x, VStype, VSvalue);
                m->chin = '?'; /* not enough */
                VStype = ERROR;
                return;
            }

            if ((pipe(pipe_a) == SYSERR) || (pipe(pipe_b) == SYSERR))
            {
                perror(sx);
                my_exit(1);
            }
            switch (fork())
            {
            case SYSERR:
                perror(sx);
                my_exit(1);
            case CHILD:
                signal(SIGINT, SIG_IGN);
                signal(SIGALRM, SIG_IGN);
                dup2(pipe_a[READ_END], CHILD_IN);
                dup2(pipe_b[WRITE_END], CHILD_OUT);
                close(pipe_a[WRITE_END]);
                close(pipe_b[WRITE_END]);
                close(pipe_a[READ_END]);
                close(pipe_b[READ_END]);
                execlp("/bin/sh", "sh", "-c", sx, 0);
                perror(sx);
                my_exit(1);
            }
            m->out = fdopen(pipe_a[WRITE_END], "w");
            m->in = fdopen(pipe_b[READ_END], "r");
            setbuf(m->in, (char*)NULL);
            setbuf(m->out, (char*)NULL);
            close(pipe_b[WRITE_END]);
            close(pipe_a[READ_END]);
            ch = m->chin;
            break;
        case EOD:
            VStype = EOD;
            return;
        default:
            error("1st arg of filter must be a string, not ", e->arg4.x, VStype, VSvalue);
            VStype = ERROR;
            return;
        }
    }
    MEMPTR memplace;
    ch = m->chin;
    current_io = m;
    for (int i = current_io->inx; i <= STPSt->stphd.word; i++)
    {
        STPPTR s1 = stpsearch(0, NULL, shashtab);
        STPPTR t1 = stpsearch(i, STPSt->stptl, thashtab);
        (void)memsearch(&memplace, e->arg5.i, s1, t1, STPSp);
        if (memplace->v.v_type == UNDEFINED)
        {
            if (current_io->ptwize)
            {
                STPPTR tt = STPSt;
                STPPTR pp = STPSp;
                STPPTR ss = STPSs;
                STPSpush;
                STPSp = pp;
                STPSs = ss;
                IOPTR save_io = current_io;
                STPSt = stpsearch((long)i, tt->stptl, thashtab);

                eval(arg2.x);

                current_io = save_io;
                VALUE x;
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
            memplace->v.v_type = (current_io->inmode == UNIX)
                ? read_c_item(&memplace->v.v_value)
                : read_p_item(&memplace->v.v_value);
        }
    }
    current_io->inx = STPSt->stphd.word + 1;
    ch = current_io->chin;
    VSpush;
    copy(*VStop, memplace->v);
    filterp[filterlevel] = false_;
    filterlevel--;
}

void f_now(EXPRPTR e)
{
    int time = STPSt->stphd.word;
    VSpush;
    VStype = NUMERIC;
    VSvalue.na.r = time;
}

void f_first(EXPRPTR e)
{
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    t = stpsearch((long) 0, t->stptl, thashtab);
    STPSpushval(s, t, p);
    eval(arg1.x);
    STPSpop;
}

void f_prev(EXPRPTR e)
{
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    t = stpsearch(t->stphd.word - 1, t->stptl, thashtab);
    STPSpushval(s, t, p);
    eval(arg1.x);
    STPSpop;
}

void f_next(EXPRPTR e)
{
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;
    t = stpsearch(t->stphd.word+1, t->stptl, thashtab);
    STPSpushval(s, t, p);
    eval(arg1.x);
    STPSpop;
}

void f_before(EXPRPTR e)
{
    STPPTR s = STPSs;
    STPPTR p = STPSp;
    STPPTR t = STPSt;
    if (t->stphd.word >= 0)
    {
        eval(arg2.x);
    }
    else
    {
        t = stpsearch(t->stphd.word + 1, t->stptl, thashtab);
        STPSpushval(s, t, p);
        eval(arg1.x);
        STPSpop;
    }
}

void f_fby(EXPRPTR e)
{
    STPPTR s = STPSs;
    STPPTR p = STPSp;
    STPPTR t = STPSt;
    if (t->stphd.word <= 0)
    {
        eval(arg1.x);
    }
    else
    {
        t = stpsearch(t->stphd.word - 1, t->stptl, thashtab);
        STPSpushval(s, t, p);
        eval(arg2.x);
        STPSpop;
    }
}

void f_asa(EXPRPTR e)
{
    STPPTR s = STPSs;
    STPPTR p = STPSp;
    STPPTR t = STPSt;
    STPSpush;
    STPSs = s;
    STPSp = p;
    for (int i = 0; ; i++)
    {
        STPSt = stpsearch((long)i, t->stptl, thashtab);
        eval(arg2.x);
        D(30) {
            fprintf(stderr, " asa\n");
        }
        switch (VStype)
        {
        case WORD:
            if (VSvalue.wrd == true_)
            {
                VSpop;
                eval(arg1.x);
                STPSpop;
                return;
            }
            VSpop;
            break;
        case ERROR:
            error("right arg of asa is ", e->arg3.x, ERROR, VSvalue);
            VStype = ERROR;
            STPSpop;
            return;
        case EOD:
            VStype = EOD;
            STPSpop;
            return;
        default:
            error("right arg of asa must be logical, not ", e->arg3.x, VStype, VSvalue);
            STPSpop;
            VStype = ERROR;
            return;
        }
    }
}

void f_attime(EXPRPTR e)
{
    STPPTR s = STPSs;
    STPPTR t = STPSt;
    STPPTR p = STPSp;

    eval(arg2.x);

    D(30) {
        fprintf(stderr, "attime \n");
    }

    char type;
    CELLUNION value;
    switch (VStype)
    {
    case NUMERIC:
        t = stpsearch((long)VSvalue.na.r, t->stptl, thashtab);
        STPSpushval(s, t, p);
        eval(arg1.x);
        type = VStype;
        value = VSvalue;
        switch (VStype)
        {
        case ERROR:
            error(
                "left arg of attime \n"
                "or 1st arg of wvr or upon is ",
                e->arg3.x,
                VStype,
                VSvalue);
        default:
            break;
        }
        STPSpop;
        VSpop;
        VStype = type;
        VSvalue = value;
        return;
    case EOD:
        VStype = EOD;
        return;
    case ERROR:
        error(
            "right arg of attime \n"
            "or condition of wvr or upon is ",
            e->arg3.x,
            ERROR,
            VSvalue);
        return;
    default:
        error("right arg of attime must be numeric, not ", e->arg3.x, VStype, VSvalue);
        VStype = ERROR;
        return;
    }
}
