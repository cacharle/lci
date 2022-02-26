#include "parse.h"
#include "reduce.h"
#include <ctype.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// void
// eval_print(char *s)
// {
//     parse_result_t result;
//     parse(&result, s);
//     if (result->tag == PARSE_STMT)
//     {
//         result->stmt.expr = reduce(result->stmt.expr);
//         push_binding(&globals, &globals_len, parsed->stmt.name,
//         parsed->stmt->expr); return;
//         // skip_spaces(&s);
//         // char *name = parse_sym(s, &s);
//         // if (name != NULL)
//         //     abort();
//         // skip_spaces(&s);
//         // if (strncmp(s, ":=", 2) != 0)
//         //     abort();
//     }
//     expr_println(reduce(parse(s)));
// }

/*
 * func ::= \<name>. <expr>
 * expr ::= (<func> | <name>) <expr>* | '(' <expr> ')'
 * stmt ::= <name> := <func>
 * name ::= any visible character except '(' ')' '.' and '\'
 */

int
main(int argc, char **argv)
{
    expr_t *expr = parse( "(foo ((bar baz) qux)");
    expr_print_tree(expr);

    return 0;

    if (argc == 3 && strcmp(argv[1], "-c") == 0)
    {
        // eval_print(argv[2]);
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
        // eval_print(line);
        fputc('\n', stdout);
    }
    free(line);
    return 0;
}
