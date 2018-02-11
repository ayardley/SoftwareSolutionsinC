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
 *	func.c -- intrinsic functions
 */
#include "expr.h"

static int
fn_sqrt(void)
{
	REAL n;

	if (pop_value(&n) != 0) {
		return -1;
	}
	n = sqrt(n);
	return push_value(n);
}

static int
fn_cbrt(void)
{
	REAL n;

	if (pop_value(&n) != 0) {
		return -1;
	}
	n = Cbrt(n);
	return push_value(n);
}

static int
fn_sin(void)
{
	REAL n;

	if (pop_value(&n) != 0) {
		return -1;
	}
	n = sin(n);
	return push_value(n);
}

static int
fn_cos(void)
{
	REAL n;

	if (pop_value(&n) != 0) {
		return -1;
	}
	n = cos(n);
	return push_value(n);
}

static int
fn_tan(void)
{
	REAL n;

	if (pop_value(&n) != 0) {
		return -1;
	}
	n = tan(n);
	return push_value(n);
}

static int
fn_atan(void)
{
	REAL n;

	if (pop_value(&n) != 0) {
		return -1;
	}
	n = atan(n);
	return push_value(n);
}

static int
fn_exp(void)
{
	REAL n;

	if (pop_value(&n) != 0) {
		return -1;
	}
	n = exp(n);
	return push_value(n);
}

static int
fn_log(void)
{
	REAL n;

	if (pop_value(&n) != 0) {
		return -1;
	}
	n = log(n);
	return push_value(n);
}

static int
fn_log10(void)
{
	REAL n;

	if (pop_value(&n) != 0) {
		return -1;
	}
	n = log10(n);
	return push_value(n);
}

static int
fn_atan2(void)
{
	REAL n;
	REAL m;

	if (pop_value(&m) != 0) {
		return -1;
	}
	if (pop_value(&n) != 0) {
		return -1;
	}
	n = atan2(n, m);
	return push_value(n);
}

static int
fn_logB(void)
{
	REAL n;
	REAL m;

	if (pop_value(&m) != 0) {
		return -1;
	}
	if (pop_value(&n) != 0) {
		return -1;
	}
	if ((n > 0) && (m > 0)) {
		return push_value(log(m) / log(n));
	}
	return push_value(n);
}

static int
fn_min(void)
{
	REAL n;
	REAL m;

	if (pop_value(&m) != 0) {
		return -1;
	}
	if (pop_value(&n) != 0) {
		return -1;
	}
	return (m < n) ? push_value(m) : push_value(n);
}

static int
fn_max(void)
{
	REAL n;
	REAL m;

	if (pop_value(&m) != 0) {
		return -1;
	}
	if (pop_value(&n) != 0) {
		return -1;
	}
	return (m > n) ? push_value(m) : push_value(n);
}

static FUNC func_tbl[] = {   /* List of available functions    */
	{ "sqrt",       fn_sqrt },  /* square root             */
	{ "cbrt",       fn_cbrt },  /* cube root               */
	{ "sin",        fn_sin },   /* trigonometric sine      */
	{ "cos",        fn_cos },   /* cosine                  */
	{ "tan",        fn_tan },   /* tangent                 */
	{ "atan",       fn_atan },  /* arctangent              */
	{ "exp",        fn_exp },   /* inverse natural log e^x */
	{ "log",        fn_log },   /* natural log             */
	{ "log10",      fn_log10 }, /* common log              */
	{ "atan2",      fn_atan2 }, /* two argument arc tan    */
	{ "logB",       fn_logB },  /* Log base b of x         */
	{ "min",        fn_min  },  /* Smaller of x, y         */
	{ "max",        fn_max  },  /* Larger of x, y          */
	{ (char *)0,    (FNPTR)0 }  /* Null for end of list    */
};

FUNC *
func_lookup(char *name)
{
	FUNC *fn;

	for (fn = func_tbl; fn->name; ++fn) {
		if (STREQ(fn->name, name)) {
			return fn;
		}
	}
	return (FUNC *)0;
}
