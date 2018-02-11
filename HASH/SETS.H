/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Tye McQueen.  Not derived from licensed software.
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

/* sets.h -- Macros to implement simple sets using bit arrays */

#ifndef INCLUDED_SETS_H	/* In case this file is #include'd more than once */
#define INCLUDED_SETS_H

#include <limits.h>	/* Define CHAR_BIT */

#ifndef CHAR_BIT
# define   CHAR_BIT	8
#endif /* CHAR_BIT */

/* Data type of basic clump of bits: */
#define   SetType	unsigned char

/* For internal use only: */
#define   SetTSiz	sizeof(SetType)		/* For now, always 1 */
#define   SetTBits	(CHAR_BIT*SetTSiz)
#define   Set1Bit(N)	( 1 << ((N)%SetTBits) )

/* Declare your set of N bits via:  "SetType set[SetSize(N)];" */
#define   SetSize(nelements)	( ((nelements)-1) / SetTBits + 1 )

/* Or allocate and free on the fly: "SetType *set= SetAlloc(N);" */
#define   SetAlloc(nelms)	( (SetType *) malloc(SetSize(nelms)*SetTSiz) )
#define   SetFree(set)          free(set)

/* Initialize a set to contain no members: */
#define   SetEmpty(nelms,set)	memset( set,  0, SetSize(nelms)*SetTSiz )

/* Initialize a set to contain all members: */
#define   SetFill(nelms,set)	memset( set, ~0, SetSize(nelms)*SetTSiz )

/* For the following functions:
 *	Out-of-range values for `N' are not trapped.
 *	`N' appears more than once so don't use expressions with side-effects.
 *	The above also applies for `set' in the SetAssign macro. */

/* Is `N' a member of `set'? */
#define   SetMember(N,set)	( (set)[(N)/SetTBits] &   Set1Bit(N) )

/* Make `N' a member of `set': */
#define   SetInsert(N,set)	( (set)[(N)/SetTBits] |=  Set1Bit(N) )

/* Make `N' to *not* be a member of `set': */
#define   SetDelete(N,set)	( (set)[(N)/SetTBits] &= ~Set1Bit(N) )

/* Reverse `N's membership in `set': */
#define   SetToggle(N,set)	( (set)[(N)/SetTBits] ^= ~Set1Bit(N) )

/* Make `N' a member of `set' if-and-only-if `val' is true (non-zero): */
#define   SetAssign(val,N,set)	( (val) ? SetInsert(set,N) : SetDelete(set,N) )

#endif /* INCLUDED_SETS_H */
