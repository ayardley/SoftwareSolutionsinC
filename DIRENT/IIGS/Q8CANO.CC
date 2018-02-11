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
	_Q8canon() -- convert POSIX pathname to canonical form

	public-domain implementation for GS/OS, ORCA/C 2.0

	last edit:	16-Jan-1993	Gwyn@ARL.Army.Mil

	This module provides support for pathname operations in GPLib.

	prototype:	int _Q8canon( char *buf, int buf_size );

	_Q8canon() replaces the null-terminated string pointed to by buf,
	assumed to denote any POSIX-style pathname, with an equivalent
	"canonicalized" absolute pathname suitable for direct use with
	operating system service calls; the length of the resulting string
	shall not exceed buf_size characters (including the terminating null
	character).  _Q8canon() returns 0 iff it fails (in which case the
	contents of the string pointed to by buf are now meaningless).

	Canonicalization consists of interpreting "." and ".." path
	components according to POSIX conventions and removal of redundant
	path components.

	The GS/OS version treats ':' characters as equivalent to '/', in
	order to accommodate user expectations in that environment; the
	canonical form for GS/OS uses ':' for path delimiters.	GS/OS prefix
	8 is taken to be the "current working directory", if that is needed.
 	POSIX device name "/dev/xxx" is mapped to GS/OS ".XXX".
 */

#pragma	lint	-1
#pragma	noroot

#include	<ctype.h>
#include	<errno.h>
#include	<stdlib.h>
#include	<string.h>

#include	"Q8.h"

/*
	_Q8left() and _Q8right() are specialized versions of strcpy()
	for use when the target and destination overlap.
 */

static void
_Q8left( register char *to, register const char *from )
	{
	while ( (*to++ = *from++) != '\0' )
		;
	}

static void
_Q8right( register char *to, register const char *from )
	{
	register char	*start = from;

	do
		++to;
	while ( *from++ != '\0' );

	while ( from > start )
		*--to = *--from;
	}

int
_Q8canon( char *buf, int buf_size )
	{
	enum	{ misc, sl, sldot, sldotdot }	state;
	register char	*p = buf;

	if ( p == 0 )			/* safety net */
		{
		errno = EINVAL;
		return 0;
		}

	/* Convert initial GS/OS path delimiter to POSIX path delimiter: */
	if ( *p == ':' )
		*p = '/';

	if ( *p == '/' )
		++p;			/* /abspath */
	else	{			/* relative path, supply prefix */
		register int	pfxlen;
		register char	*pfx = _Q8getcwd( (char *)0, 0 );
					/* XPG2 conformance required! */
		/* getcwd assumed to return canonical POSIX absolute path */

		if ( pfx == 0 )
			return 0;	/* errno set by getcwd() */

		pfxlen = strlen( pfx );

		if ( pfxlen + strlen( p ) + 2 > buf_size )
			{
			free( (void *)pfx );
			errno = ERANGE;	/* result would overflow buffer */
			return 0;
			}

		_Q8right( p + pfxlen + 1, p );
		(void)strcpy( p, pfx );
		p += pfxlen;
		*p++ = '/';		/* cwd/relpath */
		free( (void *)pfx );
		}

	/* Finite state automaton to reduce user path to canonical form: */

	for ( state = sl; *p != '\0'; ++p )
		{
		/* Convert GS/OS path delimiter to POSIX path delimiter: */
		if ( *p == ':' )
			*p = '/';

		switch ( state )
			{
		case misc:
			if ( *p == '/' )
				state = sl;
			/* else still in state "misc" */

			break;

		case sl:
			if ( *p == '.')
				state = sldot;
			else if ( *p == '/' )
				{	/* reduce // to / */
				_Q8left( p, p + 1 );
				--p;
				}	/* still in state "sl" */
			else
				state = misc;

			break;

		case sldot:
			if ( *p == '.' )
				state = sldotdot;
			else if ( *p == '/' )
				{	/* reduce /./ to / */
				_Q8left( p - 1, p + 1 );
				p -= 2;
				state = sl;
				}
			else
				state = misc;

			break;

		case sldotdot:
			if ( *p == '/' )
				{	/* look for preceeding delimiter */
				register char	*q;

				for ( q = p - 3; q > buf && *--q != '/'; )
					;

				_Q8left( q + 1, p + 1 );
				p = q;
				state = sl;
				}
			else		/* includes '.' */
				state = misc;

			break;
			}
		}

	switch ( state )
		{
	case misc:
		break;

	case sl:
		--p;
		break;

	case sldot:
		p -= 2;
		break;

	case sldotdot:
		/* look for preceeding delimiter */

		for ( p -= 3; p > buf && *--p != '/'; )
			;

		break;
		}

	if ( p == buf )
		++p;			/* "/..", "/." taken as root dir. */

	*p = '\0';			/* may trim trailing / etc. */

	/* Convert POSIX path delimiters to GS/OS path delimiters: */
	for ( p = buf; *p != '\0'; ++p )
		if ( *p == '/' )
			*p = ':';

	/* Convert POSIX device names to GS/OS device names: */
 	if ( strncmp( buf, ":dev:", 5 ) == 0 )
		{
		buf[0] = '.';
		_Q8left( buf + 1, buf + 5 );

		for ( p = buf + 1; *p != '\0'; ++p )
			if ( islower( *p ) )
				*p = toupper( *p );
		}

	return 1;			/* success */
	}
