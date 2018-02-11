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
	_Q8seekdir() -- reposition a directory stream

	public-domain implementation for GS/OS, ORCA/C 2.0

	last edit:	10-Jan-1993	Gwyn@ARL.Army.Mil

	complies with the following standards:
		SVID	XPG	AES

	This module does the hard work for ULib's rewinddir() and seekdir().

	Since a POSIX-conforming implementation of rewinddir() is not
	allowed to call an external function named seekdir(), the cleanest
	way to implement both rewinddir() and seekdir() is as calls to the
	common support function _Q8seekdir().
 */
/*#define	DEBUG	/* enables information report upon malfunction */

#pragma	lint	-1
#pragma	noroot

#include	"dirent.h"
#include	<errno.h>
#ifdef	DEBUG
#include	<stdio.h>
#endif
#include	<stdlib.h>

#include	<GSOS.h>
#include	<ORCA.h>

#include	"Q8.h"

#define	PARM_BLK_SIZE	0x0E		/* through name field */

void
_Q8seekdir( register DIR *dirp, long loc )
	{
	register ResultBuf255	*namp;	/* -> GS/OS-formatted dirname */
	register DirEntryRecGS	*openp;	/* -> parameters for GS/OS Open() */
	int			error;	/* errno to be set, if any */

	if ( dirp == 0 || dirp->d_refnum < 0
	  || dirp->d_seq < -1		/* set to -1 on I/O error */
	   )	{
		error = EBADF;		/* not an open directory stream */
		goto er3;
		}

	if ( loc < 0L )
		{
		error = EINVAL;		/* invalid argument */
		goto er3;
		}

	if ( dirp->d_refnum == _Q8VOL )
		{			/* special handling for root dir. */
		dirp->d_seq = (int)loc;	/* all that's necessary */
		return;
		}

	if ( (namp = (ResultBuf255 *)malloc( sizeof(ResultBuf255) )) == 0 )
		{
		error = ENOMEM;		/* not enough memory */
		goto er3;
		}

	if ( (openp = (DirEntryRecGS *)malloc( PARM_BLK_SIZE )) == 0 )
		{
		error = ENOMEM;		/* not enough memory */
		goto er2;
		}

	openp->pCount = 5;	       /* through name field */
	openp->refNum = dirp->d_refnum;
	openp->base = 0;	       /* absolute ... */
	openp->displacement = (Word)loc;       /* ... entry number */
	namp->bufSize = sizeof(namp->bufString.text);
	openp->name = namp;	       /* not used but required anyway! */
	GetDirEntry( openp );

	switch ( toolerror() )
		{
	case 0:				/* no error occurred */
		dirp->d_seq = (int)loc;	/* here we are */
		error = 0;		/* no error */
		goto er1;

/*	case paramRangeErr:		/* not a valid entry number */
	default:			/* miscellaneous I/O error */
		error = EINVAL;		/* invalid argument */
		goto er0;
		}

	/* error handling; includes normal return point: */

    er0:
	dirp->d_seq = -1;
    er1:
	free( (void *)openp );
    er2:
	free( (void *)namp );
    er3:
	if ( error != 0 )
		{
		errno = error;
#ifdef	DEBUG
		perror( "_Q8seekdir" );
#endif
		}
	}
