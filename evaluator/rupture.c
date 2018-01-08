#include <stdio.h>
#include <stdlib.h>

#include "imanifs.h"
#include "iglobals.h"
#include "input.h"
#include "dump.h"
#include "dynamic.h"
#include "rupture.h"

static void explain(void);
static void trace_ops(void);

static int rupturelevel = 0;
// not used
#if 0
int lu_time;
char rp_ch;
#endif

void rupture(int signum)
{
    int num;

    rupturelevel++;
    signal(SIGINT, rupture);
    signal(SIGALRM, hiaton);
    fprintf(stderr, "\nTyping Control-C interrupts the evaluation.\n");
    fprintf(stderr, "Do you require help ? Type h for help.\n");

    if (rupturelevel == 1)
    {
        while (1)
        {
            char input[80];

            FILE* term = fopen("/dev/tty","r");
//            while (fgets(input, 200, term) == NULL) ; // buffer over
            while (fgets(input, 80, term) == NULL) ;
            fclose(term);

            switch (input[0])
            {
            case '@':
                exit(1);
            case 'o':
                trace_ops();
                break;
            case 'z':
                for (int i = 0; i < exprquan; i++)
                {
                    retvec[i] =1000;
                }
                break;
            case 'h':
                explain();
                break;
            case 'y':
                stats();
                break;
            case 'x':
                fprintf(
                    stderr,
                    "height of stp_stack %ld\n",
                    stp_top - &stp_stack[0]);
                fprintf(
                    stderr,
                    "max height of stp_stack %d\n",
                    STPSIZE);
                fprintf(
                    stderr,
                    "height of d_stack %ld\n",
                    d_top - &d_stack[0]);
                fprintf(
                    stderr,
                    "max height of d_stack %d\n",
                    DSIZE);
                fprintf(
                    stderr,
                    "height of v_stack %ld\n",
                    v_top - &v_stack[0]);
                fprintf(
                    stderr,
                    "max height of v_stack %d\n",
                    VSIZE);
                break;
            case 'a':
                /* printout retirement ages */
                fprintf(
                    stderr,
                    "The retirement age for each varible is as follows:\n");
                for (int i = 0; i < exprquan; i++)
                {
                    fprintf(
                        stderr,
                        "%20s(%3d):%10d\n",
                        nametable[i],
                        i,
                        retvec[i]);
                }
                break;
            case 'c':
                rupturelevel--;
                signal(SIGINT, rupture);
                return;
            case 'd':
                for (int i = 0; i < exprquan; i++)
                {
                    fprintf(stderr, "%d: %s: ", i, nametable[i]);
                    echoexpr(exprtable[i]);
                    fprintf(stderr, "\n");
                }
                break;
            case 'g':
                /* force a garbage collection */
                collect();
                memusage();
                break;
            case 'm':
                /* print memory information */
                memusage();
                break;
            case 'q':
                my_exit(1);
            case 'r':
                /* remove a trace option */
                num = atoi(&input[1]);
                if (0 <= num && num < TRACEQUAN)
                {
                    tflags[num] = 0;
                }
                break;
            case 's':
                for (int i = 0; i < wordquan; i++)
                {
                    fprintf(stderr, "%d:%s:\n", i, wordtable[i]);
                }
                break;
            case 't':
                /* set a trace option */
                num = atoi(&input[1]);
                if(0 <= num && num <= TRACEQUAN)
                {
                    tflags[num] = 1;
                }
                break;
            case '!':
                system(&input[1]);
                fprintf(stderr, "!\n");
                break;
            default:
                fprintf(stderr, "invalid command\n");
                explain();
                break;
            }
        }
    }
    rupturelevel--;
}

void echoexpr(EXPRPTR p)
{
    FPTR q = &ftable[p->f];
    int n = q->nargs;
    switch (q->type)
    {
    case TERMINAL:
        if (p->f == F_CONST)
        {
            fprintf(stderr, "%s(%f", q->name, p->arg1.c.r);
        }
        else if (p->dim == 0)
        {
            fprintf(stderr, "%s(%d", q->name, p->arg1.i);
        }
        else
        {
            fprintf(stderr, "%s%d(%d", q->name, p->dim, p->arg1.i);
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
        break;
    case INTERIOR:
        if (p->dim == 0)
        {
            fprintf(stderr, "%s(", q->name);
        }
        else
        {
            fprintf(stderr, "%s%d(", q->name, p->dim);
        }

        echoexpr(p->arg1.x);

        if (n > 1)
        {
            echoexpr(p->arg2.x);
        }
        if (n > 2)
        {
            echoexpr(p->arg3.x);
        }
        if (n > 3)
        {
            echoexpr(p->arg4.x);
        }
        if (n > 4)
        {
            echoexpr(p->arg5.x);
        }

        fprintf(stderr,")");
    }
}

static void explain(void)
{
    fprintf(stderr, "The following is a list of the currently available commands\n");
    fprintf(stderr, "o - Prints out the currently available trace options\n");
    fprintf(stderr, "h - Help, prints out this list of commands\n");
    fprintf(stderr, "q - Quit the evaluator\n");
    fprintf(stderr, "z - Set the retirement age of all variables to 1000\n");
    fprintf(stderr, "y - Print out statistical information about the evaluation\n");
    fprintf(stderr, "c - Continue the evaluation the point of interrupt\n");
    fprintf(stderr, "    Note that if you interrupt in the middle of \n");
    fprintf(stderr, "    inputting a constant then you will have to re-type\n");
    fprintf(stderr, "    any input that you typed on the same line as the\n");
    fprintf(stderr, "    interrupt.\n");
    fprintf(stderr, "tN - Turn on trace option number N\n");
    fprintf(stderr, "rN - Turn off trace option number N\n");
    fprintf(stderr, "m - Request information about memory usage\n");
    fprintf(stderr, "!xxx - Execute xxx as a UNIX command\n");
    fprintf(stderr, "a - Output the retirement age of each variable\n");
    fprintf(stderr, "    used in the program\n");
    fprintf(stderr, "d - Dump the intermediate code\n");
    fprintf(stderr, "s - Print out a list of ALL strings used in the program\n");
    fprintf(stderr, "x - Output the current height and the limits of the evaluators runtime stacks\n");
}

static void trace_ops(void)
{
    fprintf(stderr, "The following is a list of the currently available trace options\n");
    fprintf(stderr, "t0 - Print out a message very time a request is made for\n");
    fprintf(stderr, "     a variable during execution of the program\n");
    fprintf(stderr, "t1 - Print a brief message about garbage collection\n");
    fprintf(stderr, "     each time it occurs during execution of the program\n");
    fprintf(stderr, "t2 - Print more information about garbage collection\n");
    fprintf(stderr, "     each time it occurs\n");
    fprintf(stderr, "t5 - Print information about action on the display each\n");
    fprintf(stderr, "     time that it occurs\n");
    fprintf(stderr, "t6 - Print information each time a variable is defined\n");
    fprintf(stderr, "     by the evaluator\n");
    fprintf(stderr, "t7 - Give additional statistical information\n");
    fprintf(stderr, "t9 - When this option is set Run time error are reported\n");
    fprintf(stderr, "     via the standard error\n");
    fprintf(stderr, "t10 - Turn off garbage collection completely\n");
    fprintf(stderr, "t25 - When set initialises the retirement age of all variables\n");
    fprintf(stderr, "t26 - does an on the fly garbage collect when set\n");
    fprintf(stderr, "      to 1000\n");
    fprintf(stderr, "t30 - Prints out a demand trace for variables as they\n");
    fprintf(stderr, "      they are demanded by the evaluator.\n");
}
