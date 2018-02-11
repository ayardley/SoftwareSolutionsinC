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
		(Aztec C65 ProDOS version)

	last edit:	30-Apr-1993	Gwyn@ARL.Army.Mil
*/

#include	<sys/types.h>
#include	<dirent.h>
#include	<errno.h>

extern int	getdents();		/* SVR3 system call emulation */

#ifndef NULL
#define	NULL	0
#endif

struct dirent *
readdir( dirp )
	register DIR		*dirp;	/* stream from opendir() */
	{
	register struct dirent	*dp;	/* -> directory data */

	if ( dirp == NULL || dirp->dd_buf == NULL )
		{
		errno = EFAULT;
		return NULL;		/* invalid pointer */
		}

	do	{
		if ( dirp->dd_loc >= dirp->dd_size )	/* empty or obsolete */
			dirp->dd_loc = dirp->dd_size = 0;

		if ( dirp->dd_size == 0	/* need to refill buffer */
		  && (dirp->dd_size = getdents( dirp->dd_fd, dirp->dd_buf,
						DIRBUF * sizeof(char)
					      )
		     ) <= 0
		   )
			return NULL;	/* EOF or error */

		dp = (struct dirent *)&dirp->dd_buf[dirp->dd_loc];
		dirp->dd_loc += dp->d_reclen;
		}
	while ( dp->d_ino <= 0 );	/* skip header, empty slots */

	return dp;
	}
