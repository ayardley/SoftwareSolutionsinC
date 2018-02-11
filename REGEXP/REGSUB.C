/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Henry Spencer.  Not derived from licensed software.
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
 * regsub
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regexp.h>
#include "regmagic.h"

/*
 - regsub - perform substitutions after a regexp match
 */
void
regsub(rp, source, dest)
const regexp *rp;
const char *source;
char *dest;
{
	register regexp * const prog = (regexp *)rp;
	register char *src = (char *)source;
	register char *dst = dest;
	register char c;
	register int no;
	register size_t len;

	if (prog == NULL || source == NULL || dest == NULL) {
		regerror("NULL parameter to regsub");
		return;
	}
	if ((unsigned char)*(prog->program) != MAGIC) {
		regerror("damaged regexp");
		return;
	}

	while ((c = *src++) != '\0') {
		if (c == '&')
			no = 0;
		else if (c == '\\' && isdigit(*src))
			no = *src++ - '0';
		else
			no = -1;

		if (no < 0) {	/* Ordinary character. */
			if (c == '\\' && (*src == '\\' || *src == '&'))
				c = *src++;
			*dst++ = c;
		} else if (prog->startp[no] != NULL && prog->endp[no] != NULL &&
					prog->endp[no] > prog->startp[no]) {
			len = prog->endp[no] - prog->startp[no];
			(void) strncpy(dst, prog->startp[no], len);
			dst += len;
			if (*(dst-1) == '\0') {	/* strncpy hit NUL. */
				regerror("damaged match string");
				return;
			}
		}
	}
	*dst++ = '\0';
}
