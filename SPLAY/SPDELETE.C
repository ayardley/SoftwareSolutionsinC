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
 * spdelete() -- Delete node from a tree.
 *
 *	n is deleted from q; the resulting splay tree has been splayed
 *	around its new root, which is the successor of n
 *
 */
void
spdelete( SPBLK *n, SPTREE *q )
{
    SPBLK *x;
    
    spsplay( n, q );
    x = spdeq( &q->root->rightlink );
    if( x == NULL )		/* empty right subtree */
    {
        q->root = q->root->leftlink;
        if (q->root)
	    q->root->uplink = NULL;
    }
    else			/* non-empty right subtree */
    {
        x->uplink = NULL;
        x->leftlink = q->root->leftlink;
        x->rightlink = q->root->rightlink;
        if( x->leftlink != NULL )
	    x->leftlink->uplink = x;
        if( x->rightlink != NULL )
	    x->rightlink->uplink = x;
        q->root = x;
    }
    
} /* spdelete */

