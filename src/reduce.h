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
    binding_t *stack;
    size_t     stack_len;
    binding_t *globals;
    size_t     globals_len;
} env_t;

expr_t *
reduce(expr_t *expr);

#endif
