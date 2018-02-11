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
 *	testrig.c -- A simple test rig for STR and LST functions.
 */
#include <stdio.h>
#include "lst.h"
#include "str.h"

#ifdef TEST
void	STR__test(void);
void	LST__test(void);
#endif /* TEST */

int
main(argc, argv)
int argc;
char **argv;
{
#ifdef TEST
	STR__test();
	LST__test();
#else /* TEST */
	puts("Compile with -DTEST to enable testing code.");
#endif /* TEST */
	return 0;
}

