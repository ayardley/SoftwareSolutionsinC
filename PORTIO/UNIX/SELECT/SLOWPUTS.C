/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by David Parsons.  Not derived from licensed software.
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
 * Linux portable I/O library using ioctls & select()
 */
#include "port.h"

/*
 * slowputs() writes a string to modem s*l*o*w*l*y
 */
void
slowputs(s)
register char *s;
{
    register long delay;
    extern int Debug;

    while (*s) {
	if (*s & 0200) {	/* anything > 127 causes delay */
	    delay = (0xff & *++s);
	    usleep(delay * 100);
	    if (Debug > 1)
		fprintf(stderr, "{%ld}", delay);
	}
	else {
	    if (Debug > 1)
		putc((*s == '\r') ? '\n' : (*s), stderr);
	    usleep(80);
	    ttyout(*s);
	}
	++s;
    }
} /* slowputs */
