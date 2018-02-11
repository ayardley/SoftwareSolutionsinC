/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Douglas A. Gwyn.  Not derived from licensed software.
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
	getsubopt -- parse suboptions from an option's parameter argument

	complies with the following standard:
		System V Interface Definition, Third Edition, Volume 1

	Declare as:

		extern int getsubopt( char **optionp, char *const *tokens,
				      char **valuep );

	Notes:

	This code implements a recent extension of the System V Command
	Syntax Standard to "suboptions".  The intention is that this
	interface be used by most C programs, in conjunction with getopt(),
	to parse such command-line arguments.

	This implementation does not support multibyte suboption names.
	(They should not be used by portable programs anyway.)

	Example of command invocation with suboptions for the -o option:
		program_name -x -o name1,name2=value -- args...
	Note that the "=value" part is optional; it is up to the program to
	determine how to handle missing or extraneous values.
 */
/* #define	ROBUST	/* sane behavior when invoked with null pointers */
/* #define	DEBUG			/* enable for assertion checking */

#ifndef DEBUG
#define NDEBUG
#endif

#include	<assert.h>
#include	<stddef.h>
#include	<stdio.h>
#include	<string.h>

#define EOS	'\0'			/* C string terminator */
#define	ERR	(-1)			/* getsubopt() error return */

int
getsubopt(
	char			**optionp,	/* -> suboptions param */
	register char *const	*tokens,	/* list of valid names */
	register char		**valuep	/* where to store value */
	 )
	{
	register int		ti;	/* indexes token[] */
	register char		*p;	/* result of strchr() */
	register char		*name;	/* points to option name */

	assert(optionp != NULL);
	assert(*optionp != NULL);
	assert(tokens != NULL);
	assert(valuep != NULL);

#if	defined(ROBUST) && !defined(DEBUG)	/* DEBUG passed assert() */
	if ( optionp == NULL
	  || *optionp == NULL
	  || tokens == NULL
	  || valuep == NULL
	   )
		return ERR;		/* give up on suboption parsing */
#endif

	name = *optionp;

	/* find following suboption, if any */

	if ( (p = strchr( name, ',' )) != NULL )
		*p++ = EOS;		/* terminate and point to next */
	else
		p = strchr( name, EOS );

	assert(p != NULL);
	*optionp = p;			/* prepare advance to next subopt */

	/* find value, if any */

	if ( (p = strchr( name, '=' )) != NULL )
		*p++ = EOS;		/* terminate and point to value */

	*valuep = p;			/* point to value (may be NULL) */

	/* find index in token list */

	for ( ti = 0; tokens[ti] != NULL; ++ti )
		if ( strcmp( tokens[ti], name ) == 0 )
			return ti;	/* matching index in token list */

	return ERR;			/* not in token list */
	}
