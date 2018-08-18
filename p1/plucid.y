/**
 * @file  plucid.y
 * @brief Yacc source code for pLucid parser.
 *        The c header file `y.tab.h' and the c source file `y.tab.c'
 *        will be generated with `yacc -d' command.
 */

%{

#include <stdio.h>

#include "plucid.h"

#define NESTINGDEPTH       50

/*
 * Used to keep track of the current expression part
 * of a case statement i.e. case <expr> of ... end
 */
static node_t*  expval[NESTINGDEPTH] = {NULL};
static int      expvalcnt = 0;

static int error_count = 0;

void yyerror(const char* message);
void report_error(const char* filename, position_t position, const char* message);

%}

/* Stack type */

%union
{
    null_token_t    null;
    number_token_t  number;
    string_token_t  string;
    node_token_t    node;
}

/* Token and its type */

%token <string> LUCID_FUNC0         /* 'index' */
%token <string> LUCID_FUNC1         /* 'first', 'next' */
%token <string> LUCID_OP            /* 'asa', 'wvr'('whenever'), 'upon', 'attime' */
%token <string> FBY                 /* 'fby' */
%token <string> SPECIAL_CONST       /* 'eod', 'error', 'nil' */
%token <string> BUILT_IN_FUNC1      /* 'abs', 'sqrt', 'sin', 'cos', 'tan', 'log', 'log10',
                                       'hd', 'tl', 'mkword', 'mkstring', 'length', 'arg'
                                       'isnull', 'isvalueer', 'islist', 'isatom', 'isword',
                                       'isstring', 'iserror', 'iseod' */
%token <string> BUILT_IN_FUNC3      /* 'substr', 'filter' */
%token <null>   COMMA               /* ',' */
%token <null>   SEMICOLON           /* ';' */
%token <null>   LEFT_PAREN          /* '(' */
%token <null>   RIGHT_PAREN         /* ')' */
%token <null>   DEFINE              /* '=' */
%token <string> WHERE               /* 'where' */
%token <string> IS                  /* 'is' */
%token <string> CURRENT             /* 'current' */
%token <string> END                 /* 'end' */
%token <string> IF                  /* 'if' */
%token <string> THEN                /* 'then' */
%token <string> ELSE                /* 'else' */
%token <string> ELSEIF              /* 'elseif' */
%token <string> FI                  /* 'fi' */
%token <string> CASE                /* 'case' */
%token <string> OF                  /* 'of' */
%token <null>   COLON               /* ':' */
%token <string> DEFAULT             /* 'default' */
%token <string> COND                /* 'cond' */
%token <string> CONS                /* '::' */
%token <string> APPEND              /* '<>' */
%token <string> OR                  /* 'or' */
%token <string> AND                 /* 'and' */
%token <string> NOT                 /* 'not' */
%token <string> REL_OP              /* 'eq', 'ne', '<=', '<', '>=', '>' */
%token <string> ADD_OP              /* '+', '-' */
%token <string> MUL_OP              /* '*', '/', 'div', 'mod' */
%token <string> EXP                 /* '**' */
%token <string> STR_CONCAT          /* '^' */
%token <null>   LIST_CONST_BEGIN    /* '[' (list literal start) */
%token <null>   LIST_CONST_END      /* ']' (list literal end) */
%token <null>   EXPR_LIST_BEGIN     /* '[%' (expression list start) */
%token <null>   EXPR_LIST_END       /* '%]' (expression list end) */
%token <string> IDENT               /* <identifier> */
%token <number> NUM_CONST           /* <numeric literal> */
%token <string> WORD_CONST          /* <word literal> (without quotes) */
%token <string> STR_CONST           /* <string literal> (without quotes) */

/* Nonterminal symbol and its type */

%type <node> accept
%type <node> expr
%type <node> where_body
%type <node> definition_list
%type <node> definition
%type <node> parameter_list
%type <node> declaration_list
%type <node> declaration
%type <node> argument_list
%type <node> constant
%type <node> list_constant
%type <node> list_constant_body
%type <node> expr_list
%type <node> expr_list_body
%type <node> if_expr
%type <node> end_if
%type <node> case_expr
%type <node> case_body
%type <node> cond_expr
%type <node> cond_body

/* Precedence (lowest to highest) and associativity */

%left       WHERE
%left       LUCID_OP
%right      FBY
%left       IF CASE COND
%right      CONS APPEND
%left       OR
%left       AND
%left       NOT
%nonassoc   REL_OP
%left       ADD_OP
%left       MUL_OP
%left       EXP
%right      STR_CONCAT
%left       SPECIAL_CONST LUCID_FUNC0 LUCID_FUNC1 BUILT_IN_FUNC1

/* initialize error count */

%initial-action {
    error_count = 0;
}

%%

accept
    : expr
        {
            $$ = $1;
            if (error_count == 0)
            {
                pass1($$.value);
                YYACCEPT;
            }
            else
            {
                YYABORT;
            }
        }
    | expr SEMICOLON
        {
            $$ = $1;
            if (error_count == 0)
            {
                pass1($$.value);
                YYACCEPT;
            }
            else
            {
                YYABORT;
            }
        }
    ;

expr
    : expr WHERE where_body END
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $4.end;
            $$.value = node_create_where_expr(
                $1.value,
                $3.value,
                node_create_file($2.filename, $2.begin, $4.end));
        }
    | expr WHERE END
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = $1.value;
        }
    | IDENT
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = node_create_variable_ref(
                $1.value,
                node_create_file($1.filename, $1.begin, $1.end));
        }
    | IDENT LEFT_PAREN argument_list RIGHT_PAREN
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $4.end;
            $$.value = node_create_func_call(
                $1.value,
                $3.value,
                node_create_file($1.filename, $1.begin, $4.end));
        }
    | constant
        {
            $$ = $1;
        }
    | list_constant
        {
            $$ = $1;
        }
    | SPECIAL_CONST
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = node_create_special_const($1.value);
        }
    | expr_list
        {
            $$ = $1;
        }
    | if_expr
        {
            $$ = $1;
        }
    | case_expr
        {
            $$ = $1;
        }
    | cond_expr
        {
            $$ = $1;
        }
    | LEFT_PAREN expr RIGHT_PAREN
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = $2.value;
        }
    | LUCID_FUNC0
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = node_create_special_op(
                $1.value,
                node_create_file($1.filename, $1.begin, $1.end));

        }
    | expr LUCID_OP expr
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_create_infix_op(
                $2.value,
                $1.value,
                $3.value,
                node_create_file($2.filename, $2.begin, $3.end));
            }
    | expr FBY expr
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_create_infix_op(
                $2.value,
                $1.value,
                $3.value,
                node_create_file($2.filename, $2.begin, $3.end));
        }
    | LUCID_FUNC1 expr
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $2.end;
            $$.value = node_create_prefix_op(
                $1.value,
                $2.value,
                node_create_file($1.filename, $1.begin, $2.end));
        }
    | BUILT_IN_FUNC1 expr
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $2.end;
            $$.value = node_create_prefix_op(
                $1.value,
                $2.value,
                node_create_file($1.filename, $1.begin, $2.end));
        }
    | BUILT_IN_FUNC3 LEFT_PAREN expr COMMA expr COMMA expr RIGHT_PAREN
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $8.end;
            $$.value = node_create_ternary_op(
                $1.value,
                $3.value,
                $5.value,
                $7.value,
                node_create_file($1.filename, $1.begin, $8.end));
        }
    | expr STR_CONCAT expr
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_create_infix_op(
                $2.value,
                $1.value,
                $3.value,
                node_create_file($2.filename, $2.begin, $3.end));
        }
    | expr CONS expr
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_create_infix_op(
                $2.value,
                $1.value,
                $3.value,
                node_create_file($2.filename, $2.begin, $3.end));
        }
    | expr APPEND expr
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_create_infix_op(
                $2.value,
                $1.value,
                $3.value,
                node_create_file($2.filename, $2.begin, $3.end));
        }
    | expr ADD_OP expr
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_create_infix_op(
                $2.value,
                $1.value,
                $3.value,
                node_create_file($2.filename, $2.begin, $3.end));
        }
    | ADD_OP expr %prec MUL_OP
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $2.end;
            if (string_is_same($1.value, "minus"))
            {
                $$.value = node_create_prefix_op(
                    "uminus",
                    $2.value,
                    node_create_file($1.filename, $1.begin, $2.end));
            }
            else
            {
                $$.value = $2.value;
            }
        }
    | expr MUL_OP expr
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_create_infix_op(
                $2.value,
                $1.value,
                $3.value,
                node_create_file($2.filename, $2.begin, $3.end));
        }
    | expr EXP expr
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_create_infix_op(
                $2.value,
                $1.value,
                $3.value,
                node_create_file($2.filename, $2.begin, $3.end));
        }
    | expr REL_OP expr
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_create_infix_op(
                $2.value,
                $1.value,
                $3.value,
                node_create_file($2.filename, $2.begin, $3.end));
        }
    | NOT expr %prec AND
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $2.end;
            $$.value = node_create_prefix_op(
                $1.value,
                $2.value,
                node_create_file($1.filename, $1.begin, $1.end));
        }
    | expr AND expr
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_create_infix_op(
                $2.value,
                $1.value,
                $3.value,
                node_create_file($2.filename, $2.begin, $2.end));
        }
    | expr OR expr
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_create_infix_op(
                $2.value,
                $1.value,
                $3.value,
                node_create_file($2.filename, $2.begin, $3.end));
        }
    | error
        {
            report_error(
                get_current_filename(),
                get_current_position(),
                "error expression.");
        }
    ;

where_body
    : definition_list
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = node_create_where_body(NULL, $1.value);
        }
    | declaration_list
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = node_create_where_body($1.value, NULL);
        }
    | declaration_list definition_list
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $2.end;
            $$.value = node_create_where_body($1.value, $2.value);
        }
    ;

definition_list
    : definition
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = node_create_list($1.value);
        }
    | definition_list definition
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $2.end;
            $$.value = node_append_list($1.value, $2.value);
        }
    ;

definition
    : IDENT DEFINE expr SEMICOLON
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $4.end;
            $$.value = node_create_variable_def(
                $1.value,
                $3.value,
                node_create_file($1.filename, $1.begin, $4.end));
        }
    | IDENT LEFT_PAREN parameter_list RIGHT_PAREN DEFINE expr SEMICOLON
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $7.end;
            $$.value = node_create_func_def(
                $1.value,
                $3.value,
                $6.value,
                node_create_file($1.filename, $1.begin, $7.end));
        }
    ;

parameter_list
    : IDENT
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = node_create_param($1.value);
        }
    | parameter_list COMMA IDENT
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_append_param($1.value, $3.value);
        }
    | parameter_list error IDENT
        {
            report_error($1.filename, $1.end, "',' expected.");
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_append_param($1.value, $3.value);
        }
    ;

declaration_list
    : declaration
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = node_create_list($1.value);
        }
    | declaration_list declaration
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $2.end;
            $$.value = node_append_list($1.value, $2.value);
        }
    ;

declaration
    : IDENT IS CURRENT expr SEMICOLON
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $5.end;
            $$.value = node_create_decl(
                $1.value,
                $4.value,
                node_create_file($1.filename, $1.begin, $5.end));
        }
    ;

argument_list
    : expr
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = node_create_list($1.value);
        }
    | argument_list COMMA expr
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_append_list($1.value, $3.value);
        }
    | argument_list error expr
        {
            report_error($1.filename, $1.end, "',' expected.");
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_append_list($1.value, $3.value);
        }
    ;

constant
    : NUM_CONST
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = node_create_number_const($1.value);
        }
    | WORD_CONST
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = node_create_word_const($1.value);
        }
    | STR_CONST
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = node_create_string_const($1.value);
        }
    ;

list_constant
    : LIST_CONST_BEGIN list_constant_body LIST_CONST_END
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = $2.value;
        }
    | LIST_CONST_BEGIN LIST_CONST_END
        {
            /* [ ] -> nil */
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $2.end;
            $$.value = node_create_special_const("nil");
        }
    ;

list_constant_body
    : constant
        {
            /* constant -> constant::nil */
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = node_create_infix_op(
                "cons",
                $1.value,
                node_create_special_const("nil"),
                node_create_file($1.filename, $1.begin, $1.end));
        }
    | list_constant
        {
            /* list_constant -> list_constant::nil */
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = node_create_infix_op(
                "cons",
                $1.value,
                node_create_special_const("nil"),
                node_create_file($1.filename, $1.begin, $1.end));
        }
    | constant list_constant_body
        {
            /* constant list_constant_body -> constant::list_constant_body */
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $2.end;
            $$.value = node_create_infix_op(
                "cons",
                $1.value,
                $2.value,
                node_create_file($1.filename, $2.begin, $2.end));
        }
    | list_constant list_constant_body
        {
            /* list_constant list_constant_body -> list_constant::list_constant_body */
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $2.end;
            $$.value = node_create_infix_op(
                "cons",
                $1.value,
                $2.value,
                node_create_file($1.filename, $2.begin, $2.end));
        }
    ;

expr_list
    : EXPR_LIST_BEGIN expr_list_body EXPR_LIST_END
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = $2.value;
        }
    | EXPR_LIST_BEGIN EXPR_LIST_END
        {
            /* [% %] -> nil */
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $2.end;
            $$.value = node_create_special_const("nil");
        }
    ;

expr_list_body
    : expr
        {
            /* expr -> expr::nil */
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $1.end;
            $$.value = node_create_infix_op(
                "cons",
                $1.value,
                node_create_special_const("nil"),
                node_create_file($1.filename, $1.begin, $1.end));
        }
    | expr COMMA expr_list_body
        {
            /* expr,expr_list_body -> expr::expr_list_body */
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = node_create_infix_op(
                "cons",
                $1.value,
                $3.value,
                node_create_file($2.filename, $2.begin, $3.end));
        }
    ;

if_expr
    : IF expr THEN expr end_if
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $5.end;
            $$.value = node_create_if_expr(
                $2.value,
                $4.value,
                $5.value,
                node_create_file($1.filename, $1.begin, $5.end));
        }
    | IF expr THEN expr error
        {
            report_error($4.filename, $4.end, "'else' or 'elseif' expected.");
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $4.end;
            $$.value = node_create_if_expr(
                $2.value,
                $4.value,
                NULL,
                node_create_file($1.filename, $1.begin, $4.end));
        }
    | IF expr error expr end_if
        {
            report_error($2.filename, $2.end, "'then' expected.");
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $5.end;
            $$.value = node_create_if_expr(
                $2.value,
                $4.value,
                $5.value,
                node_create_file($1.filename, $1.begin, $5.end));
        }
    ;

end_if
    : ELSEIF expr THEN expr end_if
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $5.end;
            $$.value = node_create_if_expr(
                $2.value,
                $4.value,
                $5.value,
                node_create_file($1.filename, $1.begin, $5.end));
        }
    | ELSEIF expr THEN expr error
        {
            report_error($4.filename, $4.end, "'else' or 'elseif' expected.");
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $4.end;
            $$.value = node_create_if_expr(
                $2.value,
                $4.value,
                NULL,
                node_create_file($1.filename, $1.begin, $4.end));
        }
    | ELSEIF expr error expr end_if
        {
            report_error($2.filename, $2.end, "'then' expected.");
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $5.end;
            $$.value = node_create_if_expr(
                $2.value,
                $4.value,
                $5.value,
                node_create_file($1.filename, $1.begin, $5.end));
        }
    | ELSE expr FI
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = $2.value;
        }
    | ELSE expr error
        {
            report_error($2.filename, $2.end, "'fi' expected.");
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $2.end;
            $$.value = $2.value;
        }
    ;

case_expr
    : CASE
        {
            expvalcnt++;
        }
      expr
        {
            expval[expvalcnt] = $3.value;
        }
      OF case_body END
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $7.end;
            $$.value = $6.value;
            expvalcnt--;
        }
    ;

case_body
    : expr COLON expr SEMICOLON case_body
        {
            /*
             * case expr of
             *     expr01: expr11;
             *     expr02: expr12;
             *     ...
             *     default: exprD;
             * end
             * ->
             * if     expr == expr01 then expr11
             * elseif expr == expr02 then expr12
             * ...
             * else exprD
             * fi
             */
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $5.end;
            $$.value = node_create_if_expr(
                node_create_infix_op(
                    "eq",
                    expval[expvalcnt],
                    $1.value,
                    node_create_file($2.filename, $2.begin, $5.end)),
                $3.value,
                $5.value,
                node_create_file($2.filename, $2.begin, $5.end));
        }
    | expr error expr SEMICOLON case_body
        {
            report_error($1.filename, $1.end, "':' expected.");
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $5.end;
            $$.value = node_create_if_expr(
                node_create_infix_op(
                    "eq",
                    expval[expvalcnt],
                    $1.value,
                    node_create_file($3.filename, $3.begin, $5.end)),
                $3.value,
                $5.value,
                node_create_file($3.filename, $3.begin, $5.end));
        }
    | expr COLON expr error case_body
        {
            report_error($3.filename, $3.end, "';' expected.");
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $5.end;
            $$.value = node_create_if_expr(
                node_create_infix_op(
                    "eq",
                    expval[expvalcnt],
                    $1.value,
                    node_create_file($2.filename, $2.begin, $5.end)),
                $3.value,
                $5.value,
                node_create_file($2.filename, $2.begin, $5.end));
        }
    | DEFAULT COLON expr SEMICOLON
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $4.end;
            $$.value = $3.value;
        }
    | DEFAULT COLON expr error
        {
            report_error($3.filename, $3.end, "';' expected.");
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = $3.value;
        }
    | DEFAULT error expr SEMICOLON
        {
            report_error($1.filename, $1.end, "':' expected.");
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $4.end;
            $$.value = $3.value;
        }
    ;

cond_expr
    : COND cond_body END
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = $2.value;
        }
    ;

cond_body
    : expr COLON expr SEMICOLON cond_body
        {
            /*
             * cond
             *     expr01: expr11;
             *     expr02: expr12;
             *     ...
             *     default: exprD;
             * end
             * ->
             * if     expr01 then expr11
             * elseif expr02 then expr12
             * ...
             * else exprD
             * fi
             */
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $5.end;
            $$.value = node_create_if_expr(
                $1.value,
                $3.value,
                $5.value,
                node_create_file($2.filename, $2.begin, $5.end));
        }
    | expr error expr SEMICOLON cond_body
        {
            report_error($1.filename, $1.end, "':' expected.");
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $5.end;
            $$.value = node_create_if_expr(
                $1.value,
                $3.value,
                $5.value,
                node_create_file($3.filename, $3.begin, $5.end));
        }
    | expr COLON expr error cond_body
        {
            report_error($3.filename, $3.end, "';' expected.");
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $5.end;
            $$.value = node_create_if_expr(
                $1.value,
                $3.value,
                $5.value,
                node_create_file($2.filename, $2.begin, $5.end));
        }
    | DEFAULT COLON expr SEMICOLON
        {
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $4.end;
            $$.value = $3.value;
        }
    | DEFAULT error expr SEMICOLON
        {
            report_error($1.filename, $1.end, "':' expected.");
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $4.end;
            $$.value = $3.value;
        }
    | DEFAULT COLON expr error
        {
            report_error($3.filename, $3.end, "';' expected.");
            $$.filename = $1.filename;
            $$.begin = $1.begin;
            $$.end = $3.end;
            $$.value = $3.value;
        }
    ;

%%

void yyerror(const char* message)
{
    fprintf(stderr, "%s\n", message);
}

void report_error(const char* filename, position_t position, const char* message)
{
    error_count++;
	fprintf(
        stderr,
        "(file: %s, line: %d, column: %d) %s\n",
        filename,
        position.line,
        position.column,
        message);
}
