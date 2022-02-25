#ifndef LCI_PARSE_H
#define LCI_PARSE_H

#define _POSIX_C_SOURCE 200809L
#include "expr.h"
#include <ctype.h>
#include <string.h>

enum parse_result_tag
{
    PARSE_ERROR,
    PARSE_STMT,
    PARSE_EXPR,
};

enum parse_error
{
    PARSE_ERR_MEMORY,
    PARSE_ERR_EXTRA_CHARACTER,
};

typedef struct
{
    enum parse_result_tag tag;
    union
    {
        struct
        {
            enum parse_error kind;
            char            *location;
        } error;
        struct
        {
            char   *name;
            expr_t *expr;
        } stmt;
        expr_t *expr;
    };
} parse_result_t;

void
parse(parse_result_t *result, char *s);

#endif
