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

# define _POSIX_SOURCE

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include "sptree.h"

/*
** spplay.c -- a play program for splay trees.
**
** This program does interactive insertion, deletion, printing
** contents of a string splay tree.  Unfortunately, UNIX specific
** because of the isatty that helps do nice prompting.  Sorry.
** Not ANSI-fied either.
**
** commands are:
**	'a %s string2' inserts datum (string2) keyed by %s.
**	's %s' prints the datum associated with %s if any.
**	'd %s' deletes the datum associated with %s if any.
**	'e stuff' echoes stuff
**	'p' prints all the items so far, in order.
*/

#define MAXSTR 512

#include "sptree.h"

#define prompt() if(prompt_p) printf(":"); else ;

char *
strsave( char *s )
{
    char *new = emalloc( strlen( s ) + 1 );
    strcpy( new, s );
    return( new );
}

void
prnode( SPBLK *spblk, void *arg )
{
	printf("%s %s\n", (char *)spblk->key, (char *)spblk->data);
}

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
main()
{
    char buf[MAXSTR];
    char cmd[8], datum[MAXSTR];
    char *cp;
    char *dp;
    char key[MAXSTR];
#	define KEYSTR "string1"
    SPBLK *result;
    int prompt_p;
    SPTREE *sp;
    
    sp = spminit( (SP_COMPARE_FUNC *)strcmp );
    prompt_p = isatty(fileno(stdin));
    prompt();
    while(fgets(buf, sizeof(buf), stdin) != NULL) {
	sscanf(buf, " %s %s", cmd, key);
	switch (*cmd) {
	case 'a':
	    sscanf(buf, " %*s %*s %[^\n]s", datum);
	    cp = strsave(key);
	    dp = strsave(datum);
	    spinstall( (KEYTYPE)cp, (DATATYPE)dp, sp);
	    break;
	case 's':
	    if ((result = splookup(key, sp)) != NULL)
		printf("found %s\n", (char *)result->data);
	    else
		printf("no match for %s\n", key);
	    break;
	case 'd':
	    if ((result = splookup(key, sp)) != NULL)
		spdelete(result, sp);
	    else
		printf("no match for %s\n", key);
	    break;
	case 'e':
	    printf("%s\n", buf );
	    break;
	case 'p':
	    spscan(prnode, NULL, NULL, sp);
	    fflush(stdout);
	    break;
	default:
	    printf("'a %s string2' inserts datum (string2) keyed by %s\n",
		   KEYSTR, KEYSTR);
	    printf("'s %s' prints the datum associated with %s if any\n",
		   KEYSTR, KEYSTR);
	    printf("'d %s' deletes the datum associated with %s if any\n",
		   KEYSTR, KEYSTR);
	    printf("'e stuff' echoes stuff\n" );
	    printf("'p' prints all the items so far\n");
	    break;
	}
	prompt();
    }
    printf("\n");
    return( 0 );
}
