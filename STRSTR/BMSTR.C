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
	bm_strstr -- string search (Boyer-Moore algorithm)

	This is a fast implementation of the algorithm published by Robert
	S. Boyer and J. Strother Moore in CACM Vol. 20 No. 10 (October 1977)
	on pp. 762-772, described in Chapter 19 of Robert Sedgewick's
	"Algorithms".
 */
/* #define	ROBUST	/* sane behavior when invoked with null pointers */
/* #define	DEBUG			/* enable for assertion checking */

#ifndef DEBUG
#define NDEBUG
#endif

#include	<assert.h>
#include	<limits.h>
#include	<stddef.h>
#include	<string.h>

#define EOS		'\0'		/* C string terminator */

typedef const unsigned char	cuc;	/* char variety used in algorithm */

static cuc	*pattern = NULL;	/* search-pattern string */
static size_t	pat_len;		/* length of search pattern */
static size_t	skip[UCHAR_MAX + 1];	/* pattern skip table; [0] unused */
#if	defined(ROBUST) && !defined(DEBUG)
static const char	empty[] = "";
#endif

void
bm_setpat( register const char *p )
	{
	register size_t 	n;	/* length of pattern (mostly) */

	assert(p != NULL);

#if	defined(ROBUST) && !defined(DEBUG)	/* DEBUG passed assert() */
	if ( p == NULL )
		p = empty;		/* assume dummy (empty) pattern */
#endif

	pattern = (cuc *)p;

	/* Precompute pattern length and skip intervals: */

	n = strlen( p );
	pat_len = n;	/* XXX -- see if ORCA/C allows merging yet */

	{
	register unsigned char	c = UCHAR_MAX;

	assert(n == pat_len);

	do
		skip[c] = n;
	while ( --c > 0 );
	}

	assert((cuc *)p == pattern);
	assert(n == pat_len);

	while ( n > 1 )			/* don't include final character */
		skip[(cuc)*p++] = --n;

	assert(pattern != NULL);
	assert(pattern[pat_len] == EOS);
	assert(pat_len == 0 || (cuc *)p == &pattern[pat_len - 1]);

	return;
	}

char *
bm_search( register const char *tx )
	{
	register cuc		*tk;	/* -> tx[] character being tested */
	register size_t		pj;	/* indexes pattern[] ditto */
	register size_t		ti;	/* indexes tx[] anchor location */
	register size_t		d;	/* amount to skip */
	register size_t		n;	/* length of tx[], alas */

	assert(pattern != NULL);	/* else usage error */

	if ( pat_len == 0 )
		return (char *)tx;	/* special case */

	assert(tx != NULL);		/* else usage error */

#if	defined(ROBUST) && !defined(DEBUG)	/* DEBUG passed assert()s */
	if ( tx == NULL || pattern == NULL )
		return NULL;
#endif

	if ( (n = strlen( tx )) < pat_len )
		return NULL;		/* another special case */

	for ( ti = pat_len; ; ti += d )
		{
		/* Try matching pattern from right to left. */

		assert(ti >= pat_len);
		assert(ti <= n);

		for ( pj = pat_len, tk = (cuc *)&tx[ti];
		      *--tk == pattern[--pj];
		    )
			if ( pj == 0 )
				return (char *)tk;	/* all matched */

		assert(tk >= (cuc *)tx);

		/* Amount to advance search depends on mismatched text. */

		if ( (d = skip[(cuc)tx[ti - 1]]) > n - ti )
			return NULL;	/* no match */

		assert(d >= 0);
		assert(d <= pat_len);
		}
	}

void
bm_clrpat( void )
	{
	assert(pattern != NULL);	/* else usage error */

	/* There would be no point in resetting skip[] here. */

#if	defined(ROBUST) || defined(DEBUG)
	pattern = NULL;
#endif
	return;
	}

char *
bm_strstr( const char *tx, const char *p )
	{
	register char	*r;		/* return value */

	bm_setpat( p );
	r = bm_search( tx );
	bm_clrpat();
	return r;
	}
