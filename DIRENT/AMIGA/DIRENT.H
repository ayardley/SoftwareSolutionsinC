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
	<dirent.h> -- definitions for standard directory access functions
		(PUBLIC-DOMAIN AmigaDOS, Lattice C version)

	Created:	13-Jul-1987	MWM@Violet.Berkeley.Edu
	Last edit:	01-May-1993	Gwyn@ARL.Army.Mil
*/

#include	<exec/types.h>
#include	<libraries/dos.h>
#include	<libraries/dosextens.h>

struct dirent				/* data from readdir() */
	{
	long	d_ino;			/* unique file ID number (faked) */
	char	d_name[32];		/* name of file (incl. NUL term.) */
	};

typedef struct
	{
	off_t			d_seq;	/* current entry number */
	struct FileLock	       *d_lock;	/* lock on directory */
	struct FileInfoBlock	d_info;	/* default info block */
	}	DIR;			/* stream data from opendir() */

extern DIR		*opendir( char * );
extern struct direct	*readdir( DIR * );
extern off_t		telldir( DIR * );
extern void		seekdir( DIR *, off_t );
extern void		rewinddir( DIR * );
extern int		closedir( DIR * );
