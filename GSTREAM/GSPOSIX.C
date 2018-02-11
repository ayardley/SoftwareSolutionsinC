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
**	GSTREAM interface to POSIX file i/o
*/

#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "gstream.h"
#include "gsposix.h"

typedef struct {
	GSTREAM	gs;
	int	fd;
} ioSTREAM;

#define	MODE	(S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

static int	GSioread(GSTREAM *gs, void *buf, int cnt);
static int	GSiowrite(GSTREAM *gs, void *buf, int cnt);
static long	GSioseek(GSTREAM *gs, long cnt, int origin);

GSTREAM *
GSioopen(char *filename, int mode)
{
	int fd;
	ioSTREAM *io;
	int rmode;

	rmode = ((mode & (O_RDONLY|O_WRONLY|O_RDWR)) == O_RDONLY);
	if ((fd = open(filename, mode, MODE)) >= 0) {
		if (io = (ioSTREAM *)malloc(sizeof(ioSTREAM))) {
			io->fd = fd;
			if (GSstartup((GSTREAM *)io,
			    (rmode ? GSioread : (GSrfn *)0),
			    (rmode ? (GSwfn *)0 : GSiowrite),
			    (rmode ? GSioseek : (GSsfn *)0)) == 0) {
				return (GSTREAM *)io;
			}
			free(io);
		}
		close(fd);
	}
	return (GSTREAM *)0;
}

static int
GSioread(GSTREAM *gs, void *buf, int cnt)
{
	ioSTREAM *io = (ioSTREAM *)gs;
	int rv;

	rv = read(io->fd, buf, (unsigned int)cnt);
	if (rv < 0) {
		GSseterr(gs, errno);
		rv = -1;
	}
	return rv;
}

static int
GSiowrite(GSTREAM *gs, void *buf, int cnt)
{
	ioSTREAM *io = (ioSTREAM *)gs;
	int rv;

	rv = write(io->fd, buf, (unsigned int)cnt);
	if (rv < 0) {
		GSseterr(gs, errno);
		rv = -1;
	}
	return rv;
}

static long
GSioseek(GSTREAM *gs, long cnt, int origin)
{
	ioSTREAM *io = (ioSTREAM *)gs;
	off_t rv;

	rv = (long)lseek(io->fd, (off_t)cnt, origin);
	if (rv < 0) {
		GSseterr(gs, errno);
		rv = -1;
	}
	return (long)rv;
}

int
GSioclose(GSTREAM *gs)
{
	ioSTREAM *io = (ioSTREAM *)gs;
	int sv;
	int cv;

	sv = GSshutdown(gs);
	cv = close(io->fd);
	free(io);
	return (sv || cv ? -1 : 0);
}
