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
#ifndef _PORT_H
#define _PORT_H

#include <stdio.h>		/* definition of EOF */
#include <sys/types.h>		/* system-level types */
#include <sys/ioctl.h>		/* all the ioctl definitions */
#include <sys/fcntl.h>		/* for O_RDWR and O_NDELAY */
#include <time.h>		/* for carrier() */
#include <errno.h>		/* for ttyspeed() error codes */

#include "ttyio.h"

extern struct termio tty_active;
extern struct termio tty_normal;
extern int _modem;

#endif/*_PORT_H*/
