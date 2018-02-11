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

void
clear_hash(csp)
	struct compress_state *csp;
{
	long *p, h;

	for (p = csp->htab, h = HSIZE; h > 0; --h, ++p)
		*p = -1;
}

static unsigned char lmask[] = {
	0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00
};
static unsigned char rmask[] = {
	0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
};

int
writeindex(csp, index)
	struct compress_state *csp;
	int index;
{
	int clear, r_off, bits;
	char *bp;

	if (index < 0) {
		int bytes;

		/* At EOF, write whatever bits are buffered. */

		bytes = (csp->boffset + 7) / 8;

		if (bytes > 0) {
			csp->bytes_out += bytes;
			bytes = write(csp->iofd, csp->bbuffer, bytes);
		}

		return (bytes);
	}

	clear = index == CLEAR && csp->clear_enabled;

	bp = csp->bbuffer + (csp->boffset >> 3);
	r_off = csp->boffset & 0x7;

	/*
	 * Since index is always >= 8 bits, only need to mask the first
	 * hunk on the left.
	 */

	*bp = (*bp & rmask[r_off]) | ((index << r_off) & lmask[r_off]);
	bits = csp->n_bits + r_off - 8;
	index >>= 8 - r_off;

	if (bits >= 8) {
		*++bp = index;
		index >>= 8;
		bits -= 8;
	}

	if (bits)
		bp[1] = index;

	if ((csp->boffset += csp->n_bits) == (csp->n_bits << 3)) {
		csp->bytes_out += csp->n_bits;
		csp->boffset = 0;

		if (write(csp->iofd, csp->bbuffer, csp->n_bits) != csp->n_bits)
			return (-1);
	}

	/* If the next entry is going to be too big for the index size, increase
	 * the index size, if possible.  Write the whole buffer, because the
	 * input side won't discover the size increase until after it is read
	 */

	if (clear || csp->freeindex > csp->maxindex) {
		if (csp->boffset > 0) {
			csp->bytes_out += csp->n_bits;
			csp->boffset = 0;

			if (write(csp->iofd, csp->bbuffer, csp->n_bits) !=
						csp->n_bits)
				return (-1);
		}

		if (clear) {
			csp->n_bits = INIT_BITS;
			csp->maxindex = MAXINDEX(INIT_BITS);
		} else if (++csp->n_bits >= MAXBITS) {
			csp->n_bits = MAXBITS;
			csp->maxindex = MAXMAXINDEX(MAXBITS);
		} else
			csp->maxindex = MAXINDEX(csp->n_bits);
	}

	return (0);
}

int
compress_flush(gs, vbuf, n)
	GSTREAM *gs;
	void *vbuf;
	int n;
{
	struct compress_state *csp;
	int i, prevp, prevn;
	unsigned char *cp;

	if ((prevn = n) <= 0)
		return (0);

	csp = (struct compress_state *) gs;
	cp  = vbuf;

	if (csp->bytes_in == 0) {
		i = write(csp->iofd, compress_magic, sizeof(compress_magic));

		if (i == sizeof(compress_magic)) {
			char info = csp->maxbits;

			if (csp->clear_enabled)
				info |= CLEARTBL;

			i = write(csp->iofd, &info, 1);
		}
		if (i < 0)
			return (-1);

		csp->bytes_in = 1;
		prevp = *cp++;
		--n;
	} else
		prevp = csp->previndex;

	while (--n >= 0) {
		int fcode, curp = *cp++;

		++csp->bytes_in;

		fcode = (long)(((long) curp << MAXBITS) + prevp);
		i = (curp << 8) ^ prevp;

		if (csp->htab[i] == fcode) {
			prevp = csp->indextab[i];
			continue;
		}
		if (csp->htab[i] >= 0) {
			int disp;

			if (i == 0)
				disp = 1;
			else
				disp = HSIZE - i;
		probe:
			if ((i -= disp) < 0)
				i += HSIZE;

			if (csp->htab[i] == fcode) {
				prevp = csp->indextab[i];
				continue;
			}

			if (csp->htab[i] > 0)
				goto probe;
		}

		writeindex(csp, prevp);
		prevp = curp;

		if (csp->freeindex < MAXMAXINDEX(MAXBITS)) {
			csp->indextab[i] = csp->freeindex++;
			csp->htab[i] = fcode;
		} else if (csp->clear_enabled &&
					csp->bytes_in >= csp->bytes_chkpt) {
			long rat;

			csp->bytes_chkpt = csp->bytes_in + CHECK_GAP;

			if (csp->bytes_in > 0x007fffff) {
				rat = csp->bytes_out >> 8;

				if (rat == 0)
					rat = 0x7fffffff;
				else
					rat = csp->bytes_in / rat;
			} else
				rat = (csp->bytes_in << 8) / csp->bytes_out;

			if (rat > csp->cratio)
				csp->cratio = rat;
			else {
				csp->cratio = 0;
				clear_hash(csp);
				csp->freeindex = FIRST;
				writeindex(csp, CLEAR);
			}
		}
	}

	csp->previndex = prevp;
	return (prevn);
}
