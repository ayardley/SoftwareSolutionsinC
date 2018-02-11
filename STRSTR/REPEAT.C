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
	repeat -- copies text from stdin to stdout, removing white space
		and underlining repeated sequences of a minimum specified
		length from 1 to 100 inclusive (default 3)

	Usage:	$ repeat [algorithm] [min_length] < ciphertext > worksheet

	The following 3-phase string-searching algorithms are available:

		Name	Description

		sc	system C library implementation of strstr()
		ni	naive implementation of "brute-force" search
		bf	careful implementation of "brute-force" search
		kmp	Knuth-Morris-Pratt
		bm	Boyer-Moore (default)
		rk	Rabin-Karp
		qs	Sunday's "quick search"
		nm	always non-matching (for timing overhead)

	The "sc" algorithm isn't actually 3 phases but is just the system
	strstr() function.  To exclude it, define LOWAL as 0 when compiling.

	NOTE:  This isn't the fastest solution for this problem, but it is
	acceptably fast for typical cryptanalytic applications, and it
	serves as an example of the use of 3-phase string search algorithms.
	By timing execution for each of the algorithms using the same input
	file, one can get an idea of their performance in this particular
	application, which is neither the best nor the worst possible case.

	The original name was "repeats", which was shortened to 6 characters
	for export.

	If you are having difficulty timing the execution of this utility,
	define TIMING when compiling, and start/stop timestamps (and the
	algorithm used) will be logged on the standard error output.

	If you cannot conveniently redirect standard input and standard output
	files on the command line (e.g., Turbo C++ Visual Edition for Windows),
	define NO_REDIRECTION when compiling, in which case input will be taken
	from file "repeat.in" and output written to "repeat.X" where X is
	replaced by the algorithm name (e.g., "repeat.bf".
 */
#ifndef	LOWAL
#define	LOWAL	1		/* 0 to include strstr(), 1 otherwise */
#endif
	
#include	<ctype.h>
#include	<limits.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#ifdef	TIMING
#include	<time.h>
#endif

#define	WIDTH	50			/* printout line width */
#define	DEFPAT	3			/* default repeated-seq. length */
#define	MAXPAT	100			/* max. supported pattern length */

#if	UINT_MAX > 150000
#define	MAXMSG	150000u			/* max. supported input length */
#else
#define	MAXMSG	50000u			/* max. supported input length */
#endif

static char	*msg;			/* input text */
static char	*rpt;			/* repeat flags */
static char	*pat;			/* search pattern (repeated text) */
static size_t	n;			/* length of message */
static int	lpat = DEFPAT;		/* minimum seq. length to flag */

extern void	ni_setpat( const char * );
extern char	*ni_search( const char * );
extern void	ni_clrpat( void );

extern void	bf_setpat( const char * );
extern char	*bf_search( const char * );
extern void	bf_clrpat( void );

extern void	kmp_setpat( const char * );
extern char	*kmp_search( const char * );
extern void	kmp_clrpat( void );

extern void	bm_setpat( const char * );
extern char	*bm_search( const char * );
extern void	bm_clrpat( void );

extern void	rk_setpat( const char * );
extern char	*rk_search( const char * );
extern void	rk_clrpat( void );

extern void	qs_setpat( const char * );
extern char	*qs_search( const char * );
extern void	qs_clrpat( void );

extern void	nm_setpat( const char * );
extern char	*nm_search( const char * );
extern void	nm_clrpat( void );

struct afp				/* algorithm function pointers */
	{
	const char	*lname;		/* name for use on command line */
	const char	*uname;		/* upper-case mapping of lname */
	const char	*descr;		/* description for usage msg. */
	void		(*setpat)( const char * );	/* set pattern */
	char		*(*search)( const char * );	/* find in text */
	void		(*clrpat)( void ); 	/* finished with pattern */
	}	alg[] =
	{
	"sc",	"SC",	"system C library implementation of strstr()",
		NULL,		NULL,		NULL,
	"ni",	"NI",	"naive implementation of \"brute-force\" search",
		ni_setpat,	ni_search,	ni_clrpat,
	"bf",	"BF",	"careful implementation of \"brute-force\" search",
		bf_setpat,	bf_search,	bf_clrpat,
	"kmp",	"KMP",	"Knuth-Morris-Pratt",
		kmp_setpat,	kmp_search,	kmp_clrpat,
#define	DEFAL	4			/* default algorithm (bm): */
	"bm",	"BM",	"Boyer-Moore (default)",
		bm_setpat,	bm_search,	bm_clrpat,
	"rk",	"RK",	"Rabin-Karp",
		rk_setpat,	rk_search,	rk_clrpat,
	"qs",	"QS",	"Sunday's \"quick search\"",
		qs_setpat,	qs_search,	qs_clrpat,
	"nm",	"NM",	"always non-matching (for timing overhead)",
		nm_setpat,	nm_search,	nm_clrpat,
	};

static int	al = DEFAL;		/* selected search algorithm */
#define	MAXAL	(sizeof alg / sizeof(struct afp))	/* max. al + 1 */

#define	strsetpat	alg[al].setpat
#define	strsearch	alg[al].search
#define	strclrpat	alg[al].clrpat

static char	*progname;		/* "repeats" or similar */

static void				/* prints error message then dies */
Fatal( const char *mess )		/* message (NULL for usage msg.) */
	{
	if ( mess != NULL )
		fprintf( stderr, "%s -%s: %s\n",
			 progname, alg[al].lname, mess
		       );
	else	{
		fprintf( stderr,
	   "Usage:\t%s [algorithm] [min_length] < ciphertext > worksheet\n",
			 progname
		       );

		fprintf( stderr, "\talgorithm is one of:\n" );

		for ( al = LOWAL; al < MAXAL; ++al )
			fprintf( stderr, "\t\t%s\t%s\n",
				 alg[al].lname, alg[al].descr
			       );

		fprintf( stderr,
			 "\tmin_length must be from 1 to %lu, inclusive.\n",
			 (unsigned long)MAXPAT
		       );
		}

	exit( EXIT_FAILURE );
	}

static void
PutChar( int c )			/* error-checking putchar() */
	{
	if ( putchar( c ) == EOF )
		Fatal( "error writing output" );

	return;
	}

int
main( int argc, char *argv[] )
	{
	register int	c;
	register int	i;
	register char	*t;
	register size_t	m;
	register size_t	lim;
	register size_t	k;
	int		flagged;	/* for flagging optimization */
#ifdef	TIMING
	time_t		timestamp;

	time( &timestamp );
	fputs( ctime( &timestamp ), stderr );
#endif

	/* Save "simple part" of program name for diagnostics. */

	if ( (progname = strrchr( argv[0], '/' )) != NULL )
		++progname;
	else
		progname = argv[0];

	/* Determine the string-search algorithm to be used. */

	if ( argc > 1 && !isdigit( argv[1][0] ) )
		{
		/* Some systems may map arguments to upper case,
		   so we check both lower- and upper-case names. */

		for ( al = LOWAL; al < MAXAL; ++al )
			if ( strcmp( argv[1], alg[al].lname ) == 0
			  || strcmp( argv[1], alg[al].uname ) == 0
			   )
				break;	/* al now matches user selection */

		if ( al >= MAXAL )
			Fatal( (const char *)NULL );

		++argv;
		--argc;
		}

#ifdef NO_REDIRECTION
	if ( freopen( "repeat.in", "r", stdin ) == NULL )
		Fatal( "can't open \"repeat.in\"" );

	{
	char	outname[26];

	(void)sprintf( outname, "repeat.%s", alg[al].lname );

	if ( freopen( outname, "w", stdout ) == NULL )
		{
		(void)sprintf( outname, "can't create \"repeat.%s\"",
			       alg[al].lname
			     );
		Fatal( outname );
		}
	}
#endif

	/* Determine the minimum repeat length to be flagged. */

	if ( argc > 1 )
		if ( argc > 2
		  || (lpat = atoi( argv[1] )) <= 0 || lpat > MAXPAT
		   )
			Fatal( (const char *)NULL );

	/* Read the message text into an internal buffer. */

	if ( (msg = (char *)malloc( (MAXMSG+1) * sizeof(char) )) == NULL )
		Fatal( "not enough memory" );

	for ( n = 0; (c = getchar()) != EOF; )
		if ( !isspace( c ) )	/* (remove white space) */
			if ( n < MAXMSG )
				msg[n++] = c;
			else
				Fatal( "input too long" );

	msg[n] = '\0';

	if ( (t = (char *)realloc( msg, (n + 1) * sizeof(char) )) != NULL )
		msg = t;		/* waste not, want not */

	/* Allocate remaining arrays and initialize all repeat flags. */

	if ( (rpt = (char *)malloc( (n + 1) * sizeof(char) )) == NULL
	  || (pat = (char *)malloc( (lpat + 1) * sizeof(char) )) == NULL
	   )
		Fatal( "not enough memory" );

	for ( m = n; m > 0; )
		rpt[--m] = ' ';		/* initially, not a repeat */

	pat[lpat] = '\0';		/* (do once only) */

#ifdef	TIMING
	fprintf( stderr, "%s: algorithm: %s\n", progname, alg[al].descr );
#endif

	/* Try each pattern location against remainder of message. */

	for ( m = 0; m + lpat < n; ++m )
		{
		t = &msg[m];

		for ( i = 0; i < lpat; ++i )
			pat[i] = t[i];

/*		pat[lpat] = '\0';	/* (already done) */

#if	LOWAL == 0
		if ( al != 0 )
#endif
			strsetpat( pat );

		for ( flagged = 0; ; )
			{
#if	LOWAL == 0
			if ( al == 0 )
				t = strstr( t + 1, pat );
			else
#endif
				t = strsearch( t + 1 );

			if ( t == NULL )
				break;

     			k = t - msg;

			/* Flag repeated sequence (both places). */

			for ( i = 0; i < lpat; ++i )
				rpt[k + i] = '-';

			if ( !flagged )
				{
				for ( i = 0; i < lpat; ++i )
					rpt[m + i] = '-';

				flagged = 1;
				}
			}

#if	LOWAL == 0
		if ( al != 0 )
#endif
			strclrpat();
		}

	/* Print text with each line followed by repeat flags. */

	PutChar( '\n' );

	for ( m = 0, lim = 0; lim < n; )
		{
		PutChar( (int)msg[lim] );

		if ( ++lim % WIDTH == 0 )
			{
			PutChar( '\n' );

			while ( m < lim )
				PutChar( (int)rpt[m++] );

			PutChar( '\n' );

			if ( lim < n )
				PutChar( '\n' );
			}
		}

	if ( n % WIDTH != 0 )
		{
		PutChar( '\n' );

		while ( m < n )
			PutChar( (int)rpt[m++] );

		PutChar( '\n' );
		}

#ifdef	TIMING
	time( &timestamp );
	fputs( ctime( &timestamp ), stderr );
#endif
	return EXIT_SUCCESS;
	}
