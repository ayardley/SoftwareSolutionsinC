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
 *	expr.c -- expression evaluator top-level functions
 */
#include "expr.h"

STR
prompt(char *msg)
{
	fprintf(stderr, "%s", msg);
	fflush(stderr);
	return STRgets(stdin);
}

int
main(int argc, char **argv)
{
	STR str;
	REAL num;
	int rv;
	LST expr;
	VAR *var;

	var = var_lookup("pi");
	var->value = M_PI;
	var = var_lookup("e");
	var->value = M_E;
	fprintf(stderr, "Enter expression(s) to evaluate,\n");
	fprintf(stderr, "or a blank line to quit.\n");
	for (str = (STR)0; str = prompt("\n? "); str = STRfree(str)) {
		if (*str == '\0') {
			break;
		}
		expr = parse(str);
		if (expr == NULL) {
			fprintf(stderr, "SYNTAX ERROR.\n");
			continue;
		}
		rv = eval(&num, expr);
		if (rv != 0) {
			fprintf(stderr, "EVALUATION ERROR.\n");
			continue;
		}
		fprintf(stderr, "%-.6lg\n", num);
	}
	fprintf(stderr, "\nBye.\n");
	return(0);
}
