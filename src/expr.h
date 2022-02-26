#ifndef LCI_EXPR_H
#define LCI_EXPR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum parse_error
{
    PARSE_ERR_EXTRA_CHARACTER,
    PARSE_ERR_UNEXPECTED_END,
    PARSE_ERR_MISSING_CLOSING_PARENTHESIS,
    PARSE_ERR_MISSING_OPENING_PARENTHESIS,
    PARSE_ERR_MISSING_DOT_SEPARATOR,
};

enum expr_tag
{
    EXPR_FUNC,
    EXPR_VAR,
    EXPR_LIST,
    EXPR_STMT,
    EXPR_PARSE_ERROR,
};

typedef struct expr
{
    enum expr_tag tag;
    union
    {
        struct
        {
            char        *param_name;
            struct expr *body;
        } func;
        struct
        {
            char *name;
        } var;
        struct
        {
            size_t        len;
            struct expr **exprs;
        } list;
        struct
        {
            char        *name;
            struct expr *expr;
        } stmt;
        struct
        {
            enum parse_error kind;
            char            *location;
        } error;
    };
} expr_t;

expr_t *
expr_new(enum expr_tag tag);
void
expr_destroy(expr_t *expr);
void
expr_print(expr_t *expr);
void
expr_println(expr_t *expr);
void
expr_print_tree(expr_t *expr);

#endif
