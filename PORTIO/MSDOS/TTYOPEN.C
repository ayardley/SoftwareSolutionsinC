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

/* statics for conversing with the local fossil driver
 */
int comport;
union REGS regs;

/* variables used to track carrier and make sure we don't spend too
 * much time checking for it
 */
time_t _cd_stale;		/* when we need to check cd again */
char _cd_checked;		/* has cd been checked? */
char _cd_state=0;		/* what's cd now? */


/*
 * ttyopen() attaches ourself to a modem port
 */
int
ttyopen(dev, flow)
char *dev;
int flow;
{
    _cd_checked = 0;
    comport = dev[strlen(dev)-1]-'1';
    /* perform fossil init
     */
    regs.h.ah = TOPEN;
    regs.x.bx = 0;
    regs.x.cx = 0;
    regs.x.dx = comport;
    int86(FOSSIL, &regs, &regs);

    if (regs.x.ax == FOSSIL_FLAG) {
	/* we can either do rts/cts flow control or none at
	 * all
	 */
	if (flow == TTYF_RTSCTS) {
	    regs.h.ah = TFLOW;
	    regs.h.al = 0xf2;
	    regs.x.dx = comport;
	    int86(FOSSIL, &regs, &regs);
	}

	return 1;
    }
    return 0;
} /* ttyopen() */
