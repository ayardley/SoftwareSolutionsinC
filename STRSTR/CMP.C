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
	cmp -- compare two files

	last edit:	30-May-1993	Gwyn@ARL.Army.Mil

	This implementation differs from the UNIX "cmp" utility in that it
	does not support any options nor any way to read one of the files
 	from the standard input.  It is intended as a simple way to verify
  	that two named files contain the same data.
 */

#include	<stdio.h>
#include	<stdlib.h>

int
main( int argc, char *argv[] )
	{
	register FILE	*fa, *fb;	/* streams being compared */
	register int	ca, cb;		/* bytes from respective streams */
	register long	loc;		/* current location in stream */

	if ( argc != 3 )
		{
		fprintf( stderr, "Usage:\tcmp filea fileb\n" );
		exit( EXIT_FAILURE );
		}

	if ( (fa = fopen( argv[1], "rb" )) == NULL )
		{
		fprintf( stderr, "cmp: couldn't open \"%s\"\n", argv[1] );
		exit( EXIT_FAILURE );
		}

	if ( (fb = fopen( argv[2], "rb" )) == NULL )
		{
		fprintf( stderr, "cmp: couldn't open \"%s\"\n", argv[2] );
		exit( EXIT_FAILURE );
		}

	for ( loc = 0; ; ++loc )
 		{
		if ( (ca = getc( fa )) == EOF && ferror( fa ) )
			{
			fprintf( stderr, "cmp: error reading \"%s\"\n",
				 argv[1]
			       );
			exit( EXIT_FAILURE );
			}

		if ( (cb = getc( fb )) == EOF && ferror( fb ) )
			{
			fprintf( stderr, "cmp: error reading \"%s\"\n",
				 argv[2]
			       );
			exit( EXIT_FAILURE );
			}

		if ( ca == EOF && cb == EOF )
			break;		/* done, no differences found */

		if ( ca == EOF )
			{
			fprintf( stderr, "cmp: premature EOF on \"%s\"\n",
				 argv[1]
			       );
			exit( EXIT_FAILURE );
			}

		if ( cb == EOF )
			{
			fprintf( stderr, "cmp: premature EOF on \"%s\"\n",
				 argv[2]
			       );
			exit( EXIT_FAILURE );
			}

		if ( ca != cb )
			{
			fprintf( stderr,
		"cmp: \"%s\", \"%s\" differ at location %ld: 0%o vs. 0%o\n",
				 argv[1], argv[2], loc, ca, cb
			       );
			exit( EXIT_FAILURE );
			}
		}

	return EXIT_SUCCESS;
	}
