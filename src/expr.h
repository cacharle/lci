#ifndef LCI_EXPR_H
#define LCI_EXPR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum expr_tag
{
    EXPR_FUNC,
    EXPR_VAR,
    EXPR_LIST,
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
    };
} expr_t;

expr_t *
expr_new(enum expr_tag tag);
void
expr_print(expr_t *expr);
void
expr_println(expr_t *expr);

#endif
