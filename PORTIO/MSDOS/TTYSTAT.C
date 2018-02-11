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
 * modem i/o for dos.  This assumes you're gonna have a fossil driver
 * (from FIDO) of some sort or another and it will fail if it can't
 * find one
 */
#include "port.h"
/*
 * ttystat() tells us if there are characters waiting on input
 */
int
ttystat()
{
    regs.h.ah = TSTAT;
    regs.x.dx = comport;
    int86(FOSSIL, &regs, &regs);
    _cd_stale = clock() + (long)CLK_TCK;
    _cd_state = (regs.h.al & 0x80);
    _cd_checked = 1;

    return (regs.h.ah & 0x01);
} /* ttystat */
