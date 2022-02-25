#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
            char *param_name;
            struct expr *body;
        } func;
        struct
        {
            char *name;
        } var;
        struct
        {
            size_t len;
            struct expr **exprs;
        } list;
    };
} expr_t;

expr_t *expr_new(enum expr_tag tag)
{
    expr_t *expr = malloc(sizeof(expr_t));
    if (expr == NULL)
        return NULL;
    memset(expr, 0, sizeof(expr_t));
    expr->tag = tag;
    return expr;
}

char *parse_sym(char *s, char **end)
{
    char *curr = s;
    while (*curr != '\0' && *curr != '.' && *curr != '(' && *curr != ')' && !isspace(*curr))
        curr++;
    char *ret = strndup(s, curr - s);
    *end = curr;
    return ret;
}

expr_t *parse_expr(char *s, char **end)
{
    while (isspace(*s))
        s++;
    if (*s == '\\')
    {
        expr_t *expr = expr_new(EXPR_FUNC);
        s++;
        expr->func.param_name = parse_sym(s, &s);
        if (*s != '.')
            return NULL;
        s++;
        expr->func.body = parse_expr(s, end);
        return expr;
    }
    else if (*s == '(')
    {
        s++;
        expr_t *expr = expr_new(EXPR_LIST);
        expr->list.len = 0;
        expr->list.exprs = NULL;
        while (*s != ')')
        {
            expr->list.len++;
            expr->list.exprs = realloc(expr->list.exprs, sizeof(expr_t) * expr->list.len);
            expr_t *sub_expr = parse_expr(s, &s);
            if (sub_expr == NULL)
                return NULL;
            expr->list.exprs[expr->list.len - 1] = sub_expr;
        }
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
        return expr;
    }
}

// expr_t *parse_statement(char *s, char **end)
// {
//     expr_t *result = parse_expr(s, end);
//     if (result != NULL)
//         return result;
//     char *sym;
//     parse_sym(&s, &sym);
//     check_has_assignment_op(&s); // :=
//     expr_t *expr = parse_expression(s, end);
//     env_assign(sym, expr);
//     return NULL;
// }

/*
 * func ::= \<name>. <expr>
 * expr ::= (<func> | <name>) <expr>* | '(' <expr> ')'
 * stmt ::= <name> := <func>
 * name ::= any visible character except '(' ')' '.' and '\'
 */

int main()
{
    char *end;
    expr_t *expr = parse_expr("(yo) ye", &end);
    printf("expr=%p\n", expr);
    printf("expr.tag=%d\n", expr->tag);
    if (expr->tag == EXPR_VAR)
        printf("expr.var.name=%s\n", expr->var.name);
    if (expr->tag == EXPR_FUNC)
    {
        printf("expr.func.param_name=%s\n", expr->func.param_name);
        printf("expr.func.body=%p\n", expr->func.body);
    }
    if (expr->tag == EXPR_LIST)
    {
        printf("expr.list.len=%zu\n", expr->list.len);

    }
    printf("end=%s\n", end);
    return 0;
}
