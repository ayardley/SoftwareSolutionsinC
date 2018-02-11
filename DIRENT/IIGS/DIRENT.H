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
	<dirent.h> -- format of directory entries

	public-domain implementation for GS/OS, ORCA/C 2.0

	last edit:	16-Jan-1993	Gwyn@ARL.Army.Mil

	complies with the following standards:
		POSIX	SVID	XPG	AES

	POSIX allows addition of other identifiers starting with "d_".
 */

#ifndef	__dirent__
#define	__dirent__			/* (typedef is not idempotent) */

struct dirent				/* data from readdir() */
	{
	unsigned long	d_ino;		/* file serial number */
	char		d_name[256];	/* name of entry, NUL-terminated */
	};

typedef struct
	{
	int		d_refnum;	/* directory I/O handle,
					   or _Q8VOL if "root directory" */
	int		d_seq;		/* preceding sequence number */
	struct dirent	d_buf;		/* private buffer for entry */
	}	DIR;			/* stream data from opendir() */

/* Note: POSIX does not permit defining these as _Q8*() via macros! */
extern DIR		*opendir( const char * );
extern struct dirent	*readdir( DIR * );
extern void		rewinddir( DIR * );
extern int		closedir( DIR * );

#if defined(_XOPEN_SOURCE) || defined(_AES_SOURCE)	/* (also SVID) */
extern void	seekdir( DIR *, long );
extern long	telldir( DIR * );
#endif

#endif	/* __dirent__ */
