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

/* spexample.c */

# include <stdio.h>
# include <strings.h>
# include <stdlib.h>
# include <unistd.h>
# include "sptree.h"



typedef struct
{
    SPBLK blk;
    char  *state;
    long  timestamp;
} OBJECT;


void *
    emalloc( int size )
{
    char *block;
    if( NULL == (block = (char *)malloc( size )) )
    {
	fprintf(stderr, "error allocating %d bytes\n", size );;
	abort();
    }
    return( block );
}


int
    main( int argc, char **argv )
{
    SPTREE	objects;
    SPTREE	strcache;
    
    
    OBJECT	*object;
    SPBLK	*sp;
    FILE	*fp;
    char	buf[ BUFSIZ ];
    char	obj[ BUFSIZ ];
    char	state[ BUFSIZ ];
    int		timestamp;
    int		nr;
    
    spuinit( &objects, (SP_COMPARE_FUNC *)strcmp );
    spuinit( &strcache, (SP_COMPARE_FUNC *)strcmp );
    
    
    if( argc != 2 )
    {
	fprintf(stderr, "Usage: %s transaction-file\n", argv[0] );
	exit( 1 );
    }
    fp = fopen( argv[1], "r" );
    if( fp == NULL )
    {
	fprintf(stderr, "Can't open %s\n", argv[1] );
	exit( 2 );
    }
    
    
    for( nr = 1; fgets( buf, sizeof(buf), fp ) != NULL ; nr++ )
    {
	if( sscanf( buf, "%i %s %s", &timestamp, obj, state ) != 3 )
	{
	    fprintf(stderr, "syntax error in file %s %d:%s\n",
		    argv[1], nr, buf );
	    exit( 3 );
	}
	
	
	if( NULL == (object = (OBJECT *)splookup( (void *)obj, &objects ) ) )
	{
	    
	    
	    object = (OBJECT *)emalloc( sizeof *object + strlen( obj ) + 1 );
	    object->blk.key = (void *)((char *)object + sizeof(*object));
	    strcpy( (char *)object->blk.key, obj );
	    spenq( &object->blk, &objects );
	}
	
	
	if( NULL == (sp = splookup( (void *)state, &strcache )) )
	{
	    sp = (SPBLK *)emalloc( sizeof( *sp ) + strlen( state ) + 1 );
	    sp->key = (void *)((char *)sp + sizeof(*sp) );
	    strcpy( (char *)sp->key, state );
	    spenq( sp, &strcache );
	}
	
	
	object->timestamp = timestamp;
	object->state = (char *)sp->key;
    }
    fclose( fp );
    
    
    for( sp = spfhead( &objects ); sp != NULL; sp = spfnext( sp ) )
    {
	object = (OBJECT *)sp;
	printf("%s\t%s\t%d\n", (char *)object->blk.key, object->state,
	       object->timestamp );
    }
    exit( 0 );
}

