#include "parse.h"
#include "reduce.h"
#include <ctype.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
eval_print(char *s)
{
    expr_t *expr = parse(s);
    if (expr->tag == EXPR_PARSE_ERROR)
        parse_error_print(expr->error.kind, expr->error.location, s);
    else
    {
        expr_t *expr = reduce(parse(s));
        expr_println(expr);
        // expr_print_tree(expr);
        bool found_same = false;
        for (size_t i = 0; i < env.globals.len; i++)
            if (expr_eq(expr, env.globals.data[i].expr))
            {
                if (!found_same)
                {
                    fputs("Globals with same value: ", stdout);
                    found_same = true;
                }
                printf("%s ", env.globals.data[i].name);
            }
        if (found_same)
        {
            fputc('\n', stdout);
        }
    }
}

/*
 * func ::= \<name>. <expr>
 * expr ::= (<func> | <name>) <expr>* | '(' <expr> ')'
 * stmt ::= <name> := <func>
 * name ::= any visible character except '(' ')' '.' and '\'
 */

int
main(int argc, char **argv)
{
    // reduce(parse("TRUE := \\x. \\y. x"));
    // // expr_t *expr = reduce(parse("(\\x. \\y. x) (\\x. \\y. x)"));
    // expr_t *expr = reduce(parse("TRUE TRUE"));
    // expr_print(expr);
    // return 0;
    if (argc == 3 && strcmp(argv[1], "-c") == 0)
    {
        eval_print(argv[2]);
        return 0;
    }

    char *line = NULL;
    while (true)
    {
        free(line);
        line = readline("λ> ");
        if (line == NULL || strcmp(line, ":q") == 0)
            break;
        if (strlen(line) == 0)
            continue;
        add_history(line);
        eval_print(line);
        fputc('\n', stdout);
    }
    free(line);
    return 0;
}
