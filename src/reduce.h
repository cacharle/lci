#ifndef LCI_REDUCE_H
#define LCI_REDUCE_H

#define _POSIX_C_SOURCE 200809L
#include "expr.h"
#include <ctype.h>
#include <string.h>

typedef struct
{
    char   *name;
    expr_t *expr;
} binding_t;

typedef struct
{
    binding_t *exprs;
    size_t     len;
} bindings_t;

typedef struct
{
    bindings_t stack;
    bindings_t globals;
} env_t;

expr_t *
reduce(expr_t *expr);

void globals_binginds_push(char *name, expr_t *expr);

#endif
