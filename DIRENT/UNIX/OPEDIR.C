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
	opendir -- open a directory stream
		(PUBLIC-DOMAIN implementation for UNIX)

	last edit:	20-Jan-1994	Gwyn@ARL.Army.Mil
*/

#include	<sys/types.h>
#include	<sys/stat.h>
#include	<dirent.h>
#include	<errno.h>

extern int	errno;			/* should be in <errno.h> */

#ifdef BSD_SYSV
#define open	_open			/* avoid emulation overhead */
#endif

#ifdef	__STDC__
typedef void	*pointer;
#else
typedef char	*pointer;
#endif

extern void	free();
extern pointer	malloc();
extern int	close(), open(), stat();

#ifndef NULL
#define	NULL	0
#endif

#ifndef O_RDONLY
#define	O_RDONLY	0
#endif

#ifndef S_ISDIR				/* macro to test for directory file */
#define	S_ISDIR( mode )		(((mode) & S_IFMT) == S_IFDIR)
#endif

DIR *
opendir( dirname )
	char			*dirname;	/* name of directory */
	{
	register DIR		*dirp;	/* -> malloc'ed storage */
	register int		fd;	/* file descriptor for read */
	/* The following is static just to keep the stack small. */
	static struct stat	sbuf;	/* result of stat() */

	/* stat() before open() to avoid opening a device special file, etc.: */

	if ( stat( dirname, &sbuf ) != 0 || !S_ISDIR( sbuf.st_mode ) )
		{
		errno = ENOTDIR;
		return NULL;		/* not a directory */
		}

	/* know that dirname refers to a directory; no need for O_NDELAY: */

	if ( (fd = open( dirname, O_RDONLY )) < 0 )
		return NULL;		/* errno set by open() */

	if ( (dirp = (DIR *)malloc( sizeof(DIR) )) == NULL
	  || (dirp->d_buf = (char *)malloc( DIRBUF * sizeof(char) )) == NULL
	   )	{
		register int	serrno = errno;
					/* errno set to ENOMEM by sbrk() */

		if ( dirp != NULL )
			free( (pointer)dirp );

		(void)close( fd );
		errno = serrno;
		return NULL;		/* not enough memory */
		}

	dirp->d_fd = fd;
	dirp->d_loc = dirp->d_size = 0;	/* refill needed */

	return dirp;
	}
