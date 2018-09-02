/**
 * @file  plucid.h
 * @brief C header file for pLucid lexical analyzer and parser.
 */

#pragma once

#include "common.h"
#include "node.h"

typedef struct null_token   null_token_t;
typedef struct number_token number_token_t;
typedef struct string_token string_token_t;
typedef struct node_token   node_token_t;

struct null_token
{
    const char* filename;
    position_t begin;
    position_t end;
};

struct number_token
{
    const char* filename;
    position_t begin;
    position_t end;
    double value;
};

struct string_token
{
    const char* filename;
    position_t begin;
    position_t end;
    const char* value;
};

struct node_token
{
    const char* filename;
    position_t begin;
    position_t end;
    node_t* value;
};

int yylex(void);
int yyparse(void);

void set_input(const char* filename);
const char* get_current_filename(void);
position_t get_current_position(void);
