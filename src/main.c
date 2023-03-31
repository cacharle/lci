#include "parse.h"
#include "reduce.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void
repl_expr_print(const expr_t *expr)
{
    expr_print(expr);
    bool found_same = false;
    for (size_t i = 0; i < env.globals.len; i++)
        if (expr_eq(expr, env.globals.data[i].expr))
        {
            if (!found_same)
            {
                fputs("             | Bindings: ", stdout);
                found_same = true;
            }
            printf("%s ", env.globals.data[i].name);
        }
    fputc('\n', stdout);
}

void
eval_print(char *s)
{
    expr_t *expr = parse(s);
    if (expr->tag == EXPR_PARSE_ERROR)
        parse_error_print(expr->error.kind, expr->error.location, s);
    else
    {
        expr_t *expr = reduce(parse(s));
        repl_expr_print(expr);
    }
}

const char *print_command = ":print";

void
eval_file(FILE *file)
{
    char  *line = NULL;
    size_t line_len = 0;
    while (getline(&line, &line_len, file) != -1)
    {
        char *new_line = strrchr(line, '\n');
        if (new_line != NULL)
            *new_line = '\0';
        if (strlen(line) == 0 || line[0] == '#')
            continue;
        bool printed = strncmp(line, print_command, strlen(print_command)) == 0;
        if (printed)
        {
            char *new_start = line + strlen(print_command);
            memmove(line, new_start, strlen(new_start) + 1);
            while (isblank(*line))
                memmove(line, line + 1, strlen(line + 1) + 1);
        }
        expr_t *expr = parse(line);
        if (expr->tag == EXPR_PARSE_ERROR)
        {
            parse_error_print(expr->error.kind, expr->error.location, line);
            exit(EXIT_FAILURE);
        }
        expr = reduce(expr);
        if (printed)
            repl_expr_print(expr);
    }
    free(line);
}

/*
 * func ::= \<name>. <expr>
 * expr ::= (<func> | <name>) <expr>* | '(' <expr> ')'
 * stmt ::= <name> := <func>
 * name ::= any visible character except '(' ')' '.' and '\'
 */

int
main(int argc, char **argv, char **environment)
{
    char   history_path[PATH_MAX] = {'\0'};
    char  *xdg_cache_home_str = "XDG_CACHE_HOME";
    size_t xdg_cache_home_str_len = strlen(xdg_cache_home_str);
    for (; *environment != NULL; environment++)
    {
        if (strncmp(*environment, xdg_cache_home_str, xdg_cache_home_str_len) == 0)
            strcpy(history_path, *environment + xdg_cache_home_str_len + 1);
    }
    int option;
    while ((option = getopt(argc, argv, "c:")) > 0)
    {
        switch (option)
        {
        case 'c': eval_print(argv[2]); return 0;
        }
    }
    if (argc == 3 && strcmp(argv[1], "-c") == 0)
    {
    }
    if (argc == 1)
    {
        if (!isatty(fileno(stdin)))
        {
            eval_file(stdin);
            return 0;
        }
        using_history();
        stifle_history(1000);
        strcat(history_path, "/lci");
        struct stat stat_buf;
        if (stat(history_path, &stat_buf) != 0)
        {
            if (errno == ENOENT)
                mkdir(history_path, 0755);
            else
            {
                perror("lci: ");
                return 1;
            }
        }
        strcat(history_path, "/history");
        read_history(history_path);
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
        write_history(history_path);
        free(line);
        return 0;
    }

    for (int i = 1; i < argc; i++)
    {
        FILE *file = fopen(argv[i], "r");
        if (file == NULL)
        {
            fprintf(stderr,
                    "lci: Error while opening %s: %s\n",
                    argv[i],
                    strerror(errno));
            continue;
        }
        eval_file(file);
        fclose(file);
    }
    return 0;
}
