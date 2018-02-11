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
	readdir -- read next entry from a directory stream
		(PUBLIC-DOMAIN implementation for MS-DOS)

	last edit:	13-Oct-1993	Gwyn@ARL.Army.Mil
*/

#include <dirent.h>
#include <errno.h>
#include <stddef.h>

#ifndef	EFAULT
#define	EFAULT	EDOM
#endif

struct dirent *
readdir(register DIR *dirp)
{
	register struct dirent	*dp;	/* -> next entry */

	if (dirp == NULL) {
		errno = EFAULT;
		return -1;		/* invalid pointer */
	}

	if ((dp = dirp->d_next) != NULL) {
		dirp->d_next = dp->d_next;
	}

	return dp;			/* NULL when list exhausted */
}
