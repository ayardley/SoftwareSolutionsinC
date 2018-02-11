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
	kmp_strstr -- string search (Knuth-Morris-Pratt algorithm)

	This is a fast implementation of the algorithm published by Donald
	E. Knuth, James H. Morris, Jr., and Vaughan R. Pratt in SIAM J.
	Comput. Vol. 6 No. 2 (June 1977) on pp. 323-349, described in
	Chapter 19 of Robert Sedgewick's "Algorithms".

	If not enough dynamic memory is available, it reverts to the "brute
	force" algorithm.  For convenience, this also occurs when the
	pattern string is empty.
 */
/* #define	ROBUST	/* sane behavior when invoked with null pointers */
/* #define	DEBUG			/* enable for assertion checking */

#ifndef DEBUG
#define NDEBUG
#endif

#include	<assert.h>
#include	<stdlib.h>
#include	<string.h>

extern void	bf_setpat( const char * );
extern char	*bf_search( const char * );
extern void	bf_clrpat( void );

#define EOS		'\0'		/* C string terminator */

static const char	*pattern = NULL;	/* search-pattern string */
static size_t		pat_len = 0;	/* length of search pattern */
static size_t		*next = NULL;	/* pattern advancement table */
#if	defined(ROBUST) && !defined(DEBUG)
static const char	empty[] = "";
#endif

void
kmp_setpat( register const char *p )
	{
	register size_t		pi, pj;	/* index p[] (same as pattern[]) */

	assert(p != NULL);

#if	defined(ROBUST) && !defined(DEBUG)	/* DEBUG passed assert() */
	if ( p == NULL )
		p = empty;		/* assume dummy (empty) pattern */
#endif

	pattern = p;

	/* Precompute pattern length and advancement intervals: */

	assert(pattern != NULL);	/* else usage error */
	assert(next == NULL);		/* else usage error */

	if ( (pat_len = strlen( p )) == 0
	  || (next = (size_t *)calloc( pat_len + 2, sizeof(size_t) ))
   		== NULL			/* + 2 to avoid "next[index - 1]" */
	   )	{
/*		assert(pat_len == 0 || next != NULL);	/* flag reversion */

#ifdef	ROBUST
		pat_len = 0;
#ifndef	DEBUG				/* DEBUG already pased assert() */
		next = NULL;
#endif
#endif
		bf_setpat( p );		/* reverting to bf_strstr */
		return;
		}

	assert(pat_len > 0);
	assert(next != NULL);

	for ( pi = 0, pj = 1, next[1] = 0; p[pj - 1] != EOS; )
		{
		assert(pi < pj);
		assert(pj <= pat_len);

   		while ( pi > 0 && p[pi - 1] != p[pj - 1] )
			pi = next[pi];

		if ( p[pi++] == p[pj++] )
			next[pj] = next[pi];
		else
			next[pj] = pi;
		}

	assert(pj - 1 == pat_len);

#ifdef	DEBUG
	assert(next[1] == 0);

	for ( pi = 2; pi <= pat_len; ++pi )
		{
		assert(next[pi] < pat_len);
		}
#endif

	return;
	}

char *
kmp_search( register const char *tx )
	{
	register size_t		pi;	/* indexes pattern[] */
	register char		c;	/* character being tested */
	register const char	*p = pattern;	/* fast copy of "pattern" */
#ifdef	DEBUG
	register const char	*otx = tx;
#endif

	assert(p != NULL);		/* else usage error */

	if ( next == NULL )
		return bf_search( tx );	/* revert to "brute force" search */

	assert(pat_len > 0);		/* else should have reverted */
	assert(tx != NULL);		/* else usage error */

#if	defined(ROBUST) && !defined(DEBUG)	/* DEBUG passed assert()s */
	if ( tx == NULL || p == NULL )
		return NULL;
#endif

	assert(next != NULL);
	assert(p == pattern);

	for ( pi = 1; ; ++pi, ++tx )
		{
		if ( p[pi - 1] == EOS )	/* entire pattern matched */
			{
			assert(pi - 1 == pat_len);
			assert(tx >= otx + pat_len);

			return (char *)tx - pat_len;
			}

		if ( (c = *tx) == EOS )
			return NULL;	/* no match */

		assert(pi <= pat_len);

		while ( pi > 0 && p[pi - 1] != c )
			{
			pi = next[pi];

			assert(pi < pat_len);
			}
		}
	}

void
kmp_clrpat( void )
	{
	assert(pattern != NULL);	/* else usage error */

	if ( next == NULL )
		bf_clrpat();		/* reverted to bf_strstr */
	else	{
		assert(pat_len > 0);	/* else should have reverted */

		free( (void *)next );
		}

#if	defined(ROBUST) || defined(DEBUG)
	pat_len = 0;
	pattern = NULL;
	next = NULL;
#endif
	return;
	}

char *
kmp_strstr( const char *tx, const char *p )
	{
	register char	*r;		/* return value */

	kmp_setpat( p );
	r = kmp_search( tx );
	kmp_clrpat();
	return r;
	}
