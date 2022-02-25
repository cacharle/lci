#include "reduce.h"

binding_t *stack = NULL;
size_t     stack_len = 0;
binding_t *globals = NULL;
size_t     globals_len = 0;

void
push_binding(binding_t **bindings, size_t *bindings_len, char *name, expr_t *expr)
{
    size_t len = *bindings_len;
    len++;
    *bindings = realloc(*bindings, sizeof(binding_t) * len);
    (*bindings)[len - 1].name = name;
    (*bindings)[len - 1].expr = expr;
    *bindings_len = len;
}

expr_t *
reduce(expr_t *expr)
{
    switch (expr->tag)
    {
    case EXPR_VAR:
        for (size_t i = stack_len; i > 0; i--)
            if (strcmp(stack[i - 1].name, expr->var.name) == 0)
                return reduce(stack[i - 1].expr);
        for (size_t i = globals_len; i > 0; i--)
            if (strcmp(globals[i - 1].name, expr->var.name) == 0)
                return reduce(globals[i - 1].expr);
        return expr;
    case EXPR_FUNC: expr->func.body = reduce(expr->func.body); return expr;
    case EXPR_LIST:
        if (expr->list.len == 1)
            return expr->list.exprs[0];
        // for (size_t i = 0; i < expr->list.len; i++)
        //     expr->list.exprs[i] = reduce(expr->list.exprs[i]);
        expr_t *called = reduce(expr->list.exprs[0]);
        if (called->tag != EXPR_FUNC)
            return called;
        expr_t *arg = reduce(expr->list.exprs[1]);
        push_binding(&stack, &stack_len, called->func.param_name, arg);
        expr_t *result = reduce(called->func.body);
        stack_len--;
        expr->list.exprs[0] = result;
        expr->list.len--;
        memmove(&expr->list.exprs[1], &expr->list.exprs[2], expr->list.len);
        return reduce(expr);
    }
    return NULL;
}
