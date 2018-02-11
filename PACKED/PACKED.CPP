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

// Implementation of packed decimal functions
// Larry W. Loen

// Included when:  PACKDEF_IMP is defined

// Include from:  packed.H

// Class:  decop
// Parent Class:  none
// Type:   concrete
// Purpose:
//     Support packed<ndig,nfrac> class by providing an interface
//     to do a table look up (esp. for addition) so as
//     to have a portable interface to decimal arithmetic.
// Behavior:
//     void packedAdd(UCHAR* w,int x,int y, UCHAR* z int a, int b)
//     {  decop  add_d(int *table_of_dec_add);
//     int i,j,k, carry=0;
//     i = next_dec_digit(w);
//     j = next_dec_digit(z);
//     k = add_d(i,j+carry);  // do add via table lookup
//     if (k>10) { k-=10; carry=1; }; // account for carry (typical)
//     w = x + y;               // x and y both become vpacked
// Virtual functions:  No (none allowed)
// Friends:  No.
// Delete/Copy Ctor/Assignment:  Assignment only.
// Overloaded operators:  ( ) (as old-style FORTRAN 2-d Array Ref)
// Printable:  No.
// Traceable:  No.
// Static Data: Indirectly; static data sent to constructor.
//
// Notes:  "Helper" class; for this module only.
//
//     See static instances:  add_d and sub_d in this module.

  class decop  { // define decimal operations as a table function
     int *loc;   // a 11x10 array of integers (includes carry)
   public:
     decop(int *l) : loc(l) {};
     int operator() (int a,int b) { return loc[(a*11)+b]; };
  };  // end 'helper' class decop

// Class:  ddref
// Parent Class:  none
// Type:   concrete
// Purpose:
//     Support decdig and cdecdig classes by providing some needed
//     support to extract an individual digit or store an
//     individual digit.
// Behavior:
//     decdig x(UCHAR *some_user_packed_data);
//     // These statements create ddref as temps as part of []
//     // implementation
//     x[0] = 0;       // set high order dec digit to 0
//     x[1] = x[3];    // move x[3] to x[1]
//
// Virtual functions:  No (none allowed)
// Friends:  <<  Shift (ersatz assignment operator).
// Overloaded operators:  +  (add two digits together)
// Delete/Copy Ctor/Assignment:  Assignment only.
// Printable:  Yes
// Static Data:  None.
// Notes:  Cannot have virtual functions as this must map over PLI,
//     COBOL or RPG data structures.
//     This class should not be routinely instantiated.
  class ddref  { // reference to a decimal digit
    friend ostream& operator << (ostream &x,ddref &y)
           { x << y(); return x; };
    friend int operator << (int &x,ddref &y)
           { x = y(); return x; };

    UCHAR *x;
    int y;
   public:
    ddref(UCHAR *xx,int yy) : x(xx),y(yy) {};
    int operator() ()  { int xx; if (y&1) xx= x[y/2]& 0x0F;
                                 else xx= (x[y/2]>>4)&15;
                         return xx; };
    ddref& operator= (int z)  {
            if (y&1) {x[y/2] &= 0xF0;
                      x[y/2] |= (z&0x0F);}
            else     { x[y/2]&= 0x0F;
                       int zz= (z<<4)& (15*16);
                       x[y/2] |= zz;
                     };
            return *this;
                              };
    ddref& operator= (ddref &z) { (*this)= z(); return *this; };
    int operator+ (int x) { return (*this)()+x; };
    UCHAR operator+ (UCHAR x) { return (*this)()+x;};
    int operator== (int x) { return (*this)()==x;};
 };  // end ddref

// Class:  decdig, cdecdig
// Parent Class:  none
// Type:   concrete
// Purpose:
//     Support packed<ndig,nfrac> class by providing an interface
//     to give an lvalue or an rvalue for an individual decimal digit.
//     NOTE:  The only difference between decdig and cdecdig is that
//     the latter is an rvalue only.
// Behavior:
//     void packedAdd(UCHAR* w,int x,int y, UCHAR* z int a, int b)
//     {
//     decdig w_translate(w);
//     i = w_translate[0];  // fetch the high order dec. digit of w
//
// Virtual functions:  No (none allowed)
// Friends:  No.
// Delete/Copy Ctor/Assignment:  Assignment only
// Overloaded operators:  []  ("subscript" extract of decimal digits)
// Printable:  No.
// Static Data:  None.
// Notes:  Cannot have virtual functions as this must map over PLI
//     COBOL or RPG data structures.
//     This class should only be instantiated in this module.
  class decdig  { // extract digits from UCHAR array
    private:
     UCHAR *x;
    public:
      decdig(UCHAR *y) : x(y) {};
      decdig(packed<39,10> &y) : x(y.data) {};
      ddref operator[] (int z)
         { return ddref(x,z);};
  }; // end decdig -- decimal digit

//
// see decdig prologue above for cdecdig
//
  class cdecdig  { // extract digits from UCHAR array
     const UCHAR *x;
    public:
      cdecdig(const UCHAR *y) : x(y) {};
      cdecdig(const packed<39,10> &y) : x(y.data) {};
      int operator[] (int z)     // rvalue only for cdecdig
        { if (z&1) return x[z/2]&15;
          else return (x[z/2]/16)&15; };
  }; // end cdecdig -- const decimal digit array

 //
 // Table lookup for decimal add
 //
 static int addtbl[110]= {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
    1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,
    2, 3, 4, 5, 6, 7, 8, 9,10,11,12,
    3, 4, 5, 6, 7, 8, 9,10,11,12,13,
    4, 5, 6, 7, 8, 9,10,11,12,13,14,
    5, 6, 7, 8, 9,10,11,12,13,14,15,
    6, 7, 8, 9,10,11,12,13,14,15,16,
    7, 8, 9,10,11,12,13,14,15,16,17,
    8, 9,10,11,12,13,14,15,16,17,18,
    9,10,11,12,13,14,15,16,17,18,19
 };

 //
 // Table lookup for decimal subtract
 //
 static int subtbl[110]= {
    0,19,18,17,16,15,14,13,12,11,10,
    1, 0,19,18,17,16,15,14,13,12,11,
    2, 1, 0,19,18,17,16,15,14,13,12,
    3, 2, 1, 0,19,18,17,16,15,14,13,
    4, 3, 2, 1, 0,19,18,17,16,15,14,
    5, 4, 3, 2, 1, 0,19,18,17,16,15,
    6, 5, 4, 3, 2, 1, 0,19,18,17,16,
    7, 6, 5, 4, 3, 2, 1, 0,19,18,17,
    8, 7, 6, 5, 4, 3, 2, 1, 0,19,18,
    9, 8, 7, 6, 5, 4, 3, 2, 1, 0,19,
 };

 // forward declare output routine for packed decimal output
 void outPackedC(ostream &x,const UCHAR* data,
                 int ndig,int nfrac); // used for print, see later

 // Necessary constants:
 //
 //  decsign supplies index for proper sign into decnewsign
 //  decinvsign suppolies index for proper inversion of a sign
 //  decclean is used to 'clean' a potentially incorrect decimal number
 //     to a defined value (0 for all non-decimal digit, preferred sign)
 //  decnewsign is the value of the preferred sign (0 neg, 1 pos)
 //  decoutsign is the output characters for sign
 //  add_d is the object wrapper for addtbl (decimal addition)
 //  sub_d is the object wrapper for subtbl (decimal subtraction)
 //
 //     digit value          0 1 2 3 4 5 6 7 8 9 A B C D E F
 static int decsign[16]=   { 1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1 };
 static int decinvsign[16]={ 0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0 };
 static int decclean[16]=  { 0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0 };
 static int decnewsign[2]= { 13,15 };   // preferred sign, AS/400
 static UCHAR decoutsign[2]= { '-',' '}; // for output

 static decop add_d(addtbl);

 static decop sub_d(subtbl);

  // function:  packedDoAdd
  // purpose:   perform decimal addition after previous
  //            routines decided it really is a decimal add.
  // input:     packed decimal data string with
  //            number of digits and number of fractional
  //            digits.  (data2,ndig2,nfrac2)
  // input/output:  packed decimal data string with
  //            number of digits and number of fractional
  //            digits (data, ndig, nfrac)
  // notes:  This function actually does the Add.  It is
  //    called from Add, Sub, Mul.
  void packedDoAdd(UCHAR* data,int ndig,int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrac2)
  {
    int i,ii,j; int cry=0;
    decdig rslt(data);
    cdecdig src(data2);

    // General outline:
    //   1) check to see if well-defined (digits in right magnitude)
    //   2) 3 for loops.
    //      A) logically copy over any excess fractional digits.
    //      B) Do actual addition
    //      C) copy over any excess integer digits, accounting for carry

    //   1) well-defined check
    if((ndig>=nfrac) && (ndig2>=nfrac2)) {   // if well-defined

      // 2) A)
      // copy over any excess fractional digits from result to new result
      for (i=(ndig-1); (nfrac>nfrac2)&&(i>(ndig-(nfrac-nfrac2)-1)); --i) ;
         // the above for loop is logically:  rslt[i]=rslt[i];

      // 2) B)
      // do actual addition on part both have digits for
      // account for fact src may have excess fractional digits over rslt
      // note carryover of i from previous loop
      for (ii=( (nfrac2<=nfrac) ? (ndig2-1):(ndig2-1-(nfrac2-nfrac)) );
                                    // init src digit
          (i>=0)&&(ii>=0);          // escape when either is done
          --i,--ii )                // decrement both variables
      { // perform actual table lookup
        j = add_d(rslt[i](),src[ii]+cry);
        if(j>=10) { cry=1; j-=10; rslt[i]=j; }
        else { cry=0; rslt[i]=j; };
      };  // end for loop that does actual addition

      // 2) C)
      // copy over any excess digits from result, account for carry
      for (  ; i>=0; --i) {
           j= add_d(rslt[i](),cry);
           if (j>=10) { cry=1; j-=10; rslt[i]= j; }
           else { cry=0; rslt[i]=j;
           };
      };
    };  // if well-defined

  };

  // function:  packedDoSub
  // purpose:   perform decimal subtraction after previous
  //            routines decided it really is a decimal subtract.
  // input:     packed decimal data string with
  //            number of digits and number of fractional
  //            digits.  (data2,ndig2,nfrac2)
  // input/output:  packed decimal data string with
  //            number of digits and number of fractional
  //            digits (data, ndig, nfrac)
  // notes:  This function actually does the Subtract.  It
  //    is called from Add, Sub, and Div.
  //
  // This routine ASSUMES that the left hand side is greater than
  //   the right hand side.
  void packedDoSub(UCHAR* data,int ndig,int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrac2)
  {
    int i,ii,j; int brw=0;
    decdig rslt(data);
    cdecdig src(data2);

    // General outline:
    //   1) check to see if well-defined (digits in right magnitude)
    //   2) 3 for loops.
    //      A) logically copy over any excess fractional digits.
    //      B) Do actual subtraction
    //      C) copy over any excess integer digits, accounting for carry

    //   1) well-defined check
    if((ndig>=nfrac) && (ndig2>=nfrac2)) {   // if well-defined

      // 2) A)
      // copy over any excess fractional digits from result to new result
      for (i=(ndig-1); (nfrac>nfrac2)&&(i>(ndig-(nfrac-nfrac2)-1)); --i) ;
         // the above for loop is logically:  rslt[i]=rslt[i];

      // 2) B)
      // do actual subtraction on part both have digits for
      // account for fact src may have excess fractional digits over rslt
      // note carryover of i from previous loop
      for (ii=( (nfrac2<=nfrac) ?
                (ndig2-1):(ndig2-1-(nfrac2-nfrac)) ); // init src digit
          (i>=0)&&(ii>=0);          // escape when either is done
          --i,--ii )                // decrement both variables
      { // perform subtract via table lookup
        j = sub_d(rslt[i](),src[ii]+brw);
        if(j>=10) { brw=1; j-=10; rslt[i]=j; }
        else { brw=0; rslt[i]=j; };
      };  // end for loop that does actual subtraction

      // 2) C)
      // copy over any excess digits from result, account for carry
      for (  ; i>=0; --i) {
           j= sub_d(rslt[i](),brw);
           if (j>=10) { brw=1; j-=10; rslt[i]= j; }
           else { brw=0; rslt[i]=j; };
      };

    };  // if well-defined

  };

  // function:  packedDoSubR
  // purpose:   perform decimal subtraction after previous
  //            routines decided it really is a decimal subtract.
  // input:     packed decimal data string with
  //            number of digits and number of fractional
  //            digits.  (data2,ndig2,nfrac2)
  // input/output:  packed decimal data string with
  //            number of digits and number of fractional
  //            digits (data, ndig, nfrac)
  // notes:  This function actually does the Subtract.  It
  //    is called from Add, Sub, and Div.
  //
  // This routine ASSUMES that the right hand side is greater than
  //   the left hand side.
  // This handles the oddball case of  a+b where a is target,
  //   and is neg and b is positive.
  void packedDoSubR(UCHAR* data,int ndig,int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrac2)
  {
    int i,ii,j; int brw=0;
    decdig rslt(data);
    cdecdig src(data2);

    // General outline:
    //   1) check to see if well-defined (digits in right magnitude)
    //   2) 3 "for" loops.
    //      A) logically copy over any excess fractional digits.
    //      B) Do actual subtraction
    //      C) copy over any excess integer digits, accounting for carry
    //   3) Copy over inverse sign of right hand side to left hand side.

    // 1) well-defined check
    if((ndig>=nfrac) && (ndig2>=nfrac2)) {   // if well-defined
      // 2 A)
      // copy over any excess fractional digits from result to new result
      for (i=(ndig-1); (nfrac>nfrac2)&&(i>(ndig-(nfrac-nfrac2)-1)); --i) ;
         // the above for loop is logically:  rslt[i]=rslt[i];

      // 2) B)
      // do actual subtaction on part both have digits for
      // account for fact src may have excess fractional digits over rslt
      // note carryover of i from previous loop
      for (ii=( (nfrac2<=nfrac) ? (ndig2-1):(ndig2-1-(nfrac2-nfrac)) );
                                    // init src digit
          (i>=0)&&(ii>=0);          // escape when either is done
          --i,--ii )                // decrement both variables
        // note reverse of who goes where in this next line
      { j = sub_d(src[ii],rslt[i]+brw);
        if(j>=10) { brw=1; j-=10; rslt[i]=j; }
        else { brw=0; rslt[i]=j; };
      };  // end for loop that does actual subtraction

      // 2) C)
      // copy over any excess digits from result, account for carry
      for (  ; i>=0; --i) {
           j= sub_d(0,rslt[i]+brw);
           if (j>=10) { brw=1; j-=10; rslt[i]= j; }
           else { brw=0; rslt[i]=j; };
      };

      // 3) Copy over the sign from right hand side.
      rslt[ndig]= decnewsign[decsign[src[ndig2]]];
    };  // if well-defined

  };

  // function:  outPacked, outPackedC
  // purpose:   outputs packed decimal number
  // input:     packed decimal data string with
  //            number of digits and number of fractional
  //            digits.  (data2,ndig2,nfrac2)
  // output:    ostream file (x)
  // notes:     outPackedC same as outpacked, except
  //            input string is const
  void outPacked(ostream &x,UCHAR* data,int ndig,int nfrac)
  {
    int i,j;
    decdig rslt(data);
    UCHAR rsltc;
    i = (rslt[ndig])();  // extract value of last (sign) digit
                         //    & assign (not shift)
    x << decoutsign[ decsign[ i]]; // output sign digit

    if (ndig==nfrac) x << '.';     // leading dec point case

    for (i=0; i<(ndig); ++i)
    {
      rsltc = rslt[i]() + '0';
      x << rsltc;
      if (i==(ndig-nfrac-1)) x << '.';   // output dec point
    };

  };  // end outPacked

  void outPackedC(ostream &x,const UCHAR* data,int ndig,int nfrac)
  {
    int i,j;
    cdecdig rslt(data);
    UCHAR rsltc;
    i = rslt[ndig];  // extract value of last (sign) digit
                     //    & assign (not shift)
    x << decoutsign[ decsign[ i]]; // output sign digit

    if (ndig==nfrac) x << '.';     // leading dec point case

    for (i=0; i<(ndig); ++i)
    {
      rsltc = rslt[i] + '0';
      x << rsltc;
      if (i==(ndig-nfrac-1)) x << '.';   // output dec point
    };

  };  // end outPacked

  void inPacked(istream &x,UCHAR* data,int ndig,int nfrac);

  // function:  packedCompAbs
  // purpose:   Compare two signed packed decimal numbers
  // input:     two packed decimal data strings with
  //            number of digits and number of fractional
  //            digits.  (data,ndig,nfrac; data2,ndig2,nfrac2)
  //            The sign is ignored; only absolute value used.
  // output:    integer -1 (left < right) 0 (left == right) 1
  //            (left>right)
  // notes:     packedComp following compares the
  //            signed value and is the general case function.
  int packedCompAbs(const UCHAR* data,const int ndig,const int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrac2)
  {
   // This routine assumes that ndig and ndig2 are identical
   // It is used for 'absolute value compare' in add and subtract.
   cdecdig rslt(data);
   cdecdig src(data2);
   int i;
   for(i=0; i<ndig; ++i) {
     if (rslt[i]>src[i]) return 1;
     else if (rslt[i]<src[i]) return -1; }
   return 0;
  };

  // function:  packedComp
  // purpose:   Compare two signed packed decimal numbers
  // input:     two packed decimal data strins with
  //            number of digits and number of fractional
  //            digits.  (data,ndig,nfrac; data2,ndig2,nfrac2)
  // output:    integer -1 (left < right) 0 (left == right) 1
  //            (left>right)
  // notes:     packedCompAbs above compares the
  //            absolute value and is a special case function.
  //            This one handles variable number of digits.
  //            See packedSub for restrictions on variability.
  int packedComp(const UCHAR* data,const int ndig,const int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrac2)
  { UCHAR* temp;
    int j;
    temp = new UCHAR((ndig+1)/2);
    for (j=0; j<((ndig+1)/2); ++j) temp[j]=data[j];
   packedSub(temp,ndig,nfrac,data2,ndig2,nfrac2);
   cdecdig rslt(temp);
   int i;

   // if subtraction negative, return -1 as it isn't zero
   if (decsign[rslt[ndig]]==decsign[13])
     { delete temp; return -1; };

   // positive or zero case
   for (i=0; i<ndig; ++i)
    if (rslt[i]>0)
      { delete temp; return 1; }; // found positive nonzero

   // found zero case
   delete temp;
   return 0;
  };

  // function:  packedAdd,packedSub
  // purpose:   Adds (subtracts) two signed packed decimal numbers
  // input:     two packed decimal data strins with
  //            number of digits and number of fractional
  //            digits.  (data,ndig,nfrac; data2,ndig2,nfrac2)
  // output:    first packed decimal string has sum (difference) of
  //            first string plus (minus) second string.
  // notes:     Unlimited precision if ndig==ndig2, nfrac==nfrac2,
  //            limited to <19,5> otherwise.
  void packedAdd(UCHAR* data,int ndig,int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrac2)
  {
    decdig rslt(data);
    cdecdig src(data2);
    if ((ndig==ndig2)&&(nfrac==nfrac2))
     // if same type of decimal numbers, then...
     if (decsign[rslt[ndig]()]==decsign[src[ndig2]]) // if signs agree
       if (rslt[ndig]==decsign[15]) // positive sign
         packedDoAdd(data,ndig,nfrac,data2,ndig2,nfrac2);
       else                      // negative sign
         packedDoAdd(data,ndig,nfrac,data2,ndig2,nfrac2);
     else                        // sign mis-match
       { int i=packedCompAbs(data,ndig,nfrac,
                            data2,ndig2,nfrac2);
       if (i== -1)               // rslt < src
         packedDoSubR(data,ndig,nfrac,data2,ndig2,nfrac2);
       else                      // positive sign source
         if (i==1)
          packedDoSub(data,ndig,nfrac,data2,ndig2,nfrac2);
         else
           { int j;
             for (j=0; j<ndig; ++j) rslt[j]=0;
             rslt[ndig]= decnewsign[1];
           };
       }
    else
       {  // if numbers not of same type, copy to
          // max supported size & recurse
          packed<19,5> rslt1(data,ndig,nfrac);
          vpacked orig(data,ndig,nfrac);  // copy of original
          const packed<19,5> src1(data2,ndig2,nfrac2);
          rslt1+=src1;     // recursive call.
          orig = rslt1;    // assign result (truncates correctly)
       };
  }; // end packedAdd

 // see packedAdd above for prolog
 void packedSub(UCHAR* data,int ndig,int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrac2)
 {
    // Must not use packedComp.  May use packedCompAbs.
    decdig rslt(data);
    cdecdig src(data2);

    if ((ndig==ndig2) && (nfrac==nfrac2)) // everything matches
      if (rslt[ndig]==src[ndig2])         // signs agree
        { int i=packedCompAbs(data,ndig,nfrac,
                          data2,ndig2,nfrac2);
        if (i==-1)                        // |lhs| < |rhs|
          {
            packedDoSubR(data,ndig,nfrac,data2,ndig2,nfrac2);
            // Copy over the inverted sign from right hand side.
            rslt[ndig]= decnewsign[decinvsign[src[ndig2]]];
          }
        else
          if (i==1)                       // |lhs| > |rhs|
            packedDoSub(data,ndig,nfrac,data2,ndig2,nfrac2);
          else
            { int j;                      // |lhs| == |rhs|
              for (j=0; j<ndig; ++j) rslt[j]=0;
              rslt[ndig]= decnewsign[1];
            };
        }
      else                                // signs don't agree
        { int i=packedCompAbs(data,ndig,nfrac,
                          data2,ndig2,nfrac2);
        if (i==-1)                        // |lhs| < |rhs|
          packedDoAdd(data,ndig,nfrac,data2,ndig2,nfrac2);
        else
          if (i==1)                       // |lhs| >= |rhs|
            packedDoAdd(data,ndig,nfrac,data2,ndig2,nfrac2);
           else
            {                                // |lhs| == |rhs|
              packedDoAdd(data,ndig,nfrac,data2,ndig2,nfrac2);
            };
        }
    else
       {  // if numbers not of same type, copy to
          // max supported size & recurse
          packed<19,5> rslt1(data,ndig,nfrac);
          vpacked orig(data,ndig,nfrac);  // copy original result
          const packed<19,5> src1(data2,ndig2,nfrac2);
          rslt1-=src1;     // recursive call.
          orig = rslt1;    // assign result (truncates correctly)
       };
  };

  // function:  packedMul,packedDiv
  // purpose:   Multiplies (divides) two signed packed decimal numbers
  // input:     two packed decimal data strings with
  //            number of digits and number of fractional
  //            digits.  (data,ndig,nfrac; data2,ndig2,nfrac2)
  // output:    first packed decimal string has sum of orignal
  //            string plus second string.
  // notes:     Precision limited to max of <19,5> whether ndig==ndig2
  //            or not for both of these.
  void packedMul(UCHAR* data,int ndig,int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrac2) {
    int i, j, allzero=1;
    vpacked src1(data,ndig,nfrac);
    decdig rslt(data);
    cdecdig src(data2);
    vpacked src2(data2,ndig2,nfrac2,1);
    packed<39,10> op1= 0;
    packed<39,10> op3(data,ndig,nfrac);
    cdecdig op3c(op3);
    packed<39,10> dbg(25);

    {
    packed<39,10> op2;
    op2=src2;
    decdig op2c(op2);
    op2c[39]=rslt[ndig]; // make signs agree
    // multiply digits left of dec point first.  Don't
    // bother with any more digits than necessary
    for (i=(38-10); i>(28-(ndig-nfrac)); --i)  {
     for ( j=op3c[i]; j>0; --j) op1+=op2;
     //if ( (op3 >= dbg) && (op3c[i]!=0))
     //  cout << "Debug mul: " << i << ',' << j  << ':' << op2c[i]()
     //     << ' ' << op1 << ' ' << op2 << endl;
     op2<<= 1;  // shift op 2 one to left
     };
    };  // boundary for first op2

    // do right of decimal point
    {
    packed<39,10> op2;
    op2=src2;
    op2>>=1;     // get it over into 'tens' position
    decdig op2c(op2);
    op2c[39] = rslt[ndig]; // make signs agree
    // multiply digits left of dec point first
    for (i=(39-10); i<(29+nfrac); ++i)  {
     for (j=op3c[i]; j>0; --j) op1+=op2;
     //if ((op3 > dbg)&& (op3c[i]!=0))
     //  cout << "Debug mul: " << i << ',' << j << ':' << op2c[i]()
     //     << ' ' << op1 << ' ' << op2 << endl;
     op2>>= 1;  // shift op 2 one to right
     };
    };  // boundary for second op2

    // return result, get sign right
    src1 = op1;     // magnitude copied, but maybe sign wrong
    for(i=0;i<ndig;++i) if ( rslt[i]()!=0) allzero=0;
    if ( (allzero!=1) && (rslt[ndig]() != src[ndig2]))
      rslt[ndig]= decnewsign[0];
    else rslt[ndig]= decnewsign[1];
    //cout << "Final Answer: " << src1 << endl;

  }; // end packedMul

  // see packedMul above for prolog
  void packedDiv(UCHAR* data,int ndig,int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrac2)
  {  packed<39,10> divid; divid = vpacked(data,ndig,nfrac);
     packed<39,10> dvsr; dvsr = vpacked(data2,ndig2,nfrac2,1);
     packed<39,10> subtr = 1;
     packed<39,10> rslt=0;
     const packed<19,5> zerod=0;    // for sorting out sign later
     int i,j;

     dvsr <<= 19;      // shift over 19 places
     subtr <<= 19;     // shift over 19 places

     for (i=0; i<20; ++i) {  // do the division
        // cout << "divid:" << divid << '\n';
        // cout << "dvsr :" << dvsr << '\n';
        while ( packedCompAbs(divid.data,39,10,dvsr.data,39,10)>=0)
           { rslt+=subtr; packedDoSub(divid.data,39,10,
                                       dvsr.data,39,10);
	      // cout << "rslt: " <<rslt << '\n';
	   }; // end while loop
           subtr>>=1; dvsr>>=1;
     };  // end for loop
     packedCpy(data,ndig,nfrac,rslt.data,39,10);

     // set sign in result
     if (  ( (data[(ndig/2)]&0x0F)!=(data2[ndig2/2]&0x0F) )
          && ( packedCompAbs(data,ndig,nfrac,zerod.data,ndig,nfrac)!=0 )  )
       data[ndig/2]=((data[ndig/2]&0xF0)| decnewsign[0]);
     else data[ndig/2]=((data[ndig/2]&0xF0)| decnewsign[1]);
 };  // end packedDiv

  // function:  packedCpy
  // purpose:   Copies a signed packed decimal number to another
  //            Copy handles different sizes on both sides of
  //            decimal point and does truncation correctly.
  // input:     two packed decimal data strins with
  //            number of digits and number of fractional
  //            digits.  (data,ndig,nfrac; data2,ndig2,nfrac2)
  // output:    first packed decimal string has copy of other
  //            with correct truncation rules followed.
  void packedCpy(UCHAR* data,int ndig,int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrac2)
  {
    int i,ii,j;

    decdig rslt(data);
    cdecdig src(data2);

    // General outline:  See packedDoAdd for similar outline

    if((ndig>=nfrac) && (ndig2>=nfrac2)) {   // if well-defined
      // copy over sign bit
      j= src[ndig2];
      rslt[ndig]=j;

      // zero out any excess fractional digits from result to new result
      for (i=(ndig-1); (nfrac>nfrac2)&&(i>(ndig-(nfrac-nfrac2)-1)); --i)
             rslt[i]=0;  // zero out 'excess' digits

      // do actual movment on part both have digits for
      // account for fact src may have excess fractional digits over rslt
      // note carryover of i from previous loop
      for (ii=( (nfrac2<=nfrac) ?
               (ndig2-1):(ndig2-1-(nfrac2-nfrac)) ); // init src digit
               (i>=0)&&(ii>=0);     // escape when either is done
          --i,--ii )                // decrement both variables
      { j = src[ii];
        rslt[i]=j;
      };  // end for loop that does actual digit copy

      // zero out any excess digits from result
      for (  ; i>=0; --i) rslt[i]=0;
    };  // if well-defined

  };     // end packedCpy

  // function:  packedLsh
  // purpose:   Shifts a signed packed decimal number left by
  //            amount in second parameter.  If shift results in
  //            0, this is handled.  Shift is one decimal digit
  //            per number in second count.
  // input:     one packed decimal data strins with
  //            number of digits and number of fractional
  //            digits.  (data,ndig,nfrac)
  //            integer count of decimal digits to left shift.
  // output:    first packed decimal string has shifted number
  //            with zero handled correctly.
  void packedLsh(UCHAR* data,int ndig,int nfrac,const int shft)
  { int i,j,tmp; int allz=1;
      decdig rslt(data);

      // individual shifts, except last time.
      for(j=shft; j>1; --j) {
       for(i=0; i<(ndig-1); ++i)  { tmp=(rslt[i+1])(); rslt[i]=tmp;};
       rslt[ndig-1]=0;
      };    // end for j

      // very last shift include check for zeros to fix sign
      for( ; j>0; --j) {
       for(i=0; i<(ndig-1); ++i)
       { tmp=(rslt[i+1])(); rslt[i]=tmp;
         if (tmp!=0) allz=0;
       };    // end for i. . .
       rslt[ndig-1]=0;
      };      // end for j. . .
      if ((allz==1)&&(shft>0))
         rslt[ndig]= decnewsign[1]; // result zero, so set + sign
  };  // end packedLsh (Left Shift in place)

  // function:  packedRsh
  // purpose:   Shifts a signed packed decimal number right by
  //            amount in second parameter.  If shift results in
  //            0, this is handled.  Shift is one decimal digit
  //            per number in second count.
  // input:     one packed decimal data strins with
  //            number of digits and number of fractional
  //            digits.  (data,ndig,nfrac)
  //            integer count of decimal digits to left shift.
  // output:    first packed decimal string has shifted number
  //            with zero handled correctly.
  void packedRsh(UCHAR* data,int ndig,int nfrac,const int shft)
  { int i,j,tmp; int allz=1;
      decdig rslt(data);
      // all but last time
      for(j=shft; j>1; --j) {
       for(i=(ndig-1); i>0; --i) { tmp=(rslt[i-1])();rslt[i]=tmp; };
       rslt[0]=0;
      };    // end for j
      // last time, include check for all zeroes
      for( ; j>0; --j) {
       for(i=(ndig-1); i>0; --i) {
         tmp=(rslt[i-1])();rslt[i]=tmp;
         if (tmp!=0) allz=0;
       };    // end for i. . .
       rslt[0]=0;
      };      // end for j. . .
      if ((allz==1)&&(shft>0))
        rslt[ndig]= decnewsign[1]; // result zero, so set + sign
  };  // end packedRsh (Right Shift in place)

  // function:  packedfrInt
  // purpose:   Initializes a packed decimal number from an
  //            integer.
  // input:     one packed decimal data string with
  //            number of digits and number of fractional
  //            digits.  (data,ndig,nfrac)
  //            integer with value to be put in number.
  // output:    first packed decimal string has decimal number
  //            equal to passed integer.
  void packedfrInt(UCHAR* data,int ndig,int nfrac,const int a)
  {
   int i,j,val,valdig;
   decdig rslt(data);
   // cout << "Start conversion of " << a << '\n';
   val = a;
   if (val<0) {rslt[ndig]=decnewsign[0]; val= 0-val;}
   else rslt[ndig]=decnewsign[1];

   // all fractional digits are 0
   for(j=ndig-1;j>(ndig-nfrac-1);--j) rslt[j]=0;

   // set each digit until input integer exhausted or out of digits
   for(  ; (val>0)&&(j>=0) ; --j)  {
      valdig = val%10;
      rslt[j]= valdig;
      val=val-valdig;   // get rid of surplus to
      val=val/10;       // clear Borland's rounding error
   };

   for(  ; (j>=0) ; --j)           {
      rslt[j]=0;  // zero fill on left as needed
   };

   // cout << "End conversion of " << a << '\n';

  };  // end packedfrInt

  void packedfrFloat(UCHAR* data,int ndig,int nfrac,const float a) {
    float aa; aa = (a>0)? a : 0.0-a;
    int i=aa;  // convert float to truncated integer
    int j;

    packedfrInt(data,ndig,nfrac,i);  // fill out integer part
    float b=(aa-i);
    decdig rslt(data);

    for (j=(ndig-nfrac); j<ndig;++j) {
      b=b*10; i=b;   // extract next decimal digit
      rslt[j]= i;    // assign it
      b=b-i;
    };

    if (a<0) rslt[ndig]= decnewsign[0];
    else rslt[ndig]=decnewsign[1];

 };  // end packedfrFloat
  void packedfrChar(UCHAR* data,int ndig,int nfrac,const UCHAR *a);

  // function:  packedClean
  // purpose:   Cleans (sets to valid digits) a packed decimal
  //            number.  Performs "cleanse" function.
  // input:     one packed decimal data string with
  //            number of digits and number of fractional
  //            digits.  (data,ndig,nfrac)
  // output:    same packed decimal string either unchanged or
  //            with invalid digits set to 0, sign changed
  //            to preferred sign.
  void packedClean(UCHAR* data, int ndig, int nfrac) {
    decdig rslt(data);
    int i;
    // cleanse digits
    for (i=0; i<(ndig-1); ++i) rslt[i]= decclean[rslt[i]()];
    // cleanse sign
    rslt[ndig]= decnewsign[decsign[rslt[ndig]()]];
  };
