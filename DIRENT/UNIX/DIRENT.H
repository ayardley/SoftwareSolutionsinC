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
	<dirent.h> -- definitions for POSIX directory access functions (SVR3)
		(PUBLIC-DOMAIN implementation for UNIX)

	last edit:	16-Aug-1993	Gwyn@ARL.Army.Mil

	Prerequisite:	<sys/types.h>	// alas; see below
*/

/* #include	<sys/types.h>		/* enable this if <sys/types.h>
					   can safely be multiply #included */
#include	<sys/dirent.h>		/* (split for SVR3 compatibility */

/* The DIR field names used to start with "dd_", but POSIX.1 disallows that. */
typedef struct
	{
	int	d_fd;			/* file descriptor */
	int	d_loc;			/* offset in block */
	int	d_size;			/* amount of valid data */
	char	*d_buf;			/* -> directory block */
	}	DIR;			/* stream data from opendir() */

extern DIR		*opendir();
extern struct dirent	*readdir();
extern off_t		telldir();	/* SVR3 addition */
extern void		seekdir();	/* SVR3 addition */
extern void		rewinddir();
extern int		closedir();

#define	DIRBUF		8192		/* buffer size for fs-indep. dirs */
	/* must in general be larger than the filesystem buffer size */

/* NULL used to be defined here (as 0); that was a mistake. */
