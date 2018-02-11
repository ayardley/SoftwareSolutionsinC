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

#define	MAXBITS		16
#define	INIT_BITS	9
#define	CLEAR		256
#define	FIRST		257
#define	CLEARTBL	0x80
#define	NBITMASK	0x1f

#define	MAXMAXINDEX(n)	(1 << (n))
#define	MAXINDEX(n)	(MAXMAXINDEX(n) - 1)

struct decomp_state {
	GSTREAM		io;		/* Generic stream interface */

	int		clear_enabled;	/* Can the tables be flushed? */
	int		maxbits;	/* Maximum bits/index */
	int		maxmaxindex;	/* Absolute maximum index + 1 */

	int		oldindex;	/* "Old" index */
	int		leadbyte;	/* First byte of "old" index */
	int		freeindex;	/* Next free index */
	unsigned short *prefix_tbl;	/* Code prefix table */
	unsigned char  *suffix_tbl;	/* Code suffix table */
#if	defined(FASTLEN)
	unsigned short *len_tbl;	/* Code length cache */
#endif	/* FASTLEN */

	int		ufoff;		/* User "file" offset */
	int		prevoff;	/* User "file" offset at start of buf */
	int		savedindex;	/* Paritally processed index */

	int		iofd;		/* Compressed file */
	int		boffset;	/* Bit offset in current buffer */
	int		bits;		/* Current bits/index */
	int		maxindex;	/* Current maximum legal index */
	int		bsize;		/* Bits in current buffer */
	unsigned char	bbuffer[MAXBITS];
};

#define HSIZE		69001		/* hash table size */
#define	CHECK_GAP	10000		/* Check ratio at this interval */

struct compress_state {
	GSTREAM		io;			/* Generic stream interface */

	int		clear_enabled;		/* Can we clear the tables? */
	int		maxbits;		/* Maximum bits/index */
	int		maxmaxindex;		/* Absolute maximum index + 1 */

	long		*htab;
	unsigned short	*indextab;
	int		freeindex;		/* next available index */
	int		previndex;		/* Previous index */

	int		bytes_in;		/* Bytes read so far */
	int		bytes_out;		/* bytes of compressed output */
	int		bytes_chkpt;		/* when to check compression */
	unsigned int	cratio;			/* Fixed point compression
						 * ratio
						 */
	int		iofd;			/* File descriptor */
	int		n_bits;			/* length of current index */
	int		maxindex;		/* maximum index for n_bits */
	int		boffset;		/* Bits in buffer */
	char		bbuffer[MAXBITS];
};

extern char compress_magic[2];

extern void *	calloc(size_t, size_t);
extern void	free(void *);

extern int		compress_flush(GSTREAM *, void *, int);
extern int		decomp_fill(GSTREAM *, void *, int);
extern void		clear_hash(struct compress_state *);
extern int		writeindex(struct compress_state *, int);

extern GSTREAM *	GSlzw_open(char *, int);
extern int		GSlzw_close(GSTREAM *);

