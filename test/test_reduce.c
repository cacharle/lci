#include "parse.h"
#include "reduce.h"
#include <criterion/criterion.h>
#include <signal.h>

Test(reduce, var_no_bindings)
{
    expr_t *expr = reduce(parse("foo"));
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_VAR);
    cr_assert_str_eq(expr->var.name, "foo");
    expr_destroy(expr);
}

Test(reduce, func_no_bindings)
{
    expr_t *expr = reduce(parse("\\foo. bar"));
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_FUNC);
    cr_assert_str_eq(expr->func.param_name, "foo");
    cr_assert_eq(expr->func.body->tag, EXPR_VAR);
    cr_assert_str_eq(expr->func.body->var.name, "bar");
    expr_destroy(expr);
}

Test(reduce, list_no_bindings)
{
    expr_t *expr = reduce(parse("f x"));
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_LIST);
    cr_assert_eq(expr->list.len, 2);
    char *var_names[] = {"f", "x"};
    for (size_t i = 0; i < 2; i++)
    {
        cr_assert_not_null(expr->list.exprs[i]);
        cr_assert_eq(expr->list.exprs[i]->tag, EXPR_VAR);
        cr_assert_str_eq(expr->list.exprs[i]->var.name, var_names[i]);
    }
    expr_destroy(expr);
}

Test(reduce, func_call)
{
    expr_t *expr = reduce(parse("(\\x. x) foo"));
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_VAR);
    cr_assert_str_eq(expr->var.name, "foo");
    expr_destroy(expr);
}

Test(reduce, func_call_with_func_param)
{
    expr_t *expr = reduce(parse("(\\x. x) (\\x. x)"));
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_FUNC);
    cr_assert_str_eq(expr->func.param_name, "x");
    cr_assert_eq(expr->func.body->tag, EXPR_VAR);
    cr_assert_str_eq(expr->func.body->var.name, "x");
    expr_destroy(expr);
}

Test(reduce, func_call_return_func)
{
    expr_t *expr = reduce(parse("(\\x. \\y. y) foo"));
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_FUNC);
    cr_assert_str_eq(expr->func.param_name, "y");
    cr_assert_eq(expr->func.body->tag, EXPR_VAR);
    cr_assert_str_eq(expr->func.body->var.name, "y");
    expr_destroy(expr);
}

Test(reduce, func_call_return_func_but_body_has_parent_param)
{
    expr_t *expr = reduce(parse("(\\x. \\y. x) foo"));
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_FUNC);
    cr_assert_str_eq(expr->func.param_name, "y");
    cr_assert_eq(expr->func.body->tag, EXPR_VAR);
    cr_assert_str_eq(expr->func.body->var.name, "foo");
    expr_destroy(expr);
}

Test(reduce, stmt)
{
    expr_t *expr = reduce(parse("foo := bar"));
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_STMT);
    cr_assert_str_eq(expr->stmt.name, "foo");
    cr_assert_eq(expr->stmt.expr->tag, EXPR_VAR, "actual: %d\n", expr->tag);
    cr_assert_str_eq(expr->stmt.expr->var.name, "bar");
    expr_t *expr2 = reduce(parse("foo"));
    cr_assert_eq(expr2->tag, EXPR_VAR);
    cr_assert_str_eq(expr2->var.name, "bar");
    expr_destroy(expr);
}

Test(reduce, stmt_self_reference)
{
    expr_t *expr = reduce(parse("foo := foo"));
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_STMT);
    cr_assert_str_eq(expr->stmt.name, "foo");
    cr_assert_eq(expr->stmt.expr->tag, EXPR_VAR, "actual: %d\n", expr->tag);
    cr_assert_str_eq(expr->stmt.expr->var.name, "foo");
    // reduce(parse("foo"));  // would stack overflow
}

Test(reduce, stmt_true_false)
{
    reduce(parse("TRUE  := \\x. \\y. x"));
    reduce(parse("FALSE := \\x. \\y. y"));
    expr_t *expr = reduce(parse("TRUE FALSE TRUE"));
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_FUNC);
    cr_assert_str_eq(expr->func.param_name, "x");
    cr_assert_eq(expr->func.body->tag, EXPR_FUNC);
    cr_assert_str_eq(expr->func.body->func.param_name, "y");
    cr_assert_eq(expr->func.body->func.body->tag, EXPR_VAR);
    cr_assert_str_eq(expr->func.body->func.body->var.name, "y");
}

Test(reduce, stmt_true_false_reversed)
{
    cr_skip();
    reduce(parse("TRUE  := \\x. \\y. x"));
    reduce(parse("FALSE := \\x. \\y. y"));
    expr_t *expr = reduce(parse("TRUE TRUE FALSE"));
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_FUNC);
    cr_assert_str_eq(expr->func.param_name, "x");
    cr_assert_eq(expr->func.body->tag, EXPR_FUNC);
    cr_assert_str_eq(expr->func.body->func.param_name, "y");
    cr_assert_eq(expr->func.body->func.body->tag, EXPR_VAR);
    cr_assert_str_eq(expr->func.body->func.body->var.name, "x");
}

Test(reduce, stmt_circular_reduce)
{
    cr_skip();
    reduce(parse("TRUE  := \\x. \\y. x"));
    expr_t *expr = reduce(parse("TRUE TRUE"));
}
