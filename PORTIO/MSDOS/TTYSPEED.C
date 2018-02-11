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
 * ttyspeed() set tty speed
 */
int
ttyspeed(speed)
long speed;
{
    static unsigned long values[] = {
	38400L, 0043,
	19200L, 0003,
	9600L,  0343,
	4800L,  0303,
	2400L,  0243,
	1200L,  0203,
	600L,   0143,
	300L,   0103,
	0
    };
    register i;

    for (i=0; values[i] != 0; i+=2)
	if (values[i] == speed) {
	    regs.h.ah = TSPEED;
	    regs.x.dx = comport;
	    regs.h.al = (int)values[i+1];
	    int86(FOSSIL, &regs, &regs);
	    return 1;
	}
    return 0;
}
