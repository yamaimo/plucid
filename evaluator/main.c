#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "imanifs.h"
#include "iglobals.h"
#include "dynamic.h"
#include "input.h"
#include "rupture.h"
#include "dump.h"
#include "memory.h"

static void initialise(int argc, char** argv);
static int testfile(STRING p);
static EXPRPTR readexpr(int i);
static void elaborate(int n, COORDS xyz, int e_time);
static void usage(void);

int main(int argc, char** argv)
{
    memargc = argc;
    memargv = argv;

    /* first, set up files and read in the expression table */
    initialise(argc, argv);
//    int ok_to_print = (!sflag && !pflag && isatty(fileno(stdout)));   // not used

    if (dflag && isatty(fileno(stdout)))
    {
        for(int i = 0; i < exprquan; i++)
        {
            fprintf(stderr, "%d: %s: ", i, nametable[i]);
            echoexpr(exprtable[i]);
            fprintf(stderr, "\n");
        }
    }

    /* get the result */
    if (isatty(fileno(stdout)))
    {
        fprintf(stderr, "Evaluation begins .........\n");
    }

    int udc1 = 0;
    int ldc1 = 0;
    int udc2 = 0;
    int ldc2 = 0;
    int udc3 = 0;
    int ldc3 = 0;
    int i = 0;
    int t1 = 1000;
    int dimension_err = true_;
    D(13) {
        while (dimension_err)
        {
            if (!sflag)
            {
                fprintf(
                    stderr,
                    "Ranges for t(t0,t1) x(n0,n1) y(m0,m1) z(o0,01)  respectively\n");
            }
            scanf(
                " t(%d,%d) x(%d,%d) y(%d,%d) z(%d,%d)",
                &i,
                &t1,
                &ldc1,
                &udc1,
                &ldc2,
                &udc2,
                &ldc3,
                &udc3);
            if (ldc1 > udc1 || ldc2 > udc2 || ldc3 > udc3)
            {
                fprintf(stderr, "unaccaptable dimensions\n");
                dimension_err = true_;
            }
            else
            {
                dimension_err = false_;
            }
        }
    }

    if (isatty(fileno(stdout)))
    {
        if (signal(SIGINT, SIG_IGN) != SIG_IGN)
        {
            signal(SIGINT, rupture);
        }
    }
    else
    {
        signal(SIGINT, SIG_IGN);
    }
    signal(SIGALRM, hiaton);

    while (true_)
    {
        c3 = udc3;
        while (true_)
        {
            if (!sflag)
            {
                D(13) {
                    fprintf(stderr, "Slice|\n");
                    fprintf(stderr, "%5d|", c3);
                    for (int j = ldc1; j <= udc1; j++)
                    {
                        fprintf(stderr, "%10d ", j);
                    }
                    fprintf(stderr, "\n");
                    for (int j = 1; j < 11 * abs(udc1 - ldc1) + 17; j++)
                    {
                        fprintf(stderr, "-");
                    }
                    fprintf(stderr, "\n");
                }
            }
            c2 = udc2;
            while (true_)
            {
                if (!sflag)
                {
                    D(13) {
                        fprintf(stderr, "%5d|", c2);
                    }
                }
                c1 = ldc1;
                while (true_)
                {
                    COORDS xyz;
                    xyz.d1 = c1;
                    xyz.d2 = c2;
                    xyz.d3 = c3;
                    D(13) {
                        if (c1 > udc1)
                        {
                            break;
                        }
                    }
                    elaborate(0, xyz, i);
                    E(13) {
                        if (v_stack[1].v_type == EOD)
                        {
                            my_exit(1);
                        }
                        if (!sflag)
                        {
                            printf("\n");
                            fprintf(stderr, "output");
                            dumpt(STPSt);
                            fprintf(stderr, " : ");
                        }
                    }
                    D(13) {
                        if (v_stack[1].v_type == EOD)
                        {
                            break;
                        }
                    }
                    dumpval(stdout, &v_stack[1]);
                    E(13) {
                        break;
                    }
                    c1++;
                }
                E(13) {
                    break;
                }

                /* newlines separating outp */
                if (!sflag)
                {
                    printf("\n");
                }

                if (v_stack[1].v_type == EOD && c1 == 0)
                {
                    break;
                }
                if (c2-- <= ldc2)
                {
                    break;
                }
            }
            E(13) {
                break;
            }
            if (!sflag)
            {
                for (int j = 1; j < 11 * abs(udc1 - ldc1) + 17; j++)
                {
                    fprintf(stderr, "-");
                }
                fprintf(stderr, "\n");
                fprintf(stderr, " Time|");
                for (int j = ldc1; j <= udc1; j++)
                {
                    fprintf(stderr, "%10d ", j);
                }
                fprintf(stderr, "\n%5d|", i);
                fprintf(stderr, "\n\n\n");
            }
            if (v_stack[1].v_type == EOD && c2 == 0)
            {
                break;
            }
            if (c3-- <= ldc3)
            {
                break;
            }
        }
        D(13) {
            if (v_stack[1].v_type == EOD && c3 == 0)
            {
                my_exit(1);
            }
        }

        i++;
        if (i == t1)
        {
            break;
        }

        newout = true_;
    }
}

static void initialise(int argc, char** argv)
{
    /*
     * set up environment. read in expressions, words, etc.
     */
    pid = getpid();
    if (argc == 1)
    {
        usage();
    }
    for (int i = 0; i < TRACEQUAN; tflags[i++] = 0) ;
    cflag = 0;
    sflag = 0;
    dflag = 0;
    pflag = 0;
    infile = NULL;
    current_in = NULL;
    current_out = NULL;
    current_var = -1;
    current_inx = -1;
    current_outx = -1;
    notfoundcount = 0;
    errcount = 0;
    varcount = 0;
    stpcount = 0;
    memcount = 0;
    brecycle = 0;
    srecycle = 0;
    open_file = 3; /* stdin,stdout,stderr */
    ch = -2;

    /* predefined words */
    false_ = 0; // must be set before calling any functions.
    true_ = 1;  // same above.
    newout = true_;
    evalerr = false_;
    filterlevel = 0;
    eodch = EOF;

    for (int i = 0; i < 30; i++)
    {
        filterp[i] = false_;
    }
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            int num;
            switch (argv[i][1])
            {
            case 't':
                num = atoi(&argv[i][2]);
                if (0 <= num && num < TRACEQUAN)
                {
                    tflags[num] = 1;
                }
                else
                {
                    fprintf(stderr, "bad option %s\n", argv[i]);
                    my_exit(-1);
                }
                break;
            case 'd':
                dflag = 1;
                break;
            case 'c':
                cflag = 1;
                break;
            case 's':
                sflag = 1;
                break;
            case 'p':
                pflag = 1;
                break;
            default:
                fprintf(stderr, "bad option %s\n", argv[i]);
                my_exit(1);
            }
        }
        else
        {
            if (testfile(argv[i]))
            {
                infilename = argv[i];
                if ((infile = fopen(argv[i], "r")) == NULL)
                {
                    fprintf(stderr, "cannot open %s\n", argv[i]);
                    my_exit(1);
                }
                active_filters = i;
                break;
            }
            else
            {
                if (argv[i] == NULL)
                {
                    fprintf(stderr, "silly");
                    my_exit(2);
                }
                fprintf(stderr, "%s: not a legal lucid expression file\n", argv[i]);
                my_exit(1);
            }
        }
    }

    if (infile == NULL)
    {
        fprintf(stderr, "no input file\n");
        my_exit(1);
    }
    for (int i = 0; i < MAXVARS; i++)
    {
        io_info[i] = NULL;
    }
    exprquan = getw(infile);
    if (dflag)
    {
        fprintf(stderr, "\n%d expressions\n", exprquan);
    }
    if (exprquan > MAXVARS)
    {
        fprintf(stderr, "program too large\n");
        my_exit(1);
    }
    nametable = (STRING*)calloc(exprquan, sizeof(STRING));
    exprtable = (EXPRPTR*)calloc(exprquan, sizeof(EXPRPTR));
    for (int i = 0; i < exprquan; i++)
    {
        STRING p;
        char buffer[500];

        for (p = buffer; (*p = fgetc(infile)) != '\n'; p++) ;
        *p = '\0';
        nametable[i] = strsave(buffer);
        exprtable[i] = readexpr(i);
        if (dflag)
        {
            fprintf(stderr, "%d : %s :", i, nametable[i]);
            echoexpr(exprtable[i]);
            fprintf(stderr, "\n");
        }
    }
    wordquan = getw(infile);
    if (wordquan > MAXWORDS)
    {
        fprintf(stderr, "too many words in compiled program\n");
        my_exit(1);
    }
    wordtable = (STRING*)calloc(MAXWORDS, sizeof(STRING));
    for (int i = 0; i < MAXWORDS; wordtable[i++] = NULL) ;  // maybe not needed; calloc clears to zero.
    for (int i = 0; i < wordquan; i++)
    {
        STRING p;
        char buffer[500];

        int nwords = getw(infile);
        p = buffer;
        for (int k = 0; k < nwords; k++)
        {
            *p = fgetc(infile);
            p++;
        }
        fgetc(infile);
        *p = '\0';
        wordtable[i] = strsave(buffer);
    }
    fclose(infile);
    ngc_allocsize = (MAXSTRINGS) * 20 + (MAXWORDS - wordquan) * AVGWRDLEN + MAXFILTERSPACE;
    ngc_allocbuf = (char*)calloc(ngc_allocsize, 1);
    ngc_allocp = ngc_allocbuf;

    /* the memory is initially empty, too */
    for (int i = 0; i < NHASHSIZE; nhashtab[i++] = NULL) ;  // use memset.
    for (int i = 0; i < THASHSIZE; thashtab[i++] = NULL) ;
    for (int i = 0; i < PHASHSIZE; phashtab[i++] = NULL) ;
    for (int i = 0; i < SHASHSIZE; shashtab[i++] = NULL) ;

    /*
     * initialise run time stacks
     */
    v_top = &v_stack[0];
    stp_top = &stp_stack[0];
    d_top = &d_stack[0];
    d_top->d_envg = NULL;
    d_top->d_envf = NULL;
    d_top->d_sf = NULL;
    d_top->d_sg = NULL;
    d_top->d_tg = NULL;
    d_top->d_tf = NULL;
    d_top->d_pg = NULL;
    d_top->d_pf = NULL;
    /*  dont buffer output */
    setbuf(stdout, (char*)NULL);
    setbuf(stdin, (char*)NULL);

    /* and, lets set up our dynamic allocation system */

    s_recquan = 2000;
    b_recquan = 2000;
    initspace();
}

static int testfile(STRING p)
{
    int j;
    for(j = 0; p[j]; j++);
    return (j > 2 && p[j - 1] == 'i' && p[j - 2] == '.');
}

static EXPRPTR readexpr(int i)
{
    int temp = getw(infile);
    FPTR q = &ftable[temp];
    int n = q->nargs;
    if (temp == F_FILTER )
    {
        n++;
    }

    EXPRPTR new;
    switch (n)
    {
    case 1:
        new = (EXPRPTR)calloc(1, sizeof(U_EXPR));
        break;
    case 2:
        new = (EXPRPTR)calloc(1, sizeof(B_EXPR));
        break;
    case 3:
        new = (EXPRPTR)calloc(1, sizeof(T_EXPR));
        break;
    case 4:
        new = (EXPRPTR)calloc(1, sizeof(Q_EXPR));
        break;
    case 5:
        new = (EXPRPTR)calloc(1, sizeof(QU_EXPR));
        break;
    default:
        fprintf(stderr, "error: illegal arg count\n");
        my_exit(1);
    }
    new->f = temp;
    switch (q->type)
    {
    case TERMINAL:
        new->arg1.i = getw(infile);
        if (n > 1)
        {
            new->arg2.i = getw(infile);
        }
        if (n > 2)
        {
            new->arg3.i = getw(infile);
        }
        if (n > 3)
        {
            new->arg4.i = getw(infile);
        }
        if (n > 4)
        {
            new->arg5.i = getw(infile);
        }
        break;
    case INTERIOR:
        new->dim = getw(infile);
        new->arg1.x = readexpr(i);
        if (n > 1)
        {
            new->arg2.x = readexpr(i);
        }
        if (n > 2)
        {
            new->arg3.x = readexpr(i);
        }
        if (n > 3)
        {
            new->arg4.x = readexpr(i);
        }
        if (n > 4)
        {
            new->arg5.i = i;
        }
        break;
    default:
        fprintf(stderr, "error: illegal type of fnode\n");
        fprintf(stderr, "%s is fvalue\n", q->name);
        my_exit(1);
    }
    return new;
}

static void elaborate(int n, COORDS xyz, int e_time)
{
    STPPTR t = stpsearch((long)e_time, (STPPTR)NULL, thashtab);
    outside = t;
    STPPTR s = ssearch(xyz);
    d_top = &d_stack[0];
    VStop = &v_stack[0];
    VSvalue.na.r = 0;
    VSvalue.na.i = 0;
    VStype = 0;
    STPStop = &stp_stack[0];
    STPSs = s;
    STPSt = t;
    STPSp = NULL;
    (*ftable[exprtable[n]->f].apply)(exprtable[n]);
}

static void usage(void)
{
    fprintf(stderr, "usage: luval [-d] [-c] [-s] [-t?] <fname.i>\n");
    my_exit(1);
}
