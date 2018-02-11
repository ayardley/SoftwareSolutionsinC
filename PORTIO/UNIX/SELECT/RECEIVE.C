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
 * Linux portable I/O library using ioctls & select()
 */
#include "port.h"

/*
 * receive() - waits for input for N seconds, returning that input
 * or EOF if nothing came in
 */
int
receive(int timeout)
{
    struct timeval tick;	/* interval to look for */
    fd_set inputs;		/* input bits array for select() */

    int rv;			/* return from selected = # file descriptors
    				 * selected
    				 */
    char ch;			/* character gotten after successful select */


    tick.tv_sec = timeout;	/* set our timeout */
    tick.tv_usec = 0;

    FD_ZERO(&inputs);		/* and mark the fd we're waiting on */
    FD_SET(_modem, &inputs);

    /* wait for input */
    rv = select(FD_SETSIZE, &inputs, (fd_set*)0, (fd_set*)0, &tick);

    /* if there is input waiting, get it and return */
    return (rv > 0) ? ttyin() : EOF;
} /* receive */
