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
	dirent.c -- POSIX-compatible directory access functions
		(PUBLIC-DOMAIN AmigaDOS, Lattice C version)

	Created:	13-Jul-1987	MWM@Violet.Berkeley.Edu
	Last edit:	01-May-1993	Gwyn@ARL.Army.Mil

	Not having any AmigaDOS documentation, I tried not to change
	anything I wasn't fairly sure of.

	Restriction:  While one process has a given directory open,
	other processes will be unable to open that directory.
*/

#include	<dirent.h>		/* includes <libraries/dos.h>, .. */
#include	<errno.h>
#include	<string.h>

typedef char	*pointer;		/* generic pointer type */

extern void		free( pointer );
extern pointer		malloc( size_t );

extern int		Examine( FileLock *, FileInfoBlock * );
extern int		ExNext( FileLock *, FileInfoBlock * );
extern struct FileLock *Lock( char *, int );
extern void		UnLock( FileLock * );

DIR *
opendir( dirname )
	char		*dirname;
	{
	register DIR	*dirp;

	if ( dirname == NULL )
		{
#ifdef	ENOTDIR
		errno = ENOTDIR;
#endif
		return NULL;		/* invalid argument */
		}

	if ( (dirp = (DIR *)malloc( sizeof(DIR) )) == NULL )
		{
#ifdef	ENOMEM
		errno = ENOMEM;
#endif
		return NULL;
		}

	if ( (dirp->d_lock = Lock( dirname, ACCESS_READ )) == NULL
					/* if we can't examine it */
	  || !Examine( dirp->d_lock, &dirp->d_info )
					/* or it's not a directory */
	  || dirp->d_info.fib_DirEntryType < 0
	   )	{
		free( (pointer)dirp );
#ifdef	ENOTDIR
		errno = ENOTDIR;
#endif
		return NULL;
		}

	dirp->d_seq = 0;
	return dirp;
	}

struct dirent *
readdir( dirp )
	DIR			*dirp;
	{
	static struct dirent	result;

	if ( dirp == NULL || dirp->d_lock == NULL || dirp->d_seq < 0 )
		{
#ifdef	EFAULT
		errno = EFAULT;
#endif
		return NULL;		/* invalid pointer */
		}

	if ( !ExNext( dirp->d_lock, &dirp->d_info ) )
		return NULL;		/* end of directory */

	++dirp->d_seq;
	(void)strcpy( result.d_name, dirp->d_info.fib_FileName );
	return &result;
	}

off_t
telldir( dirp )
	DIR	*dirp;
	{
	if ( dirp == NULL || dirp->d_lock == NULL || dirp->d_seq < 0 )
		{
#ifdef	EFAULT
		errno = EFAULT;
#endif
		return -1;		/* invalid pointer */
		}

	return dirp->d_seq;
	}

void
seekdir( dirp, loc )
	DIR	*dirp;
	off_t	loc;
	{
	if ( dirp == NULL || dirp->d_lock == NULL || dirp->d_seq < 0 )
		{
#ifdef	EFAULT
		errno = EFAULT;
#endif
		return;			/* invalid pointer */
		}

	if ( dirp->d_seq > loc )
		rewinddir( dirp );

	while ( dirp->d_seq < loc && readdir( dirp ) != NULL )
		;

#ifdef	EINVAL
	if ( dirp->d_seq != loc )
		errno = EINVAL;		/* invalid offset */
#endif
	}

void
rewinddir( dirp )
	DIR	*dirp;
	{
	if ( dirp == NULL || dirp->d_lock == NULL || dirp->d_seq < 0 )
		{
#ifdef	EFAULT
		errno = EFAULT;
#endif
		return;			/* invalid pointer */
		}

	if ( Examine( dirp->d_lock, &dirp->d_info ) )
		dirp->d_seq = 0;
	else	{
		dirp->d_seq = -1;	/* make the next readdir fail */
#ifdef	EINVAL
		errno = EINVAL;		/* invalid offset (nonrewindable) */
#endif
		}
	}

int
closedir( dirp )
	DIR	*dirp;
	{
	if ( dirp == NULL || dirp->d_lock == NULL || dirp->d_seq < -1 )
		{
#ifdef	EFAULT
		errno = EFAULT;
#endif
		return -1;		/* invalid pointer */
		}

	UnLock( dirp->d_lock );
	free( (pointer)dirp );
	return 0;
	}
