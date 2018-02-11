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
	<dirent.h> -- definitions for POSIX directory access functions
		(PUBLIC-DOMAIN implementation for MS-DOS)

	last edit:	13-Oct-1993	Gwyn@ARL.Army.Mil
*/

#define	d_MAXNAMLEN	12	/* maximum filename length */

struct dirent {			/* data from readdir() */
	struct dirent	*d_next;	/* -> next entry, or NULL */
	long		d_ino;		/* entry index number */
	char		d_name[d_MAXNAMLEN + 1];	/* name */
};

typedef struct {
	struct dirent	*d_next;	/* -> next entry, or NULL */
	struct dirent	*d_first;	/* -> first entry, or NULL */
}	DIR;			/* stream from opendir() */

extern DIR		*opendir(const char *);
extern struct dirent	*readdir(DIR *);
extern void		rewinddir(DIR *);
extern int		closedir(DIR *);

#ifndef _POSIX_SOURCE			/* nonstandard additions */
extern long		telldir(DIR *);
extern void		seekdir(DIR *, long);
#endif
