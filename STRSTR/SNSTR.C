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
	sn_strstr -- string search (slow version of naive algorithm)

	This is a faithful reimplementation of the incredibly slow
	algorithm used in SunOS Release 4.1.1, apparently originally
	obtained from 4.1BSD.

	I have called it "strstr" so you can simply link it into the test
	program "repeats" to emulate the situation found by default on
	SunOS 4.1.1.
 */

#include	<stddef.h>
#include	<string.h>

char *
strstr( register const char *tx, register const char *p )
	{
	size_t	pat_len = strlen( p );

	if ( pat_len == 0 )
		return (char *)tx;

	for ( ; strlen( tx ) >= pat_len; ++tx )	/* SLOW! */
		if ( strncmp( tx, p, pat_len ) == 0 )
			return (char *)tx;	/* match */

	return NULL;			/* no match */
	}
