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
 *	op.c -- operators
 */
#include "expr.h"

static int
op_fact(void)
{
	REAL n;
	int i;

	if (pop_value(&n) != 0) {
		return -1;
	}
	i = (int)floor(n);
	n = 1;
	while (i > 1) {
		n *= i--;
	}
	return push_value(n);
}

static int
op_pos(void)
{
	REAL n;

	if (pop_value(&n) != 0) {
		return -1;
	}
	return push_value(n);
}

static int
op_neg(void)
{
	REAL n;

	if (pop_value(&n) != 0) {
		return -1;
	}
	return push_value(-n);
}

static int
op_pow(void)
{
	REAL n;
	REAL m;

	if (pop_value(&m) != 0) {
		return -1;
	}
	if (pop_value(&n) != 0) {
		return -1;
	}
	if ((floor(m) == m) && (fabs(m) <= 0x7FFFFFFF)) {
		n = XtoI(n, floor(m));
	} else if (n > 0) {
		n = exp(m * log(n));
	} else if (n == 0) {
		n = 1;                  /* { n^0 = 1 } */
	} else if (m == 0) {
		n = 0;                  /* { 0^n = 0 } */
	} else {
		return -1;
	}
	return push_value(n);
}

static int
op_mul(void)
{
	REAL n;
	REAL m;

	if (pop_value(&m) != 0) {
		return -1;
	}
	if (pop_value(&n) != 0) {
		return -1;
	}
	return push_value(n * m);
}

static int
op_div(void)
{
	REAL n;
	REAL m;

	if (pop_value(&m) != 0) {
		return -1;
	}
	if (pop_value(&n) != 0) {
		return -1;
	}
	if (m == 0) {
		return -1;              /* division by zero */
	}
	return push_value(n / m);
}

static int
op_idiv(void)
{
	REAL n;
	REAL m;

	if (pop_value(&m) != 0) {
		return -1;
	}
	if (pop_value(&n) != 0) {
		return -1;
	}
	m = floor(m);
	n = floor(n);
	if (m == 0) {
		return -1;              /* division by zero */
	}
	n = floor(n / m);
	return push_value(n);
}

static int
op_mod(void)
{
	REAL n;
	REAL m;

	if (pop_value(&m) != 0) {
		return -1;
	}
	if (pop_value(&n) != 0) {
		return -1;
	}
	if (m == 0) {
		return -1;              /* division by zero */
	}
	n = fmod(n, m);
	return push_value(n);
}

static int
op_add(void)
{
	REAL n;
	REAL m;

	if (pop_value(&m) != 0) {
		return -1;
	}
	if (pop_value(&n) != 0) {
		return -1;
	}
	return push_value(n + m);
}

static int
op_sub(void)
{
	REAL n;
	REAL m;

	if (pop_value(&m) != 0) {
		return -1;
	}
	if (pop_value(&n) != 0) {
		return -1;
	}
	return push_value(n - m);
}

static int
op_let(void)
{
	REAL n;
	VAR *var;

	if (pop_value(&n) != 0) {
		return -1;
	}
	if (pop_var(&var) != 0) {
		return -1;
	}
	var->value = n;
	return push_value(n);
}

static OP op_tbl[] = {          /* List of available operators */
	{ '!',  (OP_UNARY | 0x0F),              op_fact },
	{ '+',  (OP_RIGHT | OP_UNARY | 0x0E),   op_pos },
	{ '-',  (OP_RIGHT | OP_UNARY | 0x0E),   op_neg },
	{ '^',  (OP_RIGHT | 0x0A),              op_pow },
	{ '*',  (0x09),                         op_mul },
	{ '/',  (0x09),                         op_div },
	{ '\\', (0x09),                         op_idiv },
	{ '%',  (0x09),                         op_mod },
	{ '+',  (0x08),                         op_add },
	{ '-',  (0x08),                         op_sub },
	{ '=',  (OP_RIGHT | 0x01),              op_let },
	{ '\0', (0x00),                         (FNPTR)0 }
};

OP *
op_lookup(int id)
{
	OP *op;

	for (op = op_tbl; op->id; ++op) {
		if (op->id == id) {
			return op;
		}
	}
	return (OP *)0;
}

OP *
op_alt_lookup(OP *op)
{
	OP *nop;

	nop = op;
	while ((++nop)->id) {
		if (nop->id == op->id) {
			return nop;
		}
	}
	return (OP *)0;
}
