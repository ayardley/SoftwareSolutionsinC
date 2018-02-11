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
 *      NAME...
 *              zero - find a zero of a function evaluated
 *                      by the calling program
 *
 *      SYNOPSIS...
 *
 *      #include <zero.h>
 *      ...
 *      zero_t *inversion=0;
 *      ...
 *      inversion = init_zero(&x, inversion);      -- initialize
 *      inversion = advise_zero(x2, inversion);    -- suggest 2nd x value
 *      do {
 *              y = (x + 1.)*x*(x - 1.);           -- evaluate function
 *              del = y - target;
 *              if (fabs(del) < .00001) break;     -- optional convergence test
 *              error = seek_zero(del, inversion); -- seek_zero() updates x
 *      } while (!error);
 *
 */
#include <math.h>
#include <stddef.h>     /* for NULL */
#include <stdlib.h>     /* for malloc */
#include <float.h>	/* for DBL_MIN and DBL_MANT_DIG */
#include "zero.h"

/* #define TEST to enable trace output */
#ifdef TEST
#include <stdio.h>
#include <assert.h>
#define __assertfail printf
#define TRACE(foo) printf foo
#else
#define assert(x) 0
#define TRACE(foo)
#endif
#define MAX_EVALS 90            /* maximum # evaluations of user's function */
#define STEP_GROWTH 1.6         /* steps in a given direction grow by
                                 * this factor */
#define HALF_STEP_GROWTH 1.26   /* steps in alternating directions grow
                                 * by this factor (should be approximately
                                 * square root of STEP_GROWTH) */
#define RANGE_INCREASE 1.e2     /* If the function range grows by more than
                                 * this factor after first straddling, it is
                                 * assumed divergent */
#define SLOPE_INCREASE 1.e4     /* if the apparent function slope increases
                                   by more than this, the function is
                                   assumed discontinuous */
#define DOMAIN_INCREASE 1.e2    /* If the estimated search interval grows
                                 * by more than this factor the function is 
                                 * assumed to decrease only asymptotically */



/*
 *      These #defines provide some syntactic sugar, so that entries in the
 *      supplied zero_t can be referred to like plain variables.  The
 *      effect is similar to the "with" statement in Pascal.
 */
#define state           zp->state_rec  /* state of calculation: */
#       define STARTING 0
#       define FINDING_SLOPE 1
#       define DOWNHILL 2
#       define LOCALIZED 3
#       define STRADDLING 4
#define evals           zp->evals_rec  /* # evaluations of user's function */


#define x               zp->x_rec      /* array of previous x values */
#define xa              zp->xa_rec     /* x values bracketing local min */
#define xb              zp->xb_rec
#define f1              zp->f1_rec     /* corresponding function values */
#define f2              zp->f2_rec
#define f3              zp->f3_rec
#define f4              zp->f4_rec
#define fa              zp->fa_rec
#define fb              zp->fb_rec
#define f0              zp->f0_rec

#define step            zp->step_rec   /* current step size */
#define xp              zp->xp_rec     /* points to user's independent
                                           variable */
#define funrange        zp->funrange_rec /* function span between first pair
                                           of points which apparently
                                           straddle a root. */
#define fundomain       zp->fundomain_rec /* difference between
                                           first pair of points which
                                           apparently straddle a root. */
#define nlast           zp->nlast_rec
#define u               zp->u_rec      /* array of z differences */
#define d0              zp->d0_rec
#define e               zp->e_rec      /* size of last step */
#define d               zp->d_rec      /* size of previous step */
#define r               zp->r_rec      /* size of interval containing root */

/*
 * Initialize the zero_t pointed to by zp or (if zp is NULL) a
 * new one malloc'ed from the heap.  *xp_ is the independent
 * variable for the user's function, which will be updated by
 * seek_zero().  Returns a pointer to the zero_t, or NULL if memory is
 * exhausted.
 */
zero_t *
init_zero( double *xp_, zero_t *zp )
{
        if (zp == NULL) {
                zp = (zero_t *)malloc(sizeof(zero_t));
                if(zp == NULL)
                        return NULL;   /* not enough memory */
                zp->max_suggestions = 4;
        } 
        zp->magic = 1776;
        state = STARTING;
        xp = xp_;
        zp->max_evals = MAX_EVALS;
        zp->step_growth = STEP_GROWTH;
        zp->half_step_growth = HALF_STEP_GROWTH;
        zp->range_increase = RANGE_INCREASE;
        zp->slope_increase = SLOPE_INCREASE;
        zp->domain_increase = DOMAIN_INCREASE;
        zp->num_suggestions = 0;

        zp->precision = pow(.5, (double)DBL_MANT_DIG);
        zp->dwarf = DBL_MIN;
        return zp;
}
/*
 * Saves the suggested abscissa 'xx', where the function should
 * be evaluated, in the zero_t pointed to by 'zp'.
 */
zero_t *
advise_zero(double xx, zero_t *zp )
{
        if(zp->num_suggestions == zp->max_suggestions) {
                zp->max_suggestions += 8;
                zp = (zero_t *)realloc(zp, sizeof(zero_t) +
                     (zp->max_suggestions - 4)*sizeof(double));
                if(!zp) 
                        return NULL;
        }
        zp->suggestion[zp->num_suggestions++] = xx;
        return zp;
}

/* This test won't fail with underflow for small a & b */
#define SAME_SIGN(s,t) (((s) > 0. && (t) > 0.) || ((s) < 0. && (t) < 0.))

#ifndef min
#define min(x,y) ((x)<(y)?(x):(y))
#endif

/*
 * 'zp' points to the zero_t holding the record of the inversion,
 * including a pointer to the user's independent variable 'x'.
 * 'funvalue' is the value of the user's function there.  seek_zero() adjusts
 * 'x' in an attempt to find a root of the user's function.  The return
 * value is normally zero, otherwise an error code of type enum
 * zero_status.
 */
enum zero_status
seek_zero( double funvalue, zero_t *zp )
{
        double fmax;    /* temporary */
        register double temp;   /* used to exchange values */
        int n, i;
        double h, s, t; /* terms from equation 9 */
        double hlast, z, d1, c, tol;

        if (!zp || zp->magic != 1776) {
                return INVALID_RECORD;          /*
                                                 * FAIL - null pointer
                                                 * or invalid record
                                                 */
        }

        switch(state) {                         /* pick up where we left off */
                case STARTING:          goto STARTING_L;
                case FINDING_SLOPE:     goto FINDING_SLOPE_L;
                case DOWNHILL:          goto DOWNHILL_L;
                case LOCALIZED:         goto LOCALIZED_L;
                case STRADDLING:        goto STRADDLING_L;
                default:
                        return INVALID_RECORD;  /* FAIL - invalid state */
        }

STARTING_L:
        x[1] = x[2] = *xp;                  /* record 1st function value */
        f1 = f2 = funvalue;
        if (fabs(x[1]) < 1.) {
                step = .01;
        } else {
                step = .01*x[1];
        }

        /* we assume there are no zeros between x[1] and x[2],
         * and look beyond x[2] for a smaller value */
        for (evals = 1; f2 != 0.; ) {
                if (++evals > zp->max_evals)
                        return LOCAL_EXTREMUM;
                if (zp->num_suggestions) {
                        *xp = zp->suggestion[--zp->num_suggestions];
                        step = *xp - x[1];
                        TRACE(("-- suggested --  \n"));
                } else {
                        *xp = x[1] + step;
                        TRACE(("-- outward step --  \n"));
                }

                state = FINDING_SLOPE;
                return OK;      /* get 2nd function value */
FINDING_SLOPE_L:

                if (funvalue/f2 < 1.) {
                        break;
                }
                x[2] = x[1]; f2 = f1;
                x[1] = *xp; f1 = funvalue;
                /* try both sides of starting region */
                step *= -zp->half_step_growth;
        } /* finding slope */

        xa = x[1]; fa = f1;
        x[1] = *xp; f1 = funvalue;
        state = DOWNHILL;
        if (!SAME_SIGN(f1, f2)) {
                state = STRADDLING;
        } else {
                if (fabs(f1) < fabs(f2)) {
                        /* swap points 1 and 2 so f2 will be closer to zero */
                        temp = x[1]; x[1] = x[2]; x[2] = temp;
                        temp = f1; f1 = f2; f2 = temp;
                }
                fundomain = zp->domain_increase*fabs((x[2]-x[1])*f2/(f2-f1));
        }
        while(state == DOWNHILL) {
                if (fabs(x[2] - x[1]) > fundomain)
                        return DIVERGENCE;
                if (++evals > zp->max_evals)
                        return SLOW_CONVERGENCE;
                if (zp->num_suggestions) {
                        *xp = zp->suggestion[--zp->num_suggestions];
                        step = *xp - x[1];
                        TRACE(("-- suggested --  \n"));
                } else {
                        *xp = x[2] + zp->step_growth*(x[2] - x[1]);
                        TRACE(("-- downhill step --  \n"));
                }
                return OK;
DOWNHILL_L:
                x[3] = *xp;
                f3 = funvalue;
                if (!SAME_SIGN(f3, f2)) {
                        state = STRADDLING;
                        x[1] = x[2]; f1 = f2;
                        x[2] = x[3]; f2 = f3;
                } else if (fabs(f3) > fabs(f2)) {
                        state = LOCALIZED;
                } else {
                        x[1] = x[2]; f1 = f2;
                        x[2] = x[3]; f2 = f3;
                }
        } /* downhill */
        xb = x[3]; fb = f3;

        /* within the region xa to xb, there is
         * a local minimum between x[1] and x[2] */

        while(state == LOCALIZED) {
                assert(fabs(f1) > fabs(f2));
                assert(fabs(f3) > fabs(f2));
                assert(SAME_SIGN(x[3] - x[2], x[2] - x[1]));
                if (++evals > zp->max_evals)
                        return LOCAL_EXTREMUM;
                fmax = fabs(f1);
                if(fabs(f3) > fmax) fmax = fabs(f3);
                step = .382*(x[3] - x[2]);
                if(fabs(f2) > .95*fmax) {
#ifdef TIMID
                        return LOCAL_EXTREMUM;
#else
                        TRACE(("-- local extremum, widening search --  \n"));
                        x[1] = xa; f1 = fa;
                        x[2] = xb; f2 = fb;
                        step = zp->step_growth*(x[2] - x[1]);
                        *xp = x[1] + step;
                        state = FINDING_SLOPE;
                        return OK;
#endif
                }

                TRACE(("-- golden section --  \n"));
                *xp = x[2] + step;
/*              *xp = x[3] + .618*(x[2] - x[3]); */
                return OK;
LOCALIZED_L:
                x[4] = *xp;
                f4 = funvalue;
                if (!SAME_SIGN(f4, f2)) {
                        state = STRADDLING;
                        x[1] = x[4]; f1 = f4;
                } else if (fabs(f4) < fabs(f2)) {
                        if (SAME_SIGN(x[3] - x[4], x[4] - x[2])) {
                                /* x[4] between x[2] & x[3] (normal) -
                                 * new minimum */
                                x[1] = x[2]; f1 = f2;
                                x[2] = x[4]; f2 = f4;
                             /* x[3] = x[3]; f3 = f3; */
                        } else if (SAME_SIGN(x[2] - x[4], x[4] - x[1])) {
                                /* new minimum between x[1] & x[2] */
                                x[3] = x[2]; f3 = f2;
                                x[2] = x[4]; f2 = f4;
                             /* x[1] = x[1]; f1 = f1; */
                        } else {
                        /* try opposite side */
                        temp = x[1]; x[1] = x[3]; x[3] = temp;
                        temp = f1; f1 = f3; f3 = temp;
                        }
                } else if (SAME_SIGN(x[4] - x[2], x[3] - x[2])) {
                        /* x[4] and x[3] on same side of x[2] (normal) -
                         * new bound on minimum */
                        x[3] = x[1]; f3 = f1;
                     /* x[2] = x[2]; f2 = f2; */
                        x[1] = x[4]; f1 = f4;
                } else {
                        /* x[4] and x[1] on same side of x[2] -
                         * new bound on minimum */
                        x[1] = x[4]; f1 = f4;
                     /* x[2] = x[2]; f2 = f2; */
                     /* x[3] = x[3]; f3 = f3; */
                }
        } /* localized */
        TRACE(("-- straddling --\n"));
        funrange = fabs(f1 - f2);
        fundomain = fabs(x[1] - x[2]);

        x[0] = x[2];
        x[1] = x[1];
        f0 = f2;
        if (f0 == 0. || f1 == 0.) { /* no search needed */
                if (fabs(f1) >= fabs(f0)) *xp = x[0];
                else *xp = x[1];
                return FINISHED;
        }
        r = x[1] - x[0];
        e = 0.;
        while(++evals <= zp->max_evals) {       /* main loop */
                /* primary convergence test */
                tol = 2.*zp->precision*fabs(x[0]) + zp->dwarf;
                if (fabs(r) <= 2.*tol) break;
                u[1] = r*(f0/(f0 - f1));
                if (e < tol || fabs(f0) >= fabs(f2)) goto BISECT;

                /* interpolate */

                hlast = h = u[1];
                nlast = 1;
                s = r;
                for (i = 2; i <= n; i++)
                        {s += u[i] - u[i-1];
                        t = x[i] - x[0] - s;
                        if (t == 0.) break;
                        u[i] = h*(s/t);
                        h += u[i];
                        d1 = fabs(u[i]);
                        if (i > 2) {
                                if (d1 >= d0) 
                                        /* correction didn't decrease */
                                        break;
                                t = h/r;
                                if (t <= 0. || t >= 1.) 
                                        /* interpolant is outside
                                         * the range x[0] to x[1] */
                                        break;
                        }
                        hlast = h;
                        nlast = i;
                        d0 = d1;
                }
                z = x[0] + hlast;
                d0 = fabs(hlast);
                d1 = fabs(z - x[1]);
                c = e;
                e = d;
                d = min(d0, d1);
                if (d >= .5*c) {
                        /* bisect */
BISECT:
                        TRACE(("-- bisection --  \n"));
                        h = .5*r;
                        z = x[0] + h;
                        nlast = 1;
                        e = d = fabs(h);
                } else if (d < tol) {
                        /* take minimum step from end of interval */
                        TRACE(("-- tiny step --  \n"));
                        if (d0 >= tol) {
                                if (r < 0.) tol = -tol;
                                z = x[1] - tol;
                        } else {
                                if (r < 0.) tol = -tol;
                                z = x[0] + tol;
                        }
                } else {
                        /* accept rational interpolant */
                        TRACE(("-- rational --  \n"));
                }
                /* end interpolate */

                /* evaluate and organize */

                *xp = z;
                return OK;
STRADDLING_L:
                z = *xp;
                if (funvalue == 0.) {
                        x[0] = z;
                        f0 = funvalue;
                        break;
                }
                n = min(nlast+1, MAXDEG);
                for (i = n; i >= 2; i--) {
                        x[i] = x[i-1];
                        u[i] = u[i-1];
                }
                if (SAME_SIGN(funvalue, f1)) {
                        x[1] = x[0];
                        f2 = f1;
                        f1 = f0;
                } else {
                        x[2] = x[0];
                        f2 = f0;
                        u[2] -= r;
                }
                x[0] = z;
                f0 = funvalue;
                r = x[1] - x[0];
                if (fabs(f1 - f0) > zp->range_increase*funrange)
                        return DIVERGENCE;
                if (fabs(x[0] - x[1])*zp->slope_increase*funrange < 
                                      fundomain*fabs(f0 - f1))
                        return DISCONTINUITY;

                /* end evaluate and organize */

        }       /* end main loop */
        
        if (fabs(f1) >= fabs(f0)) *xp = x[0];
        else *xp = x[1];
        if (evals < zp->max_evals) return FINISHED;
        else return SLOW_CONVERGENCE;
}

enum zero_status 
find_root( double (*f)(double), double *xp_, double reltol, double abstol )
{
        zero_t zr;
        double val;
        enum zero_status err;

        (void)init_zero(xp_, &zr);
        if(reltol > zr.precision) zr.precision = reltol;
        if(abstol > zr.dwarf) zr.dwarf = abstol;
        do {
                val = (*f)(*xp_);
        } while (!(err = seek_zero(val, &zr)));
        return err;
}
