#ifndef _LISPY
#define _LISPY

#include <stdint.h>
#include "mpc.h"

#define LASSERT(args, cond, err) \
  if (!(cond)) { lval_del(args); return lval_err(err); } 

enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR };
typedef enum lerr_e { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM } lerr_e;

typedef struct lval {
    int type;
    long num;
    char *err;
    char *sym;
    
    int count;
    struct lval** cell;
} lval;

lval *lval_num(long x);
lval *lval_err(char *m);
lval *lval_sym(char *s);
lval *lval_sexpr(void);
lval *lval_qexpr(void);
void lval_del(lval *v);
lval *lval_read_num(mpc_ast_t *t);
lval *lval_add(lval *v, lval *x);
lval *lval_read(mpc_ast_t *t);
lval *lval_pop(lval *v, int i);
lval *lval_join(lval *x, lval *y);
void lval_print(lval *v);
void lval_expr_print(lval *v, char open, char close);
void lval_println(lval *v );
lval *lval_eval(lval *v);
lval *lval_pop(lval *v, int i);
lval *lval_take(lval *v, int i);
lval *lval_builtin_op(lval* a, char *op);
lval* lval_builtin_head(lval* a);
lval* lval_builtin_tail(lval* a);
lval *lval_builtin_list(lval *a);
lval *lval_builtin_eval(lval *a);
lval *lval_builtin_join(lval *a);
lval *lval_builtin_len(lval *a);
lval *lval_builtin_cons(lval *a);
lval *lval_builtin_init(lval *a);
lval *lval_builtin(lval *a, char *func);
lval *lval_eval_sexpr(lval *v);
lval *lval_eval(lval *v);

#endif