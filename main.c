#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readline.h"
#include "mpc.h"
#include "lispy.h"

mpc_parser_t *Lispy;

int main(int argc, char **argv) {
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *String = mpc_new("string");
    mpc_parser_t *Comment = mpc_new("comment");
    mpc_parser_t *Symbol = mpc_new("symbol");
    mpc_parser_t *Sexpr = mpc_new("sexpr");
    mpc_parser_t *Qexpr = mpc_new("qexpr");
    mpc_parser_t *Expr = mpc_new("expr");
    Lispy = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT, 
    " \
        number : /-?[0-9]+/ ; \
        string : /\"(\\\\.|[^\"])*\"/ ; \
        comment : /;[^\\r\\n]*/ ; \
        symbol : /[a-zA-Z0-9_+\\-*%^\\/\\\\=<>!&|]+/ ; \
        sexpr : '(' <expr>* ')' ; \
        qexpr : '{' <expr>* '}' ; \
        expr : <number> | <string> | <comment> | <symbol> | <sexpr> | <qexpr> ; \
        lispy : /^/ <expr>* /$/ ; \
    ", 
    Number, String, Comment, Symbol, Sexpr, Qexpr, Expr, Lispy);

    lenv *e = lenv_new();
    lenv_add_builtins(e);

    puts("Lispy Version 0.0.0");
    puts("Press Ctrl+c to exit");
    while (e->run) {
        char *input = readline("lispy> ");
        add_history(input);

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            // mpc_ast_print(r.output);

            lval *x = lval_eval(e, lval_read(r.output));
            lval_println(x);
            lval_del(x);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    printf("Good bye!\n");
    mpc_cleanup(8, Number, String, Comment,  Symbol, Sexpr, Qexpr, Expr, Lispy);
    lenv_del(e);
}