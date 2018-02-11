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
 *	eval.h -- expression parser/evaluator definitions
 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include "../str_lst/lst.h"
#include "../str_lst/str.h"

typedef unsigned char	BYTE;
typedef double		REAL;
typedef int		(*FNPTR)(void);

typedef struct op {
	char		id;
	BYTE		attrib;
#define	OP_RIGHT	(0x80)		/* bit 7 */
#define	OP_UNARY	(0x40)		/* bit 6 */
#define	OP_PREC		(0x0F)		/* bits 3..0 */
	FNPTR		fn;
} OP;

typedef struct func {
	char *		name;
	FNPTR		fn;
} FUNC;

typedef struct var {
	char *		name;
	REAL		value;
} VAR;

typedef union generic {
	REAL		num;
	OP *		op;
	FUNC *		func;
	VAR *		var;
} GENERIC;

typedef enum tk_type {
	TK_ERROR,
	TK_CONST,
	TK_OP,
	TK_FUNC,
	TK_VAR,
	TK_OPEN,
	TK_SEP,
	TK_CLOSE
} TK_TYPE;

typedef struct token {
	TK_TYPE		type;
	GENERIC		value;
} TOKEN;

#define	NEW(type)		((type *)malloc(sizeof(type)))
#define	STREQ(p,q)		(((*p)==(*q)) && (strcmp((p),(q))==0))

/* expr.c */
STR		prompt(char *msg);
int		main(int argc, char **argv);

/* eval.c */
int		pop_value(REAL *n_p);
int		pop_var(VAR **var_p);
int		push_value(REAL n);
int		push_var(VAR *var_p);
int		eval(REAL *n_p, LST expr);

/* var.c */
VAR *		var_lookup(char *name);

/* func.c */
FUNC *		func_lookup(char *name);

/* op.c */
OP *		op_lookup(int id);
OP *		op_alt_lookup(OP *op);

/* parser.c */
LST		parse(char *expr);

/* lexer.c */
TOKEN *		free_token(TOKEN *tk);
void		set_input(char *s);
TOKEN *		lex(void);

/* util.c */
REAL		Cbrt(REAL x);
REAL		XtoI(REAL x, int i);
REAL		TenToI(int i);
STR		STRgets(FILE *f);

