#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mpc.h"
#include "lispy.h"

lenv *lenv_new() {
    lenv *e = malloc(sizeof(lenv));
    e->run = 1;
    e->count = 0;
    e->syms = NULL;
    e->vals = NULL;
    return e;
}

void lenv_del(lenv *e) {
    for (int i = 0; i < e->count; i++) {
        free(e->syms[i]);
        lval_del(e->vals[i]);
    }
    free(e->syms);
    free(e->vals);
    free(e);
}

lval* lenv_get(lenv* e, lval* k) {
    for (int i = 0; i < e->count; i++) {
        // check if string matches symbol, if it does return a copy of it.
        if (strcmp(e->syms[i], k->sym) == 0) {
            return lval_copy(e->vals[i]);
        }
    }

    return lval_err("unbound symbol'%s'!", k->sym);
}

void lenv_put(lenv* e, lval* k, lval* v) {
    // check if variable already exists
    for (int i = 0; i < e->count; i++) {
        // if variable is found replace it
        if (strcmp(e->syms[i], k->sym) == 0) {
            lval_del(e->vals[i]);
            e->vals[i] = lval_copy(v);
            return;
        }
    }

    // if variable was not found allocate space for it
    e->count++;
    e->vals = realloc(e->vals, sizeof(lval*) * e->count);
    e->syms = realloc(e->syms, sizeof(char*) * e->count);
    
    // copy lval and symbol to new location
    e->vals[e->count-1] = lval_copy(v);
    e->syms[e->count-1] = malloc(strlen(k->sym)+1);
    strcpy(e->syms[e->count-1], k->sym);
}

void lenv_add_builtin(lenv *e, char *name, lbuiltin fun) {
    lval *k = lval_sym(name);
    lval *v = lval_fun(fun);
    lenv_put(e, k, v);
    lval_del(k);
    lval_del(v);
}

void lenv_add_builtins(lenv *e) {
    // list functions
    lenv_add_builtin(e, "list", lval_builtin_list);
    lenv_add_builtin(e, "head", lval_builtin_head);
    lenv_add_builtin(e, "tail", lval_builtin_tail);
    lenv_add_builtin(e, "eval", lval_builtin_eval);
    lenv_add_builtin(e, "join", lval_builtin_join);
    lenv_add_builtin(e, "len", lval_builtin_len);
    lenv_add_builtin(e, "cons", lval_builtin_cons);
    lenv_add_builtin(e, "init", lval_builtin_init);

    // variable functions
    lenv_add_builtin(e, "def", lval_builtin_def);

    // general functions
    lenv_add_builtin(e, "exit", lval_builtin_exit);

    // arithmetic functions
    lenv_add_builtin(e, "+", lval_builtin_add);
    lenv_add_builtin(e, "-", lval_builtin_sub);
    lenv_add_builtin(e, "*", lval_builtin_mul);
    lenv_add_builtin(e, "/", lval_builtin_div);
    lenv_add_builtin(e, "%", lval_builtin_mod);
    lenv_add_builtin(e, "^", lval_builtin_exp);
}

lval *lval_num(long x) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

lval *lval_err(char *fmt, ...) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_ERR;

    va_list va;
    va_start(va, fmt);

    v->err = malloc(512);

    vsnprintf(v->err, 511, fmt, va);
    v->err = realloc(v->err, strlen(v->err)+1);

    va_end(va);
    return v;
}

lval *lval_sym(char *s) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

lval *lval_sexpr(void) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

lval *lval_qexpr(void) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

lval *lval_fun(lbuiltin fun) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_FUN;
    v->fun = fun;
    return v;
}

const char *lval_type_name(int t) {
    switch (t) {
        case LVAL_FUN: return "Function";
        case LVAL_NUM: return "Number";
        case LVAL_ERR: return "Error";
        case LVAL_SYM: return "Symbol";
        case LVAL_QEXPR: return "Q-Expression";
        case LVAL_SEXPR: return "S-Expression";
        default: return "Unknown";
    }
}

void lval_del(lval *v) {
    switch(v->type) {
        case LVAL_NUM: break;
        case LVAL_ERR: free(v->err); break;
        case LVAL_SYM: free(v->sym); break;
        case LVAL_QEXPR:
        case LVAL_SEXPR:
            for (int i = 0; i < v->count; i++) {
                lval_del(v->cell[i]);
            }
            free(v->cell);
            break;
        case LVAL_FUN: break;
    }

    free(v);
}

lval *lval_read_num(mpc_ast_t *t) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval *lval_add(lval *v, lval *x) {
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

lval *lval_read(mpc_ast_t *t) {
    if (strstr(t->tag, "number")) { return lval_read_num(t); }
    if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }

    lval *x = NULL;
    if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
    if (strstr(t->tag, "sexpr")) { x = lval_sexpr(); }
    if (strstr(t->tag, "qexpr")) { x = lval_qexpr(); }

    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
        if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
        if (strcmp(t->children[i]->tag, "regex") == 0) { continue; }
        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}

lval *lval_join(lval *x, lval *y) {
    while (y->count) {
        x = lval_add(x, lval_pop(y, 0));
    }

    lval_del(y);
    return x;
}

void lval_print(lval *v) {
    switch (v->type) {
        case LVAL_NUM: printf("%li", v->num); break;
        case LVAL_ERR: printf("Error: %s", v->err); break;
        case LVAL_SYM: printf("%s", v->sym); break;
        case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
        case LVAL_QEXPR: lval_expr_print(v, '{', '}'); break;
        case LVAL_FUN: printf("<function>"); break;
    }
}

lval *lval_copy(lval *v) {
    lval *x = malloc(sizeof(lval));
    x->type = v->type;

    switch (v->type) {
        case LVAL_FUN: x->fun = v->fun; break;
        case LVAL_NUM: x->num = v->num; break;

        case LVAL_ERR:
            x->err = malloc(strlen(v->err) + 1);
            strcpy(x->err, v->err); break;

        case LVAL_SYM:
            x->sym = malloc(strlen(v->sym) + 1);
            strcpy(x->sym, v->sym); break;

        case LVAL_SEXPR:
        case LVAL_QEXPR:
            x->count = v->count;
            x->cell = malloc(sizeof(lval*) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = lval_copy(v->cell[i]);
            }
        break;
    }

    return x;
}

void lval_expr_print(lval *v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->count; i++) {
        lval_print(v->cell[i]);

        if (i != (v->count - 1)) {
            putchar(' ');
        }
    }
    putchar(close);
}

void lval_println(lval *v ) { lval_print(v); putchar('\n'); }

lval *lval_pop(lval *v, int i)  {
    lval *x = v->cell[i];

    // shift memory after the item at i over the top
    memmove(&v->cell[i], &v->cell[i + 1], sizeof(lval*) * (v->count - i - 1));

    // decrease cells counter
    v->count--;

    // reallocate the memory used
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    return x;
}

lval *lval_take(lval *v, int i) {
    lval *x = lval_pop(v, i);
    lval_del(v);
    return x;
} 

lval *lval_builtin_op(lenv *e, lval* a, char *op) {
    // ensure all args are numbers
    for (int i = 0; i < a->count; i++) {
        if (a->cell[i]->type != LVAL_NUM) {
            lval_del(a);
            return lval_err("Function '%s' passed incorrect type for argument %i. "
            "Got %s, Expected %s", op, i, lval_type_name(a->cell[i]->type), lval_type_name(LVAL_NUM));
        }
    }

    // pop the first element
    lval *x = lval_pop(a, 0);

    // if no args and sub then perform unary negation
    if ((strcmp(op, "-") == 0) && a->count == 0) {
        x->num = -x->num;
    }

    // loop remaining elements
    while (a->count > 0) {
        lval *y = lval_pop(a, 0);

        if (strcmp(op, "+") == 0) { x->num += y->num; }
        if (strcmp(op, "-") == 0) { x->num -= y->num; }
        if (strcmp(op, "*") == 0) { x->num *= y->num; }
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {
                lval_del(x);
                lval_del(y);
                x = lval_err("Division by zero!");
                break;
            }
            x->num /= y->num;
        }
        if (strcmp(op, "%") == 0) { x->num %= y->num; }
        if (strcmp(op, "^") == 0) { x->num = pow(x->num, y->num); }
        lval_del(y);
    }

    lval_del(a);
    return x;
}

lval* lval_builtin_head(lenv *e, lval* a) {
    // check error conditions
    LASSERT(a, a->count == 1, 
        "Function 'head' passed too many arguments!"
        "Got %i, Expected %i", a->count, 1);
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, 
        "Function 'head' passed incorrect types!"
        "Got %s, Expected %s", lval_type_name(a->cell[0]->type), lval_type_name(LVAL_QEXPR));
    LASSERT(a, a->cell[0]->count != 0, "Function 'head' passed {}!");

    // otherwise take first argument
    lval* v = lval_take(a, 0);

    // delete all elements that are not head and return
    while (v->count > 1) { lval_del(lval_pop(v, 1)); }
    return v;
}

lval* lval_builtin_tail(lenv *e, lval* a) {
    // check error conditions
    LASSERT(a, a->count == 1, "Function 'tail' passed too many arguments!");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'tail' passed incorrect types!");
    LASSERT(a, a->cell[0]->count != 0, "Function 'tail' passed {}!");

    // take first argument
    lval* v = lval_take(a, 0);

    // delete first element and return
    lval_del(lval_pop(v, 0));
    return v;
}

lval *lval_builtin_list(lenv *e, lval *a) {
    a->type = LVAL_QEXPR;
    return a;
}

lval *lval_builtin_eval(lenv *e, lval *a) {
    LASSERT(a, a->count == 1, "Function 'eval' passed too many arguments");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'eval' passed incorrect type!");

    lval *x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval *lval_builtin_join(lenv *e, lval *a) {
    for (int i = 0; i < a->count; i++) {
        LASSERT(a, a->cell[i]->type == LVAL_QEXPR, "Function 'join' passed incorrect type!");
    }

    lval *x = lval_pop(a, 0);
    while (a->count) {
        x = lval_join(x, lval_pop(a, 0));
    }

    lval_del(a);
    return x;
}

lval *lval_builtin_def(lenv *e, lval *a) {
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
        "Function 'def' passed incorrect type!");
    
    // first arg is symbol list
    lval *syms = a->cell[0];

    // ensure all elements of first list are symbols
    for (int i = 0; i < syms->count; i++) {
        LASSERT(a, syms->cell[i]->type == LVAL_SYM,
            "Function 'def' cannot define non-symbol!");
    }

    // check correct number of symbols and values
    LASSERT(a, syms->count == a->count - 1,
        "Function 'def' cannot define incorrect "
        "number of values to symbols");

    for (int i = 0; i < syms->count; i++) {
        lenv_put(e, syms->cell[i], a->cell[i + 1]);
    }

    lval_del(a);
    return lval_sexpr();
}

lval *lval_builtin_exit(lenv *e, lval *a) {
    e->run = 0;
    return lval_sexpr();
}

lval *lval_builtin_len(lenv *e, lval *a) {
    LASSERT(a, a->count == 1, "Function 'len' only accepts one parameter!");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'len' passed incorrect type!");

    lval *x = lval_num(a->cell[0]->count);
    return x;
}

lval *lval_builtin_cons(lenv *e, lval *a) {
    LASSERT(a, a->count == 2, "Function 'cons' requires two parameters!");
    LASSERT(a, a->cell[1]->type == LVAL_QEXPR, "Function 'cons' passed incorrect type!");

    lval *x = lval_qexpr();
    lval_add(x, lval_pop(a, 0));

    while (a->count) {
        x = lval_join(x, lval_pop(a, 0));
    }
    lval_del(a);
    return x;
}

lval *lval_builtin_init(lenv *e, lval *a) {
    LASSERT(a, a->count == 1, "Function 'init' requires one parameter!");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'init' passed incorrect type!");
    LASSERT(a, a->cell[0]->count != 0, "Function 'init' passed {}!");

    lval* v = lval_take(a, 0);
    lval_del(lval_pop(v, v->count - 1));
    return v;
}

lval *lval_builtin_add(lenv *e, lval *a) {
    return lval_builtin_op(e, a, "+");
}

lval *lval_builtin_sub(lenv *e, lval *a) {
    return lval_builtin_op(e, a, "-");
}
lval *lval_builtin_mul(lenv *e, lval *a) {
    return lval_builtin_op(e, a, "*");
}

lval *lval_builtin_div(lenv *e, lval *a) {
    return lval_builtin_op(e, a, "/");
}

lval *lval_builtin_mod(lenv *e, lval *a) {
    return lval_builtin_op(e, a, "%");
}

lval *lval_builtin_exp(lenv *e, lval *a) {
    return lval_builtin_op(e, a, "^");
}

lval *lval_eval_sexpr(lenv *e, lval *v) {
    // evaluate children
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = lval_eval(e, v->cell[i]);
    }

    // error checking
    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
    }

    // empty expression
    if (v->count == 0) { return v; }

    // single expression
    if (v->count == 1) { return lval_take(v, 0); }

    // ensure first element is symbol
    lval *f = lval_pop(v, 0);
    if (f->type != LVAL_FUN) {
        lval_del(f); 
        lval_del(v);
        return lval_err("First element is not a function");
    }
    
    // invoke function
    lval *result = f->fun(e, v);
    lval_del(f);
    return result;
}

lval *lval_eval(lenv *e, lval *v) {
    if (v->type == LVAL_SYM) {
        lval *x = lenv_get(e, v);
        lval_del(v);
        return x;
    }
    if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(e, v); }
    return v;
}
