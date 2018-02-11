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
 * portable I/O library using ioctls. (Linux)
 */
#include "port.h"

/*
 * ttytimeout() sets the modem input timeout via ioctl
 */
static void
ttytimeout(int seconds)
{
    int timeout = seconds * 10; /* VTIME timeout is in 10ths of seconds */

    if (tty_active.c_cc[VTIME] != timeout) {
	/* no need to re'ioctl the modem if we're not changing the
	 * timeout
	 */
	tty_active.c_cc[VTIME] = timeout;
	tty_active.c_cc[VMIN] = 0;
	tty_active.c_lflag &= ~ICANON;
	ioctl(_modem, TCSETA, &tty_active);
    }
} /* ttytimeout */
