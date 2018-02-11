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
	rk_strstr -- string search (Rabin-Karp algorithm)

	This is a fast implementation of the algorithm devised by Richard M.
	Karp and Michael O. Rabin in Aiken Computation Lab. TR-31-81 (1981),
	described in Chapter 19 of Robert Sedgewick's "Algorithms".
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

#define	d	(UCHAR_MAX + 1)		/* size of character set */

#if	UCHAR_MAX == 255
#define	q	8388593ul		/* large nonoverflowing prime */
#endif
/*
	XXX -- For other values of UCHAR_MAX, define suitable q; namely,
	as large a prime as you can such that (d+1)*q is representable as
	an unsigned long.
*/

typedef const unsigned char	cuc;	/* char variety used in algorithm */

static cuc	*pattern = NULL;	/* search-pattern string */
static size_t	pat_len;		/* length of search pattern */
static unsigned long	pat_hash;	/* hash value for pattern */
static unsigned long	dm;		/* d ^ (pat_len - 1) */
#if	defined(ROBUST) && !defined(DEBUG)
static const char	empty[] = "";
#endif

void
rk_setpat( register const char *p )
	{
	register size_t 	m;	/* length of pattern */
	register unsigned long	h;	/* accumulator */

	assert(p != NULL);

#if	defined(ROBUST) && !defined(DEBUG)	/* DEBUG passed assert() */
	if ( p == NULL )
		p = empty;		/* assume dummy (empty) pattern */
#endif

	pattern = (cuc *)p;

	/* Precompute hash value for the pattern;
	   the pattern length is determined as a side effect: */

	for ( m = 0, h = 0; *p != EOS; ++m )
		h = (h * d + (cuc)*p++) % q;

	pat_len = m;			/* length of pattern */
	pat_hash = h;			/* pattern hash value */

	/* Precompute d ^ (pat_len - 1): */

	assert(m == pat_len);

	for ( h = 1; m > 1; --m )
		h = (h * d) % q;

	dm = h;				/* d ^ (pat_len - 1) */

	assert(pattern != NULL);
	assert(pattern[pat_len] == EOS);
	assert(dm >= 1);

	return;
	}

char *
rk_search( register const char *tx )
	{
	register const char	*ti;	/* -> text character being tested */
	register unsigned long	h;	/* accumulator for tx hash value */
	register size_t 	m;	/* length of pattern */

	assert(pattern != NULL);	/* else usage error */

	if ( pat_len == 0 )
		return (char *)tx;	/* special case */

	assert(tx != NULL);		/* else usage error */

#if	defined(ROBUST) && !defined(DEBUG)	/* DEBUG passed assert()s */
	if ( tx == NULL || pattern == NULL )
		return NULL;
#endif

	/* Compute hash value for the first pat_len characters of tx[]: */

	for ( h = 0, ti = tx, m = 0; m < pat_len; ++ti, ++m )
		{
		if ( *ti == EOS )
			return NULL;	/* text shorter than pattern */

		h = (h * d + (cuc)*ti) % q;
		}

	/* Try to find the pattern in the text string: */

	assert(m == pat_len);

	for ( ti = tx; ; ++ti )
		{
		if ( h == pat_hash	/* probable match */
		  && strncmp( ti, (const char *)pattern, m ) == 0
		   )			/* confirmed */
			return (char *)ti;

		if ( ti[m] == EOS )
			return NULL;	/* no match */

		h = (h + d * q - dm * (cuc)*ti) % q;
		h = (h * d + (cuc)ti[m]) % q;
		}
	}

void
rk_clrpat( void )
	{
	assert(pattern != NULL);	/* else usage error */

#if	defined(ROBUST) || defined(DEBUG)
	pattern = NULL;
#endif
	return;
	}

char *
rk_strstr( const char *tx, const char *p )
	{
	register char	*r;		/* return value */

	rk_setpat( p );
	r = rk_search( tx );
	rk_clrpat();
	return r;
	}
