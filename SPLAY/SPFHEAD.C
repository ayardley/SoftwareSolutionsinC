/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by David Brower.  Not derived from licensed software.
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

# include "sptree.h"

/*----------------
 *
 * spfhead() --	return the "lowest" element in the tree.
 *
 *	returns a reference to the head event in the event-set q.
 *	avoids splaying but just searches for and returns a pointer to
 *	the bottom of the left branch.
 */
SPBLK *
spfhead( SPTREE *q )
{
    SPBLK *x;

    if( NULL != ( x = q->root ) )
	while( x->leftlink != NULL )
	    x = x->leftlink;

    return( x );

} /* spfhead */

