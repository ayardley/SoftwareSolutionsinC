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
 * ttyio: definition of the various tty functions
 */
#ifndef _TTYIO_D
#define _TTYIO_D

	/* FLOW CONTROL */
#define	TTYF_RTSCTS	0			/* RTS/CTS */
#define	TTYF_NONE	1			/* or nothing at all */

extern int  ttyopen(char *, int);	/* open the tty */
extern void ttyclose();			/* close the tty */
extern int  ttystat();			/* are characters waiting? */
extern void dobreak(int);		/* set or clear a break */
extern int  ttyspeed(long);		/* set linespeed */
extern int  carrier();			/* do we have carrier? */
extern void disable();			/* disable the modem */
extern void enable();			/* enable the modem */
extern void flush();			/* flush input */
extern int  receive(int);		/* get a character within some time */
extern int  ttyin();			/* get a character */
extern void slowputs(char *);		/* slowly write output */
extern void ttyout(char);		/* write a character */
extern void ttywrite(char *, int);	/* write a string */

#endif /*_TTYIO_D*/
