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
	readdir() -- read next entry from a directory stream

	public-domain implementation for GS/OS, ORCA/C 2.0

	last edit:	10-Jan-1993	Gwyn@ARL.Army.Mil

	complies with the following standards:
		POSIX	SVID	XPG	AES

	Note that the "root directory", POSIX name "/", requires special
	handling under GS/OS.  (Another thing UNIX got right.)
 */
/*#define	DEBUG	/* enables information report upon failure */

#pragma	lint	-1
#pragma	noroot

#include	"dirent.h"
#include	<errno.h>
#ifdef	DEBUG
#include	<stdio.h>
#endif
#include	<stdlib.h>
#include	<string.h>

#include	<GSOS.h>
#include	<ORCA.h>

#include	"Q8.h"

#define	GETD_BLK_SIZE	0x10		/* through entryNum field */
#define	INFO_BLK_SIZE	0x08		/* through devName field */
#define	VOL_BLK_SIZE	0x0A		/* through volName field */

static struct dirent *
read_dir( register DIR *dirp )		/* genuine GS/OS directory entry */
	{
	register ResultBuf255	*namp;	/* -> GS/OS-formatted dir. entry */
	register DirEntryRecGS	*getdp;	/* -> params. for GetDirEntry() */
	int			error = errno;	/* errno to set, if any */

	if ( (namp = (ResultBuf255 *)malloc( sizeof(ResultBuf255) )) == 0 )
		{
		error = ENOMEM;		/* not enough memory */
		goto er3;
		}

	namp->bufSize = sizeof namp->bufString.text;

	if ( (getdp = (DirEntryRecGS *)malloc( GETD_BLK_SIZE )) == 0 )
		{
		error = ENOMEM;		/* not enough memory */
		goto er2;
		}

	getdp->pCount = 6;		/* through entryNum field */
	getdp->refNum = dirp->d_refnum;
	getdp->base = 1;		/* relative ... */
	getdp->displacement = 1;	/* ... next entry number */
	getdp->name = namp;		/* for returned entry name */
	GetDirEntry( getdp );

	switch ( toolerror() )
		{
	case 0:				/* no error occurred */
		{
		int			len = namp->bufString.length;
		register struct dirent	*dp = &dirp->d_buf;	/* buffer */

		dirp->d_seq = getdp->entryNum;	/* sequence number, aka: */
		dp->d_ino = (long)dirp->d_seq;	/* ...file serial number */
		(void)strncpy( dp->d_name, (char *)namp->bufString.text, len
			     );
		dp->d_name[len] = '\0';	/* string terminator */

		free( (void *)getdp );
		free( (void *)namp );

		errno = error;		/* just in case */
		return dp;		/* success */
		}

	case endOfDir:			/* end of directory (not error) */
		/* leave dirp->d_seq unchanged, for telldir() */
		/* leave errno unchanged as req'd by IEEE Std 1003.1-1990 */
		goto er1;

	case paramRangeErr:
		error = EINVAL;		/* invalid argument */
		goto er0;

	default:
		error = EBADF;		/* miscellaneous I/O error */
		goto er0;
		}

	/* error handling: */

    er0:
	dirp->d_seq = -1;
    er1:
	free( (void *)getdp );
    er2:
	free( (void *)namp );
    er3:
	errno = error;
#ifdef	DEBUG
	perror( "read_dir" );
#endif
	return (struct dirent *)0;
	}

static struct dirent *
read_vol( register DIR *dirp )		/* fake entry: GS/OS volume name */
	{
/* To avoid extra memory allocation, these chunks do double duty; sorry! */
	register void		*namp;	/* -> GS/OS-formatted names */
#define	nam32p	((ResultBuf32 *)namp)	/* -> GS/OS 32-character name */
#define	nam255p ((ResultBuf255 *)namp)	/* -> GS/OS 255-character name */
	register void		*paramp;	/* -> syscall parameters */
#define	infop	((DInfoRecGS *)paramp)	/* -> parameters for DInfo() */
#define	volp	((VolumeRecGS *)paramp)	/* -> parameters for Volume() */
	register struct dirent	*dp = &dirp->d_buf;	/* name buffer */
	int			len;	/* length of entry name */
	int			error = errno;	/* errno to set, if any */

	if ( (namp = malloc( _Q8Max( sizeof(ResultBuf32),	/* loser */
				     sizeof(ResultBuf255)	/* winner */
				   )
			   )
	     ) == 0
	   )	{
		error = ENOMEM;		/* not enough memory */
		goto er3;
		}

	nam32p->bufSize = sizeof nam32p->bufString.text;

	if ( (paramp = malloc( _Q8Max( INFO_BLK_SIZE, VOL_BLK_SIZE ) )) == 0
	   )	{
		error = ENOMEM;		/* not enough memory */
		goto er2;
		}

	infop->pCount = 2;		/* through devName field */
	infop->devNum = dirp->d_seq + 1;	/* next device number */
	infop->devName = nam32p;	/* for returned device name */
	DInfo( infop );

	if ( toolerror() != 0 )		/* s.b. invalidDevNum */
		/* leave dirp->d_seq unchanged, for telldir() */
		/* leave errno unchanged as req'd by IEEE Std 1003.1-1990 */
		goto er1;

	/* Record device name as "good enough" in case Volume() fails. */

	len = nam32p->bufString.length;
	dirp->d_seq = infop->devNum;	/* sequence number, aka: */
	dp->d_ino = (long)dirp->d_seq;	/* ...file serial number */
	(void)strncpy( dp->d_name, (char *)nam32p->bufString.text, len );
	dp->d_name[len] = '\0'; 	/* string terminator */

	nam255p->bufSize = sizeof nam255p->bufString.text;

	volp->pCount = 2;		/* through volName field */
	/* skating on thin ice to share the following, but should work: */
	volp->devName = &nam32p->bufString;	/* returned device name */
	volp->volName = nam255p;	/* for returned volume name */
	Volume( volp );

	if ( toolerror() == 0 )
		{			/* was able to obtain volume name */
		len = nam255p->bufString.length;
		(void)strncpy( dp->d_name, (char *)nam255p->bufString.text,
			       len
			     );
		dp->d_name[len] = '\0';	/* string terminator */
		}
	/* else ignore Volume() failure; just return the device name */

	free( (void *)paramp );		/* aka "infop" and "volp" */
	free( (void *)namp );		/* aka "nam32p" and "nam255p" */

	errno = error;			/* just in case */
	return dp;			/* success */

	/* error handling: */

    er0:
	dirp->d_seq = -1;
    er1:
	free( (void *)paramp );		/* aka "infop" and "volp" */
    er2:
	free( (void *)namp );		/* aka "nam32p" and "nam255p" */
    er3:
	errno = error;
#ifdef	DEBUG
	perror( "read_vol" );
#endif
	return (struct dirent *)0;
	}

struct dirent *
readdir( register DIR *dirp )
	{
	if ( dirp == 0 || dirp->d_refnum < 0 || dirp->d_seq < 0 )
		{
		errno = EBADF;		/* not an open directory stream */
#ifdef	DEBUG
		perror( "readdir" );
#endif
		return (struct dirent *)0;
		}

	if ( dirp->d_refnum == _Q8VOL )
		return read_vol( dirp );	/* fake root directory */
	else
		return read_dir( dirp );	/* any other directory */
	}
