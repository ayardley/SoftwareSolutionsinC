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
	opendir() -- open a directory stream

	public-domain implementation for GS/OS, ORCA/C 2.0

	last edit:	10-Jan-1993	Gwyn@ARL.Army.Mil

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
#include	<string.h>

#include	<GSOS.h>
#include	<ORCA.h>

#include	"Q8.h"

#define	DIR_FILE_TYPE	0x0F		/* GS/OS Directory file type */
#define	PARM_BLK_SIZE	0x16		/* through storageType field */

DIR *
opendir( const char *dirname )
	{
	register char		*name = (char *)dirname;
	register GSString255	*namp;	/* -> GS/OS-formatted path */
	register OpenRecGS	*openp;	/* -> parameters for Open() */
	register DIR		*dirp;	/* -> malloc()ed storage */
	int			refnum;
	int			error;	/* errno to be set, if any */

	if ( name == 0 || name[0] == '\0' )
		{
		error = ENOENT;		/* no such file or directory */
		goto er3;
		}

	if ( strlen( name ) >= sizeof namp->text )
		{
		error = ENAMETOOLONG;	/* filename too long */
		goto er3;
		}

	if ( (namp = (GSString255 *)malloc( sizeof(GSString255) )) == 0 )
		{
		error = ENOMEM;		/* not enough memory */
		goto er3;
		}

	name = strcpy( (char *)namp->text, name );	/* (known to fit) */

	/* Convert POSIX pathname to GS/OS canonical path: */

	if ( !_Q8canon( name, (int)(sizeof namp->text) ) )
		{
		error = errno;		/* errno was set by _Q8canon() */
		goto er2;
		}

	namp->length = strlen( name );

	if ( name[1] == '\0' )		/* i.e., name is "/" */
		{			/* special handling for root dir. */
		free( (void *)namp );	/* not going to attempt Open("/") */

		if ( (dirp = (DIR *)malloc( sizeof(DIR) )) == 0 )
			{
			error = ENOMEM;	/* not enough memory */
			goto er3;
			}

		dirp->d_refnum = _Q8VOL;    /* special flag for root dir. */
		dirp->d_seq = 0;	/* first device has sequence # 1 */

		return dirp;
		}

	if ( (openp = (OpenRecGS *)malloc( PARM_BLK_SIZE )) == 0 )
		{
		error = ENOMEM;		/* not enough memory */
		goto er2;
		}

	openp->pCount = 8;		/* through storageType field */
	openp->pathname = namp;
	openp->requestAccess = readEnable;
	openp->resourceNumber = 0;	/* data fork */
	Open( openp );

	switch ( toolerror() )
		{
	case 0: 			/* no error occurred */
		refnum = openp->refNum;	/* save handle for open directory */

		if ( openp->fileType != DIR_FILE_TYPE
		  || openp->storageType != directoryFile
		   )	{
			error = ENOTDIR;	/* not a directory */
			goto er0;
			}

		if ( (dirp = (DIR *)malloc( sizeof(DIR) )) == 0 )
			{
			error = ENOMEM;	/* not enough memory */
			goto er0;
			}

		dirp->d_refnum = refnum;	/* handle for open dir. */
		dirp->d_seq = 0;	/* first entry has sequence # 1 */

		free( (void *)openp );
		free( (void *)namp );

		return dirp;

	case badPathSyntax:
	case pathNotFound:
	case volNotFound:
	case fileNotFound:
		error = ENOENT;		/* no such file or directory */
		goto er1;

	default:			/* miscellaneous I/O error */
		error = ENFILE;		/* too many open files in system */
		goto er1;
		}

	/* error handling: */

    er0:
	openp->pCount = 1;		/* through refNum field */
	openp->refNum = refnum;
	Close( openp );
    er1:
	free( (void *)openp );
    er2:
	free( (void *)namp );
    er3:
	errno = error;
#ifdef	DEBUG
	perror( "opendir" );
#endif
	return (DIR *)0;
	}
