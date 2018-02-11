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
 * modem i/o for os/2, using Microsoft C for OS/2
 */
#include "port.h"

/*
 * carrier() - detects carrier
 *
 * note: we don't want to spend too much time doing ioctls, so we'll only
 * check for carrier on 1-second intervals; if we've checked within the
 * last second, we'll return the value we found then.
 */
int
carrier()
{
    static char _cd_carrier = 0;	/* latest carrier-detection flag */
    static char _cd_active  = 0;	/* have we checked at all? */
    static clock_t _cd_stale= 0;	/* time when _cd_carrier goes stale */

    unsigned char flags;
    clock_t now;

    now = clock();
    if (_cd_active == 0 || now > _cd_stale) {
	DosDevIOCtl(&flags, (int*)0, 0x67, 1, _modem);
	_cd_stale = now+1000L;	/* only check cd at 1 sec intervals */
	_cd_active = 1;
	_cd_carrier = flags & (0x80);
    }
    return _cd_carrier;
} /* carrier */
