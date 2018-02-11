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
 *  spenqprior() -- insert into tree before other equal keys.
 *
 *  put n in q before all other nodes with the same key; after this is
 *  done, n will be the root of the splay tree representing q, all nodes in
 *  q with keys less than that of n will be in the left subtree, all with
 *  greater or equal keys will be in the right subtree; the tree is split
 *  into these subtrees from the top down, with rotations performed along
 *  the way to shorten the left branch of the right subtree and the right
 *  branch of the left subtree; the logic of spenqprior is exactly the
 *  same as that of spenq except for a substitution of comparison
 *  operators
 */
SPBLK *
spenqprior( SPBLK *n, SPTREE *q )
{

    SPBLK *left;	/* the rightmost node in the left tree */
    SPBLK *right;	/* the leftmost node in the right tree */
    SPBLK *next;	/* the root of unsplit part of tree */
    SPBLK *temp;

    KEYTYPE key;

    int cmpval;
    SP_COMPARE_FUNC *cmp;

    cmp = q->compare;
    n->uplink = NULL;
    next = q->root;
    q->root = n;
    if( next == NULL )	/* trivial enq */
    {
        n->leftlink = NULL;
        n->rightlink = NULL;
    }
    else		/* difficult enq */
    {
        key = n->key;
        left = n;
        right = n;

        /* n's left and right children will hold the right and left
	   splayed trees resulting from splitting on n->key;
	   note that the children will be reversed! */

        if( COMPARE( next->key, key ) >= 0 )
	    goto two;

    one:	/* assert next->key < key */

	do	/* walk to the right in the left tree */
	{
            temp = next->rightlink;
            if( temp == NULL )
	    {
                left->rightlink = next;
                next->uplink = left;
                right->leftlink = NULL;
                goto done;	/* job done, entire tree split */
            }
            if( COMPARE( temp->key, key ) >= 0 )
	    {
                left->rightlink = next;
                next->uplink = left;
                left = next;
                next = temp;
                goto two;	/* change sides */
            }
            next->rightlink = temp->leftlink;
            if( temp->leftlink != NULL )
		temp->leftlink->uplink = next;
            left->rightlink = temp;
            temp->uplink = left;
            temp->leftlink = next;
            next->uplink = temp;
            left = temp;
            next = temp->rightlink;
            if( next == NULL )
	    {
                right->leftlink = NULL;
                goto done;	/* job done, entire tree split */
            }

	} while( COMPARE( next->key, key ) < 0 );	/* change sides */

    two:	/* assert next->key >= key */

	do	 /* walk to the left in the right tree */
	{
            temp = next->leftlink;
            if( temp == NULL )
	    {
                right->leftlink = next;
                next->uplink = right;
                left->rightlink = NULL;
                goto done;	/* job done, entire tree split */
            }
            if( COMPARE( temp->key, key ) < 0 )
	    {
                right->leftlink = next;
                next->uplink = right;
                right = next;
                next = temp;
                goto one;	/* change sides */
            }
            next->leftlink = temp->rightlink;
            if( temp->rightlink != NULL )
		temp->rightlink->uplink = next;
            right->leftlink = temp;
            temp->uplink = right;
            temp->rightlink = next;
            next->uplink = temp;
            right = temp;
            next = temp->leftlink;
            if( next == NULL )
	    {
                left->rightlink = NULL;
                goto done;	/* job done, entire tree split */
            }

	} while( COMPARE( next->key, key ) >= 0 );	/* change sides */

        goto one;

    done:	/* split is done, branches of n need reversal */

        temp = n->leftlink;
        n->leftlink = n->rightlink;
        n->rightlink = temp;
    }

    return( n );

} /* spenqprior */

