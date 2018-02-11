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

/*
 *	NAME...
 *		demo - demonstrate root finding with seek_zero()
 */

#include <stdio.h>
#include <math.h>
#include "zero.h"

#ifdef TEST
#undef TEST
#endif

void show_progress(double x, double dif);

int evals;

/* test function for find_root() */
double f(double x){ evals++; return (x*x - 2.)*x - 5.;}

double targets[]={.5, .4, .3849, .3, .2, .1, .0, -.3849, -.1};

char *status_strings[]={
	"success",
	"FINISHED",
	"INVALID RECORD",
	"LOCAL EXTREMUM",
	"SLOW CONVERGENCE",
	"DIVERGENCE",
	"DISCONTINUITY"
	};

/*	uses seek_zero() to find roots of several related functions */
main(argc, argv)
int argc;
char **argv;
{
	double x, y, del, target;
	int error, it, i;
	zero_t *inversion=NULL;

	/* keep trying forever (exercises conversion test) */
	inversion = init_zero(&x, inversion);
	x = -.7;
	target = -.1;
	printf("searching for solution of: x^3 - x = %f\n", target);
	evals = 0;
	do {
		y = (x+1.)*x*(x-1.);
		evals++;
		del = y - target;
		show_progress(x, del);
		error = seek_zero(y - target, inversion);
	} while (!error);
	y = (x+1.)*x*(x-1.);
	printf("             target = %11.7f\n", target);
	printf("         y - target = %11.7f\n", y - target);
	printf("             status = %11d  (%s) with %d evaluations\n\n", 
				error, status_strings[error], evals);


	/* searches with early success */
	for (it = 1; it <= 3; it++) {
		inversion = init_zero(&x, inversion);
		x = -.7;
		target = -.1;
		for (i = 1; i < it; i++)
			error = seek_zero(1., inversion);
		error = seek_zero(0., inversion);
		printf(" root found in %d probes: status = %d (%s)\n", 
					it, error, status_strings[error]);
	}

	/*
	 *	the first two inversion attempts will fail initially, 
	 *	because they begin too near a local extremum.  Widening
	 *	the search allows them to succeed.
	 */
	for (it = 0; it < sizeof(targets)/sizeof(double); it++) {
		target = targets[it];
		x = -0.7;
		printf("searching for solution of: x^3 - x = %f\n", target);
		inversion = init_zero(&x, inversion);
		evals = 0;
		do {
			/*
			 * this is x^3 - x, which has extrema  
			 * at x = +-sqrt(1/3) of -+2/3 sqrt(1/3)
			 * so that f(-.5773503) =  .3849002   (local maximum)
			 *     and f( .5773503) = -.3849002   (local minimum)
			 */
			y = (x+1.)*x*(x-1.); 
			evals++;
			del = y - target;
			show_progress(x, del);
			if (fabs(del) < .00000001) break;
			error = seek_zero(del, inversion);
		} while (!error);
		y = (x+1.)*x*(x-1.);
		if (error) 
			printf("\n");
		printf("             target = %11.7f\n", target);
		printf("         y - target = %11.7f\n", y - target);
		printf("             status = %11d  (%s) with %d evaluations\n\n", 
					error, status_strings[error], evals);
	}

	/* repeat the last search, giving a hint to speed the initial search */

	inversion = init_zero(&x, inversion);
	inversion = advise_zero(-0.9, inversion);
	x = -.7;
	target = -.1;
	printf("searching for solution of: x^3 - x = %f\n", target);
	evals = 0;
	do {
		y = (x+1.)*x*(x-1.);
		evals++;
		del = y - target;
		show_progress(x, del);
		if (fabs(del) < .00000001) break;
		error = seek_zero(y - target, inversion);
	} while (!error);
	y = (x+1.)*x*(x-1.);
	printf("             target = %11.7f\n", target);
	printf("         y - target = %11.7f\n", y - target);
	printf("             status = %11d  (%s) with %d evaluations\n\n", 
				error, status_strings[error], evals);

#define TEST(f,x0)							\
	x = x0;								\
	printf("searching for solution of: "#f"\n");			\
	evals = 0;							\
	do {								\
		y = f;	/* root is at x = 10 */				\
		evals++;						\
		show_progress(x, y);					\
/*		if (fabs(y) < .00000001) break;		*/		\
	} while (!(error = seek_zero(y, inversion)));			\
	y = f;								\
	printf("                             y = %11.7f\n", y);	        \
	printf("%20s -> status =   %d  (%s) with %d evaluations\n\n",   \
				#f, error, status_strings[error], evals)

#define INIT_TEST(f,x0) 							\
	inversion = init_zero(&x, inversion);				\
	TEST(f,x0)

	/* a function with many local minima */
	INIT_TEST(sin(10.*x) - x + 10., -.7);

	/* a function with a double root */
	INIT_TEST((x-2.)*(x-2.), .5);

	/* a function with no zero on the real axis */
	inversion = init_zero(&x, inversion);
	inversion = advise_zero(-0.9, inversion);
	TEST(1./(x*x*x), -.7);

	/* Repeat, starting by straddling the discontinuity */
	inversion = init_zero(&x, inversion);
	inversion = advise_zero(0.9, inversion);
	TEST(1./(x*x*x), -.7);

	/* 
	 * This function has a "knee", which would 
	 * be tough for plain false position method.
	 */
	inversion = init_zero(&x, inversion);
	inversion = advise_zero(1., inversion);       /* ignored */
	inversion = advise_zero(2., inversion);       /* ignored */
	inversion = advise_zero(12., inversion);      /* 1st change of sign */
	inversion = advise_zero(11., inversion);
	inversion = advise_zero(14., inversion);
	TEST(1./(x*x) - .01, .01);	/* root is at x = 10 */

	/* 
	 * This function has a step.
	 */
	inversion = init_zero(&x, inversion);
	inversion = advise_zero(2., inversion);
	TEST(x>0?x+1:x-1, 1.);

	/* 
	 * Used by Wallis to exhibit Newton's method
	 * (cited by de Morgan, then Whittaker & Robinsion, 
	 * then Forsythe, Malcolm, and Moler)
	 */
	INIT_TEST((x*x - 2.)*x - 5, 2.);

	/* examples used by F. Acton */
	INIT_TEST(.4*x*cos(x)-sin(x),4.);
	INIT_TEST((x-10.)*x+1,0.);
	INIT_TEST(1/x-tan(x), .2);

	
	evals = 0;
	x = 10.;
	error = find_root(f, &x, 0., 0.);
	printf("%20s -> status =   %d  (%s) with %d evaluations\n\n",
		"(x*x - 2.)*x - 5.", error, status_strings[error], evals);

	return 0;
}

/*
 * print current x value, function value, and relative error in 
 * function value (unless target function value is zero, in which
 * case the absolute error is reported).
 */
void show_progress(double x, double dif) 
{	
	double bits, target=1.;
	
	if (dif) 
		bits = -log(fabs(dif/target))/log(2.);
	else 
		bits = 60.;	/* answer is exact - just use a large number */
	printf("                   fun(%20.15f ) = %15.11f  %5.2f bits ok\n", 
								x, dif, bits);
}

