/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Dennis Jelcic.  Not derived from licensed software.
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
 *	lst.h -- definitions for dynamic list data structure
 */
#ifndef LST_H
#define	LST_H

#include <stddef.h>		/* ...for size_t */

typedef void **LST;

LST	LSTinit(void *p, ... /*, (void *)0 */);
size_t	LSTlen(LST lst);
LST	LSTadd(LST lst, LST cur, void * p);
void *	LSTrem(LST lst, LST cur);
LST	LSTnorm(LST lst);
LST	LSTfree(LST lst);

#define LSTpush(lst,p)		LSTadd(lst, lst, p)
#define LSTpop(lst)		LSTrem(lst, lst)
#define LSTput(lst,p)		LSTadd(lst, (LST)0, p)

#endif /* LST_H */
