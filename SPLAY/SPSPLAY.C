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

void
spsplay( SPBLK *n, SPTREE *q )
{
    SPBLK *up;		/* points to the node being dealt with */
    SPBLK *prev;	/* a descendent of up, already dealt with */
    SPBLK *upup;	/* the parent of up */
    SPBLK *upupup;	/* the grandparent of up */
    SPBLK *left;	/* the top of left subtree being built */
    SPBLK *right;	/* the top of right subtree being built */

    left = n->leftlink;
    right = n->rightlink;
    prev = n;
    up = prev->uplink;

    while( up != NULL )
    {
	/* walk up the tree towards the root, splaying all to the left of
	   n into the left subtree, all to right into the right subtree */

	upup = up->uplink;
	if( up->leftlink == prev )	/* up is to the right of n */
	{
	    if( upup != NULL && upup->leftlink == up ) 
	    {
		upupup = upup->uplink;
		upup->leftlink = up->rightlink;
		if( upup->leftlink != NULL )
		    upup->leftlink->uplink = upup;
		up->rightlink = upup;
		upup->uplink = up;
		if( upupup == NULL )
		    q->root = up;
		else if( upupup->leftlink == upup )
		    upupup->leftlink = up;
		else
		    upupup->rightlink = up;
		up->uplink = upupup;
		upup = upupup;
	    }
	    up->leftlink = right;
	    if( right != NULL )
		right->uplink = up;
	    right = up;

	}
	else				/* up is to the left of n */
	{
	    if( upup != NULL && upup->rightlink == up )
	    {
		upupup = upup->uplink;
		upup->rightlink = up->leftlink;
		if( upup->rightlink != NULL )
		    upup->rightlink->uplink = upup;
		up->leftlink = upup;
		upup->uplink = up;
		if( upupup == NULL )
		    q->root = up;
		else if( upupup->rightlink == upup )
		    upupup->rightlink = up;
		else
		    upupup->leftlink = up;
		up->uplink = upupup;
		upup = upupup;
	    }
	    up->rightlink = left;
	    if( left != NULL )
		left->uplink = up;
	    left = up;
	}
	prev = up;
	up = upup;
    }

# ifdef DEBUG
    if( q->root != prev )
    {
	fprintf(stderr, " *** bug in spsplay: n not in q *** " );
	abort();
    }
# endif

    n->leftlink = left;
    n->rightlink = right;
    if( left != NULL )
	left->uplink = n;
    if( right != NULL )
	right->uplink = n;
    q->root = n;
    n->uplink = NULL;

} /* spsplay */
