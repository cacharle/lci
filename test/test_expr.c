#include <criterion/criterion.h>
#include "expr.h"

Test(expr, new_func)
{
    expr_t *expr = expr_new(EXPR_FUNC);
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_FUNC);
    cr_assert_null(expr->func.param_name);
    cr_assert_null(expr->func.body);
}

Test(expr, new_var)
{
    expr_t *expr = expr_new(EXPR_VAR);
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_VAR);
    cr_assert_null(expr->var.name);
}

Test(expr, new_list)
{
    expr_t *expr = expr_new(EXPR_LIST);
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_LIST);
    cr_assert_eq(expr->list.len, 0);
    cr_assert_null(expr->list.exprs);
}
