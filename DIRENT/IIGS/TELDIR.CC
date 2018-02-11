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
	telldir() -- report directory stream position

	public-domain implementation for GS/OS, ORCA/C 1.2

	last edit:	04-Jan-1993	Gwyn@ARL.Army.Mil

	complies with the following standards:
		SVID Issue 3
		XPG Issue 3
		AES Revision A

	This emulation is intended solely to support existing UNIX
	applications and the UNIX-compatible C library implementation.
	This function should NOT be used in new applications on any system.
	telldir() is not required for conformance with IEEE Std 1003.1-1990,
	as it cannot be implemented reasonably on systems that compact
	directories.  XPG should NOT have required seekdir() or telldir()!
 */
/*#define DEBUG   /* enables information report if function reports failure */

#pragma lint		-1
#pragma optimize	-1
#pragma noroot

#include	"dirent.h"
#include	"errno.h"
#ifdef	DEBUG
#include	<stdio.h>
#endif

long					/* return "offset" of next entry */
telldir( register DIR *dirp )
	{
	if ( dirp == 0 || dirp->d_refnum < 0 || dirp->d_seq < 0 )
		{
		errno = EBADF;		/* not an open directory stream */
#ifdef	DEBUG
		perror( "telldir" );
#endif
		return -1L;
		}

	return (long)dirp->d_seq;	/* works even for root directory */
	}
