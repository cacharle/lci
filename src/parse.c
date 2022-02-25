#include "parse.h"

void
skip_spaces(char **s)
{
    while (isspace(**s))
        (*s)++;
}

char *
parse_sym(char *s, char **end)
{
    char *curr = s;
    while (*curr != '\0' && *curr != '.' && *curr != '(' && *curr != ')' &&
           !isspace(*curr))
        curr++;
    char *ret = strndup(s, curr - s);
    *end = curr;
    return ret;
}

expr_t *
parse_expr(char *s, char **end);

expr_t *
parse_list(char *s, char **end)
{
    expr_t *expr = expr_new(EXPR_LIST);
    expr->list.len = 0;
    expr->list.exprs = NULL;
    while (*s != ')' && *s != '\0')
    {
        expr->list.len++;
        expr->list.exprs =
            realloc(expr->list.exprs, sizeof(expr_t) * expr->list.len);
        expr_t *sub_expr = parse_expr(s, &s);
        if (sub_expr == NULL)
            abort();
        expr->list.exprs[expr->list.len - 1] = sub_expr;
    }
    *end = s;
    return expr;
}

expr_t *
parse_expr(char *s, char **end)
{
    skip_spaces(&s);
    if (*s == '\\')
    {
        expr_t *expr = expr_new(EXPR_FUNC);
        s++;
        expr->func.param_name = parse_sym(s, &s);
        if (*s != '.')
            abort();
        s++;
        expr->func.body = parse_expr(s, end);
        return expr;
    }
    else if (*s == '(')
    {
        s++;
        expr_t *expr = parse_list(s, &s);
        if (*s != ')')
            abort();
        s++;
        *end = s;
        return expr;
    }
    else
    {
        expr_t *expr = expr_new(EXPR_VAR);
        expr->var.name = parse_sym(s, end);
        if (expr->var.name == NULL)
            abort();
        return expr;
    }
}

expr_t *
parse(char *s)
{
    char   *end;
    expr_t *expr = parse_list(s, &end);
    skip_spaces(&end);
    if (*end != '\0')
    {
        fprintf(stderr, "extra characters `%s`\n", end);
        abort();
    }
    return expr;
}
