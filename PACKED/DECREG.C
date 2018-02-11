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

/* Implementation of decimal register functions
   Larry W. Loen

   Included when:  DECREG_IMP is defined

   Include from:  decreg.h                   */

/* Key constants, NDIG, NDIG2, NFRAC, NFRAC2 */
#define NDIG 19
#define NDIG2 19
#define NFRAC 5
#define NFRAC2 5
#define UCHAR unsigned char


 /* Necessary constants:

  decsign supplies index for proper sign into decnewsign
  decinvsign suppolies index for proper inversion of a sign
  decclean is used to 'clean' a potentially incorrect decimal
     number to a defined value (0 for all non-decimal digit,
     preferred sign)
  decnewsign is the value of the preferred sign (0 neg, 1 pos)
  decoutsign is the output characters for sign
  add_d is the object wrapper for addtbl (decimal addition)
  sub_d is the object wrapper for subtbl (decimal subtraction)

        digit value          0 1 2 3 4 5 6 7 8 9 A B C D E F */
 static int decsign[16]=   { 1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1 };
 static int decinvsign[16]={ 0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0 };
 static int decclean[16]=  { 0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0 };
 static int decnewsign[2]= { 13,15 };   /* preferred sign
                for AS/400.  If S/370 preferred sign wanted,
                use { 13,12 }, but either works on both.    */
 static UCHAR decoutsign[2]= { '-',' '}; /* for output      */

  /* function:  decregDoAdd
     purpose:   perform decimal addition after previous
                routines decided it really is a decimal add.
     input:     decreg decimal data string with
                number of digits and number of fractional
                digits.  (data2)
     input/output:  decreg decimal data string with
                number of digits and number of fractional
                digits (data)
     notes:  This function actually does the Add.  It is
        called from Add, Sub, Mul.                          */
  void decregDoAdd(decimal_register* data,
       const decimal_register* data2)
  {
    int i,j; int cry=0;

      /* do actual addition                                 */
      for (i=( NDIG-1) ; i>=0; --i)
      { /* perform actual addition                          */
        j = (data->digit[i])+(data2->digit[i])+cry;
        if(j>=10) { cry=1; j-=10; data->digit[i]=j; }
        else { cry=0; data->digit[i]=j; };
      };  /* end for loop that does actual addition         */

  };

  /* function:  decregDoSub
     purpose:   perform decimal subtraction after previous
                routines decided it really is a decimal subtract.
     input:     decreg decimal data string with
                number of digits and number of fractional
                digits.  (data2,NDIG2,NFRAC2)
     input/output:  decreg decimal data string with
                number of digits and number of fractional
                digits (data, NDIG, NFRAC)
     notes:  This function actually does the Subtract.  It
        is called from Add, Sub, and Div.

       This routine ASSUMES that the left hand side is greater
       than the right hand side.                            */
  void decregDoSub(decimal_register* data,
       const decimal_register* data2)
  {
    int i,j; int brw=0;

      /* do actual subtraction                              */
      for (i= (NDIG-1); i>=0 ; --i)
      { /* perform subtract                                 */
        j = (data->digit[i]); j-=(data2->digit[i]); j-=brw;
        if(j<0) { brw=1; j+=10; data->digit[i]=j; }
        else { brw=0; data->digit[i]=j; };
      };  /* end for loop that does actual subtraction      */

  };

  /* function:  decregDoSubR
     purpose:   perform decimal subtraction after previous
                routines decided it really is a decimal subtract.
     input:     decreg decimal data string with
                number of digits and number of fractional
                digits.  (data2,NDIG2,NFRAC2)
     input/output:  decreg decimal data string with
                number of digits and number of fractional
                digits (data, NDIG, NFRAC)
     notes:  This function actually does the Subtract.  It
        is called from Add, Sub, and Div.

     This routine ASSUMES that the right hand side is greater
     than the left hand side.
     This handles the oddball case of  a+b where a is target,
       and is neg and b is positive.                        */
  void decregDoSubR(decimal_register* data,
       const decimal_register* data2)
  {
    int i,j; int brw=0;

      /* do actual subtraction                              */
      for (i= (NDIG-1) ; i>=0; --i)
        /* note reverse of who goes where in this next line */
      { j = data2->digit[i]; j-=(data->digit[i]); j-=brw;
        if(j<  0) { brw=1; j+=10; data->digit[i]=j; }
        else { brw=0; data->digit[i]=j; };
      };  /* end for loop that does actual subtraction      */

  };

  /* function:  decregCompAbs
     purpose:   Compare two signed decreg decimal numbers
     input:     two decreg decimal data strings with
                number of digits and number of fractional
                digits.  (data,NDIG,NFRAC; data2,NDIG2,NFRAC2)
                The sign is ignored; only absolute value used.
     output:    integer -1 (left < right) 0 (left == right) 1
                (left>right)
     notes:     decregComp following compares the
                signed value and is the general case function. */
  int decregCompAbs(const decimal_register* data,
       const decimal_register* data2)
  {
   /* This routine is used for 'absolute value compare'
      in add and subtract.                                  */
   int i;
   for(i=0; i<NDIG; ++i) {
     if ((data->digit[i])>(data2->digit[i])) return 1;
     else if ((data->digit[i])<(data2->digit[i])) return -1; }
   return 0;
  };

  /* function:  compReg
     purpose:   Compare two signed decreg decimal numbers
     input:     two decreg decimal data strings with
                number of digits and number of fractional
                digits.  (data,data2)
     output:    integer -1 (left < right) 0 (left == right)
                1 (left>right)
     notes:     decregCompAbs above compares the absolute
                value and is a special case function        */
  int compReg(const decimal_register* data,
       const decimal_register* data2)
  { decimal_register  temp;
    int j,i;
    for (j=0; j<(NDIG); ++j) temp.digit[j]=data->digit[j];
    temp.sign = data->sign;
   subReg(&temp,data2);

   /* if subtraction negative, return -1 as it isn't zero    */
   if (decsign[temp.sign]==decsign[13])
     return -1;            /* found negative nonzero         */

   /* positive or zero case                                  */
   for (i=0; i<(NDIG); ++i)
    if (temp.digit[i]>0)
      return 1;            /* found positive nonzero         */

   /* found zero case                                        */
   return 0;
  };

  /* function:  addReg,subReg
     purpose:   Adds (subtracts) two signed decreg decimal numbers
     input:     two decreg decimal data strings with
                number of digits and number of fractional
                digits.  (data,NDIG,NFRAC; data2,NDIG2,NFRAC2)
     output:    first decreg decimal string has sum (difference)
                of first string plus (minus) second string.     */
  void addReg(decimal_register *data,
       const decimal_register *data2)
  {
     if (decsign[data->sign]==decsign[data2->sign])
       if (data->sign==decsign[15]) /* positive sign     */
         decregDoAdd(data,data2);
       else                      /* negative sign        */
         decregDoAdd(data,data2);
     else                        /* sign mis-match       */
       { int i=decregCompAbs(data,data2);
       if (i== -1)               /* rslt < src           */
         decregDoSubR(data,data2);
       else                      /* positive sign source */
         if (i==1)
          decregDoSub(data,data2);
         else
           { int j;
             for (j=0; j<(NDIG); ++j) data->digit[j]=0;
             data->sign = decnewsign[1];
           };
       };
  }; /* end addReg     */

 /* see addReg above for prologue                           */
 void subReg(decimal_register *data,
       const decimal_register* data2)
 {
    /* Must not use packedComp.  May use packedCompAbs.     */

      if (data->sign == data2->sign)     /* signs agree     */
        { int i=decregCompAbs(data,data2);
        if (i==-1)                       /* |lhs| < |rhs|   */
          {
            decregDoSubR(data,data2);
            /* Copy over the inverted sign from right
                         hand side.*/
            data->sign = decnewsign[decinvsign[data2->sign]];
          }
        else
          if (i==1)                      /* |lhs| > |rhs|   */
            decregDoSub(data,data2);
          else
            { int j;                      /* |lhs| == |rhs| */
              for (j=0; j<(NDIG); ++j) data->digit[j]=0;
              data->sign= decnewsign[1];
            };
        }
      else                          /* signs don't agree    */
        { int i=decregCompAbs(data,data2);
        if (i==-1)                        /* |lhs| < |rhs|  */
          decregDoAdd(data,data2);
        else
          if (i==1)                       /* |lhs| >= |rhs| */
            decregDoAdd(data,data2);
           else
            {                             /* |lhs| == |rhs| */
              decregDoAdd(data,data2);
            };
        } ;
  };

  /* function:  cpyReg
     purpose:   Copies a signed decreg string to another
     input:     two decreg decimal data strings.
     output:    first decreg string has copy of the other   */
  void cpyReg(decimal_register *data,
              const decimal_register *data2)
  { int i,ii,j;

      /* copy over sign bit          */
      j= data2->sign;
      data->sign=j;

      /* do actual movment           */
      for (ii= 0 ; ii <  NDIG; ++ii)
      { data->digit[ii]=data2->digit[ii];
      };  /* end for loop that does actual digit copy       */

  };     /* end cpyReg                                      */

  /* function:  lshiftReg
     purpose:   Shifts a signed decreg decimal number left by
                amount in second parameter.  If shift results in
                0, this is handled.  Shift is one decimal digit
                per number in second count.
     input:     one decreg decimal data string with
                number of digits and number of fractional
                digits.  (data,NDIG,NFRAC)
                integer count of decimal digits to left shift.
     output:    first packed decimal string has shifted number
                with zero handled correctly.                */
  void lshiftReg(decimal_register *data,const int shft)
  { int i,j,tmp; int allz=1;

      /* individual shifts, except last time.               */
      for(j=shft; j>1; --j) {
       for(i=0; i<(NDIG-1); ++i)  { tmp=(data->digit[i+1]);
                                    data->digit[i]=tmp;};
       data->digit[NDIG-1]=0;
      };    /* end for j */

      /* very last shift include check for zeros to fix sign*/
      for( ; j>0; --j) {
       for(i=0; i<(NDIG-1); ++i)
       { tmp=(data->digit[i+1]); data->digit[i]=tmp;
         if (tmp!=0) allz=0;
       };    /* end for i. . .  */
       data->digit[NDIG-1]=0;
      };      /* end for j. . . */
      if ((allz==1)&&(shft>0))
         data->sign= decnewsign[1]; /* result zero,
                                         so set + sign      */
  };  /* end lshiftReg (Left Shift in place)                */

  /* function:  rshiftReg
     purpose:   Shifts a signed decreg decimal number right by
                amount in second parameter.  If shift results in
                0, this is handled.  Shift is one decimal digit
                per number in second count.
     input:     one decreg decimal data string with
                number of digits and number of fractional
                digits.  (data,NDIG,NFRAC)
                integer count of decimal digits to left shift.
     output:    first decreg decimal string has shifted number
                with zero handled correctly.                */
  void rshiftReg(decimal_register* data,const int shft)
  { int i,j,tmp; int allz=1;
      /* all but last time                                  */
      for(j=shft; j>1; --j) {
       for(i=(NDIG-1); i>0; --i) { tmp=(data->digit[i-1]);
                                   data->digit[i]=tmp; };
       data->digit[0]=0;
      };    /* end for j                            */
      /* last time, include check for all zeroes    */
      for( ; j>0; --j) {
       for(i=(NDIG-1); i>0; --i) {
         tmp=(data->digit[i-1]);data->digit[i]=tmp;
         if (tmp!=0) allz=0;
       };    /* end for i. . .     */
       data->digit[0]=0;
      };     /* end for j. . .     */
      if ((allz==1)&&(shft>0))
        data->sign= decnewsign[1]; /* result zero,
                                         so set + sign      */
  };  /* end rshiftReg (Right Shift in place)               */

  void outReg(const decimal_register *data) {
       int i;
       printf("%c",decoutsign[decsign[data->sign]]);
       for (i=0; i<NDIG ; ++i) printf("%d",data->digit[i]);
  };  /*  end outReg                            */

  void packedToReg(decimal_register *data,const UCHAR *data2,
               const int ndig, const int nfrac) {
    /* assumes ndig is less than or equal to 14 digits
       before the decimal point & less than or equal to 5
       digits after the decimal point             */
    int ii=(ndig+1)/2;    /* total bytes in data2 */
    int i,j;
    char temp;
    /*   zero out digits which precede data2      */
    for (j=0; j<(NDIG-ndig); ++j) data->digit[j]=0;

    /*   take care of digits following 2 at a time
         note that this right justifies number in the
         field, including the sign   */
    for (i=0; i<ii; ++i)  {
        temp = (data2[i]>>4);
        temp &= 0x0F;
        data->digit[j++] = temp;
        temp = (data2[i]);
        temp &= 0x0F;
        data->digit[j++] = temp;
    };

    /*  The packed decimal number has been translated,
           right justified, into the field.  Now adjust
           for the decimal point difference, if any */
    if (NFRAC!=nfrac)
      if (NFRAC>nfrac) lshiftReg(data,NFRAC-nfrac);
      else rshiftReg(data,nfrac-NFRAC);
    cleanReg(data);    /* ensure digits, sign well-defined  */

 };   /* end packedtoReg                          */

  /* function:  cleanReg
     purpose:   Cleans (sets to valid digits) a decreg decimal
                number.  Performs "cleanse" function.
     input:     one decreg decimal data string with
                number of digits and number of fractional
                digits.  (data,NDIG,NFRAC)
     output:    same decreg decimal string either unchanged or
                with invalid digits set to 0, sign changed
                to preferred sign.             */
  void cleanReg(decimal_register *data) {
    int i;
    /* cleanse digits                                       */
    for (i=0; i<(NDIG-1); ++i) data->digit[i]=
                               decclean[data->digit[i]];
    /* cleanse sign                                         */
    data->sign= decnewsign[decsign[data->sign]];
  };
