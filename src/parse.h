#ifndef LCI_PARSE_H
#define LCI_PARSE_H

#define _POSIX_C_SOURCE 200809L
#include "expr.h"
#include <ctype.h>
#include <string.h>

expr_t *
parse(char *s);

#endif
