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
    if (expr == NULL)
        return;
    switch (expr->tag)
    {
    case EXPR_FUNC:
        free(expr->func.param_name);
        expr_destroy(expr->func.body);
        break;
    case EXPR_VAR: free(expr->var.name); break;
    case EXPR_LIST:
        for (size_t i = 0; i < expr->list.len; i++)
            expr_destroy(expr->list.exprs[i]);
        free(expr->list.exprs);
        break;
    case EXPR_STMT:
        free(expr->stmt.name);
        expr_destroy(expr->stmt.expr);
        break;
    case EXPR_PARSE_ERROR: break;
    }
    free(expr);
}

expr_t *
expr_clone(const expr_t *expr)
{
    expr_t *clone = malloc(sizeof(expr_t));
    if (clone == NULL)
        return NULL;
    memcpy(clone, expr, sizeof(expr_t));
    switch (clone->tag)
    {
    case EXPR_FUNC:
        clone->func.param_name = strdup(expr->func.param_name);
        clone->func.body = expr_clone(expr->func.body);
        break;
    case EXPR_VAR:
        clone->var.name = strdup(expr->var.name);
        break;
    case EXPR_LIST:
        clone->list.exprs = malloc(sizeof(expr_t*) * expr->list.len);
        for (size_t i = 0; i < expr->list.len; i++)
            clone->list.exprs[i] = expr_clone(expr->list.exprs[i]);
        break;
    case EXPR_STMT:
        clone->stmt.name = strdup(expr->stmt.name);
        clone->stmt.expr = expr_clone(expr->stmt.expr);
        break;
    case EXPR_PARSE_ERROR:
        fputs("Cannot clone parse error expr\n", stderr);
        abort();
    }
    return clone;
}

bool
expr_eq(const expr_t *a, const expr_t *b)
{
    if (a->tag != b->tag)
        return false;
    switch (a->tag)
    {
    case EXPR_FUNC:
        return strcmp(a->func.param_name, b->func.param_name) == 0 &&
               expr_eq(a->func.body, b->func.body);
    case EXPR_VAR: return strcmp(a->var.name, b->var.name) == 0;
    case EXPR_LIST:
        if (a->list.len != b->list.len)
            return false;
        for (size_t i = 0; i < a->list.len; i++)
            if (!expr_eq(a->list.exprs[i], b->list.exprs[i]))
                return false;
        return true;
    case EXPR_STMT:
        return strcmp(a->stmt.name, b->stmt.name) == 0 &&
               expr_eq(a->stmt.expr, b->stmt.expr);
    case EXPR_PARSE_ERROR:
        return a->error.kind == b->error.kind &&
               a->error.location == b->error.location;
    }
    return false;
}

void
expr_print(const expr_t *expr)
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
        break;
    case EXPR_STMT:
        printf("%s := ", expr->stmt.name);
        expr_print(expr->stmt.expr);
        break;
    case EXPR_PARSE_ERROR:
        fputs("Cannot print error expr", stderr);
        abort();
        break;
    }
}

void
expr_println(const expr_t *expr)
{
    expr_print(expr);
    fputc('\n', stdout);
}

static void
expr_print_tree_rec(const expr_t *expr, size_t indent)
{
    for (size_t i = 0; i < indent; i++)
        fputc(' ', stdout);
    switch (expr->tag)
    {
    case EXPR_VAR: printf("[var %s]\n", expr->var.name); break;
    case EXPR_FUNC:
        printf("[func %s]\n", expr->func.param_name);
        expr_print_tree_rec(expr->func.body, indent + 2);
        break;
    case EXPR_LIST:
        printf("[list (len=%zu)]\n", expr->list.len);
        for (size_t i = 0; i < expr->list.len; i++)
            expr_print_tree_rec(expr->list.exprs[i], indent + 2);
        break;
    case EXPR_STMT:
        printf("[stmt %s]\n", expr->stmt.name);
        expr_print_tree_rec(expr->stmt.expr, indent + 2);
        break;
    case EXPR_PARSE_ERROR:
        printf("[parse error (kind=%d) (location=%p=\"%s\")]\n",
               expr->error.kind,
               expr->error.location,
               expr->error.location);
        break;
    }
}

void
expr_print_tree(const expr_t *expr)
{
    expr_print_tree_rec(expr, 0);
}
