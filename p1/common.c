/**
 * @file  common.c
 * @brief C source file for common functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

char* string_duplicate(const char* original)
{
    size_t buf_size = strlen(original) + 1u;
    char* duplicated = (char*)malloc(sizeof(char) * buf_size);
    if (duplicated == NULL)
    {
        fprintf(stderr, "ran out of space\n");
    }
    else
    {
        strncpy(duplicated, original, buf_size);
    }

    return duplicated;
}

bool string_is_same(const char* string1, const char* string2)
{
    return (strcmp(string1, string2) == 0);
}

void my_exit(int status)
{
    fprintf(stdout, "%c\n", '\032');    // ^Z
    exit(status);
}
