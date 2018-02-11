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
 * spprev() -- return previous node in a tree, or NULL.
 *
 *	return the predecessor of n in q, represented as a splay tree;
 *	the predecessor becomes the root; an alternate version is
 *	provided which is faster on the average because it does not do
 *	any splaying
 *
 */
SPBLK *
spprev( SPBLK *n, SPTREE *q )
{
    SPBLK *prev;
    SPBLK *x;
    
    /* splay version;
       note: deleting the last "if" undoes the amortized bound */
    
    x = n->leftlink;
    if( x != NULL )
	while( x->rightlink != NULL )
	    x = x->rightlink;
    prev = x;
    if( x != NULL )
	spsplay( x, q );
    
    return( prev );
    
} /* spprev */
