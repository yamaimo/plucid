/**
 * @file  node.c
 * @brief C source file for abstract syntax tree node.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "node.h"

/*----------------------------------------*/

/* base */

typedef void (*output_func_t)(const node_t* node);

struct node
{
    output_func_t output;
};

static void output(const node_t* node)
{
    node->output(node);
}

/*----------------------------------------*/

/* list */

/*
 * list structure is the following:
 *
 *           p <- list_node
 *          / \
 *        ... node
 *       e2
 *      / \
 *     e1 node2
 *    / \
 * null node1
 *
 */

typedef struct list_node list_node_t;

struct list_node
{
    node_t base;
    node_t* prev;
    node_t* node;
};

static int list_node_get_count(node_t* node);
static void list_node_output(const node_t* node);

node_t* node_create_list(node_t* node)
{
    list_node_t* list_node = (list_node_t*)malloc(sizeof(list_node_t));

    list_node->base.output = list_node_output;
    list_node->prev = NULL;
    list_node->node = node;

    return (node_t*)list_node;
}

node_t* node_append_list(node_t* list, node_t* node)
{
    list_node_t* list_node = (list_node_t*)malloc(sizeof(list_node_t));

    list_node->base.output = list_node_output;
    list_node->prev = list;
    list_node->node = node;

    return (node_t*)list_node;
}

static int list_node_get_count(node_t* list)
{
    list_node_t* list_node = (list_node_t*)list;
    return (list_node->prev == NULL) ? 1 : (list_node_get_count(list_node->prev) + 1);
}

static void list_node_output(const node_t* node)
{
    list_node_t* list_node = (list_node_t*)node;
    if (list_node->prev != NULL)
    {
        output(list_node->prev);
    }
    output(list_node->node);
}

/*----------------------------------------*/

/* constant */

typedef struct number_const_node    number_const_node_t;
typedef struct string_const_node    word_const_node_t;
typedef struct string_const_node    string_const_node_t;
typedef struct string_const_node    special_const_node_t;

struct number_const_node
{
    node_t base;
    double value;
};

struct string_const_node
{
    node_t base;
    const char* value;
};

static void number_const_node_output(const node_t* node);
static void word_const_node_output(const node_t* node);
static void string_const_node_output(const node_t* node);
static void special_const_node_output(const node_t* node);

node_t* node_create_number_const(double number)
{
    number_const_node_t* number_const_node
        = (number_const_node_t*)malloc(sizeof(number_const_node_t));

    number_const_node->base.output = number_const_node_output;
    number_const_node->value = number;

    return (node_t*)number_const_node;
}

node_t* node_create_word_const(const char* word)
{
    word_const_node_t* word_const_node
        = (word_const_node_t*)malloc(sizeof(word_const_node_t));

    word_const_node->base.output = word_const_node_output;
    word_const_node->value = word;

    return (node_t*)word_const_node;
}

node_t* node_create_string_const(const char* string)
{
    string_const_node_t* string_const_node
        = (string_const_node_t*)malloc(sizeof(string_const_node_t));

    string_const_node->base.output = string_const_node_output;
    string_const_node->value = string;

    return (node_t*)string_const_node;
}

node_t* node_create_special_const(const char* name)
{
    special_const_node_t* special_const_node
        = (special_const_node_t*)malloc(sizeof(special_const_node_t));

    special_const_node->base.output = special_const_node_output;
    special_const_node->value = name;

    return (node_t*)special_const_node;
}

static void number_const_node_output(const node_t* node)
{
    number_const_node_t* number_const_node = (number_const_node_t*)node;
    if (number_const_node->value < 0)
    {
        fprintf(stdout, " [ const [ numb ~%-10.5f ]  ] ", - number_const_node->value);
    }
    else
    {
        fprintf(stdout, " [ const [ numb %-10.5f ]  ] ", number_const_node->value);
    }
}

static void word_const_node_output(const node_t* node)
{
    word_const_node_t* word_const_node = (word_const_node_t*)node;
    fprintf(stdout, " [ const [ word `%s' ]  ] ", word_const_node->value);
}

static void string_const_node_output(const node_t* node)
{
    string_const_node_t* string_const_node = (string_const_node_t*)node;
    fprintf(stdout, " [ const [ string `%s' ]  ] ", string_const_node->value);
}

static void special_const_node_output(const node_t* node)
{
    special_const_node_t* special_const_node = (special_const_node_t*)node;
    fprintf(stdout, " [ const [ special `%s' ]  ] ", special_const_node->value);
}

/*----------------------------------------*/

/* expression */

typedef struct where_node   where_node_t;
typedef struct body_node    body_node_t;
typedef struct op_node      op_node_t;
typedef struct op_node      var_node_t;

struct where_node
{
    node_t base;
    node_t* expr;
    node_t* body;
    node_t* file;
};

struct body_node
{
    node_t base;
    node_t* decl_list;
    node_t* def_list;
};

struct op_node
{
    node_t base;
    const char* name;
    int arg_count;
    node_t* arg_list;
    node_t* file;
};

static void where_node_output(const node_t* node);
static void body_node_output(const node_t* node);
static void op_node_output(const node_t* node);
static void var_node_output(const node_t* node);

node_t* node_create_where_expr(node_t* expr, node_t* body, node_t* file)
{
    where_node_t* where_node = (where_node_t*)malloc(sizeof(where_node_t));

    where_node->base.output = where_node_output;
    where_node->expr = expr;
    where_node->body = body;
    where_node->file = file;

    return (node_t*)where_node;
}

node_t* node_create_where_body(node_t* decl_list, node_t* def_list)
{
    body_node_t* body_node = (body_node_t*)malloc(sizeof(body_node_t));

    body_node->base.output = body_node_output;
    body_node->decl_list = decl_list;
    body_node->def_list = def_list;

    return (node_t*)body_node;
}

node_t* node_create_if_expr(node_t* cond_expr, node_t* true_expr, node_t* false_expr, node_t* file)
{
    /* 'if expression' is treated as a ternary operator. */
    op_node_t* op_node = (op_node_t*)malloc(sizeof(op_node_t));

    node_t* arg_list = node_create_list(cond_expr);
    arg_list = node_append_list(arg_list, true_expr);
    arg_list = node_append_list(arg_list, false_expr);

    op_node->base.output = op_node_output;
    op_node->name = "if";
    op_node->arg_count = 0; // NOTE: following the original code.
    op_node->arg_list = arg_list;
    op_node->file = file;

    return (node_t*)op_node;
}

node_t* node_create_special_op(const char* name, node_t* file)
{
    op_node_t* op_node = (op_node_t*)malloc(sizeof(op_node_t));

    op_node->base.output = op_node_output;
    op_node->name = name;
    op_node->arg_count = 0;
    op_node->arg_list
        = node_create_list(node_create_number_const(0.0));  // NOTE: following the original code.
    op_node->file = file;

    return (node_t*)op_node;
}

node_t* node_create_prefix_op(const char* name, node_t* arg, node_t* file)
{
    op_node_t* op_node = (op_node_t*)malloc(sizeof(op_node_t));

    op_node->base.output = op_node_output;
    op_node->name = name;
    op_node->arg_count = 0;
    op_node->arg_list = node_create_list(arg),
    op_node->file = file;

    return (node_t*)op_node;
}

node_t* node_create_infix_op(const char* name, node_t* left, node_t* right, node_t* file)
{
    op_node_t* op_node = (op_node_t*)malloc(sizeof(op_node_t));

    node_t* arg_list = node_create_list(left);
    arg_list = node_append_list(arg_list, right);

    op_node->base.output = op_node_output;
    op_node->name = name;
    op_node->arg_count = 0; // NOTE: following the original code.
    op_node->arg_list = arg_list;
    op_node->file = file;

    return (node_t*)op_node;
}

node_t* node_create_ternary_op(const char* name, node_t* arg1, node_t* arg2, node_t* arg3, node_t* file)
{
    op_node_t* op_node = (op_node_t*)malloc(sizeof(op_node_t));

    node_t* arg_list = node_create_list(arg1);
    arg_list = node_append_list(arg_list, arg2);
    arg_list = node_append_list(arg_list, arg3);

    op_node->base.output = op_node_output;
    op_node->name = name;
    op_node->arg_count = 3;
    op_node->arg_list = arg_list;
    op_node->file = file;

    return (node_t*)op_node;
}

node_t* node_create_variable_ref(const char* name, node_t* file)
{
    var_node_t* var_node = (var_node_t*)malloc(sizeof(var_node_t));

    var_node->base.output = var_node_output;
    var_node->name = name;
    var_node->arg_count = 0;
    var_node->arg_list = NULL;
    var_node->file = file;

    return (node_t*)var_node;
}

node_t* node_create_func_call(const char* name, node_t* arg_list, node_t* file)
{
    var_node_t* var_node = (var_node_t*)malloc(sizeof(var_node_t));

    var_node->base.output = var_node_output;
    var_node->name = name;
    var_node->arg_count = list_node_get_count(arg_list),
    var_node->arg_list = arg_list;
    var_node->file = file;

    return (node_t*)var_node;
}

static void where_node_output(const node_t* node)
{
    where_node_t* where_node = (where_node_t*)node;
    fprintf(stdout, " [ where ");
    output(where_node->expr);
    output(where_node->body);
    output(where_node->file);
    fprintf(stdout," ] ");
}

static void body_node_output(const node_t* node)
{
    body_node_t* body_node = (body_node_t*)node;
    if (body_node->decl_list != NULL)
    {
        output(body_node->decl_list);
    }
    if (body_node->def_list != NULL)
    {
        output(body_node->def_list);
    }
}

static void op_node_output(const node_t* node)
{
    op_node_t* op_node = (op_node_t*)node;
    fprintf(stdout, "[ op %s %d ", op_node->name, op_node->arg_count);
    if (op_node->arg_list != NULL)
    {
        output(op_node->arg_list);
    }
    output(op_node->file);
    fprintf(stdout," ] ");
}

static void var_node_output(const node_t* node)
{
    var_node_t* var_node = (var_node_t*)node;
    fprintf(stdout, " [ var %s %d ", var_node->name, var_node->arg_count);
    if (var_node->arg_list != NULL)
    {
        output(var_node->arg_list);
    }
    output(var_node->file);
    fprintf(stdout, " ] ");
}

/*----------------------------------------*/

/* definition */

typedef struct def_node         def_node_t;
typedef struct param_list_node  param_list_node_t;

struct def_node
{
    node_t base;
    const char* name;
    int param_count;
    node_t* param_list;
    node_t* expr;
    node_t* file;
};

/*
 * NOTE: param_list_node structure is the same as list_node,
 * except item is not node_t*, but string.
 */
struct param_list_node
{
    node_t base;
    node_t* prev;
    const char* name;
};

static int param_list_node_get_count(node_t* param_list);
static void def_node_output(const node_t* node);
static void param_list_node_output(const node_t* node);

node_t* node_create_variable_def(const char* name, node_t* expr, node_t* file)
{
    def_node_t* def_node = (def_node_t*)malloc(sizeof(def_node_t));

    def_node->base.output = def_node_output;
    def_node->name = name;
    def_node->param_count = 0;
    def_node->param_list = NULL;
    def_node->expr = expr;
    def_node->file = file;

    return (node_t*)def_node;
}

node_t* node_create_func_def(const char* name, node_t* param_list, node_t* expr, node_t* file)
{
    def_node_t* def_node = (def_node_t*)malloc(sizeof(def_node_t));

    def_node->base.output = def_node_output;
    def_node->name = name;
    def_node->param_count = param_list_node_get_count(param_list);
    def_node->param_list = param_list;
    def_node->expr = expr;
    def_node->file = file;

    return (node_t*)def_node;
}

node_t* node_create_param(const char* name)
{
    param_list_node_t* param_list_node = (param_list_node_t*)malloc(sizeof(param_list_node_t));

    param_list_node->base.output = param_list_node_output;
    param_list_node->prev = NULL;
    param_list_node->name = name;

    return (node_t*)param_list_node;
}

node_t* node_append_param(node_t* param_list, const char* name)
{
    param_list_node_t* param_list_node = (param_list_node_t*)malloc(sizeof(param_list_node_t));

    param_list_node->base.output = param_list_node_output;
    param_list_node->prev = param_list;
    param_list_node->name = name;

    return (node_t*)param_list_node;
}

static int param_list_node_get_count(node_t* param_list)
{
    param_list_node_t* param_list_node = (param_list_node_t*)param_list;
    return (param_list_node->prev == NULL) ? 1 : (param_list_node_get_count(param_list_node->prev) + 1);
}

static void def_node_output(const node_t* node)
{
    def_node_t* def_node = (def_node_t*)node;
    fprintf(stdout, " [ defn %s %d ", def_node->name, def_node->param_count);
    if (def_node->param_count > 0)
    {
        output(def_node->param_list);
    }
    output(def_node->expr);
    output(def_node->file);
    fprintf(stdout, " ] ");
}

static void param_list_node_output(const node_t* node)
{
    param_list_node_t* param_list_node = (param_list_node_t*)node;
    if (param_list_node->prev != NULL)
    {
        output(param_list_node->prev);
    }
    fprintf(stdout, " %s ", param_list_node->name);
}

/*----------------------------------------*/

/* declaration */

typedef struct decl_node decl_node_t;

struct decl_node
{
    node_t base;
    const char* name;
    node_t* expr;
    node_t* file;
};

static void decl_node_output(const node_t* node);

node_t* node_create_decl(const char* name, node_t* expr, node_t* file)
{
    decl_node_t* decl_node = (decl_node_t*)malloc(sizeof(decl_node_t));

    decl_node->base.output = decl_node_output;
    decl_node->name = name;
    decl_node->expr = expr;
    decl_node->file = file;

    return (node_t*)decl_node;
}

static void decl_node_output(const node_t* node)
{
    decl_node_t* decl_node = (decl_node_t*)node;
    fprintf(stdout, " [ decl %s ", decl_node->name);
    output(decl_node->expr);
    output(decl_node->file);
    fprintf(stdout, " ] ");
}

/*----------------------------------------*/

/* file */

typedef struct file_node file_node_t;

struct file_node
{
    node_t base;
    const char* name;
    position_t begin;
    position_t end;
};

static void file_node_output(const node_t* node);

node_t* node_create_file(const char* name, position_t begin, position_t end)
{
    file_node_t* file_node = (file_node_t*)malloc(sizeof(file_node_t));

    file_node->base.output = file_node_output;
    file_node->name = name;
    file_node->begin = begin;
    file_node->end = end;

    return (node_t*)file_node;
}

static void file_node_output(const node_t* node)
{
    file_node_t* file_node = (file_node_t*)node;
    fprintf(
        stdout,
        " [ file `%s' %d %d %d ] ",
        file_node->name,
        file_node->begin.line,
        file_node->end.line,
        file_node->begin.column);
}

/*----------------------------------------*/

/* output */

void pass1(node_t* node)
{
    output(node);
}
