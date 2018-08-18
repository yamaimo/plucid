/**
 * @file  node.h
 * @brief C header file for abstract syntax tree node.
 */

#pragma once

#include "common.h"

typedef struct position position_t;
typedef struct node node_t;

struct position
{
    int line;
    int column;
};

/* list */
node_t* node_create_list(node_t* node);
node_t* node_append_list(node_t* list, node_t* node);

/* constant */
node_t* node_create_number_const(double number);
node_t* node_create_word_const(const char* word);
node_t* node_create_string_const(const char* string);
node_t* node_create_special_const(const char* name);

/* expression */
node_t* node_create_where_expr(node_t* expr, node_t* body, node_t* file);
node_t* node_create_where_body(node_t* decl_list, node_t* def_list);
node_t* node_create_if_expr(node_t* cond_expr, node_t* true_expr, node_t* false_expr, node_t* file);
node_t* node_create_special_op(const char* name, node_t* file);
node_t* node_create_prefix_op(const char* name, node_t* arg, node_t* file);
node_t* node_create_infix_op(const char* name, node_t* left, node_t* right, node_t* file);
node_t* node_create_ternary_op(const char* name, node_t* arg1, node_t* arg2, node_t* arg3, node_t* file);
node_t* node_create_variable_ref(const char* name, node_t* file);
node_t* node_create_func_call(const char* name, node_t* arg_list, node_t* file);

/* definition */
node_t* node_create_variable_def(const char* name, node_t* expr, node_t* file);
node_t* node_create_func_def(const char* name, node_t* param_list, node_t* expr, node_t* file);
node_t* node_create_param(const char* name);
node_t* node_append_param(node_t* param_list, const char* name);

/* declaration */
node_t* node_create_decl(const char* name, node_t* expr, node_t* file);

/* file */
node_t* node_create_file(const char* name, position_t begin, position_t end);

/* output */
void pass1(node_t* node);
