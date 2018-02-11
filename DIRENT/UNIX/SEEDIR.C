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
	seekdir -- reposition a directory stream
		(PUBLIC-DOMAIN implementation for UNIX)

	last edit:	02-May-1993	Gwyn@ARL.Army.Mil

	An unsuccessful seekdir() will in general alter the current
	directory position; beware.

	NOTE:	4.nBSD directory compaction makes seekdir() & telldir()
		practically impossible to do right.  Avoid using them!
*/

#include	<sys/types.h>
#include	<dirent.h>
#include	<errno.h>

extern int	errno;			/* should be in <errno.h> */

extern off_t	lseek();

#ifndef	NULL
#define	NULL	0
#endif

#ifndef	SEEK_SET
#define	SEEK_SET	0
#endif

typedef int	bool;			/* Boolean data type */
#define	false	0
#define	true	1

void
seekdir( dirp, loc )
	register DIR	*dirp;		/* stream from opendir() */
	register off_t	loc;		/* position from telldir() */
	{
	register bool	rewind;		/* "start over when stymied" flag */

	if ( dirp == NULL || dirp->d_buf == NULL )
		{
		errno = EFAULT;
		return;			/* invalid pointer */
		}

	/* A (struct dirent)'s d_off is an invented quantity on 4.nBSD
	   NFS-supporting systems, so it is not safe to lseek() to it. */

	/* Monotonicity of d_off is heavily exploited in the following. */

	/* This algorithm is tuned for modest directory sizes.  For
	   huge directories, it might be more efficient to read blocks
	   until the first d_off is too large, then back up one block,
	   or even to use binary search on the directory blocks.  I
	   doubt that the extra code for that would be worthwhile. */

	if ( dirp->d_loc >= dirp->d_size	/* invalid index */
	  || ((struct dirent *)&dirp->d_buf[dirp->d_loc])->d_off > loc
					/* too far along in buffer */
	   )
		dirp->d_loc = 0;	/* reset to beginning of buffer */
	/* else save time by starting at current dirp->d_loc */

	for ( rewind = true; ; )
		{
		register struct dirent	*dp;

		/* See whether the matching entry is in the current buffer. */

		if ( (dirp->d_loc < dirp->d_size	/* valid index */
		   || readdir( dirp ) != NULL	/* next buffer read */
		   && (dirp->d_loc = 0, true)	/* beginning of buffer set */
		     )
		  && (dp = (struct dirent *)&dirp->d_buf[dirp->d_loc])->d_off
			<= loc		/* match possible in this buffer */
		   )	{
			for ( /* dp initialized above */ ;
			      (char *)dp < &dirp->d_buf[dirp->d_size];
			      dp = (struct dirent *)((char *)dp + dp->d_reclen)
			    )
				if ( dp->d_off == loc )
					{	/* found it! */
					dirp->d_loc = (char *)dp - dirp->d_buf;
					return;
					}

			rewind = false;	/* no point in backing up later */
			dirp->d_loc = dirp->d_size;	/* set end of buffer */
			}
		else			/* whole buffer past matching entry */
			if ( !rewind )
				{	/* no point in searching further */
				errno = EINVAL;
				return;	/* no entry at specified loc */
				}
			else	{	/* rewind directory and start over */
				rewind = false;	/* but only once! */

				dirp->d_loc = dirp->d_size = 0;

				if ( lseek( dirp->d_fd, (off_t)0, SEEK_SET )
					!= 0
				   )
					return;	/* errno already set (EBADF) */

				if ( loc == 0 )
					return; /* save time */
				}
		}
	}
