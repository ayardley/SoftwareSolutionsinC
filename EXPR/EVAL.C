/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Robert D. Miller.  Not derived from licensed software.
 * From the book "Software Solutions in C", edited by Dale Schumacher.
 *
 * Permission is granted to anyone to use this software for any
 * purpose on any computer system, and to redistribute it in any way,
 * subject to the following restrictions:
 *
 *   1. The author is not responsible for the consequences of use of
 *	this software, no matter how awful, even if they arise
 *	from defects in it.
 *
 *   2. The origin of this software must not be misrepresented, either
 *	by explicit claim or by omission.
 *
 *   3. Altered versions must be plainly marked as such, and must not
 *	be misrepresented (by explicit claim or omission) as being
 *	the original software.
 *
 *   4. This notice must not be removed or altered.
 */

/*
 *	eval.c -- evaluation engine
 */
#include "expr.h"

static TOKEN *eval_stack = (TOKEN *)0;
static size_t eval_stack_size = 0;
static size_t eval_stack_ptr = 0;

int
pop_value(REAL *n_p)
{
	TOKEN *tk;

	if (eval_stack_ptr > 0) {
		tk = &eval_stack[--eval_stack_ptr];
		if (tk->type == TK_CONST) {
			*n_p = tk->value.num;
			return 0;
		}
		if (tk->type == TK_VAR) {
			*n_p = tk->value.var->value;
			return 0;
		}
		return -1;              /* bad operand type */
	}
	return -1;                      /* stack underflow */
}

int
pop_var(VAR **var_p)
{
	TOKEN *tk;

	if (eval_stack_ptr > 0) {
		tk = &eval_stack[--eval_stack_ptr];
		if (tk->type == TK_VAR) {
			*var_p = tk->value.var;
			return 0;
		}
		return -1;              /* bad operand type */
	}
	return -1;                      /* stack underflow */
}

static int
grow_eval_stack(void)
{
	if (eval_stack_ptr >= eval_stack_size) {
		eval_stack_size += 4;
		eval_stack = (TOKEN *)realloc(eval_stack,
					eval_stack_size * sizeof(TOKEN));
		if (eval_stack == NULL) {
			return -1;      /* stack overflow */
		}
	}
	return 0;
}

int
push_value(REAL n)
{
	if (grow_eval_stack() != 0) {
		return -1;
	}
	eval_stack[eval_stack_ptr].type = TK_CONST;
	eval_stack[eval_stack_ptr].value.num = n;
	++eval_stack_ptr;
	return 0;
}

int
push_var(VAR *var)
{
	if (grow_eval_stack() != 0) {
		return -1;
	}
	eval_stack[eval_stack_ptr].type = TK_VAR;
	eval_stack[eval_stack_ptr].value.var = var;
	++eval_stack_ptr;
	return 0;
}

int
eval(REAL *n_p, LST expr)
{
	TOKEN *tk;

	while (tk = LSTpop(expr)) {
		switch (tk->type) {
		case TK_CONST:
			if (push_value(tk->value.num) != 0) {
				goto fail;
			}
			break;
		case TK_VAR:
			if (push_var(tk->value.var) != 0) {
				goto fail;
			}
			break;
		case TK_OP:
			if ((*tk->value.op->fn)() != 0) {
				goto fail;
			}
			break;
		case TK_FUNC:
			if ((*tk->value.func->fn)() != 0) {
				goto fail;
			}
			break;
		default:
			goto fail;
		}
	}
	return pop_value(n_p);
fail:
#ifdef DEBUG
	fprintf(stdout, "--error--\n");
#endif /* DEBUG */
	return -1;
}
