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
	getdents -- get directory entries in a file system independent format
			(SVR3 system call emulation)
		(PUBLIC-DOMAIN implementation for UNIX)

	last edit:	16-Aug-1993	Gwyn@ARL.Army.Mil

	This function emulates the getdents() system call provided
	with UNIX System V Release 3 and later.

	This single source file supports several different methods of
	getting directory entries from the operating system.  Define
	whichever one of the following describes your system:

	UFS	original UNIX filesystem (14-character name limit)
	BFS	4.2BSD (also 4.3BSD) native filesystem (long names)
	NFS	getdirentries() system call

	Also define any of the following flags that are pertinent:

	ATT_SPEC	check user buffer address for longword alignment
	BSD_SYSV	BRL UNIX System V emulation environment on 4.nBSD
	INT_SIGS	<signal.h> thinks that signal handlers have
			return type int (rather than the standard void)
	SIZ_LONG	if type of "sizeof" is unsigned long
	NEG_DELS	deleted entries have negative inode number rather
			than the usual 0 (also see readdir.c)
	UNK		have _getdents() system call, but kernel may not
			support it:

	If your C library has a getdents() system call interface, but you
	can't count on all kernels on which your application binaries may
	run to support it, change the system call interface name to
	_getdents() and define "UNK" to enable the system-call validity
	test in this "wrapper" around _getdents().

	If your system has a getdents() system call that is guaranteed
	to always work, you shouldn't be using this getdents() EMULATION at all;
	use the native getdents().
*/

#include	<sys/types.h>
#ifdef BSD_SYSV
#include	<sys/bsddir.h>		/* BSD flavor, not System V */
#else
#include	<sys/dir.h>
#undef	MAXNAMLEN			/* avoid conflict with SVR3 */
	/* Good thing we don't need to use the DIRSIZ() macro! */
#ifdef d_ino				/* 4.3BSD/NFS using d_fileno */
#undef	d_ino				/* (not absolutely necessary) */
#else
#define	d_fileno	d_ino		/* (struct direct) member */
#endif
#endif
#include	<sys/dirent.h>
#include	<sys/stat.h>
#ifdef UNK
#ifndef UFS
#include "***** ERROR ***** UNK applies only to UFS"
/* One could do something similar for getdirentries(), but I didn't bother. */
#endif
#include	<signal.h>
#endif
#include	<errno.h>

extern int	errno;			/* should be in <errno.h> */

#if defined(UFS) + defined(BFS) + defined(NFS) != 1	/* sanity check */
#include "***** ERROR ***** exactly one of UFS, BFS, or NFS must be defined"
#endif

#ifdef BSD_SYSV
struct dirent	__dirent;		/* (just for the DIRENTBASESIZ macro) */
#endif

#ifdef UFS
#define	RecLen( dp )	sizeof(struct direct)	/* fixed-length entries */
#else	/* BFS || NFS */
#define	RecLen( dp )	(dp)->d_reclen	/* variable-length entries */
#endif

#ifdef NFS
#ifdef BSD_SYSV
#define	getdirentries	_getdirentries	/* package hides this system call */
#endif
extern int	getdirentries();
static long	dummy;			/* getdirentries() needs basep */
#define	GetBlock( fd, buf, n )	getdirentries( fd, buf, (n) * sizeof(char), \
					       &dummy \
					     )
#else	/* UFS || BFS */
#ifdef BSD_SYSV
#define read	_read			/* avoid emulation overhead */
#endif
extern int	read();
#define	GetBlock( fd, buf, n )	read( fd, buf, (n) * sizeof(char) )
#endif

#ifdef UNK
extern int	_getdents();		/* actual system call */
#endif

extern int	fstat();
extern off_t	lseek();
extern char	*strncpy();

#ifndef DIRBLKSIZ
#define	DIRBLKSIZ	4096		/* directory file read buffer size */
#endif

/* ***** NOTE: DIRBLKSIZ must not exceed DIRBUF ***** */

#ifndef NULL
#define	NULL	0
#endif

#ifndef SEEK_CUR
#define	SEEK_CUR	1
#endif

#ifndef S_ISDIR				/* macro to test for directory file */
#define	S_ISDIR( mode )		(((mode) & S_IFMT) == S_IFDIR)
#endif

#ifdef UFS

/*
	The following routine is necessary to handle DIRSIZ-long entry names.
	Thanks to Richard Todd for pointing this out.
*/

static int
NameLen( name )				/* return # chars in embedded name */
	char		*name;		/* -> name embedded in struct direct */
	{
	register char	*s;		/* -> name[.] */
	register char	*stop = &name[DIRSIZ];	/* -> past end of name field */

	for ( s = &name[1];		/* (empty names are impossible) */
	      *s != '\0'		/* not NUL terminator */
	   && ++s < stop;		/* < DIRSIZ characters scanned */
	    )
		;

	return (int)(s - name);		/* # valid characters in name */
	}

#else	/* BFS || NFS */

#ifdef	SIZ_LONG
extern unsigned long	strlen();
#else
extern unsigned		strlen();
#endif

#define	NameLen( name )	strlen( name )	/* names are always NUL-terminated */

#endif

#ifdef UNK
static enum	{ maybe, no, yes }	state = maybe;
					/* does _getdents() work? */

#ifdef INT_SIGS
#define	RET_SIG	int
#else
#define	RET_SIG	void
#endif

/*ARGSUSED*/
static RET_SIG
sig_catch( sig )
	int	sig;			/* must be SIGSYS */
	{
	state = no;			/* attempted _getdents() faulted */
#ifdef INT_SIGS
	return 0;			/* telling lies */
#endif
	}
#endif	/* UNK */

int
getdents( fildes, buf, nbyte )		/* returns # bytes read;
					   0 on EOF, -1 on error */
	int			fildes;	/* directory file descriptor */
	char			*buf;	/* where to put the (struct dirent)s */
#ifdef	SIZ_LONG
	unsigned long		nbyte;	/* size of buf[] (should be size_t) */
#else
	unsigned		nbyte;	/* size of buf[] (should be size_t) */
#endif
	{
	int			serrno;	/* entry errno */
	off_t			offset;	/* initial directory file offset */
	/* The following are static just to keep the stack small. */
	static struct stat	statb;	/* fstat() info */
	static union
		{
		char		dblk[DIRBLKSIZ
#ifdef UFS
				     +1	/* for last entry name terminator */
#endif
				    ];
					/* directory file block buffer */
		struct direct	dummy;	/* just for alignment */
		}	u;		/* (avoids having to malloc()) */
	register struct direct	*dp;	/* -> u.dblk[.] */
	register struct dirent	*bp;	/* -> buf[.] */

#ifdef UNK
	if ( state == yes )		/* _getdents() is known to work */
		return _getdents( fildes, buf, nbyte );

	if ( state == maybe )		/* first time only */
		{
		RET_SIG		(*shdlr)();	/* entry SIGSYS handler */
		register int	retval;	/* return from _getdents() if any */

		shdlr = signal( SIGSYS, sig_catch );
		retval = _getdents( fildes, buf, nbyte );	/* try it */
		(void)signal( SIGSYS, shdlr );

		if ( state == maybe )	/* SIGSYS did not occur */
			{
			state = yes;	/* so _getdents() must have worked */
			return retval;
			}
		}

	/* state == no; perform emulation */
#endif

	if ( buf == NULL
#ifdef ATT_SPEC
	  || (unsigned long)buf % sizeof(long) != 0	/* ugh */
#endif
	   )	{
		errno = EFAULT;		/* invalid pointer */
		return -1;
		}

	if ( fstat( fildes, &statb ) != 0 )
		return -1;		/* errno set by fstat() */

	if ( !S_ISDIR( statb.st_mode ) )
		{
		errno = ENOTDIR;	/* not a directory */
		return -1;
		}

	if ( (offset = lseek( fildes, (off_t)0, SEEK_CUR )) < 0 )
		return -1;		/* errno set by lseek() */

#ifdef BFS				/* no telling what remote hosts do */
	if ( (unsigned long)offset % DIRBLKSIZ != 0 )
		{
		errno = ENOENT;		/* file pointer probably misaligned */
		return -1;
		}
#endif

	serrno = errno;			/* save entry errno */

	for ( bp = (struct dirent *)buf; bp == (struct dirent *)buf; )
		{			/* convert next directory block */
		int	size;

		do	size = GetBlock( fildes, u.dblk, DIRBLKSIZ );
		while ( size == -1 && errno == EINTR );

		if ( size <= 0 )
			return size;	/* EOF or error (EBADF) */

		for ( dp = (struct direct *)u.dblk;
		      (char *)dp < &u.dblk[size];
		      dp = (struct direct *)((char *)dp + RecLen( dp ))
		    )	{
#ifndef UFS
			if ( dp->d_reclen <= 0 )
				{
				errno = EIO;	/* corrupted directory */
				return -1;
				}
#endif

#ifdef NEG_DELS
			if ( dp->d_fileno >= 0 )
#else
			if ( dp->d_fileno != 0 )
#endif
				{	/* non-empty; copy to user buffer */
				register int	reclen =
					DIRENTSIZ( NameLen( dp->d_name ) );

				if ( (char *)bp + reclen > &buf[nbyte] )
					{
					errno = EINVAL;
					return -1;	/* buf too small */
					}

				bp->d_ino = dp->d_fileno;
				bp->d_off = offset + ((char *)dp - u.dblk);
				bp->d_reclen = reclen;

				{
#ifdef UFS
				/* Is the following kludge ugly?  You bet. */

				register char	save = dp->d_name[DIRSIZ];
					/* save original data */

				dp->d_name[DIRSIZ] = '\0';
					/* ensure NUL termination */
#endif
				(void)strncpy( bp->d_name, dp->d_name,
					       reclen - DIRENTBASESIZ
					     );	/* adds NUL padding */
#ifdef UFS
				dp->d_name[DIRSIZ] = save;
					/* restore original data */
#endif
				}

				bp = (struct dirent *)((char *)bp + reclen);
				}
			}

#if !(defined(BFS) || defined(sun))	/* 4.2BSD screwed up; fixed in 4.3BSD */
		if ( (char *)dp > &u.dblk[size] )
			{
			errno = EIO;	/* corrupted directory */
			return -1;
			}
#endif
		}

	errno = serrno;			/* restore entry errno */
	return (int)((char *)bp - buf);	/* return # bytes read */
	}
