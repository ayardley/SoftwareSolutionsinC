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

/* hash.h -- Declarations required for the hash*() (hash table) routines */

#ifndef INCLUDED_HASH_H	/* In case this file is #include'd more than once */
#define INCLUDED_HASH_H

#define   HASHTAB   struct hashtable
/* Could use "typedef" but that has little advantage for such simple
 * declarations and prevents the use of "#ifdef HASHTAB" or "HASHTAB {...};" */

/* You may need to comment these next lines out if "hash.h" is included before
 * something that does  "typedef  unsigned long  us_long;"  without checking
 * whether "us_long" has already been defined: */
#ifndef us_long
#  define   us_long   unsigned long	/* Just convenient shorthand */
#endif /* us_long */

#define  HASH_TOO_FULL   2 / 1  /* Ratio slots:entries when table is too full */
/* Size of table will be roughly doubled when table gets too full. */
/* No parentheses are used so that settings like 3/2 can be used; "(3/2)"
 * just evaluates to "1".  So don't use HASH_TOO_FULL in expressions next
 * to operators with higher presidence than "/". */

/* Define HASH_PASS_LENGTH if you want a key length passed to your hashing
 * function and key comparison functions.  For example, if you use fixed-
 * length (possibly not '\0'-terminated) strings as keys and want to use
 * memcmp() or strncmp() to compare key values.
 *
 * If you use variable-length, non-'\0'-terminated strings and so need
 * a length associated with each key value instead of the same length
 * associated with all keys from the same hash table, then define a structure
 * to hold the key value and length and use pointers to these structures as
 * your key values (i.e., pass these pointers to the hash table routines and
 * have your hashing function and key comparison functions expect them to
 * be passed to them).
 *
 * You can also define HASH_PASS_LENGTH if you have a C compiler that will
 * let you pass extra arguments to a function, such as a third argument to
 * strcmp():  strcmp(key1,key2,len).  This allows you to directly use strcmp()
 * and memcmp()/strncmp() with the same compilation of these routines.
 * Otherwise you may have to define HASH_PASS_LENGTH and define a trivial
 * function:
 *    int keycmp( void *key1, void *key2, size_t len )
 *        {  return  strcmp( key1, key2 );  }
 *
 * Note that this was more useful before ANSI C.  Strict adherence to ANSI C
 * requires that you define your own function anyway:
 *    int keycmp( void *key1, void *key2 ) { return strcmp(key1,key2); }
 * so that the "void *"s get converted to "char *"s (or "const char *"s)
 * before strcmp() is called. */
/* #define HASH_PASS_LENGTH	\* Uncomment to have one key length per table */

#ifdef HASH_PASS_LENGTH
# define   HashPassLen(X)   , X			/* Pass an extra argument */
#  include <stddef.h>				/* Define size_t */
#else /* HASH_PASS_LENGTH */
# define   HashPassLen(X)   /* nothing */	/* Ignore the extra argument */
#endif /* HASH_PASS_LENGTH */

/* If compiled with DEBUG defined, hashdump() stores whether the just-returned
 * location had a collision mark at it.  It also returns NULL for empty
 * locations that have collision marks (rather than skipping these entries). */
#ifdef DEBUG
extern int HashCollide;	/* Whether hashdump()ed location had a collision */
#endif /* DEBUG */

/* The data types of the hashing and key comparison functions: */
typedef us_long HASHFUNC( void *key  HashPassLen(size_t len) );
typedef int     HASHCMP( void *key1, void *key2  HashPassLen(size_t len) );
/* These are only used to declare pointers to these types of functions,
 * but using a typedef then "HASHFUNC *hasher" seems more intuative
 * than complicating the typedef to define a "pointer to" type */

/* The types of operations that can be performed by hashtab(): */
typedef enum {
    HASHfnd, /* Find an item (return NULL if no such item) */
    HASHnxt, /* Find another item with the same key (return NULL if none) */
    HASHadd, /* Add an item (return NULL if duplicate key already entered) */
    HASHchk, /* Same as HASHadd but return duplicate item found */
    HASHdup, /* Add an item even if a duplicate key already exists */
    HASHdel  /* Delete an item (return NULL if no such item) */
} HASHREQ;

HASHTAB *hashalloc( long room, HASHFUNC *hashkey, HASHCMP *compare
  HashPassLen(size_t keylen) );
/*
 * Allocate a hash table with initial room for a maximum of `room' entries.
 * The number of slots actually allocated is slightly more than
 * `room * HASH_TOO_FULL' so `room' entries can be stored without enlarging
 * the table.
 *
 * NULL is returned if malloc() fails (i.e., if there is not enough available
 * memory).  Otherwise a pointer to the hash table is returned.  Use this
 * pointer as the first argument to all of the other hash table functions.
 *
 * `hashkey' is the hashing function that takes a key and returns a number
 * based solely on it but with as little relationship to it as possible.
 *
 * `compare' is the key-value-comparison routine.  It takes pointers to two
 * key values and returns 0 if they are the same and a non-zero value if they
 * are different.
 *
 * If HASH_PASS_LENGTH is defined when hash.c is compiled `keylen' should also
 * be passed in.  In this case `hashkey' and `compare' will each be passed
 * this value as an additional argument.  If you specify hashing and comparison
 * functions that ignore this extra argument then you can, of course, specify
 * any value (such as "0") here to allow a single compilation of hash.c to
 * be used both with hash tables that use a length and those that don't.
 */

void hashfree( HASHTAB *tab, void (*freer)(void *entry) );
/*
 * Free a previously allocated hash table.  `Freer' is a routine to be called
 * for each entry in the hash table to free it.  If `freer' is NULL, no routine
 * is called.  Note that key values are not freed separately since the key
 * value is usually part of the entry.
 */

long hashcount( HASHTAB *tab, long *nslots );
/*
 * Return the number of entries currently in the hash table.  If `nslots' is
 * not NULL, then `*nslots' is set to the size of the hash table (how many
 * slots have been allocated).  The hash table will be enlarged if more than
 * `*nslots * HASH_TOO_FULL' entries get inserted into it.
 */

int hashresize( HASHTAB *tab, long nslots );
/*
 * Change the size of a hash table.  Returns 0 if a call to malloc() fails or
 * the new table size is insufficient to comfortably hold the number of entries
 * currently in the table.  Unlike hashalloc(), hashresize() does not mutliply
 * the requested size by HASH_TOO_FULL.  However, the final size allocated may
 * be larger as governed by the setting of SIZE_TWO_POWER in hash.c.
 *
 * Specifying 0 for `nslots' will leave the table size the same.  A long
 * series of additions and deletions can leave empty hash table slots marked
 * as collisions.  hashresize() has to reinsert each entry into an empty table
 * so calling hashresize(tab,0) may remove some collision marks.
 */

void *hashdump( HASHTAB *table, long *loc );
/*
 * Cycle through all of the entries in the hash table in an efficient but
 * not-easily-predicted order.  Not often used.
 *
 * `loc' is used to provide context.  If `*loc' is -1 then the physically
 * first entry is returned.  Otherwise the physically next entry is returned.
 * In either case `*loc' is set to the index of the returned entry.  When no
 * more entries exist, NULL is returned and `*loc' is set to -1 again.
 *
 * Here is a sample use of hashdump():
 *    long loc= -1;
 *      ...
 *      while(  NULL != ( entry= hashdump(tab,&loc) )  ) {
 *          process( entry );
 *      }
 *
 * If you insert NULL entries, see the loop inside hashfree() in hash.c for how
 * to use hashdump() in that case.
 */

void *hashtab( HASHTAB *table, HASHREQ req, void *key, long *loc, void *item );
/*
 * Insert, lookup, or delete an entry in the hash table.
 *
 * `table' is the pointer returned from a previous call to hashalloc().
 *
 * `req' is a HASHxxx value specifying what operation should be done.  See
 * above definition of HASHREQ for more information.
 *
 * `key' is a pointer to the key value to be used to look up the item to
 * be operated on or to store the new item under.
 *
 * If `loc' is not NULL and any operation succeeds, `*loc' is set to the
 * "physical" index of the item operated upon.  This value can be used in
 * a subsequent call to hashtab() to more efficiently delete (via HASHdel)
 * that same item or to access (via HASHnxt) the "next" item having the same
 * key value (added via the HASHdup operation).
 *
 * For the HASHdel operation, if `key' is NULL then `loc' must not be NULL and
 * will be used to locate the entry to be deleted instead.  In this case `*loc'
 * is the "physical" index to be used in place of the hashed value of `*key'.
 * `loc' should only be used in this way (and so `key' should only be NULL)
 * when `*loc' contains a value returned by a previous call to hashtab() or
 * hashdump().
 *
 * For HASHnxt, `key' and `loc' must not be NULL and `*loc' must contain the
 * value set by the previous HASHfnd or HASHnxt operation for the same key
 * value.  For example:
 *    ent= hashtab( tab, HASHfnd, "booger", &loc, NULL );
 *    while(  NULL != ent  ) {
 *        process( ent );
 *        ent= hashtab( tab, HASHnxt, "booger", &loc, NULL );
 *    }
 *
 * `item' is a pointer to the item to be stored.  The value of `item' is
 * ignored except for the HASHadd, HASHchk, and HASHdup operations, all of
 * which attempt to insert an item into the hash table.
 */

#endif /* INCLUDED_HASH_H */
