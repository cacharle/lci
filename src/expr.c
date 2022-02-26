#include "expr.h"

expr_t *
expr_new(enum expr_tag tag)
{
    expr_t *expr = malloc(sizeof(expr_t));
    if (expr == NULL)
        return NULL;
    memset(expr, 0, sizeof(expr_t));
    expr->tag = tag;
    return expr;
}

void
expr_destroy(expr_t *expr)
{
    switch (expr->tag)
    {
        case EXPR_FUNC:
            free(expr->func.param_name);
            expr_destroy(expr->func.body);
            break;
        case EXPR_VAR:
            free(expr->var.name);
            break;
        case EXPR_LIST:
            for (size_t i = 0; i < expr->list.len; i++)
                expr_destroy(expr->list.exprs[i]);
            free(expr->list.exprs);
            break;
        case EXPR_STMT:
            free(expr->stmt.name);
            expr_destroy(expr->stmt.expr);
            break;
        case EXPR_PARSE_ERROR:
            break;
    }
    free(expr);
}

void
expr_print(expr_t *expr)
{
    if (expr == NULL)
    {
        fputs("expr_print: expr is NULL\n", stderr);
        abort();
    }
    switch (expr->tag)
    {
    case EXPR_VAR: printf("%s", expr->var.name); break;
    case EXPR_FUNC:
        printf("\\%s. ", expr->func.param_name);
        expr_print(expr->func.body);
        break;
    case EXPR_LIST:
        for (size_t i = 0; i < expr->list.len; i++)
        {
            expr_t *e = expr->list.exprs[i];
            if (e->tag == EXPR_LIST)
                fputc('(', stdout);
            expr_print(e);
            if (e->tag == EXPR_LIST)
                fputc(')', stdout);
            if (i != expr->list.len - 1)
                fputc(' ', stdout);
        }
    case EXPR_STMT:
        printf("%s := ", expr->stmt.name);
        expr_print(expr->stmt.expr);
    case EXPR_PARSE_ERROR:
        fputs("Cannot print error expr", stderr);
        abort();
        break;
    }
}

void
expr_println(expr_t *expr)
{
    expr_print(expr);
    fputc('\n', stdout);
}
