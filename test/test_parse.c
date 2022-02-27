#include "parse.h"
#include <criterion/criterion.h>

Test(parse, var_basic)
{
    expr_t *expr = parse("foo");
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_VAR);
    cr_assert_str_eq(expr->var.name, "foo");
    expr_destroy(expr);
}

Test(parse, func_basic)
{
    expr_t *expr = parse("\\foo. bar");
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_FUNC);
    cr_assert_str_eq(expr->func.param_name, "foo");
    cr_assert_not_null(expr->func.body);
    cr_assert_eq(expr->func.body->tag, EXPR_VAR);
    cr_assert_str_eq(expr->func.body->var.name, "bar");
    expr_destroy(expr);
}

Test(parse, list_basic)
{
    expr_t *expr = parse("foo bar baz");
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_LIST);
    cr_assert_eq(expr->list.len, 3);
    char *var_names[] = {"foo", "bar", "baz"};
    for (size_t i = 0; i < 3; i++)
    {
        cr_assert_not_null(expr->list.exprs[i]);
        cr_assert_eq(expr->list.exprs[i]->tag, EXPR_VAR);
        cr_assert_str_eq(expr->list.exprs[i]->var.name, var_names[i]);
    }
    expr_destroy(expr);
}

Test(parse, stmt_basic)
{
    expr_t *expr = parse("foo := bar");
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_STMT);
    cr_assert_str_eq(expr->stmt.name, "foo");
    cr_assert_eq(expr->stmt.expr->tag, EXPR_VAR);
    cr_assert_str_eq(expr->stmt.expr->var.name, "bar");
}

Test(parse, stmt_func)
{
    expr_t *expr = parse("foo := \\bar. baz");
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_STMT);
    cr_assert_str_eq(expr->stmt.name, "foo");
    cr_assert_eq(expr->stmt.expr->tag, EXPR_FUNC);
    cr_assert_str_eq(expr->stmt.expr->func.param_name, "bar");
    cr_assert_eq(expr->stmt.expr->func.body->tag, EXPR_VAR);
    cr_assert_str_eq(expr->stmt.expr->func.body->var.name, "baz");
}

Test(parse, nested_func)
{
    expr_t *expr = parse("\\foo. \\bar. \\baz. foo bar baz");
    cr_assert_not_null(expr);
    cr_assert_eq(
        expr->tag, EXPR_FUNC, "expected: %d, actual: %d", EXPR_FUNC, expr->tag);
    cr_assert_str_eq(expr->func.param_name, "foo");
    cr_assert_eq(expr->func.body->tag, EXPR_FUNC);
    cr_assert_str_eq(expr->func.body->func.param_name, "bar");
    cr_assert_eq(expr->func.body->func.body->tag, EXPR_FUNC);
    cr_assert_str_eq(expr->func.body->func.body->func.param_name, "baz");
    cr_assert_eq(expr->func.body->func.body->func.body->tag, EXPR_LIST);
    cr_assert_eq(expr->func.body->func.body->func.body->list.len, 3);
    char *var_names[] = {"foo", "bar", "baz"};
    for (size_t i = 0; i < 3; i++)
    {
        expr_t *e = expr->func.body->func.body->func.body->list.exprs[i];
        cr_assert_not_null(e);
        cr_assert_eq(e->tag, EXPR_VAR);
        cr_assert_str_eq(e->var.name, var_names[i]);
    }
    expr_destroy(expr);
}

Test(parse, parenthesis)
{
    expr_t *expr = parse("(foo bar baz)");
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_LIST);
    cr_assert_eq(expr->list.len, 3);
    char *var_names[] = {"foo", "bar", "baz"};
    for (size_t i = 0; i < 3; i++)
    {
        cr_assert_not_null(expr->list.exprs[i]);
        cr_assert_eq(expr->list.exprs[i]->tag, EXPR_VAR);
        cr_assert_str_eq(expr->list.exprs[i]->var.name, var_names[i]);
    }
    expr_destroy(expr);
}

Test(parse, parenthesis_nested)
{
    expr_t *expr = parse("(foo ((bar baz) qux))");
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_LIST);
    cr_assert_eq(expr->list.len, 2);
    cr_assert_eq(expr->list.exprs[0]->tag, EXPR_VAR);
    cr_assert_str_eq(expr->list.exprs[0]->var.name, "foo");
    cr_assert_eq(expr->list.exprs[1]->tag, EXPR_LIST);
    cr_assert_eq(expr->list.exprs[1]->list.len, 2);
    cr_assert_eq(expr->list.exprs[1]->list.exprs[0]->tag, EXPR_LIST);
    cr_assert_eq(expr->list.exprs[1]->list.exprs[0]->list.len, 2);
    cr_assert_eq(expr->list.exprs[1]->list.exprs[0]->list.exprs[0]->tag, EXPR_VAR);
    cr_assert_str_eq(expr->list.exprs[1]->list.exprs[0]->list.exprs[0]->var.name,
                     "bar");
    cr_assert_eq(expr->list.exprs[1]->list.exprs[0]->list.exprs[1]->tag, EXPR_VAR);
    cr_assert_str_eq(expr->list.exprs[1]->list.exprs[0]->list.exprs[1]->var.name,
                     "baz");
    cr_assert_eq(expr->list.exprs[1]->list.exprs[1]->tag, EXPR_VAR);
    cr_assert_str_eq(expr->list.exprs[1]->list.exprs[1]->var.name, "qux");
    expr_destroy(expr);
}

Test(parse, error_function_no_body)
{
    char   *s = "\\foo. ";
    expr_t *expr = parse(s);
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_PARSE_ERROR);
    cr_assert_eq(expr->error.kind, PARSE_ERR_UNEXPECTED_END);
    cr_assert_str_eq(expr->error.location, s + strlen(s));
    expr_destroy(expr);
}

Test(parse, error_parenthesis_not_closed)
{
    char   *s = "(foo ((bar baz) qux)";
    expr_t *expr = parse(s);
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_PARSE_ERROR);
    cr_assert_eq(expr->error.kind, PARSE_ERR_MISSING_CLOSING_PARENTHESIS);
    cr_assert_str_eq(expr->error.location, s + strlen(s));
    expr_destroy(expr);
}

Test(parse, error_parenthesis_not_opened)
{
    char   *s = "foo bar)";
    expr_t *expr = parse(s);
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_PARSE_ERROR);
    cr_assert_eq(expr->error.kind,
                 PARSE_ERR_MISSING_OPENING_PARENTHESIS,
                 "actual: %d\n",
                 expr->error.kind);
    cr_assert_str_eq(expr->error.location, s + strlen(s) - 1);
    expr_destroy(expr);
}

Test(parse, error_func_not_dot)
{
    char   *s = "\\foo bar";
    expr_t *expr = parse(s);
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_PARSE_ERROR);
    cr_assert_eq(expr->error.kind, PARSE_ERR_MISSING_DOT_SEPARATOR);
    cr_assert_str_eq(expr->error.location, s + strlen("\\foo "));
    expr_destroy(expr);
}

Test(parse, spaces_trimed)
{
    expr_t *expr = parse(" \t \t  foo   \t\t \t ");
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_VAR);
    cr_assert_str_eq(expr->var.name, "foo");
    expr_destroy(expr);
}

// Test(parse, empty)
// {
//     expr_t *expr = parse(" \t \t  \t\t \t ");
//     expr_print_tree(expr);
//     cr_assert_not_null(expr);
//     cr_assert_eq(expr->tag, EXPR_LIST, "actual: %d", expr->tag);
//     cr_assert_eq(expr->list.len, 0);
// }

Test(parse, function_space_before_dot_not_error)
{
    expr_t *expr = parse("\\x .x");
    cr_assert_not_null(expr);
    cr_assert_neq(expr->tag, EXPR_PARSE_ERROR);
}

Test(parse, stmt_no_space_after_op)
{
    expr_t *expr = parse("foo :=bar");
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_STMT);
    cr_assert_str_eq(expr->stmt.name, "foo");
    cr_assert_eq(expr->stmt.expr->tag, EXPR_VAR);
    cr_assert_str_eq(expr->stmt.expr->var.name, "bar");
}

Test(parse, stmt_no_space_before_op)
{
    expr_t *expr = parse("foo:= bar");
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_STMT);
    cr_assert_str_eq(expr->stmt.name, "foo");
    cr_assert_eq(expr->stmt.expr->tag, EXPR_VAR);
    cr_assert_str_eq(expr->stmt.expr->var.name, "bar");
}

Test(parse, no_space_before_lambda)
{
    expr_t *expr = parse("baz\\foo.bar");
    cr_assert_not_null(expr);
    cr_assert_eq(expr->tag, EXPR_LIST);
    cr_assert_eq(expr->list.exprs[0]->tag, EXPR_VAR);
    cr_assert_str_eq(expr->list.exprs[0]->var.name, "baz");
    cr_assert_eq(expr->list.exprs[1]->tag, EXPR_FUNC);
    cr_assert_str_eq(expr->list.exprs[1]->func.param_name, "foo");
    cr_assert_not_null(expr->list.exprs[1]->func.body);
    cr_assert_eq(expr->list.exprs[1]->func.body->tag, EXPR_VAR);
    cr_assert_str_eq(expr->list.exprs[1]->func.body->var.name, "bar");
    expr_destroy(expr);
}
