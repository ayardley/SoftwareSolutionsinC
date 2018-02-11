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

/*
 *	bnf2c -- convert BNF grammar input (.bnf) to parser toolkit source (.c)
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include "str.h"
#include "lst.h"
#include "parser.h"

#define	VERBOSE		1	/* 0 = no output, 1 = echo production names */

#define	SPACE	" \t"
#define	DIGIT	"0123456789"
#define	UPPER	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define	LOWER	"abcdefghijklmnopqrstuvwxyz"
#define	ISTART	UPPER LOWER "_"
#define	IDENT	UPPER LOWER DIGIT "_"

LST	prod_list = (LST)0;

STR
parse_idtail(BNF_PARSER *P)
{
	int c;
	STR id;

	id = (STR)0;
	BNF_BEGIN(P);
		BNF_ONEOF(P, IDENT, c);
		BNF_NON(P, id = parse_idtail(P));
		id = STRpush(id, c);
	BNF_OR(P);
		id = STRinit('\0');
	BNF_END(P);
	return id;
}

STR
parse_ident(BNF_PARSER *P)
{
	int c;
	STR id;

	id = (STR)0;
	BNF_BEGIN(P);
		BNF_ONEOF(P, ISTART, c);
		BNF_NON(P, id = parse_idtail(P));
		id = STRpush(id, c);
	BNF_END(P);
	return id;
}

void
parse_comment(BNF_PARSER *P)
{
	int c;

	BNF_BEGIN(P);
		BNF_TERMINAL(P, '\n');
	BNF_OR(P);
		BNF_ANY(P, c);
		BNF_NON(P, parse_comment(P));
	BNF_END(P);
}

void
parse_optspace(BNF_PARSER *P)
{
	int c;

	BNF_BEGIN(P);
		BNF_TERMINAL(P, '#');
		BNF_NON(P, parse_comment(P));
		BNF_NON(P, parse_optspace(P));
	BNF_OR(P);
		BNF_ONEOF(P, SPACE, c);
		BNF_NON(P, parse_optspace(P));
	BNF_OR(P);
	BNF_END(P);
}

void
parse_newline(BNF_PARSER *P)
{
	BNF_BEGIN(P);
		BNF_NON(P, parse_optspace(P));
		BNF_TERMINAL(P, '\n');
		BNF_NON(P, parse_newline(P));
	BNF_OR(P);
		BNF_NON(P, parse_optspace(P));
		BNF_TERMINAL(P, '\n');
	BNF_END(P);
}

void
parse_optnewline(BNF_PARSER *P)
{
	BNF_BEGIN(P);
		BNF_NON(P, parse_newline(P));
		BNF_NON(P, parse_optspace(P));
	BNF_OR(P);
		BNF_NON(P, parse_optspace(P));
	BNF_END(P);
}

STR
parse_string(BNF_PARSER *P, int delim)
{
	int c;
	STR str;

	str = (STR)0;
	BNF_BEGIN(P);
		BNF_TERMINAL(P, delim);
		str = STRinit('\0');
	BNF_OR(P);
		BNF_TERMINAL(P, '\\');
		BNF_ANY(P, c);
		BNF_NON(P, str = parse_string(P, delim));
		str = STRpush(str, c);
		str = STRpush(str, '\\');
	BNF_OR(P);
		BNF_ANY(P, c);
		BNF_NON(P, str = parse_string(P, delim));
		str = STRpush(str, c);
	BNF_END(P);
	return str;
}

void
parse_token(BNF_PARSER *P)
{
	STR str;

	str = (STR)0;
	BNF_BEGIN(P);
		BNF_TERMINAL(P, '\'');
		BNF_NON(P, str = parse_string(P, '\''));
		printf("\t\tBNF_TERMINAL(P, '%s');\n", str);
	BNF_OR(P);
		BNF_TERMINAL(P, '\"');
		BNF_NON(P, str = parse_string(P, '\"'));
		printf("\t\tBNF_ALLOF(P, \"%s\");\n", str);
	BNF_OR(P);
		BNF_TERMINAL(P, '[');
		BNF_NON(P, str = parse_string(P, ']'));
		printf("\t\tBNF_ONEOF(P, \"%s\", c);\n", str);
	BNF_OR(P);
		BNF_TERMINAL(P, '?');
		printf("\t\tBNF_ANY(P, c);\n");
	BNF_OR(P);
		BNF_NON(P, str = parse_ident(P));
		printf("\t\tBNF_NON(P, parse_%s(P));\n", str);
	BNF_END(P);
	str = STRfree(str);
}

void
parse_alt(BNF_PARSER *P)
{
	STR str;

	str = (STR)0;
	BNF_BEGIN(P);
		BNF_NON(P, parse_token(P));
		BNF_NON(P, parse_optspace(P));
		BNF_NON(P, parse_alt(P));
	BNF_OR(P);
	BNF_END(P);
	str = STRfree(str);
}

void
parse_nextalt(BNF_PARSER *P)
{
	BNF_BEGIN(P);
		BNF_NON(P, parse_optnewline(P));
		BNF_TERMINAL(P, '|');
		printf("\tBNF_OR(P);\n");
		BNF_NON(P, parse_optspace(P));
		BNF_NON(P, parse_alt(P));
		BNF_NON(P, parse_nextalt(P));
	BNF_OR(P);
	BNF_END(P);
}

void
parse_prod(BNF_PARSER *P)
{
	STR ident;

	BNF_BEGIN(P);
		BNF_NON(P, parse_optnewline(P));
		BNF_NON(P, ident = parse_ident(P));
#if VERBOSE
		fprintf(stderr, "%s\n", ident);
#endif
		BNF_NON(P, parse_optspace(P));
		BNF_ALLOF(P, "::=");
		printf("\nvoid\n");
		printf("parse_%s(BNF_PARSER *P)\n{\n", ident);
		printf("\tint c;\n");
		printf("\n");
		printf("\tBNF_BEGIN(P);\n");
		BNF_NON(P, parse_optspace(P));
		BNF_NON(P, parse_alt(P));
		BNF_NON(P, parse_nextalt(P));
		BNF_NON(P, parse_newline(P));
		printf("\tBNF_END(P);\n");
		printf("}\n");
		prod_list = LSTput(prod_list, (void *)ident);
	BNF_END(P);
}

void
parse_grammar(BNF_PARSER *P)
{
	do {
		BNF_RESET(P);
		BNF_BEGIN(P);
			BNF_TERMINAL(P, EOF);
		BNF_OR(P);
			BNF_NON(P, parse_prod(P));
		BNF_END(P);
		BNF_CUT(P);
	} while(BNF_ACTION(P) == 2);
}

main()
{
	BNF_PARSER *P;
	LST lp;
	char *p;

	P = BNF_CREATE(stdin);
	BNF_BEGIN(P);
		printf("/*\n *\tsource file created by bnf2c\n */\n");
		printf("#include <stddef.h>\n");
		printf("#include <stdio.h>\n");
		printf("#include <stdlib.h>\n");
		printf("#include <string.h>\n");
		printf("#include \"str.h\"\n");
		printf("#include \"lst.h\"\n");
		printf("#include \"parser.h\"\n");
		BNF_NON(P, parse_grammar(P));
		printf("\n");
		printf("/* function prototypes (move to top of file) */\n");
		for (lp = prod_list; lp && (p = (STR)(*lp)); ++lp) {
			printf("void\tparse_%s(BNF_PARSER *P);\n", p);
		}
	BNF_END(P);
	BNF_DESTROY(P);
	return (exit(EXIT_SUCCESS), 0);
}
