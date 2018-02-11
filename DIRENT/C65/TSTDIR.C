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
	testdir -- basic test for C library directory access routines

	last edit:	30-Apr-1993	Gwyn@ARL.Army.Mil
*/

#include	<sys/types.h>
#include	<dirent.h>
#include	<errno.h>
#include	<stdio.h>

extern int	errno;			/* should be in <errno.h> */

#ifndef	EXIT_FAILURE
#define	EXIT_FAILURE	(-1)		/* works on most systems */
#endif

extern void	exit(), perror();
extern int	strcmp();

main( argc, argv )
	int			argc;
	register char		*argv[];
	{
	register DIR		*dirp;
	register struct dirent	*dp;
	int			status = 0;	/* EXIT_SUCCESS */

	if ( --argc < 2 )
		{
		(void)fprintf( stderr, "Usage: testdir directory entry ...\n" );
		exit( EXIT_FAILURE );
		}

	if ( (dirp = opendir( ++argv )) == NULL )
		{
		perror( "Cannot open specified directory" );
		exit( EXIT_FAILURE );
		}

	while ( --argc > 0 )
		{
		++argv;

		for ( errno = 0; (dp = readdir( dirp )) != NULL; errno = 0 )
			if ( strcmp( dp->d_name, argv[0] ) == 0 )
				{
				(void)printf( "\"%s\" found.\n", argv[0] );
				break;
				}

		if ( dp == NULL )
			{
			(void)printf( "\"%s\" not found.\n", argv[0] );

			if ( errno == 0 )
				{
				(void)fprintf( stderr,
					       "readdir() failed to set errno"
					     );
				status = EXIT_FAILURE;
				}
			else if ( errno != ENOENT )
				{
				perror( "readdir() failed" );
				status = EXIT_FAILURE;
				}
			}

		rewinddir( dirp );
		/* not allowed to test errno here, alas */
		}

	if ( closedir( dirp ) < 0 )
		{
		perror( "closedir() failed" );
		exit( EXIT_FAILURE );
		}

	return status;
	}
