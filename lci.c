#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>

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

typedef struct
{
    char *name;
    expr_t *expr;
} binding_t;

expr_t *expr_new(enum expr_tag tag)
{
    expr_t *expr = malloc(sizeof(expr_t));
    if (expr == NULL)
        return NULL;
    memset(expr, 0, sizeof(expr_t));
    expr->tag = tag;
    return expr;
}

void expr_print(expr_t *expr)
{
    if (expr == NULL)
    {
        fputs("expr_print: expr is NULL\n", stderr);
        abort();
    }
    switch (expr->tag)
    {
        case EXPR_VAR:
            printf("%s", expr->var.name);
            break;
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
    }
}

void expr_println(expr_t *expr)
{
    expr_print(expr);
    fputc('\n', stdout);
}

void skip_spaces(char **s)
{
    while (isspace(**s))
        (*s)++;
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

expr_t *parse_expr(char *s, char **end);

expr_t *parse_list(char *s, char **end)
{
    expr_t *expr = expr_new(EXPR_LIST);
    expr->list.len = 0;
    expr->list.exprs = NULL;
    while (*s != ')' && *s != '\0')
    {
        expr->list.len++;
        expr->list.exprs = realloc(expr->list.exprs, sizeof(expr_t) * expr->list.len);
        expr_t *sub_expr = parse_expr(s, &s);
        if (sub_expr == NULL)
            abort();
        expr->list.exprs[expr->list.len - 1] = sub_expr;
    }
    *end = s;
    return expr;
}

expr_t *parse_expr(char *s, char **end)
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

expr_t *parse(char *s)
{
    char *end;
    expr_t *expr = parse_list(s, &end);
    skip_spaces(&end);
    if (*end != '\0')
    {
        fprintf(stderr, "extra characters `%s`\n", end);
        abort();
    }
    return expr;
}

binding_t *stack = NULL;
size_t stack_len = 0;
binding_t *globals = NULL;
size_t globals_len = 0;

void push_binding(binding_t **bindings, size_t *bindings_len, char *name, expr_t *expr)
{
    size_t len = *bindings_len;
    len++;
    *bindings = realloc(*bindings, sizeof(binding_t) * len);
    (*bindings)[len - 1].name = name;
    (*bindings)[len - 1].expr = expr;
    *bindings_len = len;
}

expr_t *reduce(expr_t *expr)
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
        case EXPR_FUNC:
            expr->func.body = reduce(expr->func.body);
            return expr;
        case EXPR_LIST:
            if (expr->list.len == 1)
                return expr->list.exprs[0];
            // for (size_t i = 0; i < expr->list.len; i++)
            //     expr->list.exprs[i] = reduce(expr->list.exprs[i]);
            expr_t *called = reduce(expr->list.exprs[0]);
            if (called->tag != EXPR_FUNC)
                return called;
            expr_t *arg = reduce(expr->list.exprs[1]);
            push_binding(
                &stack,
                &stack_len,
                called->func.param_name,
                arg
            );
            expr_t *result = reduce(called->func.body);
            stack_len--;
            expr->list.exprs[0] = result;
            expr->list.len--;
            memmove(&expr->list.exprs[1], &expr->list.exprs[2], expr->list.len);
            return reduce(expr);
    }
    return NULL;
}

void eval_print(char *s)
{
    if (strstr(s, ":=") != NULL)
    {
        skip_spaces(&s);
        char *name = parse_sym(s, &s);
        if (name != NULL)
            abort();
        skip_spaces(&s);
        if (strncmp(s, ":=", 2) != 0)
            abort();
        expr_t *expr = reduce(parse(s));
        push_binding(&globals, &globals_len, name, expr);
        return;
    }
    expr_println(reduce(parse(s)));
}

/*
 * func ::= \<name>. <expr>
 * expr ::= (<func> | <name>) <expr>* | '(' <expr> ')'
 * stmt ::= <name> := <func>
 * name ::= any visible character except '(' ')' '.' and '\'
 */

int main(int argc, char **argv)
{
    if (argc == 3 && strcmp(argv[1], "-c") == 0)
    {
        eval_print(argv[2]);
        return 0;
    }

    char *line = NULL;
    while (true)
    {
        free(line);
        line = readline("λ ");
        if (line == NULL || strcmp(line, ":q") == 0)
            break;
        add_history(line);
        eval_print(line);
        fputc('\n', stdout);
    }
    free(line);
    return 0;
}
