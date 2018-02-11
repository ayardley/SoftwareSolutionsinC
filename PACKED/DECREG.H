/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Larry W. Loen.  Not derived from licensed software.
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

#ifndef DECREG_H
#define DECREG_H decreg

/* decreg.h   -- definition of a decimal register in C */

typedef struct {   /* A simple, portable version */
   unsigned char digit[19];
   /* digits can be though of as if declared
        unsigned char units[14];
        (decimal point goes here)
        unsigned char fract[5];   */
   char sign;
} decimal_register;

void packedToReg(decimal_register *reg,
          const unsigned char *pdec,const int ndig,
          const int nfrac); /* copy packed decimal into reg */
void packedFromReg(unsigned char *pdec,const int ndig,
          const int nfrac, const decimal_register *reg); /* copy
          reg to packed dec*/
/*   Copy one reg struct to another                         */
void cpyReg(decimal_register *rslt,const decimal_register *src);
/*   The traditional arithmetic functions follow            */
void addReg(decimal_register *rslt,const decimal_register *src);
void subReg(decimal_register *rslt,const decimal_register *src);
void mulReg(decimal_register *rslt,const decimal_register *src);
void divReg(decimal_register *rslt,const decimal_register *src);
int compReg(const decimal_register *src1,
            const decimal_register *src2);
/*   Decimal shifst and simple output routines              */
void lshiftReg(decimal_register *rslt,const int shft);
void rshiftReg(decimal_register *rslt,const int shft);
void outReg(const decimal_register *src);
/*   cleanReg ensures digits & sign well-defined            */
void cleanReg(decimal_register *rslt);

#ifdef DECREG_IMP
#include <decreg.c>
#endif
#endif

