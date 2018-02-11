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
 *  spdeq() -- return and remove head node from a subtree.
 *
 *  remove and return the head node from the node set; this deletes
 *  (and returns) the leftmost node from q, replacing it with its right
 *  subtree (if there is one); on the way to the leftmost node, rotations
 *  are performed to shorten the left branch of the tree
 */
SPBLK *
spdeq( SPBLK **np )		/* pointer to a node pointer */
{
    SPBLK *deq;		/* one to return */
    SPBLK *next;       	/* the next thing to deal with */
    SPBLK *left;      	/* the left child of next */
    SPBLK *farleft;		/* the left child of left */
    SPBLK *farfarleft;	/* the left child of farleft */

    if( np == NULL || *np == NULL )
    {
        deq = NULL;
    }
    else
    {
        next = *np;
        left = next->leftlink;
        if( left == NULL )
	{
            deq = next;
            *np = next->rightlink;

            if( *np != NULL )
		(*np)->uplink = NULL;

        }
	else for(;;)	/* left is not null */
	{
            /* next is not it, left is not NULL, might be it */
            farleft = left->leftlink;
            if( farleft == NULL )
	    {
                deq = left;
                next->leftlink = left->rightlink;
                if( left->rightlink != NULL )
		    left->rightlink->uplink = next;
		break;
            }

            /* next, left are not it, farleft is not NULL, might be it */
            farfarleft = farleft->leftlink;
            if( farfarleft == NULL )
	    {
                deq = farleft;
                left->leftlink = farleft->rightlink;
                if( farleft->rightlink != NULL )
		    farleft->rightlink->uplink = left;
		break;
            }

            /* next, left, farleft are not it, rotate */
            next->leftlink = farleft;
            farleft->uplink = next;
            left->leftlink = farleft->rightlink;
            if( farleft->rightlink != NULL )
		farleft->rightlink->uplink = left;
            farleft->rightlink = left;
            left->uplink = farleft;
            next = farleft;
            left = farfarleft;
	}
    }

    return( deq );

} /* spdeq */

