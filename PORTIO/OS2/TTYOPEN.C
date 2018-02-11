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

int _modem;			/* a readwrite attachment to the modem */
DCBINFO _modcontrol;		/* the way the modem looks */
DCBINFO _modsavedctl;		/* .. and looked, before we got to it */

/*
 * ttyopen() attaches ourself to a modem port
 */
int
ttyopen(dev, flow)
char *dev;
int flow;
{
    short info;
    char device[20];
    static char control[3] = { 8, 0, 0 };
    
    sprintf(device, "/dev/%s", dev);
    if (DosOpen(device, &_modem, &info, 0L, 0, 0x01, 0x2042, 0L) == 0) {
	/* opened the file - so now get the existing modem config
	 * and replace it with our own
	 */
	DosDevIOCtl(&_modsavedctl, (int*)0, 0x73, 1, _modem);
	_modcontrol = _modsavedctl;
	_modcontrol.usWriteTimeout = 0;		/* no write timeout */
	_modcontrol.usReadTimeout  = 0;		/* no read timeout */
	_modcontrol.fbCtlHndShake  = (flow==TTYF_RTSCTS) ? 0x08 : 0;
	_modcontrol.fbFlowReplace  = (flow==TTYF_RTSCTS) ? 0xc0 : 0;
	_modcontrol.fbTimeout      = 0x04;	/* normal timeout processing */
	DosDevIOCtl((int*)0, &_modcontrol, 0x53, 1, _modem);

	/* set the modem up for 8 characters, no parity, 1 stop bit
	 */
	DosDevIOCtl((int*)0, control, 0x42, 1, _modem);
	
	enable();
	return 1;
    }
    _modem = -1;
    return 0;
} /* ttyopen() */
