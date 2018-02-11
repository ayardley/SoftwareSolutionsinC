/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Tye McQueen.  Not derived from licensed software.
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

/* hash.c -- General-purpose hash table routines */

#include <stdio.h>	/* NULL */
#include <stdlib.h>	/* malloc() free() */
#include <string.h>	/* memset() */

#include "hash.h"	/* HASH* Hash* */
#include "sets.h"	/* SetAlloc,SetFree,SetMember,SetInsert,SetDelete */

#ifdef DEBUG
/*global*/ int HashCollide;	/* Back door for getting info on collisions */
#endif /* DEBUG */

/* Macro to allocate buffer for N items of type `type': */
#define   Alloc(N,type)   (  (type *) malloc( (N) * sizeof(type) )  )

/* Information to be kept for each item stored in the hash table: */
typedef struct hashent {
  void   *rec;	/* Item being stored in hash table */
  void   *key;	/* Key value for item (probably part of item) */
} HASHENT;

/* Main structure of a hash table: */
HASHTAB /* struct hashtable */ {
  long		 nents;		/* Number of entries being used */
  long		 nslots;	/* Number of entries we have room for */
  HASHFUNC	*hasher;	/* Routine to hash a key value */
  HASHCMP	*compar;	/* Routine to compare two key values */
  HASHENT	*ent;		/* malloc()ed array of entries */
  SetType	*coll;		/* Flag entries where collisions occurred */
#ifdef HASH_PASS_LENGTH
  size_t	 keylen;	/* Length of keys used in this table */
#endif
};

/*#define SIZE_TWO_POWER	\* Table size must be a power of two */
/* Uncomment the above line if you want the table size to always be a power
 * of two.  That way  (hashvalue & (table_size - 1))  can be used instead of
 * the slower  (hashvalue % table_size).
 *
 * Comment the above line if you want the table size to always be odd (in
 * hopes of reducing the likelyhood of collisions by allow more bits of the
 * hash value to affect the index that will be used).
 */

#ifdef SIZE_TWO_POWER
#  define   HashKey(table,key)					\
	(*table->hasher)( key HashPassLen(table->keylen) )	\
	  & ( table->nslots - 1 )
#else /* SIZE_TWO_POWER */
#  define   HashKey(table,key)					\
	(*table->hasher)( key HashPassLen(table->keylen) )	\
	  % table->nslots
#endif /* SIZE_TWO_POWER */

/* Adjust a hash table size according to the setting of SIZE_TWO_POWER: */

static long	/* Returns a valid size (perhaps larger than the input size) */
nextsize( long nslots )
{
#ifdef SIZE_TWO_POWER
    { long pow2= 1;	/* Force number of elements to be a power of two: */
	while(  pow2 < nslots  )
	    pow2 <<= 1;
	nslots= pow2;
    }
#else /* SIZE_TWO_POWER */
    nslots |= 1;	/* Force number of elements to be odd (best if prime) */
#endif /* SIZE_TWO_POWER */
    return( nslots );
}

/* Allocate a new hash table: */

HASHTAB *		/* Return a pointer to a the new table (or NULL) */
hashalloc(
  long		 room,		/* Number of items to allocate room for */
  HASHFUNC	*hashkey,	/* Function to hash key values */
  HASHCMP	*cmpkeys	/* Function to compare key values */
  HashPassLen(size_t keylen)	/* Key length for table */
) {
  HASHTAB *tab= Alloc( 1, HASHTAB );
  long nslots= nextsize( room * HASH_TOO_FULL );
    if(  NULL == tab  )			/* Not much memory at all, eh?  Yuk! */
	return( NULL );			/* Just give up */
    tab->ent= Alloc( nslots, HASHENT );
    tab->coll= SetAlloc(nslots);
    if(  NULL == tab->ent  ||  NULL == tab->coll  ) {	/* Low on memory: */
	if(  NULL != tab->ent  )   free( tab->ent );
	if(  NULL != tab->coll  )   SetFree( tab->coll );
	free( tab );
	return( NULL );
    }
    tab->nslots= nslots;
    tab->nents= 0;
    tab->hasher= hashkey;
    tab->compar= cmpkeys;
#ifdef HASH_PASS_LENGTH
    tab->keylen= keylen;
#endif /* HASH_PASS_LENGTH */
    SetEmpty( nslots, tab->coll );		/* No collisions yet */
    while(  0 < nslots--  )
	tab->ent[nslots].key= NULL;		/* All spaces are unused */
    /* tab->ent[X].rec not initialized since it is ignored when .key is NULL */
    return( tab );
}

/* Dump all entries in an efficient order: */

void *   /* Return pointer to each entry, one at a time, NULL when done */
hashdump(
  HASHTAB *table,	/* Hash table to dump entries of */
  long *loc		/* Where we left off at */
) {
  HASHENT *ent;
    if(  -1 == *loc  )		/* First time through: */
	*loc= table->nslots;		/* Start at one end */
    do {
	if(  -1 == --*loc  )	/* On to next slot; gone all the way through? */
	    return( NULL );	/* Yes; return NULL with  -1 == *loc  */
	ent= table->ent + *loc;	/* Next slot to check */
#ifdef DEBUG
	if(  SetMember(*loc,table->coll)  ) {	/* A collision occurred: */
	    HashCollide= 1;			/* Tell the caller about it */
	    return(  NULL == ent->key  ?  NULL  :  ent->rec  );
	}
#endif /* DEBUG */
    } while(  NULL == ent->key  );	/* Nothing here; keep going */
#ifdef DEBUG
    HashCollide= 0;	/* No collision; tell caller */
#endif /* DEBUG */
    return( ent->rec );	/* Found an item; return pointer to it */
}

/* Free a hash table and (possibly) each entry: */

void
hashfree(
  HASHTAB *tab,			/* Hash table to be freed */
  void   (*freer)(void *ent)	/* Routine to free each entry (or NULL) */
) {
    if(  NULL != freer  ) {				/* Free each entry: */
      void *ent;
      long loc= -1;
	do {	/* Just in case user inserts NULL items into table */
	    if(  NULL != ( ent= hashdump(tab,&loc) )  )
		(*freer)( ent );
	} while(  -1 != loc  );
    }
    free( tab->ent );
    SetFree( tab->coll );
    free( tab );
}

/* Delete a single item from the hash table: */

static void
delent(
  HASHTAB *table,	/* Table to delete entry from */
  HASHENT *ent		/* Location in table->ent to be freed */
) {
  long pos= ent - table->ent, prev;
    table->nents--;			/* One less item in table */
    table->ent[pos].key= NULL;		/* This location is now unused */
    if(  SetMember( pos, table->coll )  ) /* A collision at this location */
	/* means a collision pushed an item further down so we can't clear */
	return;		/* any collision flags */
    while(  1  ) {
	/* Figure location of previous item to check for collision bit: */
	if(  table->nslots <= ( prev= pos + 1 )  )   prev= 0;
    if(  NULL != table->ent[pos].key  ||  ! SetMember( prev, table->coll )  )
     break;	/* Give up once we find an item that might have collided */
	SetDelete( prev, table->coll );	/* Otherwise keep clearing bits */
	pos= prev;
    }
}

/* Fetch how many entries and how many slots are in the hash table: */

long		/* Return number of entries currently in hash table */
hashcount(
  HASHTAB *table,	/* Table to fetch count and/or size for */
  long	  *nslots	/* Buffer to place number of slots into */
) {
    if(  NULL != nslots  )
	*nslots= table->nslots;
    return( table->nents );
}

/* Resize the hash table (or just clean up after deletions/collisions): */

int		/* Return 0 if resize failed (and leave table unchanged) */
hashresize(
  HASHTAB *table,	/* Table to be resized */
  long	   nslots	/* New size to use */
) {
  HASHENT    *old= table->ent;	/* Remember old entry array in case we fail */
  HASHENT    *cur= old + table->nslots;	/* First entry to relocate */
  SetType *coll;
    if(  0 == nslots  )		/* Default to keeping table the same size */
	nslots= table->nslots;
    else if(  nslots <= HASH_TOO_FULL * table->nents  ) /* Don't bother: */
	return( 0 );	/* hashtab() would enlarge() before done here */
    else
	nslots= nextsize( nslots );
    /* Try to allocate room for more slots: */
    table->ent= Alloc( nslots, HASHENT );
    coll= SetAlloc(nslots);
    if(  NULL == table->ent  ||  NULL == coll  ) { /* No room for expansion: */
	if(  NULL != table->ent  )   free( table->ent );
	if(  NULL != coll  )   SetFree( coll );
	table->ent= old;	/* Restore table to original state */
	return( 0 );		/* Couldn't enlarge the table */
    }	/* Enlargement will succeed; initialize rest of structure: */
    table->nslots= nslots;
    SetFree( table->coll );
    SetEmpty( nslots, coll );
    table->coll= coll;
    while(  0 < nslots--  )
	table->ent[nslots].key= NULL;	/* All spaces are unused */
    table->nents= 0;
    /* Now reinsert all of the old entries: */
    while(  old <= --cur  ) {
	if(  NULL != cur->key  )
	    hashtab( table, HASHdup, cur->key, NULL, cur->rec );
    }
    free( old );
    return( 1 );
}

/* Enlarge the hash table: */

static int	/* Return 0 if enlargement failed (and leave table same size) */
enlarge(
  HASHTAB *table	/* Table to be enlarged */
) {
  long nslots= table->nslots;
    nslots <<= 1;				/* Double table size */
#ifndef SIZE_TWO_POWER
    nslots |= 1;				/* ...but keep size odd */
    if(  12 == ( nslots & 12 )  )
	nslots &= ~2;		/* Avoid (2^n)-1 (for no good reason) */
#endif /* SIZE_TWO_POWER */
    return(  hashresize( table, nslots )  );
}

/* Add, look up, or delete items in hash table: */

void *				/* Return pointer to record found or NULL */
hashtab(
  HASHTAB *table, /* Which hash table to use */
  HASHREQ  req,   /* Requested operation (HASHxxx) */
  void	  *key,   /* Key value to access item with */
  long    *loc,   /* Optional table location (del/dup:input, else:output) */
  void	  *rec    /* Item to be added (NULL for some requests) */
) {
  long cur;
  HASHENT *ent;
    /* Fast delete operation after a previous find returned *loc: */
    if(  NULL == key  &&  HASHdel == req  ) {
	if(  NULL == loc  ||  *loc < 0  ||  table->nslots <= *loc
	 ||  NULL == table->ent[*loc].key  )
	    return( NULL );
	delent( table, &table->ent[*loc] );
	return( table->ent[*loc].rec );
    }
    if(  NULL == key  )
	return( NULL );	/* All other operations require a key value */
    if(  HASHnxt == req  ) {	/* Find "next" item with same key: */
	if(  NULL == loc  ||  *loc < 0  ||  table->nslots <= *loc  )
	    return( NULL );	/* Invalid position to start searching from */
	cur= (0 == *loc) ? table->nslots - 1 : *loc - 1;
    } else
	cur= HashKey( table, key );
    ent= &table->ent[cur];
    if(  HASHdup != req  )	/* See if a match was found: */
     while( 1 ) {
	if(  NULL != ent->key
	 &&  0 == (*table->compar)(
		     key, ent->key HashPassLen(table->keylen) )  )
	    break;		/* Matching entry found, stop looking */
	if(  ! SetMember( ent - table->ent, table->coll )  )
	    {  ent= NULL;  break;  }	/* No collisions here, give up */
	if(  ent == table->ent  )	/* Scrolled back to first entry: */
	    ent += table->nslots;	/* Wrap to end of table */
	/* This should never happen, but if it does it could cause an
	 * infinite loop in routines using HASHnxt operations: */
	if(  --ent == table->ent+cur  )	/* Traversed entire table: */
	    {  ent= NULL;  break;  }	/* Give up! */
     }
    switch( req ) {
     case HASHdel:		/* Delete the matching entry: */
	if(  NULL != ent  )
	    delent( table, ent );
	break;
     case HASHadd:	/* Add a new entry if one doesn't already exist: */
	if(  NULL != ent  )	/* A match was found, which is an error: */
	    {  ent= NULL;  break;  }	/* Return NULL */
	/* FALLTHROUGH */
     case HASHchk:
	if(  NULL != ent  )	/* A match was found, show them which one: */
	    break;
	/* FALLTHROUGH */
     case HASHdup:		/* We *will* add an item now: */
	if(  table->nslots <= HASH_TOO_FULL*table->nents  &&  enlarge(table)  )
	    cur= HashKey( table, key );
	ent= table->ent + cur;
	while(  NULL != ent->key  ) {	/* Skip any collisions: */
	    SetInsert( ent - table->ent, table->coll );	/* Note collision */
	    if(  ent == table->ent  )
		ent += table->nslots;	/* Wrap to top of table */
	    if(  --ent == table->ent+cur  )
		return( NULL );		/* Table is full! */
	}
	table->nents++;	/* One more item in table */
	ent->key= key;  ent->rec= rec;	/* Store requested values */
	break;
     default:	break;			/* Just return the located item */
    }
    if(  NULL != loc  &&  NULL != ent  )
	*loc= ent - table->ent;
    if(  NULL == ent  )
	return( NULL );
    return( ent->rec );
}
