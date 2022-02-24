enum expr
{
    EXPR_FUNC,
    EXPR_VAR,
};

typedef struct expr
{
    enum expr tag;
    union
    {
        struct
        {
            char *parm_name;
            struct expr *body;
        } func;
        struct
        {
            char *name;
        } var;
    };
} expr_t;

expr_t *parse_expression(char *s)
{
    if (*s == '\\')
    {
        expr_t *expr = expr_new(EXPR_FUNC);
        s++;
        char *param_name_start = s;
        while (*s != '.')
            s++;
        expr->func.param_name = strndup(param_name_start, s - param_name_start);
        s++;
        expr->func.body = parse_expression(&s);
    }
    else
    {

    }
}

void parse_statement(char *s)
{
    result = parse_expression(&s);
    if (result)
        return result;
    char *sym;
    parse_sym(&s, &sym);
    check_has_assignment_op(&s); // :=
    expr_t *expr = parse_expression(&s);
    env_assign(sym, expr);
    return NULL;
}

int main()
{


    return 0;
}
