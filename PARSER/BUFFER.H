/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Mayan Moudgill.  Not derived from licensed software.
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

typedef struct BNF__buffer BNF_buffer;
typedef unsigned BNF_MARK;

struct BNF__buffer {
   FILE *     stream;	/* stream from which characters are being read */
   unsigned   readin;	/* number of characters read in from the stream 
			 * (and potentially stored in buffer)
			 */
   unsigned   current;  /* current character being processed (less than
			 * readin if backtracking has taken place)
			 */
   unsigned   done;	/* position below which backtracking is not possible */
   STR        area;   /* holds characters read in after ``done'' */
};

/* creates a buffer, that reads from the given stream. */
BNF_buffer *   BNF_buffer_create(FILE *);
/* destroys the buffer */
void           BNF_buffer_destroy(BNF_buffer *);
/* gets the ``current'' character. This may be read from the stream, or
 * from the buffer. 
 */
int      BNF_buffer_get(BNF_buffer *);
/* returns a mark which identifies the current position. Other functions
 * allow the input to be backtracked to this mark, or to prevent backtracking
 * past the mark.
 */
BNF_MARK BNF_buffer_tell(BNF_buffer *);
/* backtracks input to the mark. The next character read will be from the
 * marked position. Returns 0 if it cannot backtrack that far.
 */
int      BNF_buffer_seek(BNF_buffer *, BNF_MARK);
/* prevents backtracking past the mark. Has the side-effect of freeing up
 * storage for characters below mark.
 */
void     BNF_buffer_cut(BNF_buffer *, BNF_MARK);
