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
 * spinstall() -- install an entry in a tree, overwriting any existing node.
 *
 *	If the node already exists, replace its contents.
 *
 *	If it does not exist, allocate a new node and fill it in.
 */

SPBLK *
spinstall( KEYTYPE key, DATATYPE data, SPTREE *q )

{
    SPBLK *n;

    if( NULL == ( n = splookup( key, q ) ) )
    {
	n = (SPBLK *)emalloc( sizeof( *n ) );
	n->key = key;
	n->leftlink = NULL;
	n->rightlink = NULL;
	n->uplink = NULL;
	spenq( n, q );
    }

    n->data = data;

    return( n );
}


