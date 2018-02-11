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
	telldir -- report directory stream position
		(Aztec C65 ProDOS version)

	last edit:	30-Apr-1993	Gwyn@ARL.Army.Mil
*/

#include	<sys/types.h>
#include	<dirent.h>
#include	<errno.h>

extern off_t	lseek();

#ifndef NULL
#define	NULL	0
#endif

#ifndef SEEK_CUR
#define	SEEK_CUR	1
#endif

off_t
telldir( dirp )				/* return offset of next entry */
	DIR	*dirp;			/* stream from opendir() */
	{
	if ( dirp == NULL || dirp->dd_buf == NULL )
		{
		errno = EFAULT;
		return -1;		/* invalid pointer */
		}

	if ( dirp->dd_loc < dirp->dd_size )	/* valid index */
		return ((struct dirent *)&dirp->dd_buf[dirp->dd_loc])->d_off;
	else				/* beginning of next directory block */
		return lseek( dirp->dd_fd, (off_t)0, SEEK_CUR );
	}
