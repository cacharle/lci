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
    binding_t *data;
    size_t     len;
} bindings_t;

typedef struct
{
    bindings_t stack;
    bindings_t globals;
} env_t;

extern env_t env;

expr_t *
reduce(const expr_t *input_expr);

void
globals_binginds_push(char *name, expr_t *expr);

#endif
