/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by D'Arcy J.M. Cain.  Not derived from licensed software.
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
cash.c
Written by D'Arcy J.M. Cain

Functions to allow input and output of money normally but store
and handle it as longs

Set tabstops to 4 for best results

*/

#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<locale.h>
#include	"cash.h"

#define		CASH_BUFSZ	24
#define		TERMINATOR	(CASH_BUFSZ - 1)
#define		LAST_PAREN	(TERMINATOR - 1)
#define		LAST_DIGIT	(LAST_PAREN - 1)

/* function to convert a long to a dollars and cents representation */
const char *
cash(long value, int parens, int points, int comma)
{
	/* this is a cyclic buffer allowing the function to be called */
	/* 16 times before returning the same space */
	static char		buf[16][CASH_BUFSZ];
	static int		current_buffer = 0;
	struct lconv	*lc = localeconv();
	int				mod_group = *lc->mon_grouping;
	int				minus = 0;
	int				count = LAST_DIGIT;
	int				point_pos = LAST_DIGIT - points;
	int				comma_position = 0;

	/* see if we use locale default */
	if (comma == -1)
		comma = *lc->thousands_sep;

	if (!mod_group)
		mod_group = 3;

	/* figure out which buffer to use next */
	current_buffer = (current_buffer + 1) & 0x0f;

	/* allow more than three decimal points and separate them */
	if (comma)
	{
		point_pos -= (points - 1)/mod_group;
		comma_position = point_pos % (mod_group + 1);
	}

	/* we work with positive amounts and add the minus sign at the end */
	if (value < 0)
	{
		minus = 1;
		value *= -1;
	}

	memset(buf[current_buffer], ' ', sizeof(buf[0]));
	buf[current_buffer][TERMINATOR] = 0;

	while (value || count > point_pos - 2)
	{
		if (points && count == point_pos)
			buf[current_buffer][count--] = *lc->decimal_point;
		else if (comma && count % (mod_group + 1) == comma_position)
			buf[current_buffer][count--] = comma;

		buf[current_buffer][count--] = (value % 10) + '0';
		value /= 10;
	}

	/* see if we need to signify negative amount */
	if (minus)
	{
		if (parens)
		{
			buf[current_buffer][count--] = '(';
			buf[current_buffer][LAST_PAREN] = ')';
		}
		else
			buf[current_buffer][count--] = '-';
	}

	/* dollar sign can be ignored by adding one to return value */
	buf[current_buffer][count] = '$';

	if (buf[current_buffer][LAST_DIGIT] == ',')
	{
		buf[current_buffer][LAST_DIGIT] = buf[current_buffer][LAST_PAREN];
		buf[current_buffer][LAST_PAREN] = ' ';
	}

	return(buf[current_buffer] + count);
}

/* convert a string to a long integer */
long
atocash(const char *s, int fpoint)
{
	long			value = 0;
	long			dec = 0;
	long			sgn = 1;
	int				seen_dot = 0;
	struct lconv	*lc = localeconv();

	/* strip all leading whitespace and any leading dollar sign */
	while (isspace(*s) || *s == '$')
		s++;

	/* a leading minus or paren signifies a negative number */
	if (*s == '-' || *s == '(')
	{
		sgn = -1;
		s++;
	}
	else if (*s == '+')
		s++;

	for (; ; s++)
	{
		/* we look for digits as long as we have less */
		/* than the required number of decimal places */
		if (isdigit(*s) && dec < fpoint)
		{
			value = (value * 10) + *s - '0';

			if (seen_dot)
				dec++;
		}
		else if (*s == *lc->decimal_point && !seen_dot)
			seen_dot = 1;
		else
		{
			/* round off */
			if (isdigit(*s) && *s >= '5')
				value++;

			/* adjust for less than required decimal places */
			for (; dec < fpoint; dec++)
				value *= 10;

			return(value * sgn);
		}
	}
}


/* used by cash_words() below */
static const char *
num_word(int value)
{
	static char	buf[128];
	static const char	*small[] = {
		"zero", "one", "two", "three", "four", "five", "six", "seven",
		"eight", "nine", "ten", "eleven", "twelve", "thirteen", "fourteen",
		"fifteen", "sixteen", "seventeen", "eighteen", "nineteen", "twenty",
		"thirty", "fourty", "fifty", "sixty", "seventy", "eighty", "ninety"
	};
	const char	**big = small + 18;
	int			tu = value % 100;

	/* deal with the simple cases first */
	if (value <= 20)
		return(small[value]);

	/* is it an even multiple of 100? */
	if (!tu)
	{
		sprintf(buf, "%s hundred", small[value/100]);
		return(buf);
	}

	/* more than 99? */
	if (value > 99)
	{
		/* is it an even multiple of 10 other than 10? */
		if (value % 10 == 0 && tu > 10)
			sprintf(buf, "%s hundred %s",
				small[value/100], big[tu/10]);
		else if (tu < 20)
			sprintf(buf, "%s hundred and %s",
				small[value/100], small[tu]);
		else
			sprintf(buf, "%s hundred %s %s",
				small[value/100], big[tu/10], small[tu % 10]);
	}
	else
	{
		/* is it an even multiple of 10 other than 10? */
		if (value % 10 == 0 && tu > 10)
			sprintf(buf, "%s", big[tu/10]);
		else if (tu < 20)
			sprintf(buf, "%s", small[tu]);
		else
			sprintf(buf, "%s %s", big[tu/10], small[tu % 10]);
	}

	return(buf);
}

/* this converts a long as well but to a representation using words */
const char *
cash_words(long value)
{
	static char	buf[128];
	char	*p = buf;
	long	m0;
	long	m1;
	long	m2;
	long	m3;

	/* work with positive numbers */
	if (value < 0)
	{
		value *= -1;
		strcpy(buf, "minus ");
		p += 6;
	}
	else
		*buf = 0;

	m0 = value % 100;				/* cents */
	m1 = (value/100) % 1000;		/* hundreds */
	m2 = (value/100000) % 1000;		/* thousands */
	m3 = value/100000000 % 1000;	/* millions */

	if (m3)
	{
		strcat(buf, num_word(m3));
		strcat(buf, " million ");
	}

	if (m2)
	{
		strcat(buf, num_word(m2));
		strcat(buf, " thousand ");
	}

	if (m1)
		strcat(buf, num_word(m1));

	if (!*p)
		strcat(buf, "zero");

	strcat(buf, (int)(value/100) == 1 ? " dollar and " : " dollars and ");
	strcat(buf, num_word(m0));
	strcat(buf, m0 == 1 ? " cent" : " cents");
	*buf = toupper(*buf);
	return(buf);
}

#ifdef	TEST_MAIN
#include	<stdlib.h>
#include	"../str_lst/str.h"

STR
STRgets(FILE *f)		/* read a line of input from ``f'' */
{
	static STR str = NULL;
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

int
main(void)
{
	char	*p, *q;
	long	v;			/* the long value representing the amount */
	int		d;			/* number of decimal places - default 2 */

	while ((p = STRgets(stdin)) != NULL)
	{
		for (q = p; *q && !isspace(*q); q++)
			;

		v = atocash(p, 2);
		d = *q ? atoi(q) : 2;

		printf("%12.12s %10ld ", p, v);
		printf("%12s %s\n", cash(v, 0, d, ','), cash_words(v));
		STRfree(p);
	}

	return(0);
}
#endif	/* TEST_MAIN */
