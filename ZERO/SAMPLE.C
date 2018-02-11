/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by James R. Van Zandt.  Not derived from licensed software.
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
#include <math.h>
#include "zero.h"

main()
{
        double x, y;
        int error, evals=0;
        zero_t *inversion=NULL;

        inversion = init_zero(&x, inversion);
        x = -.7;
        do {
                y = sin(10.*x) - x + 10.;
                evals++;
                printf(" fun(%11.7f ) = %15.11f\n", x, y);
                if (fabs(y) < .00000001) break;
                error = seek_zero(y, inversion);
        } while (!error);
        y = sin(10.*x) - x + 10.;
        printf("             y = %11.7f\n", y);
        printf("        status = %11d\n", error);
        printf(" # evaluations = %11d\n", evals);
	return 0;
}
