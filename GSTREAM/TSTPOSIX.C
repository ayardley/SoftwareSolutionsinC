/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Kent Schumacher.  Not derived from licensed software.
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

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>

#include "gstream.h"
#include "gsposix.h"

int main(int argv, char *argc[]) {
	GSTREAM	*gs;
	int		i;

	gs = GSioopen("tstposix.out",O_WRONLY|O_CREAT);

	if (gs == NULL) {
		fprintf(stderr,"Open for write error\n");
		return(-1);
	}

	for (i = 0 ; i < 256 ; i++) {
		if (GSputc(i,gs) == EOF) {
			fprintf(stderr,"GSputc error #%d\n",gs->errno);;
			break;
		}
	}

	if (GSwrite("The End",8,1,gs) != 1) {
		fprintf(stderr,"GSwrite error #%d\n",GSerror(gs));
	}

	GSioclose(gs);
	return(0);
}
