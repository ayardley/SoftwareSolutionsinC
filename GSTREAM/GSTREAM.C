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
**	GSTREAM Implementation Layer
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "../str_lst/str.h"
#include "gstream.h"

#define GS_INTBUF	(1 << 0)

#define GS_DEFAULT_SIZE	(128)

static int
GSxlat_error(GSTREAM *gs, GSERRVAL gs_error)
{
	if (gs->xlat) {
		return(gs->xlat(gs, gs_error));
	}
	return(0);
}

static int 
error_stub(GSTREAM *gs, void *buf, int cnt)
{
	GSseterr(gs, GSxlat_error(gs, GS_BADFUNC));
	return(-1);
}

int
GSstartup(GSTREAM *gs, GSrfn *read, GSwfn *write, GSsfn *seek)
{
	gs->buf   = NULL;
	gs->ubuf  = NULL;
	gs->bsiz  = GS_DEFAULT_SIZE;
	gs->pos   = GS_DEFAULT_SIZE + 1;
	gs->cnt   = 0;
	gs->ofs   = 0;
	gs->flags = 0;
	gs->errno = 0;
	gs->read  = read;
	gs->write = write;
	gs->seek  = seek;
	gs->xlat  = (GSxfn *)0;

	if (write && (read || seek)) {
		GSseterr(gs, GSxlat_error(gs, GS_BADARG));
		return -1;
	}
	if (!read) {
		read = error_stub;
	}
	if (!write) {
		write = error_stub;
	}

	return 0;
}

void
GSregister_xlat(GSTREAM *gs, GSxfn *xlat)
{
	gs->xlat = xlat;
}

int
GSshutdown(GSTREAM *gs)
{
	int rv = 0;

	if (gs->write != error_stub) {
		rv = GSflush(gs);
	}
	if (gs->ubuf) {
		if (gs->flags & GS_INTBUF) {
			free(gs->ubuf);
		}
		STRfree((char *)gs->buf);
	} else {
		if (gs->flags & GS_INTBUF) {
			free(gs->buf);
		}
	}
	return rv;
}

int
GSsetvbuf(GSTREAM *gs, void *buf, int mode, size_t size)
{
	if (gs->buf) {
		GSseterr(gs, GSxlat_error(gs, GS_BUFSET));
		return -1;
	}
	switch (mode) {
	case _IOLBF:
	case _IOFBF:
		if (size > 0) {
			gs->bsiz = size;
			if (buf) {
				gs->buf = buf;
				gs->pos = 0;
			}
			break;
		}
		/* Fall through */
	case _IONBF:
		gs->bsiz = 1;
		break;
	default:
		GSseterr(gs, GSxlat_error(gs, GS_BADARG));
		return -1;
	}
	return 0;
}

static int
create_buffer(GSTREAM *gs)
{
	if ((gs->buf = (unsigned char *)malloc(gs->bsiz)) == NULL) {
		GSseterr(gs, GSxlat_error(gs, GS_NOMEM));
		return 0;
	}
	gs->flags |= GS_INTBUF;
	return 1;
}

static void
kill_unget(GSTREAM *gs)
{
	STRfree((char *)gs->buf);

	gs->buf  = gs->ubuf;
	gs->pos  = gs->upos;
	gs->cnt  = gs->ucnt;

	gs->ubuf = NULL;
}

static int
GSfill(GSTREAM *gs)
{
	if (gs->buf == NULL) {
		if (!create_buffer(gs)) {
			return -1;
		}
	}

	gs->pos = 0;
	gs->cnt = gs->read(gs, gs->buf, gs->bsiz);

	if (gs->cnt > 0) {
		gs->ofs += gs->cnt;
	}

	return gs->cnt;
}

int
GS__fill(GSTREAM *gs)
{
	if (gs->ubuf) {
		kill_unget(gs);
	}
	if (gs->pos >= gs->cnt || gs->cnt <= 0) {
		if (GSfill(gs) < 1) {
			return EOF;
		}
	}
	return gs->buf[gs->pos++];
}

size_t
GSread(void *ptr, size_t msize, size_t nmemb, GSTREAM *gs)
{
	register size_t	needed;
	register char *	p;
	register int count;
	size_t total;
	size_t size;

	total = needed = (nmemb * msize);
	p = ptr;
	while (needed > 0) {

		while ((count = (gs->cnt - gs->pos)) > 0) {
			if (count > needed) {
				count = needed;
			}

			memcpy(p, (gs->buf + gs->pos), count);

			gs->pos += count;
			needed -= count;

			if (needed == 0) {
				return nmemb;
			}

			p += count;

			if (gs->ubuf == NULL) {
				break;
			}
			kill_unget(gs);
		}

		while (needed > gs->bsiz) {
			size = (needed < INT_MAX) ? needed : INT_MAX;
			size -= size % gs->bsiz;

			if ((count = gs->read(gs, p, size)) == -1) {
				return 0;
			}
         
			gs->cnt =  0; 
			gs->ofs += count;
			needed -= count;

			if (needed == 0) {
				return nmemb;
			}
			if (count != size) {
				return ((total - needed) / msize);
			}

			p += count;
		}

		switch (GSfill(gs)) {
		case -1: return 0;
		case  0: return ((total - needed) / msize);
		}
	}

	return(nmemb);
}

int
GSseek(GSTREAM *gs, long offset, int whence)
{
	long fpos;
	long bpos;

	if (gs->seek == NULL) {
		GSseterr(gs, GSxlat_error(gs, GS_NOSEEK));
		return -1;
	}

	switch (whence) {
	case SEEK_SET:
		break;
	case SEEK_CUR:
		offset += GStell(gs);
		break;
	case SEEK_END:
		offset = gs->seek(gs, offset, whence);
		if (offset == -1) {
			return -1;
		}
		break;
	default:
		GSseterr(gs, GSxlat_error(gs, GS_BADARG));
		return -1;
	}

	bpos = offset % gs->bsiz;
	fpos = offset - bpos;

	if (gs->ubuf) {
		kill_unget(gs);
	}

	if ((gs->cnt == 0) || (fpos != (gs->ofs - gs->cnt))) {
		if (gs->seek(gs, fpos, SEEK_SET) == -1) {
			return -1;
		}
		gs->ofs = fpos;
		if (GSfill(gs) == -1) {
			return -1;
		}
	}

	if (bpos > gs->cnt) {
		gs->pos = gs->cnt;
		GSseterr(gs, GSxlat_error(gs, GS_BADPOS));
		return -1;
	}

	gs->pos = bpos;
	return 0;
}

long
GStell(GSTREAM *gs)
{
	if (gs->buf && gs->cnt) {
		return (gs->ofs - gs->cnt + gs->pos);
	}
	return gs->ofs;
}

int
GSungetc(int c, GSTREAM *gs)
{
	if (c == EOF) {
		return EOF;
	}

	if (gs->ubuf == NULL) {
		gs->ubuf = gs->buf;
		gs->ucnt = gs->cnt;
		gs->upos = gs->pos;

		gs->buf  = NULL;
		gs->pos  = 0;
		gs->cnt  = 0;
	}

	if (gs->pos > 0) {
		gs->buf[--gs->pos] = c;
	} else {
		unsigned char *p;

		p = (unsigned char *)STRpush((char *)gs->buf, c);
		if (p == NULL) {
			return EOF;
		}
		gs->buf = p;
		gs->cnt++;
	}
   
	return c;
}

size_t
GSwrite(const void *ptr, size_t msize, size_t nmemb, GSTREAM *gs)
{
	register size_t needed;
	register int size;
	char *p;

	needed = msize * nmemb;
	p      = (char *)ptr;

	if (gs->buf == NULL) {
		if (!create_buffer(gs)) {
			return -1;
		}
		gs->pos = 0;
	} 

	while (needed > 0) {

		if ((size = gs->bsiz - gs->pos) > 0) {
			if (gs->pos > 0 || size > needed) {
				if (size > needed) {
					size = needed;
				}
      
				memcpy((gs->buf + gs->pos), p, size);
      
				needed   -= size;
				p        += size;
				gs->pos  += size;
			}
		}

		if (gs->pos >= gs->bsiz) {
			if (gs->write(gs, gs->buf, gs->bsiz) == -1) {
				return 0;
			}
			gs->pos = 0;
		}

		if (needed > gs->bsiz) {
			size = ((needed < INT_MAX) ? needed : INT_MAX);
   
			if (gs->write(gs, p, size) == -1) {
				return 0;
			}
   
			p      += size;
			needed -= size;
		}

	}
	return(nmemb);
}

int
GSflush(GSTREAM *gs)
{
	int rv;

	if (gs->pos == 0) {
		return 0;
	}
	rv = gs->write(gs, gs->buf, gs->pos);
	gs->pos = 0;
	return(rv);
}

int
GS__flsh(GSTREAM *gs, int c)
{
	if (gs->buf == NULL) {
		if (!create_buffer(gs)) {
			return EOF;
		}
	} else {
		if (gs->write(gs, gs->buf, gs->pos) == -1) {
			return EOF;
		}
	}

	gs->buf[gs->pos = 0] = c;
	gs->pos++;

	return c;
}

