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
 *      zero.h - declarations for seek_zero()
 */

#define MAXDEG 3	/* maximum allowable degree of rational interpolation */

typedef struct{
        int magic;              /* integrity check: should be 1776 */
        int state_rec;          /* records current state of
                                   root finding, so seek_zero() can
                                   pick up where it left off */
        double *xp_rec;         /* points to user's
                                   independent variable */
        int evals_rec;          /* # evaluations of user's function */
        int max_evals;          /* maximum # evaluations of user's function */
        double step_rec;        /* difference between last two x values */
        double step_growth;     /* steps in a given direction grow by
                                 * this factor */
        double half_step_growth;/* steps in alternating directions grow
                                 * by this factor (should be approximately
                                 * square root of STEP_GROWTH) */
        double funrange_rec;    /* function span between first pair of points 
                                   which apparently straddle a root. */
        double range_increase;  /* If the function range grows by more than
                                 * this factor after first straddling, it is
                                 * assumed divergent */
        double fundomain_rec;   /* difference between first pair of points 
                                   which apparently straddle a root. */
        double slope_increase;  /* if the apparent function slope increases
                                   by more than this, the function is
                                   assumed discontinuous */
        double domain_increase; /* If the estimated search interval grows
                                 * by more than this factor the function is 
                                 * assumed to decrease only asymptotically */
        double precision;       /* relative machine precision (number 
                                   such that 1 + precision > 1) */
        double dwarf;           /* smallest normalized real number */




        int nlast_rec;          /* degree of rational interpolation 
                                   on previous step */
        double r_rec;           /* size of interval containing the root */
        double d0_rec;          /* distance from nearest interval end */
        double e_rec, d_rec;    /* distances of last two interpolants
                                   from ends of intervals */
        double u_rec[MAXDEG+1]; /* array of Z differences */
        double x_rec[MAXDEG<4?5:MAXDEG+1]; /* array of x values */
        double xa_rec;
        double xb_rec;
        double f0_rec;          /* corresponding function values */
        double f1_rec;
        double f2_rec;
        double f3_rec;
        double f4_rec;
        double fa_rec;
        double fb_rec;

        int max_suggestions;    /* max # suggestions */
        int num_suggestions;    /* # suggestions */
        double suggestion[4];   /* suggested abscissas */
} zero_t;

        /* values returned by seek_zero() */
enum zero_status{
        OK,
        FINISHED,
        INVALID_RECORD,
        LOCAL_EXTREMUM,
        SLOW_CONVERGENCE,
        DIVERGENCE,
        DISCONTINUITY
};

zero_t *                init_zero( double *xp, zero_t *zp );
zero_t *                advise_zero( double x, zero_t *zp );
enum zero_status        seek_zero( double funvalue, zero_t *zp );
enum zero_status        find_root( double (*f)(double), double *xp, 
                                     double reltol, double abstol );
