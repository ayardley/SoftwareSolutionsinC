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
			(SVR3 system call emulation) (Aztec C65 ProDOS version)

	last edit:	30-Apr-1993	Gwyn@ARL.Army.Mil
*/

#include	<sys/types.h>
#include	<sys/dir.h>
#include	<sys/dirent.h>
#include	<errno.h>

#define	RecLen( dp )	sizeof(struct direct)	/* fixed-length entries */

extern int	read();
#define	GetBlock( fd, buf, n )	read( fd, buf, (unsigned)n )

extern int	tolower();
extern off_t	lseek();

#ifndef NULL
#define	NULL	0
#endif

#ifndef SEEK_CUR
#define	SEEK_CUR	1
#endif

int
getdents( fildes, buf, nbyte )		/* returns # bytes read;
					   0 on EOF, -1 on error */
	int			fildes;	/* directory file descriptor */
	char			*buf;	/* where to put the (struct dirent)s */
	unsigned		nbyte;	/* size of buf[] */
	{
	off_t			offset;	/* initial directory file offset */
	union	{
		char		dblk[DIRBLKSIZ];
					/* directory file block buffer */
		struct direct	dummy;	/* just for alignment */
		}	u;		/* (avoids having to malloc()) */
	register struct direct	*dp;	/* -> u.dblk[.] */
	register struct dirent	*bp;	/* -> buf[.] */

	if ( buf == NULL )	/* no alignment constraint for Aztec C65 */
		{
		errno = EFAULT;		/* invalid pointer */
		return -1;
		}

	if ( (offset = lseek( fildes, (off_t)0, SEEK_CUR )) < 0 )
		return -1;		/* errno set by lseek() */

	for ( bp = (struct dirent *)buf; bp == (struct dirent *)buf; )
		{			/* convert next directory block */
		int	size;

		if ( (size = GetBlock( fildes, u.dblk, DIRBLKSIZ )) <= 0 )
			return size;	/* EOF or error (EBADF) */

		for ( dp = (struct direct *)(u.dblk + 2 * sizeof(daddr_t));
					/* skip block linkage pointers */
		      (char *)dp < &u.dblk[size];
		      dp = (struct direct *)((char *)dp + RecLen( dp ))
		    )	{
			if ( dp->d_namlen != 0 )
				{	/* non-empty; copy to user buffer */
				register int	reclen =
					DIRENTSIZ( dp->d_namlen & NAMELENMASK );

				if ( (char *)bp + reclen > &buf[nbyte] )
					{
					errno = EINVAL;
					return -1;	/* buf too small */
					}

				switch ( dp->d_namlen >> STSHIFT )
					{
				case ST_VOLKEY:
				case ST_SUBKEY:
					bp->d_ino = -1L;	/* kludge */
					break;

				default:
					bp->d_ino = dp->d_keyptr;
					break;
					}

				bp->d_off = offset + ((char *)dp - u.dblk);
				bp->d_reclen = reclen;

				/* translate filename to lower case */
				{
					register char	*bnp, *dnp;
					register int	i;

					bnp = bp->d_name;
					dnp = dp->d_name;

					for ( i = 0; i < dp->d_namlen; ++i )
						*bnp++ = tolower( *dnp++ );

					for ( i += DIRENTBASESIZ;
					      i < reclen;
					      ++i
					    )	/* NUL padding */
						*bnp++ = '\0';
				}

				bp = (struct dirent *)((char *)bp + reclen);
				}
			}
		}

	return (char *)bp - buf;	/* return # bytes read */
	}
