/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Dennis Jelcic.  Not derived from licensed software.
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
 *	str.h -- definitions for dynamic string data structure
 */
#ifndef STR_H
#define	STR_H

#include <stddef.h>		/* ...for size_t */

typedef char *STR;

STR	STRinit(int c, ... /*, '\0' */);
size_t	STRlen(STR str);
STR	STRcopy(char *s);
STR	STRadd(STR str, STR cur, int c);
STR	STRconc(STR str, ... /*, (char *)0 */);
int	STRrem(STR str, STR cur);
STR	STRnorm(STR str);
STR	STRfree(STR str);

#define STRpush(str,c)		STRadd(str, str, c)
#define STRpop(str)		STRrem(str, str)
#define STRput(str,c)		STRadd(str, (STR)0, c)

#endif /* STR_H */
