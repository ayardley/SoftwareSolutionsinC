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
	<errno.h> -- system error numbers

	public-domain implementation for GS/OS, ORCA/C 2.0
	(replaces ORCA/C 2.0 header)

	last edit:	13-Feb-1993	Gwyn@ARL.Army.Mil

	complies with the following standards:
		Std C	POSIX	SVID	XPG	AES

	The C standard allows addition of other macros starting with "E".
 */

#ifndef	__errno__
#define	__errno__	/* ORCA/C interlock (not strictly necessary) */

extern int	errno;		/* error number from library functions */

/* error codes 1..11 must agree with the ORCA/C 2.0 definitions: */
/* C Standard requires the following: */
#define	EDOM		1	/* domain error */
#define	ERANGE		2	/* result too large */
/* POSIX.1 requires the following: */
#define	ENOMEM		3	/* not enough memory */
#define	ENOENT		4	/* no such file or directory */
#define	EIO		5	/* input/output error */
#define	EINVAL		6	/* invalid argument */
#define	EBADF		7	/* bad file descriptor */
#define	EMFILE		8	/* too many open files */
#define	EACCES		9	/* permission denied */
#define	EEXIST		10	/* file exists */
#define	ENOSPC		11	/* no space left on device */

/* POSIX.1 requires the following beyond what ORCA/C 2.0 defined: */
#define	E2BIG		12	/* argument list too long */
#define	EAGAIN		13	/* resource temporarily unavailable */
#define	EBUSY		14	/* resource busy */
#define	ECHILD		15	/* no child processes */
#define	EDEADLK		16	/* resource deadlock avoided */
#define	EFAULT		17	/* bad address */
#define	EFBIG		18	/* file too large */
#define	EINTR		19	/* interrupted system call */
#define	EISDIR		20	/* is a directory */
#define	EMLINK		21	/* too many links */
#define	ENAMETOOLONG	22	/* filename too long */
#define	ENFILE		23	/* too many open files in system */
#define	ENODEV		24	/* no such device */
#define	ENOEXEC		25	/* exec format error */
#define	ENOLCK		26	/* no locks available */
#define	ENOSYS		27	/* function not implemented */
#define	ENOTDIR		28	/* not a directory */
#define	ENOTEMPTY	29	/* directory not empty */
#define	ENOTTY		30	/* inappropriate I/O control operation */
#define	ENXIO		31	/* no such device or address */
#define	EPERM		32	/* operation not permitted */
#define	EPIPE		33	/* broken pipe */
#define	EROFS		34	/* read-only file system */
#define	ESPIPE		35	/* invalid seek */
#define	ESRCH		36	/* no such process */
#define	EXDEV		37	/* improper link */

/* SVID requires the following beyond POSIX.1: */
#define	EBADMSG		38	/* bad message */
#define	EIDRM		39	/* identifier removed */
#define	ELIBACC		40	/* (reserved for system use) */
#define	ELIBBAD		41	/* (reserved for system use) */
#define	ELIBEXEC	42	/* (reserved for system use) */
#define	ELIBMAX		43	/* (reserved for system use) */
#define	ELIBSCN		44	/* (reserved for system use) */
#define	ELOOP		45	/* too many levels of symbolic links */
#define	ENOMSG		46	/* no message of the desired type */
#define	ENOSR		47	/* no stream resources */
#define	ENOSTR		48	/* not a stream */
#define	ENOTBLK		49	/* block device required */
#define	EOVERFLOW	50	/* (reserved for system use) */
#define	EPROTO		51	/* protocol error */
#define	ERESTART	52	/* (reserved for system use) */
#define	ESTRPIPE	53	/* (reserved for system use) */
#define	ETXTBSY		54	/* text file busy */
#define	ETIME		55	/* stream I/O control timeout */

/* following definition to support a BUG in ORCA/C 2.0; should be fixed: */
#define EACCESS 	EACCES	/* (misspelled in ORCA/C 2.0 <errno.h>) */

#endif	/* __errno__ */
