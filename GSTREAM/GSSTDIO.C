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
**	GSTREAM interface to C standard i/o
*/

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <errno.h>

#include "gstream.h"
#include "gsstdio.h"

typedef struct {
	GSTREAM	gs;
	FILE *	f;
} fSTREAM;

static int	GSfread(GSTREAM *gs, void *buf, int cnt);
static int	GSfwrite(GSTREAM *gs, void *buf, int cnt);
static long	GSfseek(GSTREAM *gs, long cnt, int origin);

GSTREAM *
GSfopen(char *filename, char *mode)
{
	FILE *f;
	fSTREAM *fs;
	int rmode;

	rmode = (mode[0] == 'r');
	if (f = fopen(filename, mode)) {
		if (fs = (fSTREAM *)malloc(sizeof(fSTREAM))) {
			fs->f = f;
			if (GSstartup((GSTREAM *)fs,
			    (rmode ? GSfread : (GSrfn *)0),
			    (rmode ? (GSwfn *)0 : GSfwrite),
			    (rmode ? GSfseek : (GSsfn *)0)) == 0) {
				return (GSTREAM *)fs;
			}
			free(fs);
		}
		fclose(f);
	}
	return (GSTREAM *)0;
}

static int
GSfread(GSTREAM *gs, void *buf, int cnt)
{
	fSTREAM *fs = (fSTREAM *)gs;
	int rv;

	rv =  fread(buf, (size_t)1, (size_t)cnt, fs->f);
	if (ferror(fs->f)) {
		GSseterr(gs, errno);
		rv = -1;
	}
	return rv;
}

static int
GSfwrite(GSTREAM *gs, void *buf, int cnt)
{
	fSTREAM *fs = (fSTREAM *)gs;
	int rv;

	rv =  fwrite(buf, (size_t)1, (size_t)cnt, fs->f);
	if (ferror(fs->f)) {
		GSseterr(gs, errno);
		rv = -1;
	}
	return rv;
}

static long
GSfseek(GSTREAM *gs, long cnt, int origin)
{
	fSTREAM *fs = (fSTREAM *)gs;

	if (fseek(fs->f, cnt, origin) != 0) {
		GSseterr(gs, errno);
		return (long)-1;
	}
	return ftell(fs->f);
}

int
GSfclose(GSTREAM *gs)
{
	fSTREAM *fs = (fSTREAM *)gs;
	int sv;
	int cv;

	sv = GSshutdown(gs);
	cv = fclose(fs->f);
	free(fs);
	return (sv || cv ? -1 : 0);
}
