#ifndef LCI_PARSE_H
#define LCI_PARSE_H

#define _POSIX_C_SOURCE 200809L
#include "expr.h"
#include <ctype.h>
#include <string.h>

expr_t *
parse(char *s);
void
parse_error_print(enum parse_error kind, char *location, const char *origin);

#endif
