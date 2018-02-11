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
	nm_strstr -- string search (non-matching dummy algorithm)

	This is a dummy three-phase string search algorithm that always
	fails to find a match.  It is intended solely for timing overhead
	of the invoking application, i.e. execution of all code except the
	actual search algorithm.  (Warning:  Since the wrong information
	is returned, the application will in general execute differently.
	This difference may affect the measured overhead time.)
 */
/* #define	ROBUST	/* sane behavior when invoked with null pointers */
/* #define	DEBUG			/* enable for assertion checking */

#ifndef DEBUG
#define NDEBUG
#endif

#include	<assert.h>
#include	<stddef.h>

#define EOS	'\0'			/* C string terminator */

static const char	*pattern = NULL;
#if	defined(ROBUST) && !defined(DEBUG)
static const char	empty[] = "";
#endif

void
nm_setpat( const char *p )
	{
	assert(p != NULL);

#if	defined(ROBUST) && !defined(DEBUG)	/* DEBUG passed assert() */
	if ( p == NULL )
		p = empty;		/* assume dummy (empty) pattern */
#endif

	pattern = p;
	return;
	}

char *
nm_search( register const char *tx )
	{
	assert(pattern != NULL);	/* else usage error */
	assert(*pattern == EOS || tx != NULL);	/* else usage error */

	return NULL;			/* never matches */
	}

void
nm_clrpat( void )
	{
	assert(pattern != NULL);	/* else usage error */

#if	defined(ROBUST) || defined(DEBUG)
	pattern = NULL;
#endif
	return;
	}

char *
nm_strstr( const char *tx, const char *p )
	{
	register char	*r;		/* return value */

	nm_setpat( p );
	r = nm_search( tx );
	nm_clrpat();
	return r;
	}
