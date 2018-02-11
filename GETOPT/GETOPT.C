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
	getopt -- parse command line arguments

	complies with the following standards:
		System V Interface Definition, Third Edition, Volume 1
		X/Open Portability Guide -- XSI System Interface and
			Headers, Issue 3
		Application Environment Specification -- Operating System
			Programming Interfaces Volume, Revision A

	Declare as:

		#include <stdio.h>	// for EOF
		extern int getopt( int argc, char *const *argv,
				   const char *optstring );
		extern char *optarg;
		extern int opterr;
		extern int optind;
		extern int optopt;	// optionally supported; see below

	Notes:

	This code fully supports the System V Command Syntax Standard as
	specified in the System V Interface Definition, Issue 3.  The
	intention is that this interface be used by most C programs to
	parse their command-line arguments.

	The origin of this code can be traced back to 7th Edition UNIX,
	through its release into the public domain in conjunction with the
	presentation of the paper "Proposed Syntax Standard for UNIX System
	Commands" by Kathleen Hemenway and Helene Armitage at the January
	1984 UniForum Conference in Washington, DC (sponsored jointly by
	/usr/group and the USENIX Association).  The code has subsequently
	been overhauled to increase its portability and to better handle
	certain unusual situations.

	This implementation does not support multibyte option characters.
	(They should not be used by portable programs anyway.)

	The second parameter of getopt() could have been written with an
	additional "const" type qualifier, but that would require most
	invocations of getopt() to use an explicit cast; this was deemed
	inappropriate and thus is not in the official interface spec.

	Unless RIGID_SPEC is defined, this implementation does not enforce
	the rules that an option-with-argument must not be clustered with
	other options in the same command-line argument and that its
	parameter argument must not be contained within the same command-
	line argument.  AT&T's getopt() never enforced these, and SVID
	Issue 3 has now officially relaxed the latter rule.  However,
	invokers of programs should follow the rules, so you may want to
	enable RIGID_SPEC in order to help users develop good habits.

 	The original version released into the public domain by AT&T also
	set an external variable "optopt" to the actual option character,
	which differs from the value returned by getopt() only when the
	latter is '?' (error indicator).  "optopt" is not specified as
	part of the official interface; if you want it, define SET_OPTOPT.
 */
/* #define	RIGID_SPEC		/* enable to enforce all rules */
/* #define	SET_OPTOPT		/* enable to support "optopt" */
/* #define	ROBUST	/* sane behavior when invoked with bad arguments */
/* #define	DEBUG			/* enable for assertion checking */

#ifndef DEBUG
#define NDEBUG
#endif

#include	<assert.h>
#include	<stdio.h>
#include	<string.h>

#define EOS	'\0'			/* C string terminator */

char		*optarg = NULL;		/* option parameter if any */
int		optind = 1;		/* next argv[] index */
static int	optci = 1;		/* next argv[optind][] index */
int		opterr = 1;		/* error => print message if set */
#ifdef	SET_OPTOPT
int		optopt = EOS;		/* option letter */
#endif

static int				/* prints diagnostic, returns '?' */
Error(	const char	*name,		/* argv[0] (program name) */
	const char	*mess,		/* diagnostic message */
	int		c		/* conflicting option character */
     )	{
	assert(name != NULL);
	assert(mess != NULL);

	if ( opterr )
		fprintf( stderr, "%s: %s -- %c\n", name, mess, c );

	return '?';			/* erroneous-option marker */
	}

int
getopt(	int			argc,		/* argc from main() */
	register char *const	*argv,		/* argv from main() */
	const char		*optstring	/* supported option letters,
						   followed by ":" when
						   option takes parameter */
      )	{
	register int		c;	/* option letter from argv */
	register const char	*cp;	/* -> option letter in optstring */
#ifdef	RIGID_SPEC
	register int		savci;	/* saved optci for parameter test */
#endif

	assert(argc > 0);
	assert(argv != NULL);
	assert(argv[0] != NULL);
	assert(optind >= argc || argv[optind] != NULL);
	assert(argv[argc] == NULL);
	assert(optstring != NULL);

#if	defined(ROBUST) && !defined(DEBUG)	/* DEBUG passed assert() */
	if ( argc <= 0
	  || argv == NULL
	  || optind < argc && argv[optind] == NULL
	  || argv[argc] != NULL
	  || optstring == NULL
	   )
		return EOF;		/* give up on argument parsing */
#endif

	optarg = NULL;

	if ( optci == 1 )		/* beginning of new argument */
		if ( optind >= argc		/* no more arguments */
		  || argv[optind][0] != '-'	/* no more options */
		  || argv[optind][1] == EOS	/* not option: stdin */
		   )
			return EOF;
		else if ( strcmp( argv[optind], "--" ) == 0 )
			{
			++optind;	/* skip over "--" */
			return EOF;	/* "--" marks end of options */
			}

	c = argv[optind][optci];	/* option letter */
#ifdef	SET_OPTOPT
	optopt = c;
#endif
#ifdef	RIGID_SPEC
	savci =
#endif
		optci++;		/* get ready for next letter */

	if ( argv[optind][optci] == EOS )	/* end of this argument */
		{
		++optind;		/* advance to next argument */
		optci = 1;		/* beginning of new argument */
		}

	if ( c == ':'			/* (not a valid option letter) */
	  || (cp = strchr( optstring, c )) == NULL	/* not listed */
	   )
		return Error( argv[0], "illegal option", c );

	if ( cp[1] == ':' )		/* option takes parameter */
		{
#ifdef	RIGID_SPEC
		/* (Minimize the number of additional spurious messages.) */

		if ( savci != 1 )
			{
			if ( optind < argc )
				++optind;	/* skip over noisy junk */

			optci = 1;	/* next option from new argument */

			return Error( argv[0],
   				      "option must not be clustered",
				      c
				    );
			}

		if ( optci != 1 )	/* parameter (?) in same argument */
			{
			++optind;	/* skip over noisy junk */
			optci = 1;	/* next option from new argument */

			return Error( argv[0],
				      "option argument must be separate",
				      c
				    );
			}
#else
		if ( optci != 1 )	/* parameter in same argument */
			{
			optarg = &argv[optind][optci];	/* -> parameter */
			optci = 1;	/* next option from new argument */
			}
		else
#endif
		if ( optind >= argc )
			return Error( argv[0],
				      "option requires an argument",
				      c
				    );
		else			/* parameter in separate argument */
			optarg = argv[optind];	/* -> parameter */

		/* (factored out the following) */
		++optind;		/* skip over parameter */
		}

	return c;			/* option letter */
	}
