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
 * ttytimeout() changes the tty read timeout
 */
ttytimeout(timeout)
int timeout;
{
    static int _prev_timeout = -1;

    if (timeout == _prev_timeout)
	return 0;

    _prev_timeout = timeout;
    /* do an ioctl to set up the new improved read timeout
     */
    _modcontrol.usReadTimeout = timeout * 100;
    if (timeout == 0)
	_modcontrol.fbTimeout |= 0x06;
    else
	_modcontrol.fbTimeout = (_modcontrol.fbTimeout & ~ 0x06) | 0x04;

    DosDevIOCtl((int*)0, &_modcontrol, 0x53, 1, _modem);
} /* ttytimeout */
