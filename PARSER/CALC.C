/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Mayan Moudgill.  Not derived from licensed software.
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

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"
#include "lst.h"
#include "parser.h"

#define	TRACE(x)		/**/
#define	DEBUG(x)	x	/**/

#define	DIGIT	"0123456789"
#define	UPPER	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define	LOWER	"abcdefghijklmnopqrstuvwxyz"
#define	IDENT	UPPER LOWER DIGIT "_"

typedef struct Var {
	char *		ident;
	int		value;
} VAR;

LST	var_list = (LST) 0;

int
set_value(STR ident, int value)
{
	VAR *v;
	int i;

	if (ident == (STR)0) {
		return 0;
	}
	if( var_list ) {
		for (i = 0; (v = var_list[i]); i++ ) {
			if (strcmp(v->ident, ident) == 0) {
			  	return v->value = value;
		   	}
		}
	}
	v = (VAR *)malloc(sizeof(VAR));
	v->ident = STRcopy(ident);
	v->value = value;
	var_list = LSTpush(var_list, v);
	return value;
}

int
get_value(STR ident)
{
	int i;
	VAR *v;

	if (ident == (STR)0) {
		return 0;
	}
	if( var_list ) {
		for (i = 0; (v = var_list[i]); i++ ) {
			if (strcmp(v->ident, ident) == 0) {
			  	return v->value;
		   	}
		}
	}
	return 0;
}

int
parse_digits(BNF_PARSER * P, int prior)
{
	int d;

	TRACE(fprintf(stderr,"in parse_digits\n"));
	BNF_BEGIN(P);
		BNF_ONEOF(P, DIGIT, d);
		d = (10 * prior) + (d - '0');
		BNF_NON(P, d = parse_digits(P, d));
	BNF_OR(P);
		BNF_ONEOF(P, DIGIT, d);
		d = (10 * prior) + (d - '0');
	BNF_END(P);
	return d;
}

STR
parse_ident(BNF_PARSER * P)
{
	STR str;
	int c;

	TRACE(fprintf(stderr,"in parse_ident\n"));
	BNF_BEGIN(P);
		BNF_ONEOF(P, IDENT, c);
		BNF_NON(P, str = parse_ident(P));
		str = STRpush(str, c);
	BNF_OR(P);
		BNF_ONEOF(P, IDENT, c);
		str = STRpush(0, c);
	BNF_END(P);
	return str;
}

int	parse_expr(BNF_PARSER * P);		/* forward declaration */

int
parse_prim(BNF_PARSER * P)
{
	int  d;
	STR id;

	TRACE(fprintf(stderr,"in parse_prim\n"));
	BNF_BEGIN(P);
		BNF_TERMINAL(P, '(');
		BNF_NON(P, d = parse_expr(P));
		BNF_TERMINAL(P, ')');
		DEBUG(fprintf(stderr,"(expr) = %d\n", d));
	BNF_OR(P);
		BNF_NON(P, d = parse_digits(P, 0));
		DEBUG(fprintf(stderr,"const = %d\n", d));
	BNF_OR(P);
		BNF_NON(P, id = parse_ident(P));
		d = get_value(id);
		DEBUG(fprintf(stderr,"var %s = %d\n", id, d));
		id = STRfree(id);
	BNF_END(P);
	return d;
}

int
parse_unop(BNF_PARSER * P)
{
	int  d;

	TRACE(fprintf(stderr,"in parse_unop\n"));
	BNF_BEGIN(P);
		BNF_TERMINAL(P, '-');
		BNF_NON(P, d = -parse_prim(P));
		DEBUG(fprintf(stderr,"-expr = %d\n", d));
	BNF_OR(P);
		BNF_TERMINAL(P, '+');
		BNF_NON(P, d = parse_prim(P));
		DEBUG(fprintf(stderr,"+expr = %d\n", d));
	BNF_OR(P);
		BNF_NON(P, d = parse_prim(P));
	BNF_END(P);
	return d;
}

int
parse_multail(BNF_PARSER * P, int value)
{
	int  d;

	TRACE(fprintf(stderr, "in parse_multail\n"));
	BNF_BEGIN(P);
		BNF_TERMINAL(P, '*');
		BNF_NON(P, d = parse_unop(P));
		DEBUG(fprintf(stderr,"%d * %d = %d\n", value, d, value * d));
		BNF_NON(P, d = parse_multail(P, value * d ));
	BNF_OR(P);
		BNF_TERMINAL(P, '/');
		BNF_NON(P, d = parse_unop(P));
		DEBUG(fprintf(stderr,"%d / %d = %d\n", value, d, value / d));
		BNF_NON(P, d = parse_multail(P, value / d ));
	BNF_OR(P);
		d = value;
	BNF_END(P);
        return d;
}

int
parse_mulop(BNF_PARSER * P)
{
	int  d;

	TRACE(fprintf(stderr,"in parse_mulop\n"));
	BNF_BEGIN(P);
		BNF_NON(P, d = parse_unop(P));
		BNF_NON(P, d = parse_multail(P, d));
	BNF_END(P);
	return d;
}

int
parse_addtail(BNF_PARSER * P, int value)
{
	int  d;

	TRACE(fprintf(stderr, "in parse_addtail\n"));
	BNF_BEGIN(P);
		BNF_TERMINAL(P, '+');
		BNF_NON(P, d = parse_mulop(P));
		DEBUG(fprintf(stderr,"%d + %d = %d\n", value, d, value + d));
		BNF_NON(P, d = parse_addtail(P, value + d ));
	BNF_OR(P);
		BNF_TERMINAL(P, '-');
		BNF_NON(P, d = parse_mulop(P));
		DEBUG(fprintf(stderr,"%d - %d = %d\n", value, d, value - d));
		BNF_NON(P, d = parse_addtail(P, value - d ));
	BNF_OR(P);
		d = value;
	BNF_END(P);
        return d;
}

int
parse_addop(BNF_PARSER * P)
{
	int  d;

	TRACE(fprintf(stderr,"in parse_addop\n"));
	BNF_BEGIN(P);
		BNF_NON(P, d = parse_mulop(P));
		BNF_NON(P, d = parse_addtail(P, d));
	BNF_END(P);
	return d;
}

int
parse_assignop(BNF_PARSER * P)
{
	STR id;
	int  d;

	TRACE(fprintf(stderr,"in parse_assignop\n"));
	id = (STR) 0;
	BNF_BEGIN(P);
		BNF_NON(P, id = parse_ident(P));
		BNF_TERMINAL(P, '=');
		DEBUG(fprintf(stderr,"lval %s = ...\n", id, d));
		BNF_NON(P, d = parse_expr(P));
		set_value(id, d);
		DEBUG(fprintf(stderr,"let %s = %d\n", id, d));
		id = STRfree(id);
	BNF_END(P);

	return d;
}

int
parse_expr(BNF_PARSER * P)
{
	int d;
 
	TRACE(fprintf(stderr,"in parse_expr\n"));
	BNF_BEGIN(P);
		BNF_NON(P, d = parse_assignop(P));
	BNF_OR(P);
		BNF_NON(P, d = parse_addop(P));
	BNF_END(P);
	return d;
}

void
parse_error_line(BNF_PARSER * P)
{
	int dummy;

	BNF_BEGIN(P);
		BNF_TERMINAL(P, '\n');
	BNF_OR(P);
		BNF_ANY(P, dummy);
		BNF_NON(P, parse_error_line(P));
	BNF_END(P);
}

void
parse_lines(BNF_PARSER * P)
{
	int value;

	do {
		printf("? ");
		fflush(stdout);
		BNF_BEGIN(P);
			BNF_TERMINAL(P, '.');
		BNF_OR(P);
			BNF_NON(P, value = parse_expr(P));
			BNF_TERMINAL(P, '\n');
		BNF_OR(P);
			BNF_NON(P, parse_error_line(P));
		BNF_END(P);

		switch(BNF_ACTION(P) ) {
		case 1:
		   	/* matched a '.' so exit */
	   		return;
		case 2:
			/* matched an expression, print the result */
		   	printf("= %d\n", value);
			break;
		case 3:
			/* was an error; */
		        printf("parse error\n");
			break;
		}

		/* we don't need to re-process the previous line, so CUT */
		BNF_CUT(P);
		/* now reset the parser for next time around the loop */
		BNF_RESET(P);
	} while(1);
}

main()
{
	BNF_PARSER *  P;

	P = BNF_CREATE(stdin);
	printf("Welcome to the simple calculator!\n");
	printf("A `.' will exit the program\n");
	parse_lines(P);
	BNF_DESTROY(P);
	var_list = LSTfree(var_list);
	return (exit(EXIT_SUCCESS), 0);
}
