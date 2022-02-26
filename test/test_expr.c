#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <criterion/criterion.h>
#include "expr.h"

Test(expr, new_func)
{
    expr_t *expr = expr_new(EXPR_FUNC);
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_FUNC);
    cr_assert_null(expr->func.param_name);
    cr_assert_null(expr->func.body);
    free(expr);
}

Test(expr, new_var)
{
    expr_t *expr = expr_new(EXPR_VAR);
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_VAR);
    cr_assert_null(expr->var.name);
    free(expr);
}

Test(expr, new_list)
{
    expr_t *expr = expr_new(EXPR_LIST);
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_LIST);
    cr_assert_eq(expr->list.len, 0);
    cr_assert_null(expr->list.exprs);
    free(expr);
}

Test(expr, new_stmt)
{
    expr_t *expr = expr_new(EXPR_STMT);
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_STMT);
    cr_assert_null(expr->stmt.name);
    cr_assert_null(expr->stmt.expr);
    free(expr);
}

Test(expr, destroy_func)
{
    expr_t *expr = expr_new(EXPR_FUNC);
    expr->func.param_name = strdup("foo");
    expr->func.body = expr_new(EXPR_VAR);
    expr->func.body->var.name = strdup("bar");
    expr_destroy(expr);
}

Test(expr, destroy_var)
{
    expr_t *expr = expr_new(EXPR_VAR);
    expr->var.name = strdup("bar");
    expr_destroy(expr);
}

Test(expr, destroy_list)
{
    expr_t *expr = expr_new(EXPR_LIST);
    expr->list.len = 2;
    expr->list.exprs = malloc(sizeof(expr_t*) * expr->list.len);
    expr->list.exprs[0] = expr_new(EXPR_VAR);
    expr->list.exprs[0]->var.name = strdup("foo");
    expr->list.exprs[1] = expr_new(EXPR_VAR);
    expr->list.exprs[1]->var.name = strdup("bar");
    expr_destroy(expr);
}

Test(expr, destroy_stmt)
{
    expr_t *expr = expr_new(EXPR_STMT);
    expr->stmt.name = strdup("foo");
    expr->stmt.expr = expr_new(EXPR_VAR);
    expr->stmt.expr->var.name = strdup("foo");
    expr_destroy(expr);
}

Test(expr, destroy_parse_error)
{
    expr_t *expr = expr_new(EXPR_PARSE_ERROR);
    expr->error.location = "don't free me";
    expr->error.kind = PARSE_ERR_EXTRA_CHARACTER;
    expr_destroy(expr);
}
