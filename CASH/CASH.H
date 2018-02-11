/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by D'Arcy J.M. Cain.  Not derived from licensed software.
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
cash.h
Written by D'Arcy J.M. Cain

functions to allow input and output of money normally but store
and handle it as long integers
*/

#ifndef		CASH_H
#define		CASH_H

const char *	cash(long value, int parens, int points, int comma);
const char *	cash_words(long value);
long		atocash(const char *str, int fpoint);

#endif		/* CASH_H */

