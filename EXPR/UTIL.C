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
 *	util.c -- utility functions
 */
#include "expr.h"

REAL
Cbrt(REAL x)
/*
 * Returns cube root(x). An approximation is made with exp(ln(x)/3)
 * and a one Newton-Raphson iteration corrects any log/exp error.
 */
{
	REAL r;

	if (x == 0) {
		return 0;
	}
	r = exp(log(abs(x)) / 3);
	r = r - (r - abs(x) / (r * r)) / 3;
	return ((x < 0) ? -r : r);
}

REAL
XtoI(REAL x, int i)
/* Returns real X to integer power. */
{
	REAL r, t;
	int n;

	if (i == 0) {
		return (x == 0) ? 0 : 1;
	}
	n = abs(i);
	t = x;
	r = 1;
	while (n > 0) {
		if (n & 1) {
			r = r * t;
		}
		if (n > 1) {
			t = t * t;
		}
		n = n >> 1;
	}
	return ((i > 0) ? r : ((r == 0) ? 0 : (1.0 / r)));
}                                       /* XtoI. */

REAL
TenToI(int i)
/* Returns 10^I. */
{
	REAL r, t;
	int n;

	if (i == 0) {
		return 1;
	}
	n = abs(i);
	t = 10;
	r = 1;
	while (n > 0) {
		if (n & 1) {
			r = r * t;
		}
		if (n > 1) {
			t = t * t;
		}
		n = n >> 1;
	}
	return ((i < 0) ? (1.0 / r) : r);
}  /* TenToI. */

STR
STRgets(FILE *f)  /* read a line of input from ``f'' */
{
	STR str;
	int c;

	str = STRcopy("");
	while (str && ((c = getc(f)) != EOF) && (c != '\n')) {
		str = STRadd(str, (STR)0, c);
	}
	if ((c == EOF) && (STRlen(str) <= 0)) {
		str = STRfree(str);
	}
	return str;
}
