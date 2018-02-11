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
	ni_strstr -- string search (naive "brute force" algorithm)

	This is a naive implementation of the obvious "brute force" string
	search algorithm, with no particular effort at optimization.
	For comparison with other algorithms, it has been implemented as
	three separate phases (set pattern, search, clear pattern).
 */
/* #define	ROBUST	/* sane behavior when invoked with null pointers */
/* #define	DEBUG			/* enable for assertion checking */

#ifndef DEBUG
#define NDEBUG
#endif

#include	<assert.h>
#include	<stddef.h>
#include	<string.h>

#define EOS	'\0'			/* C string terminator */

static const char	*pattern = NULL;
static size_t		pat_len = 0;
#if	defined(ROBUST) && !defined(DEBUG)
static const char	empty[] = "";
#endif

void
ni_setpat( const char *p )
	{
	assert(p != NULL);

#if	defined(ROBUST) && !defined(DEBUG)	/* DEBUG passed assert() */
	if ( p == NULL )
		p = empty;		/* assume dummy (empty) pattern */
#endif

	pat_len = strlen( pattern = p );
	return;
	}

char *
ni_search( register const char *tx )
	{
	assert(pattern != NULL);	/* else usage error */

	if ( pat_len == 0 )
		return (char *)tx;	/* special case */

	assert(tx != NULL);		/* else usage error */

#if	defined(ROBUST) && !defined(DEBUG)	/* DEBUG passed assert()s */
	if ( tx == NULL || pattern == NULL )
		return NULL;
#endif

	for ( ; *tx != EOS; ++tx )
		if ( strncmp( tx, pattern, pat_len ) == 0 )
			return (char *)tx;	/* match starts here */

	return NULL;			/* no match */
	}

void
ni_clrpat( void )
	{
	assert(pattern != NULL);	/* else usage error */

#if	defined(ROBUST) || defined(DEBUG)
	pattern = NULL;
#endif
	return;
	}

char *
ni_strstr( const char *tx, const char *p )
	{
	register char	*r;		/* return value */

	ni_setpat( p );
	r = ni_search( tx );
	ni_clrpat();
	return r;
	}
