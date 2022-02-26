#include "reduce.h"

env_t env = {.stack = {.exprs = NULL, .len = 0},
             .globals = {.exprs = NULL, .len = 0}};

void
bindings_push(bindings_t *bindings, char *name, expr_t *expr)
{
    bindings->len++;
    bindings->exprs = realloc(bindings->exprs, sizeof(binding_t) * bindings->len);
    bindings->exprs[bindings->len - 1].name = name;
    bindings->exprs[bindings->len - 1].expr = expr;
}

void globals_binginds_push(char *name, expr_t *expr)
{
    bindings_push(&env.globals, name, expr);
}

expr_t *
bindings_search(bindings_t *bindings, char *name)
{
    for (size_t i = bindings->len; i > 0; i--)
        if (strcmp(bindings->exprs[i - 1].name, name) == 0)
            return bindings->exprs[i - 1].expr;
    return NULL;
}

expr_t *
reduce(expr_t *expr)
{
    expr_t *found;
    switch (expr->tag)
    {
    case EXPR_VAR:
        found = bindings_search(&env.stack, expr->var.name);
        if (found != NULL)
            return reduce(found);
        found = bindings_search(&env.globals, expr->var.name);
        if (found != NULL)
            return reduce(found);
        return expr;
    case EXPR_FUNC:
        bindings_push(&env.stack, expr->func.param_name, NULL);
        expr->func.body = reduce(expr->func.body);
        env.stack.len--;
        return expr;
    case EXPR_LIST:
        if (expr->list.len == 1)
            return expr->list.exprs[0];
        expr_t *called = reduce(expr->list.exprs[0]);
        if (called->tag != EXPR_FUNC)
            return called;
        expr_t *arg = reduce(expr->list.exprs[1]);
        bindings_push(&env.stack, called->func.param_name, arg);
        expr_t *result = reduce(called->func.body);
        env.stack.len--;
        expr->list.exprs[0] = result;
        expr->list.len--;
        memmove(&expr->list.exprs[1], &expr->list.exprs[2], expr->list.len);
        return reduce(expr);
    case EXPR_STMT:
        expr->stmt.expr = reduce(expr->stmt.expr);
        bindings_push(&env.globals, expr->stmt.name, expr->stmt.expr);
        return expr;
    case EXPR_PARSE_ERROR:
        fputs("Cannot reduce parsing error expr\n", stderr);
        abort();
    }
    return NULL;
}
