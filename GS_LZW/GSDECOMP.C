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

/* Grab the next index from the real file. dsp points to the global compression
 * state.
 *
 * If there is a saved index, return it, clearing the saved location
 *
 * If the current index (dsp->freeindex) has gone beyond the current maximum
 * index (i.e., the number of index bits has increased), increment the index
 * size, up to the maximum, and read the next 8 indices. Otherwise, if we've
 * exhausted the current set of indices, read the next 8 indices.
 *
 * The index is extracted from the buffer using bytes to avoid endianess
 * dependencies
 */

static int
getindex(dsp)
	struct decomp_state *dsp;
{
	int index, r_off, bits, nbits;
	unsigned char *bp;

	if ((index = dsp->savedindex) != 0) {
		dsp->savedindex = 0;
		return (index);
	}

	nbits = dsp->bits;

	if (dsp->freeindex > dsp->maxindex) {
		if (++nbits >= dsp->maxbits) {
			dsp->bits = nbits = dsp->maxbits;
			dsp->maxindex = dsp->maxmaxindex;
		} else {
			dsp->bits = nbits;
			dsp->maxindex = MAXINDEX(nbits);
		}
	} else if ((r_off = dsp->boffset) < dsp->bsize)
		goto dont_read;

	if ((bits = read(dsp->iofd, dsp->bbuffer, nbits)) <= 0)
		return (-1);

	dsp->boffset = r_off = 0;

	dsp->bsize = (bits << 3) - (nbits - 1);

dont_read:
	dsp->boffset += nbits;
	bits = nbits;
	bp = dsp->bbuffer + (r_off >> 3);
	r_off &= 0x7;
	index = *bp >> r_off;
	r_off = 8 - r_off;

	if (bits >= 8) {
		index |= *++bp << r_off;
		bits -= 8;
		r_off += 8;
	}

	if (bits)
		index = (index | (bp[1] << r_off)) & ((1 << nbits) - 1);

	return (index);
}
#if	!defined(FASTLEN)
static int
bytecount(index, prefix)
	int index;
	unsigned short *prefix;
{
	int len;

	for (len = 1; index >= CLEAR; ++len)
		index = prefix[index];

	return (len);
}
#endif	/* !FASTLEN */

/* Unpack and store the specified index, skipping the last 'skip' characters and
 * storing no more than 'total' characters.  Return the first character of the
 * substring, used in computing state for unpacking the next index.
 */

static int
storebytes(index, prefix, suffix, buf, skip, total)
	int index;
	unsigned short *prefix;
	unsigned char *suffix, *buf;
	int skip, total;
{
	buf += total;

	while (skip-- > 0)
		index = prefix[index];

next_char:
	*--buf = suffix[index];

	if (--total > 0) {
		index = prefix[index];
		goto next_char;
	}

	while (index >= CLEAR)
		index = prefix[index];

	return (index);
}

int
decomp_fill(gs, vbuf, n)
	GSTREAM *gs;
	void *vbuf;
	int n;
{
	int prevn, oldindex, leadbyte;
	struct decomp_state *dsp;
	char *buf;

	if ((prevn = n) <= 0)
		return (0);

	dsp = (struct decomp_state *) gs;
	buf = vbuf;

	if (dsp->prevoff == 0) {
		dsp->freeindex = dsp->clear_enabled ? FIRST : CLEAR;
		dsp->maxindex = MAXINDEX(INIT_BITS);
		dsp->bits = INIT_BITS;
		dsp->boffset = 0;
		dsp->bsize = 0;

		leadbyte = oldindex = getindex(dsp);

		if (oldindex == -1) {
			GSerror(&dsp->io) = errno;
			return (0);
		}

		dsp->oldindex = oldindex;
		dsp->leadbyte = leadbyte;
		dsp->prevoff = 1;
		dsp->ufoff = 1;
		dsp->savedindex = 0;

		*buf++ = leadbyte;

		if (--n == 0)
			return (1);
	} else {
		oldindex = dsp->oldindex;
		leadbyte = dsp->leadbyte;
	}

	do {
		int index, input, clen, skip;

		if ((input = getindex(dsp)) == -1)
			break;

		if (input == CLEAR && dsp->clear_enabled) {
			for (index = 0; index < CLEAR; ++index)
				dsp->prefix_tbl[index] = 0;
#if	defined(FASTLEN)
			for (index = CLEAR; index < dsp->maxmaxindex; ++index)
				dsp->len_tbl[index] = 0;
#endif	/* FASTLEN */
			dsp->freeindex = FIRST - 1;
			dsp->bits = INIT_BITS;
			dsp->maxindex = MAXINDEX(INIT_BITS);
			dsp->boffset = dsp->bsize;
			continue;
		}
		if (input >= dsp->freeindex) {
			index = oldindex;
			clen = 1;
		} else {
			index = input;
			clen = 0;
		}
#if	defined(DEBUG)
fprintf(stderr, "%sin = %04x, free = %04x, index = %04x, last = ",
	input > dsp->freeindex ? ">>" : "", input, dsp->freeindex, index);

if (leadbyte >= ' ' && leadbyte <= '~')
	fprintf(stderr, "'%c'", leadbyte);
else
	fprintf(stderr, " %02x", leadbyte);
#endif	/* DEBUG */
#if	defined(FASTLEN)
		clen += dsp->len_tbl[index];
#else	/* !FASTLEN */
		clen += bytecount(index, dsp->prefix_tbl);
#endif	/* !FASTLEN */
		if (dsp->prevoff < dsp->ufoff)
			clen -= dsp->ufoff - dsp->prevoff;

		if (clen > n) {
			skip = clen - n;
			clen = n;
			n = 0;
		} else {
			skip = 0;
			n -= clen;
		}
		if (input < dsp->freeindex) {
			leadbyte = storebytes(index, dsp->prefix_tbl,
					dsp->suffix_tbl, buf, skip, clen);
		} else if (skip == 0) {
			buf[clen - 1] = leadbyte;
			leadbyte = storebytes(index, dsp->prefix_tbl,
					dsp->suffix_tbl, buf, 0, clen - 1);
		} else {
			leadbyte = storebytes(index, dsp->prefix_tbl,
					dsp->suffix_tbl, buf, skip - 1, clen);
		}

		dsp->ufoff += clen;

		/* If we skipped bytes at the end of the index, stash the input
		 * index away so that we'll process it first the next time we're
		 * called. Otherwise, add a new pattern (if the table is not
		 * already full) and update the state so we can continue at the
		 * next index.
		 */

		if (skip != 0)
			dsp->savedindex = input;
		else {
			buf += clen;
			dsp->prevoff = dsp->ufoff;

			if ((index = dsp->freeindex) < dsp->maxmaxindex) {
				dsp->prefix_tbl[index] = oldindex;
				dsp->suffix_tbl[index] = leadbyte;
#if	defined(FASTLEN)
				dsp->len_tbl[index] =
						dsp->len_tbl[oldindex] + 1;
#endif	/* FASTLEN */
				dsp->freeindex = index + 1;
#if	defined(DEBUG)
fprintf(stderr, " - new index %04x = ", index);

if (oldindex > 0xff)
	fprintf(stderr, "%04x", oldindex);
else if (oldindex >= ' ' && oldindex <= '~')
	fprintf(stderr, " '%c'", oldindex);
else
	fprintf(stderr, "  %02x", oldindex);
if (leadbyte >= ' ' && leadbyte <= '~')
	fprintf(stderr, " '%c'", leadbyte);
else
	fprintf(stderr, "  %02x", leadbyte);
#if	defined(FASTLEN)
fprintf(stderr, ", length = %d", dsp->len_tbl[index]);
#endif	/* FASTLEN */
#endif	/* DEBUG */
			}

			dsp->oldindex = oldindex = input;
			dsp->leadbyte = leadbyte;
		}
#if	defined(DEBUG)
fprintf(stderr, "\n");
#endif	/* DEBUG */
	} while (n > 0);

	GSerror(&dsp->io) = errno;
	return (prevn - n);
}
