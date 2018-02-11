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
# include "spcmp.h"

/*----------------
 *
 * splookup() -- given key, find a node in a tree.
 *
 *	Splays the found node to the root.
 */
SPBLK *
splookup( KEYTYPE key, SPTREE *q )
{
    SPBLK *n;
    int cmpval;
    SP_COMPARE_FUNC *cmp;

    /* find node in the tree */
    cmp = q->compare;
    n = q->root;
    while( n && (cmpval = COMPARE( key, n->key ) ) )
	n = ( cmpval < 0 ) ? n->leftlink : n->rightlink;

    /* reorganize tree around this node */
    if( n != NULL )
	spsplay( n, q );

    return( n );
}
