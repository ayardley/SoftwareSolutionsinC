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
	closedir() -- close a directory stream

	public-domain implementation for GS/OS, ORCA/C 2.0

	last edit:	09-Jan-1993	Gwyn@ARL.Army.Mil

	complies with the following standards:
		POSIX	SVID	XPG	AES
 */
/*#define	DEBUG	/* enables information report upon failure */

#pragma	lint	-1
#pragma	noroot

#include	"dirent.h"
#include	"errno.h"
#ifdef	DEBUG
#include	<stdio.h>
#endif
#include	<stdlib.h>

#include	<GSOS.h>
#include	<ORCA.h>

#include	"Q8.h"

int
closedir( register DIR *dirp )
	{
	RefNumRecGS	params;		/* parameters for GS/OS Close() */
	int		error;		/* errno to be set, if any */

	if ( dirp == 0 )
		{
		error = EFAULT;		/* bad address */
		goto er1;
		}

	if ( (params.refNum = dirp->d_refnum) < 0
	  || dirp->d_seq < -1		/* set to -1 upon I/O error */
	   )				/* not an open directory stream */
		goto er0;		/* dirp probably not safe to free */

	dirp->d_refnum = -1;		/* safety net */
	dirp->d_seq = -1;

	free( (void *)dirp );

	if ( params.refNum == _Q8VOL )
		return 0;		/* special handling for root dir. */

	params.pCount = 1;
	Close( &params );

	if ( toolerror() == 0 )		/* no error occurred */
		return 0;		/* success */
	/* else fall through to er0 */

	/* error handling: */

    er0:
	error = EBADF;			/* bad file descriptor */
    er1:
	errno = error;
#ifdef	DEBUG
	perror( "closedir" );
#endif
	return -1;
	}
