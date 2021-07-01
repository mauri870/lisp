#ifndef _LISPY
#define _LISPY

#include <stdint.h>
#include "mpc.h"

#define LASSERT(args, cond, fmt, ...) \
    if (!(cond)) { \
      lval* err = lval_err(fmt, ##__VA_ARGS__); \
      lval_del(args); \
      return err; \
    }

#define LASSERT_TYPE(func, args, index, expect) \
    LASSERT(args, args->cell[index]->type == expect, \
        "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s.", \
        func, index, lval_type_name(args->cell[index]->type), lval_type_name(expect))

#define LASSERT_NUM(func, args, num) \
    LASSERT(args, args->count == num, \
        "Function '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
        func, args->count, num)

#define LASSERT_NOT_EMPTY(func, args, index) \
    LASSERT(args, args->cell[index]->count != 0, \
        "Function '%s' passed {} for argument %i.", func, index);

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };
typedef enum lerr_e { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM } lerr_e;

typedef lval *(*lbuiltin)(lenv*, lval*);

struct lenv {
    lenv *par;
    int count;
    int run;
    char **syms;
    lval **vals;
};

struct lval {
    int type;
    
    // Basic types
    long num;
    char *err;
    char *sym;

    // Func
    lbuiltin builtin;
    lenv* env;
    lval *formals;
    lval *body;

    // Expr
    int count;
    lval** cell;
};

lenv *lenv_new(void);
void lenv_del(lenv *e);
lval *lenv_get(lenv *e, lval *k);
void lenv_put(lenv *e, lval *k, lval *v);
void lenv_add_builtin(lenv *e, char *name, lbuiltin fun);
void lenv_add_builtins(lenv *e);
lenv *lenv_copy(lenv *);
void lenv_def(lenv *, lval *k, lval *v);

lval *lval_num(long x);
lval *lval_err(char *fmt, ...);
lval *lval_sym(char *s);
lval *lval_sexpr(void);
lval *lval_qexpr(void);
lval *lval_fun(lbuiltin);
lval *lval_lambda(lval *formals, lval *body);

const char *lval_type_name(int);
void lval_del(lval *v);
lval *lval_read_num(mpc_ast_t *t);
lval *lval_add(lval *v, lval *x);
lval *lval_read(mpc_ast_t *t);
lval *lval_pop(lval *v, int i);
lval *lval_join(lval *x, lval *y);
void lval_print(lval *v);
lval *lval_copy(lval *v);
void lval_expr_print(lval *v, char open, char close);
void lval_println(lval *v );
lval *lval_eval(lenv *e, lval *v);
lval *lval_pop(lval *v, int i);
lval *lval_take(lval *v, int i);
lval *lval_builtin_op(lenv *, lval* a, char *op);
lval *lval_builtin_add(lenv *, lval *a);
lval *lval_builtin_sub(lenv *, lval *a);
lval *lval_builtin_mul(lenv *, lval *a);
lval *lval_builtin_div(lenv *, lval *a);
lval *lval_builtin_mod(lenv *, lval *a);
lval *lval_builtin_exp(lenv *, lval *a);
lval *lval_builtin_head(lenv *, lval* a);
lval *lval_builtin_tail(lenv *, lval *);
lval *lval_builtin_list(lenv *, lval *);
lval *lval_builtin_eval(lenv *, lval *);
lval *lval_builtin_join(lenv *, lval *);
lval *lval_builtin_len(lenv *, lval *);
lval *lval_builtin_cons(lenv *, lval *);
lval *lval_builtin_init(lenv *, lval *);
lval *lval_builtin_def(lenv *, lval *);
lval *lval_builtin_exit(lenv *, lval *);
lval *lval_builtin_lambda(lenv *, lval *);
lval *lval_builtin_fun(lenv *, lval *);
lval *lval_builtin_var(lenv *, lval *a, char *func);
lval *lval_eval_sexpr(lenv *e, lval *v);
lval *lval_eval(lenv *e, lval *v);
lval *lval_call(lenv *e, lval *f, lval *a);

#endif