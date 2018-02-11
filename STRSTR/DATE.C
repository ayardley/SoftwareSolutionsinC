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
	date -- print current time on stdout

	last edit:	28-May-1993	Gwyn@ARL.Army.Mil

	This implementation differs from the UNIX "date" utility in that it
	does not support resetting the system time, nor does it provide any
	means of changing its output format.  It is intended as a simple
	way to obtain "time stamps" for measuring program execution times.
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<time.h>

int
main( int argc, char *argv[] )
	{
	time_t	t;

	if ( argc != 1 )
		{
		fprintf( stderr, "date: arguments not supported\n" );
		exit( EXIT_FAILURE );
		}

	time( &t );
	fputs( ctime( &t ), stdout );

	return EXIT_SUCCESS;
	}
