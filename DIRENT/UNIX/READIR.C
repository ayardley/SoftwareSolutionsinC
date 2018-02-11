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
	readdir -- read next entry from a directory stream
		(PUBLIC-DOMAIN implementation for UNIX)

	last edit:	02-May-1993	Gwyn@ARL.Army.Mil

	Define the following flag if it is pertinent:

	NEG_DELS	deleted entries have negative inode number rather
			than the usual 0 (also see getdents.c)
*/

#include	<sys/types.h>
#include	<dirent.h>
#include	<errno.h>

extern int	errno;			/* should be in <errno.h> */

extern int	getdents();		/* SVR3 system call, or emulation */

#ifndef NULL
#define	NULL	0
#endif

struct dirent *
readdir( dirp )
	register DIR		*dirp;	/* stream from opendir() */
	{
	register struct dirent	*dp;	/* -> directory data */

	if ( dirp == NULL || dirp->d_buf == NULL )
		{
		errno = EFAULT;
		return NULL;		/* invalid pointer */
		}

	do	{
		if ( dirp->d_loc >= dirp->d_size )	/* empty or obsolete */
			dirp->d_loc = dirp->d_size = 0;

		if ( dirp->d_size == 0	/* need to refill buffer */
		  && (dirp->d_size = getdents( dirp->d_fd, dirp->d_buf,
					       DIRBUF * sizeof(char)
					     )
		     ) <= 0
		   )
			return NULL;	/* EOF or error */

		dp = (struct dirent *)&dirp->d_buf[dirp->d_loc];
		dirp->d_loc += dp->d_reclen;
		}
#ifdef NEG_DELS
	while ( dp->d_ino < 0 );	/* don't trust getdents() */
#else
	while ( dp->d_ino == 0 );	/* don't trust getdents() */
#endif

	return dp;
	}
