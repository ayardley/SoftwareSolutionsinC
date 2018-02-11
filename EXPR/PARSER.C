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
 *	parser.c -- expression parser
 */
#include "expr.h"

static LST expr = (LST)0;          /* output list   */
static LST defer = (LST)0;         /* deferred list */

#ifdef DEBUG
static char *
tk_str(TOKEN *tk)
{
	static char buf[1024];

	switch (tk->type) {
	case TK_ERROR:
		sprintf(buf, "<ERROR>");
		break;
	case TK_CONST:
		sprintf(buf, "<CONST=%lf>", tk->value.num);
		break;
	case TK_OP:
		sprintf(buf, "<OP='%c'>",
			tk->value.op ? tk->value.op->id : '?');
		break;
	case TK_FUNC:
		sprintf(buf, "<FUNC='%s'>", tk->value.func->name);
		break;
	case TK_VAR:
		sprintf(buf, "<VAR='%s'>", tk->value.var->name);
		break;
	case TK_OPEN:
		sprintf(buf, "<OPEN>");
		break;
	case TK_SEP:
		sprintf(buf, "<SEP>");
		break;
	case TK_CLOSE:
		sprintf(buf, "<CLOSE>");
		break;
	}
	return buf;
}
#endif /* DEBUG */

#define PREC(op) ((op)->attrib & OP_PREC)

static void
output(TOKEN *tk)
{
#ifdef FOLD_CONST
	static TOKEN *cexpr[4];
	int n_ops;
	int n_expr;
	int i;

	cexpr[0] = cexpr[1] = cexpr[2] = cexpr[3] = (TOKEN *)0;
	if (tk->type == TK_OP) {    /* CONST expr elimination check */
		n_ops = (tk->value.op->attrib & OP_UNARY) ? 1 : 2;
		n_expr= LSTlen(expr);
		if (n_expr >= n_ops) {  /* enough operands? */
			for(i=0; i < n_ops; ++i) {
				cexpr[i] = expr[n_expr -n_ops +i];
				if (cexpr[i]->type != TK_CONST) {
					break;       /* non-CONST! */
				}
			}
			if (i == n_ops) {    /* all are CONST */
				for(i=0; i <n_ops; i++) {
#ifdef DEBUG
					fprintf(stdout, "operand%d: %s\n",
						i+1, tk_str(cexpr[i]));
#endif /* DEBUG */
					--n_expr;
					LSTpop(&expr[n_expr]);
				}
#ifdef DEBUG
				fprintf(stdout, "operator: %s\n", tk_str(tk));
#endif /* DEBUG */
				cexpr[i] = tk;  /* op last */
				if (eval(&tk->value.num, (LST)cexpr) == 0) {
					tk->type = TK_CONST;  /* new CONST */
#ifdef DEBUG
					fprintf(stdout, "value:    %s\n", tk_str(tk));
#endif /* DEBUG */
					expr = LSTput(expr, tk);
					for (i=0; i < n_ops; ++i) {
						free_token(cexpr[i]);
					}
					return;		/* CONST expr removed */
				}
#ifdef DEBUG
				fprintf(stdout, "CONST eval error\n");
#endif /* DEBUG */
				for(i = 0; i < n_ops; ++i) {
					expr = LSTput(expr, cexpr[i]);
				}
			}
		}
	}
#endif
	expr = LSTput(expr, tk);
}

static void
defer_op(TOKEN *tk)
{
	TOKEN *tt;

	for (;;) {
		tt = (TOKEN *)defer[0];
		if (tt == NULL) {
			break;
		}
		if (tt->type != TK_OP) {
			break;
		}
		if (PREC(tk->value.op) > PREC(tt->value.op)) {
			break;
		}
		if ((tk->value.op->attrib & OP_RIGHT)
		&&  (PREC(tk->value.op) == PREC(tt->value.op))) {
			break;
		}
		tt = (TOKEN *)LSTpop(defer);
		output(tt);
	}
	defer = LSTpush(defer, tk);
}

static int
prefix_op(TOKEN *tk)
{
	while ((tk->value.op->attrib & (OP_RIGHT | OP_UNARY))
	   !=  (OP_RIGHT | OP_UNARY)) {
		tk->value.op = op_alt_lookup(tk->value.op);
		if (tk->value.op == NULL) {
			return -1;              /* invalid op */
		}
	}
	defer_op(tk);
	return 0;
}

static int
infix_op(TOKEN *tk)
{
	while ((tk->value.op->attrib & (OP_RIGHT | OP_UNARY))
	   ==  (OP_RIGHT | OP_UNARY)) {
		tk->value.op = op_alt_lookup(tk->value.op);
		if (tk->value.op == NULL) {
			return -1;              /* invalid op */
		}
	}
	defer_op(tk);
	if (tk->value.op->attrib & OP_UNARY) {
		tk = (TOKEN *)LSTpop(defer);
		output(tk);
		return 1;
	}
	return 0;
}

static int
reduce_sep(void)
{
	TOKEN *tk;

	while (tk = (TOKEN *)defer[0]) {
		if (tk->type == TK_FUNC) {
			return 0;
		}
		if (tk->type != TK_OP) {
			break;
		}
		tk = (TOKEN *)LSTpop(defer);
		output(tk);
	}
	return -1;                      /* unmatched separator */
}

static int
reduce_close(void)
{
	TOKEN *tk;

	while (tk = (TOKEN *)LSTpop(defer)) {
		if (tk->type == TK_FUNC) {
			output(tk);
			return 1;
		}
		if (tk->type == TK_OPEN) {
			return 1;
		}
		if (tk->type != TK_OP) {
			break;
		}
		output(tk);
	}
	return -1;                      /* unmatched close */
}

static int
reduce_end(void)
{
	TOKEN *tk;

	while (tk = (TOKEN *)LSTpop(defer)) {
		if (tk->type != TK_OP) {
			break;
		}
		output(tk);
	}
	if (tk != NULL) {
		return -1;              /* unmatched open */
	}
	return 0;
}

LST
free_token_list(LST token_lst)
{
	LST lp;

	if (token_lst == NULL) {
		return (LST)0;
	}
	for (lp = token_lst; *lp; ++lp) {
		free_token((TOKEN *)*lp);
	}
	free(token_lst);
	return (LST)0;
}

LST
parse(char *expr_src)
{
	int state;
	TOKEN *tk;
#ifdef DEBUG
	TOKEN *tt;
	LST lp;
#endif /* DEBUG */

	set_input(expr_src);
	expr = free_token_list(expr);
	defer = LSTinit((void *)0);
	state = 0;
	while ((state >= 0) && (state <= 2) && (tk = lex())) {
#ifdef DEBUG
		fprintf(stdout, "state:  %d\n", state);
#endif /* DEBUG */
		switch (state) {

		case 0:         /* leading operand or prefix operator */
			switch (tk->type) {
			case TK_CONST:
			case TK_VAR:
				output(tk);
				state = 1;
				break;
			case TK_FUNC:
				defer = LSTpush(defer, tk);
				state = 2;
				break;
			case TK_OP:
				state = prefix_op(tk);
				break;
			case TK_OPEN:
				defer = LSTpush(defer, tk);
				break;
			case TK_SEP:
			case TK_CLOSE:
			case TK_ERROR:
				state = -1;
				break;
			}
			break;

		case 1:         /* infix operator */
			switch (tk->type) {
			case TK_OP:
				state = infix_op(tk);
				break;
			case TK_OPEN:
				defer = LSTpush(defer, tk);
				state = 0;
				break;
			case TK_SEP:
				state = reduce_sep();
				break;
			case TK_CLOSE:
				state = reduce_close();
				break;
			case TK_CONST:
			case TK_VAR:
			case TK_FUNC:
			case TK_ERROR:
				state = -1;
				break;
			}
			break;

		case 2:         /* function call */
			case TK_OPEN:
				state = 0;
				break;
			default:
				state = -1;
				break;
			break;

		}
#ifdef DEBUG
		fprintf(stdout, "input:  %s\n", tk_str(tk));
		fprintf(stdout, "stack:  ");
		for (lp = defer; lp && (tt = *lp); ++lp) {
			fprintf(stdout, "%s", tk_str(tt));
		}
		fprintf(stdout, "\n");
		fprintf(stdout, "output: ");
		for (lp = expr; lp && (tt = *lp); ++lp) {
			fprintf(stdout, "%s", tk_str(tt));
		}
		fprintf(stdout, "\n\n");
#endif /* DEBUG */
	}

	if ((state != 1) || (reduce_end() != 0)) {
#ifdef DEBUG
		fprintf(stdout, "error:  state=%d\n", state);
#endif /* DEBUG */
		defer = free_token_list(defer);
		expr = free_token_list(expr);
		return (LST)0;
	}
	defer = free_token_list(defer);
#ifdef DEBUG
	fprintf(stdout, "final:  ");
	for (lp = expr; lp && (tt = *lp); ++lp) {
		fprintf(stdout, "%s", tk_str(tt));
	}
	fprintf(stdout, "\n");
#endif /* DEBUG */
	return expr;
}
