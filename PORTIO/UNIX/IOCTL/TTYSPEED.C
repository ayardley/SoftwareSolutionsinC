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
 * ttyspeed() sets the modem baud rate
 */
ttyspeed(long speed)
{
	static struct {
		long speed;		/* how fast we want the port to run */
		int flags;		/* ioctl flags to do what we want */
	} bps[] = {
		{ 300, B300 },
		{ 600, B600 },
		{ 1200, B1200 },
		{ 2400, B2400 },
		{ 4800, B4800 },
		{ 9600, B9600 },
#ifdef B19200
		{ 19200, B19200 },
#elif EXTA
		{ 19200, EXTA },
#endif
#ifdef B38400
		{ 38400, B38400 },
#elif EXTB
		{ 38400, EXTB },
#endif
		{ 0, B0 }
	};
#define SPEEDS	(sizeof bps / sizeof bps[0])

	int i;

	for (i=0; i < SPEEDS; i++) {
		if (speed == bps[i].speed) {
			/* take out the old baudrate and fit the new one
			 * in its place
			 */
			tty_active.c_cflag &= ~CBAUD;
			tty_active.c_cflag |= bps[i].flags;

			/* we use TCSETAF to flush all pending i/o before
			 * we change the baudrate and cause confusion
			 */
			if (ioctl(_modem, TCSETAF, &tty_active) == 0)
			    return speed;
			return EOF;
		}
	}
	errno = EINVAL;		/* bad baud rate */
	return EOF;
} /* ttyspeed */
