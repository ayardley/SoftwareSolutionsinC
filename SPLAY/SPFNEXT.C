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
 * spfnext() -- fast return next higer item in the tree, or NULL.
 *
 *	return the successor of n in q, represented as a splay tree.
 *	This is a fast (on average) version that does not splay.
 */
SPBLK *
spfnext( SPBLK *n )
{
    SPBLK *next;
    SPBLK *x;

    /* a long version, avoids splaying for fast average,
     * poor amortized bound
     */

    if( n == NULL )
        return( n );

    x = n->rightlink;
    if( x != NULL )
    {
        while( x->leftlink != NULL )
	    x = x->leftlink;
        next = x;
    }
    else	/* x == NULL */
    {
        x = n->uplink;
        next = NULL;
        while( x != NULL )
	{
            if( x->leftlink == n )
	    {
                next = x;
                x = NULL;
            }
	    else
	    {
                n = x;
                x = n->uplink;
            }
        }
    }

    return( next );

} /* spfnext */
