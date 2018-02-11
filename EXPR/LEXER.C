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
 *	lexer.c -- lexical analyser
 */
#include "expr.h"

static TOKEN *
new_token(int type)
{
	TOKEN *tk;
	if (tk = NEW(TOKEN)) {
		tk->type = type;
		/* tk->value is left undefined */
	}
	return tk;
}

TOKEN *
free_token(TOKEN *tk)
{
	if (tk) {
		free(tk);
	}
	return (TOKEN *)0;
}

static unsigned char *  ibuf;       /* input buffer base address */
static unsigned char *  ibp;        /* input buffer pointer */

#define GET()           (*ibp ? *ibp++ : EOF)
#define UNGET(c)        ((((c) != EOF) && (ibp > ibuf)) ? *--ibp : EOF)

void
set_input(char *s)		/* initialize input buffer from <s> */
{
	ibp = ibuf = (unsigned char *)s;
}

static TOKEN *
lex_const(int c)		/* scan a (numeric) constant */
/*
 * NOTE: This function checks for an optional leading sign, even though
 * any such sign would already be interpreted as a prexix operator by
 * lex(). This makes the function more useful in other applications.
 */
{
	TOKEN *tk;
	REAL num;
	int state;
	int frac_digs;
	int expon;
	char num_sign;
	char expon_sign;

	UNGET(c);
	state = 0;
	num = 0;
	frac_digs = 0;
	expon = 0;
	expon_sign = '+';
	while ((state >= 0) && (state <= 6)) {
		c = GET();
		switch (state) {

		case 0:
			if ((c == '+') || (c == '-')) {
				num_sign = c;
				state = 1;
			} else {
				UNGET(c);
				state = 1;
			}
			break;

		case 1:                         /* whole number */
			if (c == '.') {
				state = 2;
			} else if ((c == 'E') || (c == 'e')) {
				state = 4;
			} else if (isdigit(c)) {
				num = (num * 10) + (c - '0');
				state = 1;
			} else {
				state = 7;
			}
			break;

		case 2:                         /* decimal point */
			if (isdigit(c)) {
				UNGET(c);
				state = 3;
			} else {
				state = -1;
			}
			break;

		case 3:                         /* fractional digits */
			if ((c == 'E') || (c == 'e')) {
				state = 4;
			} else if (isdigit(c)) {
				num = (num * 10) + (c - '0');
				++frac_digs;
				state = 3;
			} else {
				state = 7;
			}
			break;

		case 4:                         /* exponent */
			if (isdigit(c)) {
				UNGET(c);
				state = 6;
			} else if ((c == '+') || (c == '-')) {
				expon_sign = c;
				state = 5;
			} else {
				state = -1;
			}
			break;

		case 5:                         /* exponent sign */
			if (isdigit(c)) {
				UNGET(c);
				state = 6;
			} else {
				state = -1;
			}
			break;

		case 6:                         /* exponent digits */
			if (isdigit(c)) {
				expon = (expon * 10) + (c - '0');
				state = 6;
			} else {
				state = 7;
			}
			break;

		default:                        /* bad state */
			state = -1;
			break;

		}
	}

	UNGET(c);               /* push back terminating char */
	if (state < 0) {
		tk = new_token(TK_ERROR);
		return tk;
	}
	if (num_sign == '-') {
		num = -num;
	}
	if (expon_sign == '-') {
		expon = -expon;
	}
	expon -= frac_digs;
	if (expon != 0) {
		num *= TenToI(expon);
	}
	tk = new_token(TK_CONST);
	tk->value.num = num;
	return tk;
}

static TOKEN *
lex_ident(int c)			/* scan an identifier */
{
	STR str;
	TOKEN *tk;
	FUNC  *func;
	VAR   *var;

	str = STRinit(c, '\0');
	while ((c = GET()) && isalnum(c)) {  /* accumulate ident chars */
		str = STRput(str, c);
	}
	UNGET(c);                            /* push back non-ident char */
	if (func = func_lookup(str)) {
		tk = new_token(TK_FUNC);
		tk->value.func = func;
	} else if (var = var_lookup(str)) {
		tk = new_token(TK_VAR);
		tk->value.var = var;
	} else {
		tk = new_token(TK_ERROR);
	}
	STRfree(str);
	return tk;
}

TOKEN *
lex(void)
{
	int c;
	TOKEN *tk;
	OP *op;

	while ((c = GET()) && isspace(c))       /* skip leading whitespace */
		;
	if (c == EOF) {                         /* end of input */
		return (TOKEN *)0;
	}
	if (c == '(') {                         /* sub-expr/argument start */
		tk = new_token(TK_OPEN);
	} else if (c == ',') {                  /* argument separator */
		tk = new_token(TK_SEP);
	} else if (c == ')') {                  /* sub-expr/argument end */
		tk = new_token(TK_CLOSE);
	} else if ((c == '.') || isdigit(c)) {  /* constant */
		tk = lex_const(c);
	} else if (isalpha(c)) {                /* identifier */
		tk = lex_ident(c);
	} else if (op = op_lookup(c)) {         /* operator */
		tk = new_token(TK_OP);
		tk->value.op = op;
	} else {                                /* error */
		tk = new_token(TK_ERROR);
	}
	return tk;
}
