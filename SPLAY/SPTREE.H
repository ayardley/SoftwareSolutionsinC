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

/*
** sptree.h:  The following type declarations provide the binary tree
**  representation of event-sets or priority queues needed by splay trees
**
**  assumes that data will be provided by the application
**  to hold all application specific information
**
**  assumes that key will be provided by the application, comparable
**  with the compare function applied to the addresses of two keys.
**
**  The implementation used here is based on the implementation
**  which was used in the tests of splay trees reported in:
**
**    An Empirical Comparison of Priority-Queue and Event-Set Implementations,
**	by Douglas W. Jones, Comm. ACM 29, 4 (Apr. 1986) 300-311.
**
**  The changes made include the addition of the enqprior
**  operation and the addition of up-links to allow for the splay
**  operation.  The basic splay tree algorithms were originally
**  presented in:
**
**	Self Adjusting Binary Trees,
**		by D. D. Sleator and R. E. Tarjan,
**		Proc. ACM SIGACT Symposium on Theory
**		of Computing (Boston, Apr 1983) 235-245.
**
**  The enq and enqprior routines use variations on the
**  top-down splay operation, while the splay routine is bottom-up.
**  All are coded for speed.
**
**  This code was written by:
**    Douglas W. Jones with assistance from Srinivas R. Sataluri
**
**  Translated to C and supplemented by:
**    David Brower, daveb@ingres.com
**
**  Thu Oct  6 12:11:33 PDT 1988 (daveb) Fixed spdeq, which was broken
**	handling one-node trees.  I botched the pascal translation of
**	a VAR parameter.
** 
**  Mon Apr  3 15:18:32 PDT 1989 (daveb)
**   	Apply fix supplied by Mark Moraes <moraes@csri.toronto.edu> to
** 	spdelete(), which dropped core when taking out the last element
** 	in a subtree -- that is, when the right subtree was empty and
** 	the leftlink was also null, it tried to take out the leftlink's
** 	uplink anyway.
**
**   The operations in this package also include those typically needed
**   on the pending event set in discrete event simulation.  See, for
**   example,
**   
**   Introduction to Simula 67,
**   by Gunther Lamprecht, Vieweg & Sohn, Braucschweig, Wiesbaden, 1981.
**   (Chapter 14 contains the relevant discussion.)
**   
**   Simula Begin,
**   by Graham M. Birtwistle, et al, Studentlitteratur, Lund, 1979.
**   (Chapter 9 contains the relevant discussion.)
**   
**   Many of the routines in this package use the splay procedure,
**   for bottom-up splaying of the queue.  Consequently, item n in
**   delete and item np in all operations listed above must be in the
**   event-set prior to the call or the results will be
**   unpredictable (eg:  chaos will ensue).
**   
**   Note that, in all cases, these operations can be replaced with
**   the corresponding operations formulated for a conventional
**   lexicographically ordered tree.  The versions here all use the
**   splay operation to ensure the amortized bounds; this usually
**   leads to a very compact formulation of the operations
**   themselves, but it may slow the average performance.
**   
**   Alternative versions based on simple binary tree operations are
**   provided (commented out) for head, next, and prev, since these
**   are frequently used to traverse the entire data structure, and
**   the cost of traversal is independent of the shape of the
**   structure, so the extra time taken by splay in this context is
**   wasted.
*/

# ifndef SPTREE_H
# define SPTREE_H

# ifndef NULL
# define NULL (0)
# endif

#if defined(INTKEY)
	typedef	int KEYTYPE;
#elif defined(STRKEY)
	typedef char *KEYTYPE;
#else
	typedef void *KEYTYPE;
#endif

# ifdef INTDATA
	typedef int DATATYPE;
# else
	typedef void *DATATYPE;
# endif


typedef int SP_COMPARE_FUNC( KEYTYPE a, KEYTYPE b );

typedef struct splay_block SPBLK;

struct splay_block
{
	SPBLK           *uplink;
	SPBLK           *leftlink;
	SPBLK           *rightlink;
	KEYTYPE         key;
	DATATYPE        data;
};

typedef struct splay_tree SPTREE;

struct splay_tree
{
	SPBLK           *root;
	SP_COMPARE_FUNC *compare;
};

typedef void SP_SCAN_FUNC( SPBLK *n, void *arg );

extern void *emalloc( int size );

/* delete node from tree */
extern void spdelete( SPBLK *n, SPTREE *q );

/* return and remove lowest item in subtree */
extern SPBLK *spdeq( SPBLK **np );

/* return and remove largest item in subtree */
extern SPBLK *spdeqmax( SPBLK **np );

/* is tree empty? */
extern int spempty( SPTREE *q );

/* insert item into the tree */
extern SPBLK *spenq( SPBLK *n, SPTREE *q );

/* enqueue after existing node, copying key  */
extern SPBLK *spenqafter( SPBLK *new, SPBLK *old, SPTREE *q );

/* enqueue before existing node, copying key */
extern SPBLK *spenqbefore( SPBLK *new, SPBLK *old, SPTREE *q );

/* insert before items with same key */
extern SPBLK *spenqprior( SPBLK *n, SPTREE *q );

/* fast non-splaying head */
extern SPBLK *spfhead( SPTREE *q );

/* fast non-splaying next */
extern SPBLK *spfnext( SPBLK *n );

/* fast non-splaying prev */
extern SPBLK *spfprev( SPBLK *n );

/* fast non-splaying tail */
extern SPBLK *spftail( SPTREE *q );

/* return first node in tree */
extern SPBLK *sphead( SPTREE *q );

/* enter an item, installing new data or replacing data in one found.
   If a new node is needed, space is allocated through emalloc() */

extern SPBLK *spinstall( KEYTYPE key,
			 DATATYPE data,
			 SPTREE *q );

/* find key in a tree */
extern SPBLK *splookup( KEYTYPE key, SPTREE *q );

/* setup tree, allocate new space with emalloc */
extern SPTREE *spminit( SP_COMPARE_FUNC *cmp );

/* return next node in tree */
extern SPBLK *spnext( SPBLK *n, SPTREE *q );

/* return previous node in tree */
extern SPBLK *spprev( SPBLK *n, SPTREE *q );

/* reverse scan through tree */
extern void sprscan( SP_SCAN_FUNC *f, void *arg, SPBLK *n, SPTREE *q );

/* scan forward through tree */
extern void spscan( SP_SCAN_FUNC *f, void *arg, SPBLK *n, SPTREE *q );

/* reorganize tree */
extern void spsplay( SPBLK *n, SPTREE *q );

/* find end of a tree */
extern SPBLK *sptail( SPTREE *q );

/* setup tree, use user provided space */
extern SPTREE *spuinit( SPTREE *q, SP_COMPARE_FUNC *cmp );

# endif
