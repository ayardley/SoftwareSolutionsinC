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
 * dobreak() - sets or clears a break condition
 *
 * On linux, dobreak(0) is a no-op, since the ioctl TCSBRK automagically
 * sets a 1/4 second break for us.
 */
void
dobreak(int onoff)
{
#if defined(TIOCSBRK) && defined(TIOCCBRK)	/* BSD & derivatives */
    ioctl(_modem, onoff ? TIOCSBRK : TIOCCBRK, 0);
#elif defined(TCSBRK)				/* USG & derivatives (linux) */
    if (onoff)
	ioctl(_modem, TCSBRK, 0);
#elif defined(TCSBRKP)				/* POSIX break */
    if (onoff)
	ioctl(_modem, TCSBRKP, 3);		/* 3 ms break */
#endif
} /* dobreak */
