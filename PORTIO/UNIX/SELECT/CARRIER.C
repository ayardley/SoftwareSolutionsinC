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
 * carrier() detects carrier for us
 */
carrier()
{
    static clock_t stale;		/* when current carrier value expires */
    static int gotcarrier=0;		/* last value of carrier */
    static int checked = 0;		/* have we ever checked carrier? */
    clock_t now = clock();
    int flags;

    /* only check carrier every second or so
     */
    if (!checked || now > stale) {
	stale = now+CLOCKS_PER_SEC;	/* carrier expires next second */
	checked = 1;

	ioctl(_modem, TIOCMGET, &flags);

	gotcarrier = flags & TIOCM_CAR;
    }
    return gotcarrier;
} /* carrier */
