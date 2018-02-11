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
	rewinddir -- rewind a directory stream
		(Aztec C65 ProDOS version)

	last edit:	30-Apr-1993	Gwyn@ARL.Army.Mil

	This is not simply a call to seekdir(), because seekdir()
	will use the current buffer whenever possible and we need
	rewinddir() to forget about buffered data.
*/

#include	<sys/types.h>
#include	<dirent.h>
#include	<errno.h>

extern off_t	lseek();

#ifndef NULL
#define	NULL	0
#endif

#ifndef SEEK_SET
#define	SEEK_SET	0
#endif

void
rewinddir( dirp )
	register DIR	*dirp;		/* stream from opendir() */
	{
	if ( dirp == NULL || dirp->dd_buf == NULL )
		{
		errno = EFAULT;
		return;			/* invalid pointer */
		}

	(void)lseek( dirp->dd_fd, (off_t)0, SEEK_SET );	/* may set errno */
	dirp->dd_size = getdents( dirp->dd_fd, dirp->dd_buf,
				  DIRBUF * sizeof(char)
				);
	dirp->dd_loc = 0;		/* no need to skip header */
	}
