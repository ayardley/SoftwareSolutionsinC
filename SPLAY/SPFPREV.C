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
 * spfprev() -- return fast previous node in a tree, or NULL.
 *
 *	return the predecessor of n in q, represented as a splay tree.
 *	This is a fast (on average) version that does not splay.
 */
SPBLK *
spfprev( SPBLK *n )
{
    SPBLK *prev;
    SPBLK *x;

    /* a long version,
     * avoids splaying for fast average, poor amortized bound
     */

    if( n == NULL )
        return( n );

    x = n->leftlink;
    if( x != NULL )
    {
        while( x->rightlink != NULL )
	    x = x->rightlink;
        prev = x;
    }
    else
    {
        x = n->uplink;
        prev = NULL;
        while( x != NULL )
	{
            if( x->rightlink == n )
	    {
                prev = x;
                x = NULL;
            }
	    else
	    {
                n = x;
                x = n->uplink;
            }
        }
    }

    return( prev );

} /* spfprev */
