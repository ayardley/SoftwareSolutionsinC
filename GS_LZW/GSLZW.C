/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by John Worley.  Not derived from licensed software.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include "../gstream/gstream.h"
#include "gslzw.h"

char compress_magic[] = { 0x1f, 0x9d };

static int
GSlzw_oreadfile(nm, flags, nbits, clrtbl)
	char *nm;
	int flags, *nbits, *clrtbl;
{
	int fd, bits;
	struct stat sb;
	char header[3];

	fd = -1;

	if (flags & (O_APPEND | O_CREAT | O_TRUNC))
		errno = EINVAL;
	else if (stat(nm, &sb) != 0)
		/* Nothing - errno is set */;
	else if ((sb.st_mode & S_IFMT) != S_IFREG)
		errno = EINVAL;
	else if ((fd = open(nm, flags, 0)) < 0)
		/* Nothing - errno is set */;
	else if (read(fd, header, sizeof(header)) != sizeof(header))
		/* Nothing - errno is set */;
	else if (header[0] != compress_magic[0] ||
					header[1] != compress_magic[1])
		errno = EINVAL;
	else if ((bits = header[2] & NBITMASK) < INIT_BITS || bits > MAXBITS)
		errno = EINVAL;
	else {
		*nbits = bits;
		*clrtbl = header[2] & CLEARTBL;

		return (fd);
	}

	if (fd >= 0)
		close(fd);

	return (-1);
}

GSTREAM *
GSlzw_open(fname, flags)
	char *fname;
	int flags;
{
	GSTREAM *gs;
	int fd;

	extern void *calloc();
	extern int compress_flush();
	extern int decomp_fill();

	if (flags & O_RDWR) {
		errno = EINVAL;
		return (NULL);
	}
	if (flags & O_WRONLY) {
		struct compress_state *csp;
		int tblbytes;

		if (flags & (O_APPEND)) {
			errno = EINVAL;
			return (NULL);
		}
		if ((fd = open(fname, flags, 0666)) < 0) {
			return (NULL);
		}

		tblbytes  = sizeof(*csp->htab) + sizeof(*csp->indextab);
		tblbytes *= HSIZE;
		csp	  = calloc(1, sizeof(struct compress_state) + tblbytes);

		if (csp == NULL) {
			errno = ENOMEM;
			close(fd);
			return (NULL);
		}

		GSstartup(&csp->io, (GSrfn *) 0, compress_flush, (GSsfn *) 0);

		csp->iofd = fd;
		csp->htab = (long *)(csp + 1);
		csp->indextab = (unsigned short *)(csp->htab + HSIZE);

		/* It would be nice to parameterize these, but the interface
		 * doesn't exist yet - maybe an fcntl() is needed
		 */

		csp->clear_enabled = 1;
		csp->maxbits = MAXBITS;
		csp->maxmaxindex = MAXMAXINDEX(MAXBITS);

		csp->n_bits = INIT_BITS;
		csp->maxindex = MAXINDEX(INIT_BITS);
		csp->freeindex = FIRST;
		csp->bytes_in = 0;
		csp->bytes_out = 0;
		csp->bytes_chkpt = CHECK_GAP;
		csp->cratio = 0;
		csp->boffset = 0;

		clear_hash(csp);

		gs = &csp->io;
	} else {
		struct decomp_state *dsp;
		int i, bits, clrtbl, tblsz;

		if ((fd = GSlzw_oreadfile(fname, flags, &bits, &clrtbl)) < 0)
			return (NULL);

#if	defined(FASTLEN)
		tblsz = sizeof(short) + sizeof(char) + sizeof(short);
#else	/* !FASTLEN */
		tblsz = sizeof(short) + sizeof(char);
#endif	/* !FASTLEN */
		dsp = calloc(1, sizeof(struct decomp_state) + (tblsz << bits));

		if (dsp == (struct decomp_state *) 0) {
			errno = ENOMEM;
			close(fd);
			return (NULL);
		}

		GSstartup(&dsp->io, decomp_fill, (GSwfn *) 0, (GSsfn *) 0);

		dsp->iofd = fd;
		dsp->maxmaxindex = MAXMAXINDEX(bits);
		dsp->maxbits = bits;
		dsp->clear_enabled = clrtbl;
		dsp->prevoff = 0;
		dsp->savedindex = 0;
#if	defined(FASTLEN)
		dsp->len_tbl    = (unsigned short *)(dsp + 1);
		dsp->prefix_tbl =
				(unsigned short *)(dsp->len_tbl + (1 << bits));
#else	/* !FASTLEN */
		dsp->prefix_tbl = (unsigned short *)(dsp + 1);
#endif	/* !FASTLEN */
		dsp->suffix_tbl =
			(unsigned char *)(dsp->prefix_tbl + (1 << bits));

		for (i = 0; i < CLEAR; ++i) {
			dsp->suffix_tbl[i] = i;
#if	defined(FASTLEN)
			dsp->len_tbl[i] = 1;
#endif	/* FASTLEN */
		}
#if	defined(DEBUG)
		fprintf(stderr, "DECOMPRESS: %d bits (%stable clear)\n",
					bits, dsp->clear_enabled ? "" : "No ");
#endif	/* DEBUG */
		gs = &dsp->io;
	}

	return (gs);
}

int
GSlzw_close(gs)
	GSTREAM *gs;
{
	int rv;

	if (gs->read != (GSrfn *) 0) {
		struct decomp_state *dsp = (struct decomp_state *) gs;

		rv = close(dsp->iofd);
	} else {
		struct compress_state *csp = (struct compress_state *) gs;

		if ((rv = writeindex(csp, csp->previndex)) >= 0)
			rv = writeindex(csp, -1);

		(void) close(csp->iofd);
	}

	free(gs);
	return (rv);
}
