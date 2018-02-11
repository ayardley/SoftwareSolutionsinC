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
	listdir -- list directory (portable method)

	last edit:	16-Aug-1993	Gwyn@ARL.Army.Mil
*/

#include	<stdio.h>
#include	<stdlib.h>

/* Assume that <sys/types.h> doesn't need to be included here. */
#include	<dirent.h>	/* or "mydent.h", if necessary */

int
main( int argc, char *argv[] )
	{
	register DIR		*dirp;
	register struct dirent	*dp;

	if ( (dirp = opendir( argv[1] )) == NULL )
		{
		(void)fprintf( stderr, "%s: can't open\n", argv[1] );
		return EXIT_FAILURE;
		}

	while ( (dp = readdir( dirp )) != NULL )
		(void)printf( "%s\n", dp->d_name );

	(void)closedir( dirp );
	return EXIT_SUCCESS;
	}
