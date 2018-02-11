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
	qs_strstr -- string search (D. M. Sunday's "quick search" algorithm)

	This is an original implementation based on an idea by Daniel M.
	Sunday, essentially the "quick search" algorithm described in CACM
	Vol. 33 No. 8 (August 1990) on pp. 132-142.  Unlike Sunday's
	implementation, this one does not wander past the ends of the
	strings (which can cause malfunctions under certain circumstances),
	nor does it require the length of the searched text to be
	determined in advance.  There are numerous other subtle
	improvements too, several contributed by other Internet citizens.

	If your system has minimal function-call overhead, or if your
	compiler expands invocations of memset() to in-line code, then you
	can define ZAP to enable a heuristic that improves the average
	speed of qs_clrpat().
 */
/* #define	ZAP	/* use memset() to quickly zero the shift[] array */
/* #define	ROBUST	/* sane behavior when invoked with null pointers */
/* #define	DEBUG			/* enable for assertion checking */

#ifndef DEBUG
#define NDEBUG
#endif

#include	<assert.h>
#include	<limits.h>
#include	<stddef.h>
#ifdef ZAP
#include	<string.h>
#endif

typedef const unsigned char	cuc;	/* char variety used in algorithm */

#define EOS		'\0'		/* C string terminator */

static cuc	*pattern = NULL;	/* search-pattern string */
static size_t	pat_len;		/* length of search pattern */
static size_t	shift[UCHAR_MAX + 1] = { 1 };	/* pattern shift table */
#if	defined(ROBUST) && !defined(DEBUG)
static const char	empty[] = "";
#endif

void
qs_setpat( register const char *p )
	{
	register size_t 	m = 0;	/* length of pattern */
#ifdef	DEBUG
	register unsigned char	c;
#endif

	assert(p != NULL);

#if	defined(ROBUST) && !defined(DEBUG)	/* DEBUG passed assert() */
	if ( p == NULL )
		p = empty;		/* assume dummy (empty) pattern */
#endif

	pattern = (cuc *)p;

	/* Precompute shift intervals based on the pattern;
	   the pattern length is determined as a side effect: */

#ifdef	DEBUG
	c = UCHAR_MAX;

	do
		assert(shift[c] == 0);
	while ( --c > 0 );
#endif

	while ( *p != EOS )
		shift[(cuc)*p++] = ++m;

	pat_len = m;			/* length of pattern */

	assert(pattern != NULL);
	assert(pattern[pat_len] == EOS);
	assert(shift[EOS] == 1);	/* important detail! */

	return;
	}

char *
qs_search( register const char *tx )
	{
	register cuc		*t;	/* -> text character being tested */
	register cuc		*p;	/* -> pattern char being tested */
	register cuc		*top;	/* -> high water mark in text */
	register size_t 	m ;	/* length of pattern */
	register size_t 	m1;	/* length of pattern + 1 */

	assert(pattern != NULL);	/* else usage error */

	if ( (m = pat_len) == 0 )
		return (char *)tx;	/* special case */

	assert(tx != NULL);		/* else usage error */

#if	defined(ROBUST) && !defined(DEBUG)	/* DEBUG passed assert()s */
	if ( tx == NULL || pattern == NULL )
		return NULL;
#endif

	/* Try to find the pattern in the text string: */

	for ( m1 = m + 1, top = (cuc *)tx; ; tx += m1 - shift[*(top = t)] )
		{
		for ( t = (cuc *)tx, p = pattern; ; ++t, ++p )
			{
			if ( *p == EOS )       /* entire pattern matched */
				return (char *)tx;

			if ( *p != *t )
				break;
			}

		if ( t < top )
			t = top;	/* already scanned here for EOS */

		p = (cuc *)tx + m;

		do	{
			assert(m > 0);
			assert(t < p);

			if ( *t == EOS )
				return NULL;	/* no match */
			}
		while ( ++t != p );	/* < */
		}
	}

void
qs_clrpat( void )
	{
	register cuc	*p;		/* -> pattern string */

	assert(pattern != NULL);	/* else usage error */

#ifdef	ZAP				/* heuristic: */
	if ( pat_len >= (size_t)UCHAR_MAX / sizeof(int) )
		memset( (void *)&shift[1], 0, UCHAR_MAX * sizeof(size_t) );
	else
#endif
		/* Reset only the non-zero array elements. (optimization) */

		for ( p = pattern; *p != EOS; ++p )
			shift[*p] = 0;

#if	defined(ROBUST) || defined(DEBUG)
	pattern = NULL;
#endif
	return;
	}

char *
qs_strstr( const char *tx, const char *p )
	{
	register char	*r;		/* return value */

	qs_setpat( p );
	r = qs_search( tx );
	qs_clrpat();
	return r;
	}
