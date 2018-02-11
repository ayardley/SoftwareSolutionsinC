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
	closedir -- close a directory stream
		(Aztec C65 ProDOS version)

	last edit:	30-Apr-1993	Gwyn@ARL.Army.Mil
*/

#include	<sys/types.h>
#include	<dirent.h>
#include	<errno.h>

typedef void	*pointer;

extern void	free();
extern int	close();

#ifndef NULL
#define	NULL	0
#endif

int
closedir( dirp )
	register DIR	*dirp;		/* stream from opendir() */
	{
	register int	fd;

	if ( dirp == NULL || dirp->dd_buf == NULL )
		{
		errno = EFAULT;
		return -1;		/* invalid pointer */
		}

	fd = dirp->dd_fd;		/* bug fix thanks to R. Salz */
	free( (pointer)dirp->dd_buf );
	free( (pointer)dirp );
	return close( fd );
	}
