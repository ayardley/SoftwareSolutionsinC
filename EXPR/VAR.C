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
 *	var.c -- handle user-defined variables
 */
#include "expr.h"

static LST	var_lst = (LST)0;     /* user variable list */

VAR *
var_lookup(char *name)
{
	LST lp;
	VAR *v;

	for (lp = var_lst; lp && (v = (VAR *)*lp); ++lp) {
		if (STREQ(v->name, name)) {
			return v;       /* found a match */
		}
	}
	/* variable not found ... create a new one */
	v = NEW(VAR);
	v->name= (char *)malloc(strlen(name) + 1);
	strcpy(v->name, name);
	v->value = 0;
	var_lst = LSTput(var_lst, v);
	return v;
}
