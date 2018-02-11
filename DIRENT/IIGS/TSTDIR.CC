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
	TestDir -- basic test for C library directory access routines

	public-domain implementation for Standard C, POSIX

	last edit:	13-feb-1993	Gwyn@ARL.Army.Mil

	complies with the following standards:
		POSIX	SVID	XPG	AES

	typical usage:	TestDir . existent-file1 nonexistent-file2 ...

	If telldir() is not supported (seekdir() and telldir() are required
	for SVID/XPG/AES conformance, but not for IEEE Std 1003.1-1990),
	compile with _XOPEN_SOURCE undefined.
 */
#define _XOPEN_SOURCE			/* enable if telldir() exists */
#define	LOCAL	/* use current directory's headers, not installed headers */
/* #define DEBUG	/* prints information about each readdir() call */

#ifdef	__ORCAC__
#pragma	lint	-1
#endif

#ifdef	LOCAL
#include	"errno.h"
#else
#include	<errno.h>
#endif
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#ifdef	LOCAL
#include	"sys.types.h"		/* req'd by IEEE Std 1003.1-1990 */
#include	"dirent.h"		/* interface being tested */
#else
#include	<sys/types.h>		/* req'd by IEEE Std 1003.1-1990 */
#include	<dirent.h>		/* interface being tested */
#endif

int
main( int argc, char *argv[] )
	{
	register DIR		*dirp;
	register struct dirent	*dp;
	register char		*dirname;
	long			loc;
	int			status = EXIT_SUCCESS;	/* exit status */

	if ( --argc < 2 )
		{
		(void)fprintf( stderr, "Usage: TestDir dir file ...\n" );
		exit( EXIT_FAILURE );
		}

	if ( (dirp = opendir( dirname = *++argv )) == NULL )
		{
		(void)fprintf( stderr, "Can't open \"%s\"\n", dirname );
		exit( EXIT_FAILURE );
		}

	while ( --argc > 0 )
		{
		++argv;

		while (
#ifdef	_XOPEN_SOURCE
			loc = telldir( dirp ),
#endif
			errno = 0, (dp = readdir( dirp )) != NULL
		      )
			if ( strcmp( dp->d_name, *argv ) == 0 )
				{
				(void)printf( "\"%s\" found", *argv );
#ifdef	_XOPEN_SOURCE
				(void)printf( " at %ld", loc );
#endif
				(void)printf( ".\n" );
				break;
				}
#ifdef	DEBUG			/* watch readdir operation in detail: */
			else	{
				(void)printf( "\"%s\" no match: \"%s\"",
					      *argv, dp->d_name
					    );
#ifdef	_XOPEN_SOURCE
				(void)printf( " at %ld", loc );
#endif
				(void)printf( ".\n" );
				}
#endif	/* DEBUG */

		if ( dp == NULL )
			{
			(void)printf( "\"%s\" not found in \"%s\".\n",
				      *argv, dirname
				    );
			status = EXIT_FAILURE;
			}

		rewinddir( dirp );
		}

	(void)closedir( dirp );
	return status;
	}
