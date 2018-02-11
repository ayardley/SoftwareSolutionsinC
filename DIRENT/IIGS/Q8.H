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
	"Q8.h" -- GPLib run-time support interface private definitions

	public-domain implementation for Standard C, GS/OS

	last edit:	15-Feb-1993	Gwyn@ARL.Army.Mil

	This header is NOT intended to be installed in the "usual place" for
	system header files, but rather just kept in the GPLib source
	directory for inclusion by the GPLib function implementations.

	WARNING:  Not idempotent; include at most once.
 */

/* It is okay to include system headers in this private header file. */
#include	<stdarg.h>
#include	"sys.types.h"
#include	"sys.stat.h"

#include	<TYPES.h>

/* The following implement the functions having analogous names without the
   _Q8 prefix; these are used by the GPLib implementation of other library
   functions and thus require names that are reserved for the implementation
   by the C standard.  (Actually, POSIX is not as strict as Standard C in
   this regard, but still it is a good idea to avoid internal library
   linkage that could accidentally involve an application extern symbol. */

extern int	_Q8close( int );
extern int	_Q8execve( const char *, const char *const *,
			   const char *const *
			 );
extern int	_Q8fcntl( int, int, ... );
extern char    *_Q8getcwd( char *, int );
extern int	_Q8open( const char *, int, ... );
extern ssize_t	_Q8read( int, void *, size_t );
/* extern void	_Q8seekdir( DIR *, long );	/* declare where used */
extern int	_Q8stat( const char *, struct stat * );
extern ssize_t	_Q8write( int, const void *, size_t );

/* Run-time support functions: */

/* extern void	_Q8assert( const char * );
					/* called upon assertion failure */
extern int	_Q8bits( unsigned short );
					/* converts POSIX mode to GS/OS */
extern int	_Q8canon( char*, int );	/* converts POSIX path to OS form */
extern void	_Q8cleanup( void );	/* STDIO support function */
extern int	_Q8get_heap( void );	/* allocates pseudo-heap (once) */
extern unsigned short
		_Q8mode( int );		/* converts GS/OS mode to POSIX */
extern char    *_Q8name( int );		/* converts f.d. to pathname */
extern time_t	_Q8time( TimeRec * );	/* converts GS/OS time to POSIX */
extern int	_Q8vfcntl( int, int, va_list );	/* implements fcntl() */
extern int	_Q8vopen( const char *, int, va_list );	/* implts. open() */

/* Run-time support data, types, etc. */

extern const char	*_Q8heap;	/* bottom of simulated heap */
extern const char	*_Q8brkval;	/* current simulated break */
#define	_Q8HEAP_SIZE	0x40000		/* size of simulated heap (256Kb) */

#define	_Q8VOL		32767		/* special "refNum" for root dir. */

extern char	**_Q8environ;		/* environment vector */

/* The following describes the "file descriptor" support in ORCA/C 2.0: */
typedef struct
	{
	int	_Q8refNum;		/* GS/OS file handle */
	int	_Q8flags;		/* open mode flags (O_BINARY etc) */
	}	_Q8file;		/* emulated Open File Table entry */

/* _Q8FILES must agree with OPEN_MAX in <fcntl.h>: */
#define	_Q8FILES	30		/* # of emulated file descriptors */
extern _Q8file	_Q8files[_Q8FILES];	/* emulated Open File Table */

/* Support for IEEE Std 754 floating point (SANE, FPE): */

#define	_Q8RaiseFPE()	((void)raise( 2 ))	/* 2 == ORCA/C 2.0 SIGFPE */
typedef union
	{
	double; 		/* what the data actually is */
	unsigned	_Q8s[4];	/* what we pretend it is */
	unsigned long	_Q8l[2];	/* another way to look at it */
	}	_Q8du;		/* type pun for "double" bit tests below: */
#define	_Q8IsNaN(d)	((((_Q8du *)&(d))->_Q8s[3] & 0x7FF0) == 0x7FF0 && \
			((((_Q8du *)&(d))->_Q8l[1] & 0x000FFFFF) \
						   != 0x00000000 || \
			  ((_Q8du *)&(d))->_Q8l[0] != 0x00000000))
				/* test for NaN */
#define	_Q8ChkNaN(d)	(_Q8IsNaN( d ) ? _Q8RaiseFPE() : (void)0)
				/* verify not a NaN */
#define	_Q8SetNaN(d)	((void)(((_Q8du *)&(d))->_Q8l[1] = 0xFFFFFFFF, \
				((_Q8du *)&(d))->_Q8l[0] = 0xFFFFFFFF))
				/* store a quiet NaN */
#define	_Q8IsInf(d)	(((_Q8du *)&(d))->_Q8l[1] == 0x7FF00000 && \
			 ((_Q8du *)&(d))->_Q8l[0] == 0x00000000)
				/* test for Infinity */
#define	_Q8ChkInf(d)	(_Q8IsInf( d ) ? _Q8RaiseFPE() : (void)0)
				/* verify not an Infinity */
#define	_Q8SetMInf(d)	((void)(((_Q8du *)&(d))->_Q8l[1] = 0xFFF00000, \
				((_Q8du *)&(d))->_Q8l[0] = 0x00000000))
				/* store -Infinity */
#define	_Q8SetPInf(d)	((void)(((_Q8du *)&(d))->_Q8l[1] = 0x7FF00000, \
				((_Q8du *)&(d))->_Q8l[0] = 0x00000000))
				/* store +Infinity */

/* Programming aids: */

#define	_Q8Max( a, b )	((a) > (b) ? (a) : (b))			/* UNSAFE */
#define	_Q8Min( a, b )	((a) < (b) ? (a) : (b)) 		/* UNSAFE */
