#include <stdio.h>

#include "common.h"
#include "plucid.h"

int main(int argc, char** argv)
{
    const char* filename = (argc > 1) ? argv[1] : NULL;
    set_input(filename);

    int error = yyparse();
    if (error)
    {
        fprintf(stderr, "Fatal errors: no expression file written.\n");
        my_exit(1);
    }

    return 0;
}
