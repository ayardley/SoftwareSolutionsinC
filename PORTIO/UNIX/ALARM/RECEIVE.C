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
 * receiving() data using the alarm() call
 */
#include <stdio.h>
#include <signal.h>

extern int _modem;	/* file descriptor we're gonna read from */

/*
 * dummy signal handler for alarm-interrupted input
 */
/* Note that the approved signalhandler type may change drastically
 * according to what compiler and runtime you're using.  C++ on a
 * SunOS box, for example, uses SIG_PF rather than SignalHandler, which
 * looks like a GCCism
 */
static SignalHandler
trigger()
{
    return 0;
} /* trigger */


/*
 * receive() - waits for input for N seconds, returning that input
 * or EOF if nothing came in
 */
int
receive(int timeout)
{
    static int sigset=0;	/* have we set up our signal handler yet? */

    char ch;	/* character read from _modem port */
    int rv;	/* return value from read() - either 1 (good) or not (bad) */

    if (!sigset) {
	/* set our signal handler if it's the first time in */
	signal(SIGALRM, trigger);
	sigset=1;
    }

    alarm(timeout);
    rv = read(_modem, &ch, 1);
    alarm(0);

    return (rv == 1) ? ch : EOF;
} /* receive */
