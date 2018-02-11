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
 * spnext() -- return next higer item in the tree, or NULL.
 *
 *	return the successor of n in q, represented as a splay tree; the
 *	successor becomes the root; two alternate versions are provided,
 *	one which is shorter, but slower, and one which is faster on the
 *	average because it does not do any splaying
 *
 */
SPBLK *
spnext( SPBLK *n, SPTREE *q )
{
    SPBLK *next;
    SPBLK *x;
    
    /* splay version */
    spsplay( n, q );
    x = spdeq( &n->rightlink );
    if( x != NULL )
    {
        x->leftlink = n;
        n->uplink = x;
        x->rightlink = n->rightlink;
        n->rightlink = NULL;
        if( x->rightlink != NULL )
	    x->rightlink->uplink = x;
        q->root = x;
        x->uplink = NULL;
    }
    next = x;
    
    /* shorter slower version;
       deleting last "if" undoes the amortized bound */
    
# if 0
    spsplay( n, q );
    x = n->rightlink;
    if( x != NULL )
	while( x->leftlink != NULL )
	    x = x->leftlink;
    next = x;
    if( x != NULL )
	spsplay( x, q );
# endif
    
    return( next );
    
} /* spnext */


