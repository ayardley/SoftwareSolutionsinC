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
	seekdir() -- reposition a directory stream

	public-domain implementation for Standard C

	last edit:	04-Jan-1993	Gwyn@ARL.Army.Mil

	complies with the following standards:
		SVID Issue 3
		XPG Issue 3
		AES Revision A

	An unsuccessful seekdir() may in general alter the current directory
	position; beware.  (On some systems, such as GS/OS, it does not.)

	This emulation is intended solely to support existing UNIX
	applications and the UNIX-compatible C library implementation.
	This function should NOT be used in new applications on any system.
	seekdir() is not required for conformance with IEEE Std 1003.1-1990,
	as it cannot be implemented reasonably on systems that compact
	directories.  XPG should NOT have required seekdir() or telldir()!

	Since a POSIX-conforming implementation of rewinddir() is not
	allowed to call an external function named seekdir(), the cleanest
	way to implement both rewinddir() and seekdir() is as calls to the
	common support function _Q8seekdir().
 */
#ifdef	__ORCAC__
#pragma lint		-1
#pragma optimize	-1
#pragma noroot
#endif

#include	"dirent.h"

extern void	_Q8seekdir( DIR *, long );

void
seekdir( DIR *dirp, long loc )
	{
	_Q8seekdir( dirp, loc );
	}
