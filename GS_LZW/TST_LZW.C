/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by John Worley.  Not derived from licensed software.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include "../gstream/gstream.h"
#include "gslzw.h"

#define	BSIZE	1024

char buffer[BSIZE];

int
main(argc, argv)
	int argc;
	char *argv[];
{
	for (--argc, ++argv; argc > 0; --argc, ++argv) {
		GSTREAM *dp, *cp;

		if ((dp = GSlzw_open(argv[0], O_RDONLY)) == NULL) {
			perror(argv[0]);
		} else {
			char nfile[100];

			sprintf(nfile, "%s.Z", argv[0]);

			cp = GSlzw_open(nfile, O_WRONLY | O_CREAT | O_TRUNC);

			if (cp == NULL) {
				perror(nfile);
				GSlzw_close(dp);
			} else {
				int rcnt, wcnt;
			next_read:
				rcnt = (*dp->read)(dp, buffer, BSIZE);

				if (rcnt > 0) {
					write(1, buffer, rcnt);
					wcnt = (*cp->write)(cp, buffer, rcnt);

					if (wcnt == rcnt)
						goto next_read;
				}
				if (rcnt < 0)
					perror("read error");
				else if (rcnt > 0) {
					if (wcnt >= 0)
						fprintf(stderr, "short write");
					else
						perror("write error");
				}
			}

			(void) GSlzw_close(dp);
			(void) GSlzw_close(cp);
		}
	}
	return (0);
}
