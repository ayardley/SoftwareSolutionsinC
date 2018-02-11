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
	tstopt -- tests command-line option parsing functions

	Usage:	$ tstopt -a -b -i file -o c,d,name=file -- files...

	If there are any usage messages, they will be displayed on the
	standard error output and the program will eventually return a
	"failure" status indication to the invoking environment.  If any
	named file cannot be opened, it is considered an error.  (They are
	opened as text streams; on some systems an error will occur if they
	are actually binary files.)  You should verify that usage messages
	occur whenever they should, but not otherwise.

	Since this program exercises all features of these functions, it
	can serve as a model for option parsers in application programs.
*/
/* #define	SET_OPTOPT		/* enable to support "optopt" */
/* #define	DEBUG			/* enable for assertion checking */

#ifndef DEBUG
#define NDEBUG
#endif

#include	<assert.h>
#include	<stdarg.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

extern int	getopt( int, char *const *, const char * );
extern int	getsubopt( char **, char *const *, char ** );

extern char	*optarg;
extern int	opterr;
extern int	optind;
#ifdef	SET_OPTOPT
extern int	optopt;
#endif

static int		status = EXIT_SUCCESS;	/* exit status */
static const char	*progname;	/* program name, for diagnostics */

static const char	usage[] =	/* usage message */
		    "Usage:\t%s -a -b -i file -o c,d,name=file -- files...";

static const char	optlist[] = "bi:o:a";	/* options, for getopt() */

static char *const	sublist[] =	/* -o suboptions, for getsubopt() */
	{
#define	D_INDEX		0
	"d",
#define	NAME_INDEX	1
	"name",
#define	C_INDEX		2
	"c",
	NULL
	};

static int	aflag = 0;		/* dummy flag (default false) */
static int	bflag = 1;		/* dummy flag (default true) */
static int	cflag = 0;		/* dummy flag (default false) */
static int	dflag = 1;		/* dummy flag (default true) */
static char	*ifile = NULL;		/* dummy parameter for "-i" */
static char	*ofile = NULL;		/* dummy parameter for "-o name=" */

static void
Warn( const char *format, ... )
	{
	va_list	ap;

	assert(progname != NULL);
	fprintf( stderr, "%s: ", progname );

	va_start( ap, format );
	assert(format != NULL);
	vfprintf( stderr, format, ap );
	va_end( ap );

	fputc( '\n', stderr );
	status = EXIT_FAILURE;		/* important detail */
	return;
	}

int
main( int argc, char *argv[] )
	{
	register int		c;	/* return from getopt() */
	register const char	*afile;	/* file name from argument list */
	char			*subopts;	/* -> next suboption */
	char			*value;	/* value for suboption */
	register FILE		*fp;	/* stream for non-option args */

	assert(argc > 0);
	assert(argv != NULL);
	assert(argv[0] != NULL);
	assert(argv[argc] == NULL);

	/* Save "simple part" of argv[0] for use in diagnostic printing. */

	if ( (progname = strrchr( argv[0], '/' )) != NULL )
		++progname;
	else
		progname = argv[0];

	assert(progname != NULL);

	/* Process all options. */

	/* opterr = 0;			/* suppresses getopt() messages */

	while ( (c = getopt( argc, argv, optlist )) != EOF )
		{
#ifdef	DEBUG
		fprintf( stderr, "%s: processing option '%c'\n", progname, c
		       );
#endif

		switch ( c )
			{
		case 'a':
			aflag = 1;
			break;

		case 'b':
			bflag = 0;
			break;

		case 'i':
			assert(optarg != NULL);
			ifile = optarg;
			break;

		case 'o':
			subopts = optarg;	/* don't mess with getopt */
			assert(subopts != NULL);

			if ( subopts[0] == '\0' )
				Warn( "empty -o suboption list" );

			while ( subopts[0] != '\0' )
				{
#ifdef	DEBUG
				fprintf( stderr,
				       "%s: processing suboptions \"%s\"\n",
					 progname, subopts
				       );
#endif

				switch ( getsubopt( &subopts, sublist,
						    &value
						  )
				       )
					{
				case C_INDEX:
					cflag = 1;
					break;

				case D_INDEX:
					dflag = 0;
					break;

				case NAME_INDEX:
					if ( value != NULL )
						ofile = value;
					else
						Warn( "-o name=what?" );

					break;

				default:	/* case -1 */
					Warn( "unrecognized suboption" );
					break;
					}
				}

			break;

		default:		/* case '?' */
#ifdef	SET_OPTOPT
			Warn( "illegal option '%c'", optopt );
#else
			status = EXIT_FAILURE;
#endif
			assert(c == '?');
			break;
			}
		}

	if ( status != EXIT_SUCCESS )	/* don't proceed */
		{
		Warn( usage );
		assert(status == EXIT_FAILURE);
		exit( status );
		}

	/* The following is one way to default input or output. */

#ifdef	DEBUG
	if ( ifile != NULL )
		fprintf( stderr, "%s: ifile = \"%s\"\n", progname, ifile );
#endif

	if ( ifile != NULL
	  && strcmp( ifile, "-" ) != 0
	  && freopen( ifile, "r", stdin ) == NULL
	   )	{
		Warn( "error opening \"%s\"", ifile );
		assert(status == EXIT_FAILURE);
		exit( status );		/* lost standard input! */
		}

#ifdef	DEBUG
	if ( ofile != NULL )
		fprintf( stderr, "%s: ofile = \"%s\"\n", progname, ofile );
#endif

	if ( ofile != NULL )
		{
		if ( strcmp( ofile, "-" ) == 0 )	/* optional check */
			{
			Warn( "\"-o name=-\" not acceptable " );
			assert(status == EXIT_FAILURE);
			exit( status );
			}

		if ( freopen( ofile, "w", stdout ) == NULL )
			{
			Warn( "error creating \"%s\"", ofile );
			assert(status == EXIT_FAILURE);
			exit( status );		/* lost standard output! */
			}
		}

	/* Process remaining (non-option) arguments. */

	for ( ; optind < argc; ++optind )
		{
		afile = argv[optind];
		assert(afile != NULL);
#ifdef	DEBUG
		fprintf( stderr, "%s: afile = \"%s\"\n", progname, afile );
#endif

		if ( strcmp( afile, "-" ) == 0 )
                        fp = stdin;
		else if ( (fp = fopen( afile, "r" )) == NULL )
			Warn( "error opening \"%s\"", afile );

		if ( fp != NULL )
			{
			/* Applications access the file via "fp" here: */

			/* ... */

			/* We avoid closing stdin; not only does it cause a
			   problem for some implementations, but also it
			   would prevent successful use of "-" more than
			   once in the argument list. */

			if ( fp != stdin && fclose( fp ) != 0 )
				Warn( "error closing \"%s\"", afile );
			}
		}

	return status;
	}
