/**
 * @file  plucid.l
 * @brief Lex source code for pLucid lexical analyzer.
 *        The c source code `lex.yy.c' will be generated with `lex' command.
 */

%{

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "plucid.h"
#include "y.tab.h"

static const char* libdir = "/usr/local/lib";

extern FILE* yyin;
extern char* yytext;
extern size_t yyleng;
extern YYSTYPE yylval;

static const char* current_filename;
static position_t current_position;

/* for keeping list const depth */
static size_t list_const_depth = 0u;

/* for scanning string literal */
static size_t string_length;
static char* string_buffer;

/* for including */
typedef struct context context_t;
struct context
{
    YY_BUFFER_STATE buffer;
    const char* filename;
    position_t position;
    context_t* prev;
};
static context_t* context_stack = NULL;
static char* include_filename = NULL;

static void set_yylval_position(void);
static void advance_column(void);
static void advance_line(void);

static bool find_keyword(const char* ident, int* token_id, const char** keyname);
static double get_number(const char* literal);

static void append_string(const char* string, size_t length);

static void include_file(const char* filename);
static bool recover_file(void);
static void push_context(context_t* context);
static context_t* pop_context(void);

#if DEBUG
#   include <ctype.h>
#   define DEBUG_LOG(token)                                         \
        printf("[%-15s] line: %3d, column: %3d, text: %s\n",             \
            token, current_position.line, current_position.column,  \
            (isspace(yytext[0]) ? "" : yytext))
#else
#   define DEBUG_LOG(token)
#endif

%}

IDENTIFIER      [[:alpha:]][[:alnum:]_]*
NUMBER          ~?[[:digit:]]+(\.[[:digit:]]*)?
WHITE_SPACES    [[:blank:]]+
LINE_COMMENT    "//".*$
LINE_FEED       (\r|\n|\r\n)
UNKNOWN         .
QUOTE           "\""
SIGN            [+\-*/$&=<>:#^]
BRACKET         ("("|")"|"[%"|"%]"|"(%"|"%)")
PERIOD          "."
SEPARATOR       (","|";")

%x IN_LIST_CONST
%x IN_STRING
%x IN_INCLUDE

%option noyywrap

%%

"," {
        DEBUG_LOG("COMMA");
        set_yylval_position();
        return COMMA;
    }

";" {
        DEBUG_LOG("SEMICOLON");
        set_yylval_position();
        return SEMICOLON;
    }

"(" {
        DEBUG_LOG("LEFT_PAREN");
        set_yylval_position();
        return LEFT_PAREN;
    }

")" {
        DEBUG_LOG("RIGHT_PAREN");
        set_yylval_position();
        return RIGHT_PAREN;
    }

"=" {
        DEBUG_LOG("DEFINE");
        set_yylval_position();
        return DEFINE;
    }

":" {
        DEBUG_LOG("COLON");
        set_yylval_position();
        return COLON;
    }

"::" {
        DEBUG_LOG("CONS");
        set_yylval_position();
        yylval.string.value = "cons";
        return CONS;
    }

"<>" {
        DEBUG_LOG("APPEND");
        set_yylval_position();
        yylval.string.value = "append";
        return APPEND;
    }

"+"|"-" {
        DEBUG_LOG("ADD_OP");
        set_yylval_position();
        yylval.string.value = string_is_same(yytext, "+") ? "plus" : "minus";
        return ADD_OP;
    }

"*"|"/" {
        DEBUG_LOG("MUL_OP");
        set_yylval_position();
        yylval.string.value = string_is_same(yytext, "*") ? "times" : "fdiv";
        return MUL_OP;
    }

"**" {
        DEBUG_LOG("EXP");
        set_yylval_position();
        yylval.string.value = "exp";
        return EXP;
    }

"^" {
        DEBUG_LOG("STR_CONCAT");
        set_yylval_position();
        yylval.string.value = "strconc";
        return STR_CONCAT;
    }

"<="|"<"|">="|">" {
        DEBUG_LOG("REL_OP");
        set_yylval_position();
        yylval.string.value
            = string_is_same(yytext, "<=") ? "le"
            : string_is_same(yytext, "<")  ? "lt"
            : string_is_same(yytext, ">=") ? "ge" : "gt";
        return REL_OP;
    }

<INITIAL,IN_LIST_CONST>"[" {
        DEBUG_LOG("LIST_CONST_BEGIN");
        set_yylval_position();

        if (list_const_depth == 0u)
        {
            BEGIN(IN_LIST_CONST);
        }
        list_const_depth++;

        return LIST_CONST_BEGIN;
    }

<IN_LIST_CONST>({IDENTIFIER}|{SIGN}+|{BRACKET}|{PERIOD}|{SEPARATOR}|{QUOTE}) {
        DEBUG_LOG("list:WORD_CONST");
        set_yylval_position();
        yylval.string.value = string_duplicate(yytext);
        return WORD_CONST;
    }

<IN_LIST_CONST>{WHITE_SPACES} {
        DEBUG_LOG("list:SPACES");
        advance_column();
    }

<IN_LIST_CONST>{LINE_FEED} {
        DEBUG_LOG("list:LINE_FEED");
        advance_line();
    }

<IN_LIST_CONST><<EOF>> {
        DEBUG_LOG("list:EOF");
        fprintf(stderr, "EOF reached with no closing bracket for list const.\n");
        my_exit(1);
    }

<IN_LIST_CONST>"]" {
        DEBUG_LOG("LIST_CONST_END");
        set_yylval_position();

        list_const_depth--;
        if (list_const_depth == 0u)
        {
            BEGIN(INITIAL);
        }
        return LIST_CONST_END;
    }

<IN_LIST_CONST>{UNKNOWN} {
        DEBUG_LOG("list:UNKNOWN");
        advance_column();
        fprintf(stderr, "unknown character in list const: %s\n", yytext);
        return yytext[0];
    }

"[%" {
        DEBUG_LOG("EXPR_LIST_BEGIN");
        set_yylval_position();
        return EXPR_LIST_BEGIN;
    }

"%]" {
        DEBUG_LOG("EXPR_LIST_END");
        set_yylval_position();
        return EXPR_LIST_END;
    }

{QUOTE}({IDENTIFIER}|{SIGN}+|{BRACKET}|{PERIOD}|{SEPARATOR}|{QUOTE}){QUOTE} {
        DEBUG_LOG("WORD_CONST");
        set_yylval_position();

        // trim quotes
        size_t word_length = yyleng - 2u;
        char* word = (char*)malloc(sizeof(char) * (word_length + 1u));
        strncpy(word, yytext + 1, word_length);
        word[word_length] = '\0';
        yylval.string.value = word;

        return WORD_CONST;
    }

"`" {
        DEBUG_LOG("string begin");
        set_yylval_position();

        string_buffer = (char*)malloc(sizeof(char));
        string_length = 0u;
        string_buffer[0] = '\0';
        BEGIN(IN_STRING);
    }

<IN_STRING>[^'\b\f\t\r\n\\]* {  /* characters except for ', \b, \f, \t, \r, \n, \ */
        DEBUG_LOG("str:chars");
        advance_column();
        append_string(yytext, yyleng);
    }

<IN_STRING>\b {
        DEBUG_LOG("str:\\b");
        advance_column();
        append_string("\\b", 2u);
    }

<IN_STRING>\f {
        DEBUG_LOG("str:\\f");
        advance_column();
        append_string("\\f", 2u);
    }

<IN_STRING>\t {
        DEBUG_LOG("str:\\t");
        advance_column();
        append_string("\\t", 2u);
    }

<IN_STRING>"\\"[\\`'] { /* \\ -> \, \` -> `, \' -> ' */
        /*
         * FIXME:
         * this rule may cause syntax error of pass2.
         * Scanner should not remove backslash for escape.
         */
        DEBUG_LOG("str:escape");
        advance_column();
        append_string(yytext + 1, 1u);
    }

<IN_STRING>"\\"[^\\`'\r\n] {    /* \<any char> -> \<any char> */
        DEBUG_LOG("str:ctrl");
        advance_column();
        append_string(yytext, yyleng);
    }

<IN_STRING>"\\" {
        DEBUG_LOG("str:\\");
        fprintf(stderr, "isolated backslash is found.\n");
        my_exit(1);
    }

<IN_STRING>{LINE_FEED} {
        DEBUG_LOG("str:LINE_FEED");
        fprintf(stderr, "end of line reached with no closing quote for string.\n");
        my_exit(1);
    }

<IN_STRING><<EOF>> {
        DEBUG_LOG("str:EOF");
        fprintf(stderr, "EOF reached with no closing quote for string.\n");
        my_exit(1);
    }

<IN_STRING>"'" {
        DEBUG_LOG("string end");
        advance_column();

        yylval.string.end = current_position;
        yylval.string.value = string_duplicate(string_buffer);
        free(string_buffer);

        BEGIN(INITIAL);
        return STR_CONST;
    }

<IN_STRING>{UNKNOWN} {
        DEBUG_LOG("str:UNKNOWN");
        advance_column();
        fprintf(stderr, "unknown character in string: %s\n", yytext);
        my_exit(1);
    }

"include" {
        DEBUG_LOG("include");
        advance_column();
        BEGIN(IN_INCLUDE);
    }

<IN_INCLUDE>{QUOTE}[^"<>\n]+{QUOTE} {
        DEBUG_LOG("inc:\"filename\"");
        advance_column();

        if (include_filename != NULL)
        {
            fprintf(stderr, "multiple include filename is specified.\n");
            my_exit(1);
        }

        // trim quotes
        size_t filename_length = yyleng - 2u;
        include_filename = (char*)malloc(sizeof(char) * (filename_length + 1u));
        strncpy(include_filename, yytext + 1, filename_length);
        include_filename[filename_length] = '\0';
    }

<IN_INCLUDE>"<"[^"<>\n]+">" {
        DEBUG_LOG("inc:<filename>");
        advance_column();

        if (include_filename != NULL)
        {
            fprintf(stderr, "multiple include filename is specified.\n");
            my_exit(1);
        }

        // trim brackets and insert libdir, i.e.
        // <xxx> -> /path/to/libdir/xxx
        size_t libdir_length = strlen(libdir);
        size_t filename_length = yyleng - 2u;
        include_filename = (char*)malloc(sizeof(char) * (libdir_length + 1u + filename_length + 1u));
        strncpy(include_filename, libdir, libdir_length);
        include_filename[libdir_length] = '/';
        strncpy(include_filename + libdir_length + 1, yytext + 1, filename_length);
        include_filename[libdir_length + 1 + filename_length] = '\0';
    }

<IN_INCLUDE>{WHITE_SPACES} {
        DEBUG_LOG("inc:SPACES");
        advance_column();
    }

<IN_INCLUDE>{LINE_FEED} {
        DEBUG_LOG("inc:LINE_FEED");
        fprintf(stderr, "end of line reached in including.\n");
        my_exit(1);
    }

<IN_INCLUDE><<EOF>> {
        DEBUG_LOG("inc:EOF");
        fprintf(stderr, "EOF reached in including.\n");
        my_exit(1);
    }

<IN_INCLUDE>";" {
        DEBUG_LOG("inc:SEMICOLON");
        advance_column();

        if (include_filename == NULL)
        {
            fprintf(stderr, "include filename is not specified.\n");
            my_exit(1);
        }

        include_file(include_filename);
        free(include_filename);
        include_filename = NULL;

        BEGIN(INITIAL);
    }

<IN_INCLUDE>{UNKNOWN} {
        DEBUG_LOG("inc:UNKNOWN");
        advance_column();
        fprintf(stderr, "unknown character in including: %s\n", yytext);
        my_exit(1);
    }

{IDENTIFIER} {
        DEBUG_LOG("IDENT");
        set_yylval_position();

        int token_id = 0;
        const char* keyname = NULL;
        bool found = find_keyword(yytext, &token_id, &keyname);
        if (found)
        {
            yylval.string.value = keyname;
        }
        else
        {
            token_id = IDENT;
            yylval.string.value = string_duplicate(yytext);
        }
        
        return token_id;
    }

{NUMBER} {
        DEBUG_LOG("NUMBER");
        set_yylval_position();
        yylval.number.value = get_number(yytext);
        return NUM_CONST;
    }

{WHITE_SPACES} {
        DEBUG_LOG("WHITE_SPACES");
        advance_column();
    }

{LINE_COMMENT} {
        DEBUG_LOG("LINE_COMMENT");
        advance_column();
    }

{LINE_FEED} {
        DEBUG_LOG("LINE_FEED");
        advance_line();
    }

<<EOF>> {
        DEBUG_LOG("EOF");
        bool recovered = recover_file();
        if (!recovered)
        {
            yyterminate();
        }
    }

{UNKNOWN} {
        DEBUG_LOG("UNKNOWN");
        advance_column();
        fprintf(stderr, "unknown character: %s\n", yytext);
        return yytext[0];
    }

%%

void set_input(const char* filename)
{
    if (filename != NULL)
    {
        FILE* input = fopen(filename, "r");
        if (input == NULL)
        {
            fprintf(stderr, "cannot open %s\n", filename);
            my_exit(1);
        }
        yyin = input;
        current_filename = string_duplicate(filename);
    }
    else
    {
        yyin = stdin;
        current_filename = "stdin";
    }
    current_position.line = 1;
    current_position.column = 1;
}

const char* get_current_filename(void)
{
    return current_filename;
}

position_t get_current_position(void)
{
    return current_position;
}

static void set_yylval_position(void)
{
    yylval.null.filename = current_filename;
    yylval.null.begin = current_position;
    advance_column();
    yylval.null.end = current_position;
}

static void advance_column(void)
{
    current_position.column += yyleng;
}

static void advance_line(void)
{
    current_position.line += 1;
    current_position.column = 1;
}

static bool find_keyword(const char* ident, int* token_id, const char** keyname)
{
    static const size_t KEYWORD_COUNT = 57u;
    static struct {
        const char* keyword;
        const char* keyname;    // if NULL, keyname is keyword.
        int token_id;
    } keyword_table[KEYWORD_COUNT] = {
        {"index",       "now",      LUCID_FUNC0},
        {"first",       NULL,       LUCID_FUNC1},
        {"next",        NULL,       LUCID_FUNC1},
        {"asa",         NULL,       LUCID_OP},
        {"wvr",         NULL,       LUCID_OP},
        {"whenever",    "wvr",      LUCID_OP},
        {"upon",        "delay",    LUCID_OP},
        {"attime",      "@t",       LUCID_OP},
        {"fby",         NULL,       FBY},
        {"eod",         NULL,       SPECIAL_CONST},
        {"error",       NULL,       SPECIAL_CONST},
        {"nil",         NULL,       SPECIAL_CONST},
        {"abs",         NULL,       BUILT_IN_FUNC1},
        {"sqrt",        NULL,       BUILT_IN_FUNC1},
        {"sin",         NULL,       BUILT_IN_FUNC1},
        {"cos",         NULL,       BUILT_IN_FUNC1},
        {"tan",         NULL,       BUILT_IN_FUNC1},
        {"log",         NULL,       BUILT_IN_FUNC1},
        {"log10",       NULL,       BUILT_IN_FUNC1},
        {"hd",          NULL,       BUILT_IN_FUNC1},
        {"tl",          NULL,       BUILT_IN_FUNC1},
        {"mkword",      NULL,       BUILT_IN_FUNC1},
        {"mkstring",    NULL,       BUILT_IN_FUNC1},
        {"length",      NULL,       BUILT_IN_FUNC1},
        {"arg",         NULL,       BUILT_IN_FUNC1},
        {"isnull",      "isnil",    BUILT_IN_FUNC1},
        {"isnumber",    NULL,       BUILT_IN_FUNC1},
        {"islist",      NULL,       BUILT_IN_FUNC1},
        {"isatom",      NULL,       BUILT_IN_FUNC1},
        {"isword",      NULL,       BUILT_IN_FUNC1},
        {"isstring",    NULL,       BUILT_IN_FUNC1},
        {"iserror",     NULL,       BUILT_IN_FUNC1},
        {"iseod",       NULL,       BUILT_IN_FUNC1},
        {"substr",      NULL,       BUILT_IN_FUNC3},
        {"filter",      NULL,       BUILT_IN_FUNC3},
        {"where",       NULL,       WHERE},
        {"is",          NULL,       IS},
        {"current",     NULL,       CURRENT},
        {"end",         NULL,       END},
        {"if",          NULL,       IF},
        {"then",        NULL,       THEN},
        {"else",        NULL,       ELSE},
        {"elseif",      NULL,       ELSEIF},
        {"fi",          NULL,       FI},
        {"case",        NULL,       CASE},
        {"of",          NULL,       OF},
        {"default",     NULL,       DEFAULT},
        {"cond",        NULL,       COND},
        {"or",          NULL,       OR},
        {"and",         NULL,       AND},
        {"not",         NULL,       NOT},
        {"eq",          NULL,       REL_OP},
        {"ne",          NULL,       REL_OP},
        {"div",         NULL,       MUL_OP},
        {"mod",         NULL,       MUL_OP},
        {"true",        NULL,       WORD_CONST},
        {"false",       NULL,       WORD_CONST},
    };

    bool found = false;
    for (size_t i = 0; i < KEYWORD_COUNT; i++)
    {
        if (string_is_same(ident, keyword_table[i].keyword))
        {
            found = true;
            *token_id = keyword_table[i].token_id;
            *keyname = (keyword_table[i].keyname != NULL)
                ? keyword_table[i].keyname
                : keyword_table[i].keyword;
            break;
        }
    }

    return found;
}

static double get_number(const char* literal)
{
    double sign = 1.0;
    const char* body = literal;

    if (body[0] == '~')
    {
        sign = -1.0;
        body++;
    }

    return sign * atof(body);
}

static void append_string(const char* string, size_t length)
{
    string_buffer = (char*)realloc(string_buffer, sizeof(char) * (string_length + length + 1u));
    strcpy(string_buffer + string_length, string);
    string_length += length;
}

static void include_file(const char* filename)
{
    context_t* current_context = (context_t*)malloc(sizeof(context_t));
    current_context->buffer = YY_CURRENT_BUFFER;
    current_context->filename = current_filename;
    current_context->position = current_position;
    push_context(current_context);

    set_input(filename);
    yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
}

static bool recover_file(void)
{
    context_t* prev_context = pop_context();
    if (prev_context == NULL)
    {
        return false;
    }

    yy_delete_buffer(YY_CURRENT_BUFFER);

    yy_switch_to_buffer(prev_context->buffer);
    current_filename = prev_context->filename;
    current_position = prev_context->position;

    free(prev_context);
    return true;
}

static void push_context(context_t* context)
{
    context->prev = context_stack;
    context_stack = context;
}

static context_t* pop_context(void)
{
    context_t* context = context_stack;
    if (context != NULL)
    {
        context_stack = context->prev;
    }
    return context;
}
