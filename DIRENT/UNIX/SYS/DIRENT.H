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
	<sys/dirent.h> -- file system independent directory entry (SVR3)
		(PUBLIC-DOMAIN implementation for UNIX)

	last edit:	02-May-1993	Gwyn@ARL.Army.Mil

	prerequisite:	<sys/types.h>
*/

/* DAG -- the following was moved from <dirent.h>, which was the wrong place */
#define	MAXNAMLEN	512		/* maximum filename length */

/* The following selects either a non-portable C construct that saves space
   or a portable one that wastes space.  Either way, it is UNSAFE to copy a
   struct dirent, since in one case the declared size does not include all
   the data, while in the other case the directory access library might
   not actually allocate as much storage as indicated by the declared size. */
#if 1	/* change to 0 if (and only if) forced to by your compiler */
#define	_DNAME_LEN	0		/* not Standard C conformant */
#else
#define	_DNAME_LEN	MAXNAMLEN	/* Standard C conformant */
#endif

struct dirent				/* data from getdents()/readdir() */
	{
	long		d_ino;		/* inode number of entry */
	off_t		d_off;		/* offset of disk directory entry */
	unsigned short	d_reclen;	/* length of this record */
	char		d_name[_DNAME_LEN + 1];	/* name of file */
	};

#ifdef BSD_SYSV				/* (e.g., when compiling getdents.c) */
extern struct dirent	__dirent;	/* (not actually used) */
/* The following is portable, although rather silly. */
#define	DIRENTBASESIZ		(__dirent.d_name - (char *)&__dirent)

#else
/* The following nonportable ugliness could have been avoided by defining
   DIRENTSIZ and DIRENTBASESIZ to also have (struct dirent *) arguments.
   There shouldn't be any problem if you avoid using the DIRENTSIZ() macro. */

#define	DIRENTBASESIZ		(((struct dirent *)0)->d_name \
				- (char *)&((struct dirent *)0)->d_ino)
#endif

#define	DIRENTSIZ( namlen )	((DIRENTBASESIZ + sizeof(long) + (namlen)) \
				/ sizeof(long) * sizeof(long))

/* NAME_MAX used to be defined here as (MAXNAMLEN - 1); that was a mistake. */
