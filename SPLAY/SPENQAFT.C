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
 * spenqafter() -- enter n after n1 in tree q.
 *
 *	returns a pointer to n.
 *
 *	event n is entered in the splay tree q as the immediate
 *	successor of n1; in doing so, n1 becomes the root of the tree
 *	with n as its right son
 */
SPBLK *
spenqafter( SPBLK *new, SPBLK *old, SPTREE *q )
{
    spsplay( old, q );
    new->key = old->key;
    new->rightlink = old->rightlink;
    if( new->rightlink != NULL )
	new->rightlink->uplink = new;
    new->leftlink = NULL;
    new->uplink = old;
    old->rightlink = new;
    return( new );
} /* spenqafter */

