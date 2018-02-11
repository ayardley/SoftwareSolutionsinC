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
	seekdir -- reposition a directory stream
		(PUBLIC-DOMAIN implementation for MS-DOS)

	last edit:	19-Jan-1994	Gwyn@ARL.Army.Mil
*/

#include <dirent.h>
#include <errno.h>
#include <stddef.h>

#ifndef	EFAULT
#define	EFAULT	EDOM
#endif

#ifndef	EINVAL
#define	EINVAL	ERANGE
#endif

void
seekdir(register DIR *dirp, register long loc)
{
	if (dirp == NULL) {
		errno = EFAULT;
		return;			/* invalid pointer */
	}

	if (loc < 0L) {
		errno = EINVAL;
		return;			/* invalid argument */
	}

	rewinddir(dirp);

	do {
		if (telldir(dirp) == loc) {
			return;		/* success */
		}
	} while (readdir(dirp) != NULL);

	/* at end of directory */

	if (telldir(dirp) != loc) {
		errno = EINVAL;
		return;			/* invalid argument */
	}

	return;				/* success at EOF */
}
