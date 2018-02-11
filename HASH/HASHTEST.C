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

/* hashtest.c -- Test hash table functions */

/* Usage: hashtest [init_size]
 *
 * Reads from stdin; inserts each word read into a hash table; if that
 * word is already there, then deletes it from the hash tables.
 *
 * By default, it just prints out the final contents of the hash table
 * (i.e., a list of all words that appear an odd number of times in the
 * input).
 *
 * If all components are compiled with -DDEBUG, this will dump all the hash
 * table contents and collision indicators after each step.  It will also
 * check for words that begin with the following characters:
 *   -	Try to delete the word (minus the "-") and report result
 *   +	Insert the word (minus the "+") using the HASHdup operation
 *   ?	Just look up the word (minus the "?") and report result
 *   !	Look for a duplicate word (minus the "!") and report result (HASHnxt)
 * You can test the HASHdup function with input like "dup +dup ?dup !dup !dup"
 */

#include <stdlib.h>	/* free() strtol() */
#include <string.h>	/* strcmp() strdup() */
#include <stdio.h>	/* NULL stdin stderr [f]printf() fgets() perror() */
#include "hash.h"	/* HASH* Hash* hashtab() hashdump() */

#ifndef NO_SYSEXIT_H	/* Not an ANSI-standard include file, may be missing */
# include <sysexits.h>	/* EX_OK EX_USAGE */
#endif /* NO_SYSEXIT_H */
#ifndef EX_OK
#  define   EX_OK	0	/* Successful (normal) exit status code */
#endif /* EX_OK */
#ifndef EX_USAGE
#  define   EX_USAGE	1	/* 64 for Unix <sysexits.h> */
#endif /* EX_USAGE */

extern  unsigned int	fletch( char *str );
extern  us_long		lfletch( void *str  HashPassLen(size_t ignore) );

#ifndef NonWord	/* Common delimiters in English text */
#  define   NonWord	"\t ,;(\").\r\n"
#endif /* NonWord */

static int	/* Simply converts data types for full ANSI C compliance: */
strkeycmp( void *key1, void *key2  HashPassLen(size_t len) )
{
    return strcmp(key1,key2);	/* Compare two '\0'-terminated strings */
}

#ifdef DEBUG
#  define   IfDebug(X)	X	/* Include X only if DEBUG defined */
#else /* DEBUG */
#  define   IfDebug(X)	/* nothing (don't include X) */
#endif /* DEBUG */

int
main( int argc, char **argv )
{
  HASHTAB	*tab;		/* Hash table */
  long		 initsize;	/* Initial size for hash table */
  char		 buf[4096];	/* Buffer to read input via */
  char		*word;		/* Pointer to one word found in the input */
  char		*oper;		/* English description of hash tab operation */
  HASHREQ	 opcd;		/* Code for hash tab operation */
  char		*res;		/* Result of hash tab operationn */
# ifdef DEBUG
  long		 nents;		/* Number of entries in hash table */
  long		 nslots;	/* Number of slots for entries in hash table */
  long		 loc;		/* For use by hashdump() */
  long		*ploc= NULL;	/* Either NULL or &loc in call to hashtab() */
# else /* DEBUG */
#    define	 ploc	NULL
# endif /* DEBUG */
    if(  2 < argc 		/* Too many command-line arguments or... */
     ||  1 < argc  &&  '-' == argv[1][0]  ) {	/* trying to get usage msg: */
	fprintf( stderr, "Usage: %s [init_size]\n", argv[0] );
	exit( EX_USAGE );
    } else if(  2 == argc  ) {	/* User specified an initial table size: */
	initsize= strtol( argv[1], &res, 0 );
	if(  '\0' != *res  ||  initsize < 1  ) {
	    fprintf( stderr, "%s: Invalid initial hash table size: %s\n",
	      argv[0], argv[1] );
	    exit( EX_USAGE );
	}
    } else
	initsize= 100;			/* Default init hash table size. */
    tab= hashalloc( initsize, &lfletch, &strkeycmp  HashPassLen(0) );
    if(  NULL == tab  ) {
	fprintf( stderr, "%s: Can't allocate %ld-member hash table",
	  argv[0], initsize );
	perror( "; malloc" );	/* Say malloc() failed */
	exit( EX_USAGE );	/* Not quite a usage error, but close enough */
    }
    IfDebug(  fprintf( stderr, "%s\n%s\n",
      "Start words with: -delete, +HASHdup, ?look up, !HASHnxt",
      "All other words inserted if not there, deleted is are there." );  )
    while( 1 ) {
	IfDebug(  fprintf( stderr, "Enter word(s) to be hashed: " );  )
	if(  NULL == ( fgets(buf,sizeof(buf),stdin) )  ) /* End of input: */
	    break;	/* We are done.  [Note: Never use gets()!] */
	IfDebug( ploc= &loc; )	/* Get location / allow HASHnxt */
	word= strtok( buf, NonWord );
	while(  NULL != word  ) {
	    opcd= HASHadd;	oper= "Addition";	/* Default action */
#ifdef DEBUG
	    switch(  *(word++)  ) {	/* "++" assumes first char special */
	     case '-':	opcd= HASHdel;	oper= "Deletion";  ploc= NULL;	break;
	     case '+':	opcd= HASHdup;	oper= "Duplicate";		break;
	     case '?':	opcd= HASHfnd;	oper= "Query";			break;
	     case '!':	opcd= HASHnxt;	oper= "Next";			break;
	     /* First char was not special; undo "word++" from above: */
	     default:	word--;						break;
	    }
#endif /* DEBUG */
	    word= strdup( word );	/* Save word in a "safe" place */
	    res= hashtab( tab, opcd, word, ploc, word );
	    if(  HASHadd == opcd  &&  NULL == res  ) {
		oper= "Deletion";
		res= hashtab( tab, HASHdel, word, NULL, word );
	    }
#ifndef DEBUG
	    if(  HASHdel == opcd  &&  NULL != res  )   free( res );
#else /* DEBUG */
	    if(  NULL == res  ) {
		printf( "%s of \"%s\" failed.\n", oper, word );
	    } else {
		printf( "%s of \"%s\" returned \"%s\"", oper, word, res );
		if(  HASHdel != opcd  )
		    printf( " (%ld)", loc );
		printf( ".\n" );
		if(  HASHdel == opcd  )   free( res );
	    }
	    {
		nents= hashcount( tab, &nslots );
		printf( "Current contents of hash table (%ld/%ld):\n",
		  nents, nslots );
		while( 1 ) {
		    res= hashdump(tab,&loc);
		    if(  -1 == loc  )   break;
		    printf( "\t%5ld %c ", loc, HashCollide ? '#' : '-' );
		    if(  NULL != res  )
			printf( "\"%s\" (%ld)\n", res, fletch(res) % nents );
		    else
			printf( "EMPTY\n" );
		}
	    }
#endif /* DEBUG */
	    word= strtok( NULL, NonWord );
	}
    }
#ifndef DEBUG
    { long loc= -1;
	printf( "Words appearing an odd number of times:\n" );
	while(  NULL != ( res= hashdump(tab,&loc) )  ) {
	    printf( "\t%s\n", res );
	}
    }
#endif /* DEBUG */
    hashfree( tab, free );
    exit( EX_OK );
}
