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
	bf_strstr -- string search (fine-tuned "brute force" algorithm)

	This is the obvious "brute force" string search algorithm,
	implemented with care to make it as fast as possible (on average).
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
bf_setpat( const char *p )
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
bf_search( register const char *tx )
	{
	register const char	*p = pattern;
	register const char	*t;

	assert(p != NULL);		/* else usage error */

	if ( *p == EOS )
		return (char *)tx;	/* special case */

	assert(tx != NULL);		/* else usage error */

#if	defined(ROBUST) && !defined(DEBUG)	/* DEBUG passed assert()s */
	if ( tx == NULL || p == NULL )
		return NULL;
#endif

	assert(p == pattern);

	for ( ; *tx != EOS; ++tx )
		{
		assert(p == pattern);

		if ( *p == *tx )	/* quick initial-match test */
			{
			assert(*p != EOS);
			assert(*tx != EOS);

			/* Although it is tempting, we cannot omit the test
			   for EOS in the loop, because the match may occur
			   at the very end of tx[], in which case we'd keep
			   on going past the end of both arrays.
			*/
			for ( t = tx; *++p == *++t; )
				if ( *p == EOS )	/* match at end */
					return (char *)tx;

			if ( *p == EOS )
				return (char *)tx;	/* other match */

			p = pattern;	/* reset for next iteration */
			}

		assert(p == pattern);
		}

	/* No match (we know that the original pattern isn't empty). */

	return NULL;
	}

void
bf_clrpat( void )
	{
	assert(pattern != NULL);	/* else usage error */

#if	defined(ROBUST) || defined(DEBUG)
	pattern = NULL;
#endif
	return;
	}

char *
bf_strstr( const char *tx, const char *p )
	{
	register char	*r;		/* return value */

	bf_setpat( p );
	r = bf_search( tx );
	bf_clrpat();
	return r;
	}
