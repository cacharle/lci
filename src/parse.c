#include "parse.h"

static expr_t *
error(enum parse_error kind, char *location)
{
    expr_t *expr = expr_new(EXPR_PARSE_ERROR);
    if (expr == NULL)
        return NULL;
    expr->error.kind = kind;
    expr->error.location = location;
    return expr;
}

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
           strncmp(curr, ":=", 2) != 0 && !isspace(*curr))
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
        skip_spaces(&s);
        if (sub_expr == NULL)
            return NULL;
        if (sub_expr->tag == EXPR_PARSE_ERROR)
            return sub_expr;
        expr->list.exprs[expr->list.len - 1] = sub_expr;
    }
    *end = s;
    if (expr->list.len == 1)
    {
        expr_t *ret = expr->list.exprs[0];
        expr->list.len--;
        expr_destroy(expr);
        return ret;
    }
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
        skip_spaces(&s);
        if (*s != '.')
            return error(PARSE_ERR_MISSING_DOT_SEPARATOR, s);
        s++;
        expr->func.body = parse_list(s, end);
        if (expr->func.body == NULL)
            return NULL;
        if (expr->func.body->tag == EXPR_PARSE_ERROR)
            return expr->func.body;
        return expr;
    }
    else if (*s == '(')
    {
        s++;
        expr_t *expr = parse_list(s, &s);
        if (*s != ')')
            return error(PARSE_ERR_MISSING_CLOSING_PARENTHESIS, s);
        s++;
        *end = s;
        return expr;
    }
    else
    {
        expr_t *expr = expr_new(EXPR_VAR);
        expr->var.name = parse_sym(s, end);
        if (expr->var.name == NULL)
            return NULL;
        if (strlen(expr->var.name) == 0)
            return error(PARSE_ERR_UNEXPECTED_END, s);
        return expr;
    }
}

expr_t *
parse_stmt_or_list(char *s, char **end)
{
    char   *origin = s;
    expr_t *expr = expr_new(EXPR_STMT);
    skip_spaces(&s);
    expr->stmt.name = parse_sym(s, &s);
    if (expr->stmt.name == NULL)
        return NULL;
    skip_spaces(&s);
    if (strncmp(s, ":=", 2) == 0)
    {
        expr->stmt.expr = parse_list(s + 2, end);
        return expr;
    }
    expr_destroy(expr);
    return parse_list(origin, end);
}

expr_t *
parse(char *s)
{
    char   *end;
    expr_t *expr = parse_stmt_or_list(s, &end);
    if (expr->tag == EXPR_PARSE_ERROR)
        return expr;
    skip_spaces(&end);
    if (*end != '\0')
    {
        if (*end == ')')
            return error(PARSE_ERR_MISSING_OPENING_PARENTHESIS, end);
        return error(PARSE_ERR_EXTRA_CHARACTER, end);
    }
    return expr;
}

static const char *parse_error_lookup[] = {
    [PARSE_ERR_EXTRA_CHARACTER] = "Extra characters",
    [PARSE_ERR_UNEXPECTED_END] = "Unexpected end",
    [PARSE_ERR_MISSING_CLOSING_PARENTHESIS] = "Missing closing parenthesis",
    [PARSE_ERR_MISSING_OPENING_PARENTHESIS] = "Missing closing parenthesis",
    [PARSE_ERR_MISSING_DOT_SEPARATOR] = "Missing function '.' separator",
};

void
parse_error_print(enum parse_error kind, char *location, const char *origin)
{
    fprintf(stderr, "Error: %s\n", parse_error_lookup[kind]);
    fprintf(stderr, "%s\n", origin);
    if (location != NULL && origin != NULL)
    {
        for (; origin != location; origin++)
            fputc(' ', stderr);
        fputs("\e[1m\e[31m", stderr);  // bold+red
        fputc('^', stderr);
        fputs("\e[0m", stderr);
        fputc('\n', stderr);
    }
}
