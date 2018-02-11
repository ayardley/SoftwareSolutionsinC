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
		(Aztec C65 ProDOS version)

	last edit:	30-Apr-1993	Gwyn@ARL.Army.Mil

	An unsuccessful seekdir() will in general alter the current
	directory position; beware.
*/

#include	<sys/types.h>
#include	<dirent.h>
#include	<errno.h>

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

	if ( dirp == NULL || dirp->dd_buf == NULL )
		{
		errno = EFAULT;
		return;			/* invalid pointer */
		}

	/* Monotonicity of d_off is heavily exploited in the following. */

	/* This algorithm is tuned for modest directory sizes.  For
	   huge directories, it might be more efficient to read blocks
	   until the first d_off is too large, then back up one block,
	   or even to use binary search on the directory blocks.  I
	   doubt that the extra code for that would be worthwhile. */

	if ( dirp->dd_loc >= dirp->dd_size	/* invalid index */
	  || ((struct dirent *)&dirp->dd_buf[dirp->dd_loc])->d_off > loc
					/* too far along in buffer */
	   )
		dirp->dd_loc = 0;	/* reset to beginning of buffer */
	/* else save time by starting at current dirp->dd_loc */

	for ( rewind = true; ; )
		{
		register struct dirent	*dp;

		/* See whether the matching entry is in the current buffer. */

		if ( (dirp->dd_loc < dirp->dd_size	/* valid index */
		   || readdir( dirp ) != NULL	/* next buffer read */
		   && (dirp->dd_loc = 0, true)	/* beginning of buffer set */
		     )
		  && (dp = (struct dirent *)&dirp->dd_buf[dirp->dd_loc])->d_off
			<= loc		/* match possible in this buffer */
		   )	{
			for ( /* dp initialized above */ ;
			      (char *)dp < &dirp->dd_buf[dirp->dd_size];
			      dp = (struct dirent *)((char *)dp + dp->d_reclen)
			    )
				if ( dp->d_off == loc )
					{	/* found it! */
					dirp->dd_loc =
						(char *)dp - dirp->dd_buf;
					return;
					}

			rewind = false;	/* no point in backing up later */
			dirp->dd_loc = dirp->dd_size;	/* set end of buffer */
			}
		else			/* whole buffer past matching entry */
			if ( !rewind )
				{	/* no point in searching further */
				errno = EINVAL;
				return;	/* no entry at specified loc */
				}
			else	{	/* rewind directory and start over */
				rewind = false;	/* but only once! */

				dirp->dd_loc = 0;
				dirp->dd_size = 0;

				if ( lseek( dirp->dd_fd, (off_t)0, SEEK_SET )
					!= 0
				   )
					return;	/* errno already set (EBADF) */

				if ( loc == 0 )
					return; /* save time */
				}
		}
	}
