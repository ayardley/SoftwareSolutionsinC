/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Douglas A. Gwyn.  Not derived from licensed software.
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
	sc_strstr -- string search (using system C library strstr())

	I include this source just for completeness; the "repeat" utility
	actually invokes strstr() in-line instead, to eliminate the extra
	function call overhead.
 */

#include <string.h>

static const char *pat;

void
sc_setpat(const char *p)
{
	pat = p;
	return;
}

char *
sc_search(const char *tx)
{
	return strstr(tx, pat);
}

void
sc_clrpat(void)
{
	return;
}

char *
sc_strstr(const char *tx, const char *p)
{
	char *r;

	sc_setpat(p);		/* initialize this pattern */
	r = sc_search(tx);	/* search the text */
	sc_clrpat();		/* clean up if necessary */
	return r;
}
