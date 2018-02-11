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
		(Aztec C65 ProDOS version)

	last edit:	30-Apr-1993	Gwyn@ARL.Army.Mil
*/

#include	<sys/types.h>
#include	<dirent.h>
#include	<errno.h>
#include	<fcntl.h>

typedef void	*pointer;

extern void	free();
extern pointer	malloc();
extern int	close(), getdents(), open();

#ifndef NULL
#define	NULL	0
#endif

DIR *
opendir( dirname )
	char		*dirname;	/* name of directory */
	{
	register DIR	*dirp;		/* -> malloc'ed storage */
	register int	fd;		/* file descriptor for read */

	if ( (fd = open( dirname, O_RDONLY )) < 0 )
		return NULL;		/* errno set by open() */

	if ( (dirp = (DIR *)malloc( sizeof(DIR) )) == NULL
	  || (dirp->dd_buf = (char *)malloc( DIRBUF * sizeof(char) )) == NULL
	   )	{
		if ( dirp != NULL )
			free( (pointer)dirp );

		(void)close( fd );
		errno = ENOMEM;
		return NULL;		/* not enough memory */
		}

	dirp->dd_fd = fd;
	dirp->dd_loc = 0;		/* no need to skip header */

	/* Special for ProDOS: check header entry rather than stat()ing */

	if ( (dirp->dd_size = getdents( fd, dirp->dd_buf, (unsigned)DIRBUF ))
		<= 0			/* EOF or error */
	  || ((struct dirent *)dirp->dd_buf)->d_ino != -1L	/* kludge */
	   )	{
		(void)closedir( dirp );
		errno = ENOTDIR;
		return NULL;		/* not a directory */
		}

	return dirp;
	}
