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
	<dirent.h> -- definitions for POSIX-compatible directory access funcs
		(Aztec C65 ProDOS version)

	last edit:	30-Apr-1993	Gwyn@ARL.Army.Mil

	Prerequisite:	<sys/types.h>
*/

#include	<sys/dirent.h>

typedef struct
	{
	int	dd_fd;			/* file descriptor */
	int	dd_loc;			/* offset in block */
	int	dd_size;		/* amount of valid data */
	char	*dd_buf;		/* -> directory block */
	}	DIR;			/* stream data from opendir() */

extern DIR		*opendir();
extern struct dirent	*readdir();
extern void		rewinddir();
extern int		closedir();
extern off_t		telldir();	/* SVR3 addition */
extern void		seekdir();	/* SVR3 addition */

#define	DIRBUF		364		/* buffer size for fs-indep. dirs */
