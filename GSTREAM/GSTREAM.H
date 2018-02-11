/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Kent Schumacher.  Not derived from licensed software.
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
**	GSTREAM Definitions
*/

#ifndef GSTREAM_H
#define GSTREAM_H (1)

#include <stdio.h>

typedef struct gstream GSTREAM;

/* GS internal error defines */
typedef enum {
	GS_BADFUNC,	/* Read in write context, or vice-versa	*/
	GS_BADARG,	/* Bad argument to function		*/
	GS_NOMEM,	/* malloc failure			*/
	GS_NOSEEK,	/* GSseek called with no gs->seek	*/
	GS_BADPOS,	/* Bad seek pos				*/
	GS_BUFSET	/* GSsetvbuf called twice, or too late	*/
} GSERRVAL;

typedef int	GSrfn(GSTREAM *gs, void *buf, int cnt);
typedef int	GSwfn(GSTREAM *gs, void *buf, int cnt);
typedef long	GSsfn(GSTREAM *gs, long cnt, int origin);
typedef int	GSxfn(GSTREAM *gs, GSERRVAL gs_error);

struct gstream {
	int				errno;	/* Status of stream */
	short				flags;	/* General purpose flags */
	long				ofs;	/* Offset in file */
	int				bsiz;	/* Stream buffering size */

	unsigned char *			buf;	/* Pointer to current buffer */
	int				cnt;	/* Space in current buffer */
	int				pos;	/* Position in current buffer */

	unsigned char *			ubuf;	/* Pointer to stream buffer */
	int				ucnt;	/* Space in stream buffer */
	int				upos;	/* Position in stream buffer */

	GSrfn *				read;	/* User supplied functions */
	GSwfn *				write;
	GSsfn *				seek;
	GSxfn *				xlat;
};

/*
** GSTREAM functions
*/

int	GSstartup(GSTREAM *gs, GSrfn *read, GSwfn *write, GSsfn *seek);
int	GSshutdown(GSTREAM *gs);
int	GSsetvbuf(GSTREAM *gs, void *buf, int mode, size_t size);
int	GSerrno(GSTREAM *gs);
void	GSclearerr(GSTREAM *gs);

/* input */
int	GSgetc(GSTREAM *gs);
int	GSungetc(int c, GSTREAM *gs);
size_t	GSread(void *ptr, size_t size, size_t nmemb, GSTREAM *gs);
int	GSseek(GSTREAM *gs, long offset, int whence);
long	GStell(GSTREAM *gs);

/* output */
int	GSputc(int c, GSTREAM *gs);
size_t	GSwrite(const void *ptr, size_t size, size_t nmemb, GSTREAM *gs);
int	GSflush(GSTREAM *gs);

/* internal */
int	GS__fill(GSTREAM *gs);
int	GS__flsh(GSTREAM *gs, int c);

/*
** GSTREAM macros
*/

#define	GSgetc(gs)	(((gs)->pos < (gs)->cnt) \
				? (gs)->buf[(gs)->pos++] \
				: GS__fill(gs))

#define	GSputc(c,gs)	(((gs)->pos < (gs)->bsiz) \
				? (gs)->buf[(gs)->pos++] = c \
				: GS__flsh((gs), c))

#define	GSerror(gs)	((gs)->errno)
#define	GSclearerr(gs)	(void)((gs)->errno = 0)
#define	GSseterr(gs,v)	(void)((gs)->errno = (v))

#endif /* GSTREAM_H */
