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

struct termio tty_active;
struct termio tty_normal;
int _modem;

/*
 * ttyopen() - opens the tty, sets up flow control if needed, and initializes
 *             ioctls for it
 */
int
ttyopen(char *device, int flags)
{
    int x;

    /* first open the device with ndelay so we can
     * configure it properly, then reopen it with
     * delay so that input will work
     */
    if ((x = open(device, O_RDWR|O_NDELAY)) >= 0) {
	ioctl(x, TCGETA, &tty_normal);

	tty_active = tty_normal;

	tty_active.c_cflag &= ~(CBAUD|HUPCL);
#ifdef RTSCTS
	tty_active.c_cflag |= (CS8 | CLOCAL | (flags & TTYF_RTSCTS) ? CRTSCTS : 0);
#else
	tty_active.c_cflag |= (CS8 | CLOCAL);
#endif
	tty_active.c_iflag = IGNBRK|IGNPAR;
	tty_active.c_oflag = 0;
	tty_active.c_lflag &= ~(ISIG|ICANON|XCASE|ECHO|ECHOE|ECHOK|ECHONL);

	ioctl(x, TCSETA, &tty_active);

	_modem = open(device, O_RDWR);
	close(x);

	ttytimeout(0);	/* unless told otherwise, we don't wait for i/o */
    }
    return _modem;
} /* ttyopen */
