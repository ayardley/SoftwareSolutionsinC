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

// Packed Decimal class by Larry W. Loen

// This declares template class packed
// The goal is to allocate and handle character arrays
//   of N bytes which are interpreted as IBM-style
//   packed decimal numbers.
// A 'helper' class called vpacked is defined to
//   handle variable length numbers
// Some functions common to packed<ndig,nfrac> and vpacked are
//   also declared
// A final 'helper' class called outdecbytes is defined
//   for full interchange.  cout << var.COBOL_Write() will cause
//   the raw bites to go out as if done for a COBOL write.
//   The regular cout << var will produce the results a C++
//   programmer would expect.

#ifndef PACKED_DEF
#define PACKED_DEF "defined"
#define UCHAR unsigned char

#include <iostream.h>
#include <string.h>

class decdig;
class cdecdig; // forward declares from implementation

// function headers
  void outPacked(ostream &x,UCHAR* data,int ndig,int nfrac);
  void outPackedC(ostream &x,const UCHAR* data,int ndig,int nfrac);
  void inPacked(istream &x,UCHAR* data,int ndig,int nfrac);
  int packedComp(const UCHAR* data,const int ndig,const int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrack2);
  void packedAdd(UCHAR* data,int ndig,int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrack2);
  void packedSub(UCHAR* data,int ndig,int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrack2);
  void packedMul(UCHAR* data,int ndig,int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrack2);
  void packedDiv(UCHAR* data,int ndig,int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrack2);
  void packedCpy(UCHAR* data,int ndig,int nfrac,
       const UCHAR* data2,const int ndig2,const int nfrack2);
  void packedLsh(UCHAR* data,int ndig,int nfrac,const int shft);
  void packedRsh(UCHAR* data,int ndig,int nfrac,const int shft);
  void packedfrInt(UCHAR* data,int ndig,int nfrac,const int a);
  void packedfrFloat(UCHAR* data,int ndig,int nfrac,const float a);
  void packedfrChar(UCHAR* data,int ndig,int nfrac,const UCHAR *a);
  void packedClean(UCHAR * data,int ndig,int nfrac);

// Class:  outdecbytes
// Parent Class:  none
// Type:   concrete
// Purpose:
//     Support x.COBOL_Write() functions such that (for zoned or
//     packed) the raw bytes are output as a data interchange application
//     would expect.
// Behavior:
//     zoned<5,2> x;
//     packed<5,2>  w;          // no initialization in this case
//     w = -35.4;               // w is assigned via conversion
//     x = w;                   // x assigned by conversion
//     cout << w << ' ' <<x;    //  "-035.40 -035.40" is output
//     cout << w.COBOL_Write(); //  0x03540D is output
//     cout << x.COBOL_Write(); //  0xF0F3F5F4F0 is output
// Virtual functions:  No (none allowed)
// Friends:  No.
// Delete/Copy Ctor/Assignment:  None.
// Printable:  Yes (actually, used to help print of other things)

class outdecbytes   {
  UCHAR *data;
  int size;
  friend ostream& operator<< (ostream &x,outdecbytes &y)
   { x.write(y.data,y.size); return x; };
 public:
  outdecbytes(UCHAR *d,int s) : data(d), size(s) {};
};

// Class:  vpacked
// Parent Class:  none
// Type:   concrete
// Purpose:
//     Support packed<ndig,nfrac> class by providing an interface
//     for the many cases where the decimal data items do not have
//     the same number of digits or number of digits after the decimal
//     point.
// Behavior:
//     packed<5,2>  w;          // no initialization in this case
//     packed<7,2>  x(int xx);
//     packed<7,2>  xx(float yy);
//     packed<7,3>  y(UCHAR *);  // xlate string of digits to dec.
//     packed<9,2>  z(vpacked); // helper class vpacked
//     w = x;                   // x xlate to vpacked & then assign
//     w = x + y;               // x and y both become vpacked
//
// Virtual functions:  No (none allowed)
// Friends:  Yes.
// Delete/Copy Ctor/Assignment:  All.
// Overloaded operators:  All arithmetic functions,
//     all compares (perform decimal math)
// Printable:  Yes.  Two forms  cout << x and cout << x.COBOL_Write().
//        The first is "int" style output, the second IBM-format style for
//        COBOL et. al. interchange.  See outdecbytes class for more.
// Traceable:  No.
// Static Data:  None.
// Notes:  Cannot have virtual functions as this must map over COBOL, PLI,
//     or RPG data structures.
//     This class should not be routinely instantiated.  It is intended
//     to be created by packed<n,m> below.
//     Requires vndig to be an odd number for correct operation.

class vpacked    {
 public:
    UCHAR *data;
    int vndig;
    int vnfrac;
 private:
    int alloc;  // did vpacked allocate its data or just ref elsewhere?
 // define vpacked so that it can be output in-line
 friend ostream& operator << (ostream &x,const vpacked &y)
         {   outPackedC(x,y.data,y.vndig,y.vnfrac);
             return x; } ;
 public:
 // constructor, reference form
    vpacked(UCHAR *d,const int nd,const int nf)
           : data(d),vndig(nd),vnfrac(nf) {alloc=0;};
 // constructor, allocate form (var al is just for signature purposes)
    vpacked(const UCHAR *d,const int nd,const int nf,const int al)
           : vndig(nd),vnfrac(nf)
      {
        data =
                new UCHAR[(nd+1)/2];
        packedCpy(data,nd,nf,d,nd,nf);
        alloc=1; };
 // constructor, copy
    vpacked(const vpacked &d) : vndig(d.vndig), vnfrac(d.vnfrac)
      { data= new UCHAR[(d.vndig+1)/2];
        packedCpy(data,d.vndig,d.vnfrac,d.data,d.vndig,d.vnfrac);
        alloc=1; };
 // destructor
    ~vpacked() { if (alloc==1) delete [] data; };

  // assignment
  vpacked& operator= (const vpacked &x) {
            packedCpy(data,vndig,vnfrac,x.data,x.vndig,x.vnfrac);
            return *this;
            };
  // arithmetic operators
  friend vpacked  operator+ (vpacked &w,const vpacked &x) {
           vpacked xx(w.data,w.vndig,w.vnfrac,1);
           packedAdd(xx.data,xx.vndig,xx.vnfrac,x.data,x.vndig,x.vnfrac);
           return xx;
           };
  friend vpacked  operator- (vpacked &w,const vpacked &x) {
           vpacked xx(w.data,w.vndig,w.vnfrac,1);
           packedSub(xx.data,xx.vndig,xx.vnfrac,x.data,x.vndig,x.vnfrac);
           return xx;
           };
  friend vpacked  operator* (vpacked &w, const vpacked &x) {
           vpacked xx(w.data,w.vndig,w.vnfrac,1);
           packedMul(xx.data,xx.vndig,xx.vnfrac,x.data,x.vndig,x.vnfrac);
           return xx;
           };
  friend vpacked  operator/ (vpacked &w,const vpacked &x) {
           vpacked xx(w.data,w.vndig,w.vnfrac,1);
           packedDiv(xx.data,xx.vndig,xx.vnfrac,x.data,x.vndig,x.vnfrac);
           return xx;
           };
  vpacked& operator+= (const vpacked &x) {
           packedAdd(data,vndig,vnfrac,x.data,x.vndig,x.vnfrac);
           return *this;
           };
  vpacked& operator-= (const vpacked &x) {
           packedSub(data,vndig,vnfrac,x.data,x.vndig,x.vnfrac);
           return *this;
           };
  vpacked& operator*= (const vpacked &x) {
           packedMul(data,vndig,vnfrac,x.data,x.vndig,x.vnfrac);
           return *this;
           };
  vpacked& operator/= (const vpacked &x) {
           packedDiv(data,vndig,vnfrac,x.data,x.vndig,x.vnfrac);
           return *this;
           };
  vpacked& operator<<= (const int shft) {
           packedLsh(data,vndig,vnfrac,shft);
           return *this;
           };
  vpacked& operator>>= (const int shft) {
           packedRsh(data,vndig,vnfrac,shft);
           return *this;
           };
  friend int operator== (const vpacked &w,const vpacked &x) {
             return packedComp(w.data,w.vndig,w.vnfrac,x.data,
                               x.vndig,x.vnfrac)==0; };
  friend int operator!= (const vpacked &w,const vpacked &x) {
             return packedComp(w.data,w.vndig,w.vnfrac,x.data,
                               x.vndig,x.vnfrac)!=0; };
  friend int operator<= (const vpacked &w,const vpacked &x) {
             return packedComp(w.data,w.vndig,w.vnfrac,x.data,
                               x.vndig,x.vnfrac)<=0; };
  friend int operator>= (const vpacked &w,const vpacked &x) {
             return packedComp(w.data,w.vndig,w.vnfrac,x.data,
                               x.vndig,x.vnfrac)>=0; };
  friend int operator>  (const vpacked &w,const vpacked &x) {
             return packedComp(w.data,w.vndig,w.vnfrac,x.data,
                               x.vndig,x.vnfrac)> 0; };
  friend int operator<  (const vpacked &w,const vpacked &x) {
             return packedComp(w.data,w.vndig,w.vnfrac,x.data,
                               x.vndig,x.vnfrac)< 0; };


  // functional operators
  vpacked& cleanse() { packedClean(data,vndig,vnfrac); return *this;};
  outdecbytes COBOL_Write() { return outdecbytes(data,(vndig+1)/2); };

};   // end vpacked

// Template:  unsigned int ndig,nfrac
//     -- number of total digits, fractional digits
// Class:  packed
// Parent Class:  none
// Type:   concrete
// Purpose:
//     Define a packed decimal data item
//     Define all required functions to deal with other packed
//     Define dealings with int, strings of coded characters, and floats.
// Behavior:
//     packed<5,2>  w;          // no initialization in this case
//     packed<7,2>  x(int xx);
//     packed<7,2>  xx(float yy);
//     packed<7,3>  y(UCHAR *);  // xlate string of digits to dec.
//     packed<9,2>  z(vpacked); // helper class vpacked
//     w = x;
//     w = x + 4;        // 4 xlate to packed
//     w += 4.2;         // 4.2 xlate to packed
//     y = xx*xx;        // note scaling is required
//     w = x+y;          // scaling and trunction, w/out diagnostic
//
// Virtual functions:  No (none allowed)
// Friends:  Yes.  (to define things like packed<n,m>+int)
// Delete/Copy Ctor/Assignment:  Assignment only (defaults fine for others).
// Overloaded operators:  All arithmetic functions, all compares.
// Printable:  Yes.  Two forms  cout << x and cout << x.COBOL_Write().
//        The first is "int" style output, the second IBM-format style for
//        COBOL et. al. interchange.  See outdecbytes class for more.
// Traceable:  No.
// Static Data:  None.
// Notes:  Cannot have virtual functions as this must map over COBOL, PLI,
//     or RPG data structures.
//     Assumes (without checking) that ndig is always an odd number
//     and that ndig and nfrac are positive (use unsigned int ndig,
//     unsigned int nfrac if your compiler accepts it).

template<int ndig,int nfrac>
class packed   {  // no virtual functions allowed so this can
                  // allocate exactly the same as COBOL expects
private:
 friend class decdig;
 friend class cdecdig;
 friend void packedDiv(UCHAR *d,int n1,int d1,
                      const UCHAR *d22,const int n2,const int d2);

 UCHAR data[(ndig+1/2)];   // allocate storage

 int Ndig() const  { return ndig; } ;    // needed by Turbo C++
 int Nfrac() const { return nfrac; } ;

 // define packed so that it can be output in-line.  Use ndig,nfrac
 //   instead of y.Ndig(),y.Nfrac() if your compiler can accept it.
 friend ostream& operator << (ostream &x,const packed<ndig,nfrac> &y)
         {   outPackedC(x,y.data,y.Ndig(),y.Nfrac());
             return x; } ;
public:
 // constructors
 packed()  {};            // no initial value constructor
 packed(const int a)  { packedfrInt(data,ndig,nfrac,a); };
 packed(const float a){ packedfrFloat(data,ndig,nfrac,a); };
 packed(const UCHAR *a){ packedfrChar(data,ndig,nfrac,a); };
 packed(const UCHAR *a, int x, int y){ packedCpy(data,ndig,nfrac,
                                        a,x,y); };

 // conversion operator
 operator vpacked() { return vpacked(data,ndig,nfrac); };

 // overloaded assignment
 packed<ndig,nfrac>& operator = (const vpacked &x) {
       packedCpy(data,ndig,nfrac,x.data,x.vndig,x.vnfrac);
       return *this;
 };

 // overloaded comparison operators
 int operator == (const packed<ndig,nfrac> &x) const {
       return packedComp(data,ndig,nfrac,x.data,ndig,nfrac)==0;
                 };
 int operator == (const vpacked& x) const {
       return packedComp(data,ndig,nfrac,x.data,x.vndig,x.vnfrac)==0;
                 };
 int operator != (const packed<ndig,nfrac> &x) const {
       return packedComp(data,ndig,nfrac,x.data,ndig,nfrac)!=0;
                 };
 int operator != (const vpacked& x) const {
       return packedComp(data,ndig,nfrac,x.data,x.vndig,x.vnfrac)!=0;
                 };
 int operator >= (const packed<ndig,nfrac> &x) const {
       return packedComp(data,ndig,nfrac,x.data,ndig,nfrac)>=0;
                 };
 int operator >= (const vpacked& x) const {
       return packedComp(data,ndig,nfrac,x.data,x.vndig,x.vnfrac)>=0;
                 };
 int operator <= (const packed<ndig,nfrac> &x) const {
       return packedComp(data,ndig,nfrac,x.data,ndig,nfrac)<=0;
                 };
 int operator <= (const vpacked& x) const {
       return packedComp(data,ndig,nfrac,x.data,x.vndig,x.vnfrac)<=0;
                 };
 int operator <  (const packed<ndig,nfrac> &x) const {
       return packedComp(data,ndig,nfrac,x.data,ndig,nfrac)< 0;
                 };
 int operator <  (const vpacked& x) const {
       return packedComp(data,ndig,nfrac,x.data,x.vndig,x.vnfrac)< 0;
                 };
 int operator >  (const packed<ndig,nfrac> &x) const {
       return packedComp(data,ndig,nfrac,x.data,ndig,nfrac)> 0;
                 };
 int operator >  (const vpacked& x) const {
       return packedComp(data,ndig,nfrac,x.data,x.vndig,x.vnfrac)> 0;
                 };
 // arithmetic operators
 vpacked  operator+ (const vpacked &x) {
       vpacked xx(data,ndig,nfrac,1);
       packedAdd(xx.data,xx.vndig,xx.vnfrac,x.data,x.vndig,x.vnfrac);
       return xx;
           };
 vpacked  operator- (const vpacked &x) {
       vpacked xx(data,ndig,nfrac,1);
       packedSub(xx.data,xx.vndig,xx.vnfrac,x.data,x.vndig,x.vnfrac);
       return xx;
           };
 vpacked  operator* (const vpacked &x) {
       vpacked xx(data,ndig,nfrac,1);
       packedMul(xx.data,xx.vndig,xx.vnfrac,x.data,x.vndig,x.vnfrac);
       return xx;
           };
 vpacked  operator/ (const vpacked &x) {
       vpacked xx(data,ndig,nfrac,1);
       packedDiv(xx.data,xx.vndig,xx.vnfrac,x.data,x.vndig,x.vnfrac);
       return xx;
           };
 packed<ndig,nfrac>& operator+= (const vpacked &x) {
       packedAdd(data,ndig,nfrac,x.data,x.vndig,x.vnfrac);
       return *this;
           };
 packed<ndig,nfrac>& operator-= (const vpacked &x) {
       packedSub(data,ndig,nfrac,x.data,x.vndig,x.vnfrac);
       return *this;
           };
 packed<ndig,nfrac>& operator*= (const vpacked &x) {
       packedMul(data,ndig,nfrac,x.data,x.vndig,x.vnfrac);
       return *this;
           };
 packed<ndig,nfrac>& operator/= (const vpacked &x) {
       packedDiv(data,ndig,nfrac,x.data,x.vndig,x.vnfrac);
       return *this;
           };
 packed<ndig,nfrac>  operator+ (const packed<ndig,nfrac> &x) {
       packed<ndig,nfrac> xx(*this);
       packedAdd(xx.data,ndig,nfrac,x.data,ndig,nfrac);
       return xx;
           };
 packed<ndig,nfrac>& operator<<= (const int shft) {
       packedLsh(data,ndig,nfrac,shft);
       return *this;
           };
 packed<ndig,nfrac>& operator>>= (const int shft) {
       packedRsh(data,ndig,nfrac,shft);
       return *this;
           };
 vpacked operator << (const int shft) {
       vpacked x(data,ndig,nfrac,0); // alloc vpacked
       packedLsh(x.data,ndig,nfrac,shft);
       return x;
           };
 vpacked operator >> (const int shft) {
       vpacked x(data,ndig,nfrac,0); // alloc vpacked
       packedRsh(x.data,ndig,nfrac,shft);
       return x;
           };
 packed<ndig,nfrac>  operator- (const packed<ndig,nfrac> &x) {
       packed<ndig,nfrac> xx(*this);
       packedSub(xx.data,ndig,nfrac,x.data,ndig,nfrac);
       return xx;
           };
 packed<ndig,nfrac>  operator* (const packed<ndig,nfrac> &x) {
       packed<ndig,nfrac> xx(*this);
       packedMul(xx.data,ndig,nfrac,x.data,ndig,nfrac);
       return xx;
           };
 packed<ndig,nfrac>  operator/ (const packed<ndig,nfrac> &x) {
       packed<ndig,nfrac> xx(*this);
       packedDiv(xx.data,ndig,nfrac,x.data,ndig,nfrac);
       return xx;
           };
 packed<ndig,nfrac>& operator+= (const packed<ndig,nfrac> &x) {
       packedAdd(data,ndig,nfrac,x.data,ndig,nfrac);
       return *this;
           };
 packed<ndig,nfrac>& operator-= (const packed<ndig,nfrac> &x) {
       packedSub(data,ndig,nfrac,x.data,ndig,nfrac);
       return *this;
           };
 packed<ndig,nfrac>& operator*= (const packed<ndig,nfrac> &x) {
       packedMul(data,ndig,nfrac,x.data,ndig,nfrac);
       return *this;
           };
 packed<ndig,nfrac>& operator/= (const packed<ndig,nfrac> &x) {
       packedDiv(data,ndig,nfrac,x.data,ndig,nfrac);
       return *this;
           };

 // functional operator
 packed<ndig,nfrac>& cleanse()
       { packedClean(data,ndig,nfrac); return *this;};
 outdecbytes COBOL_Write() { return outdecbytes(data,(ndig+1)/2); };

};        // end template class packed

// Optionally include implementation
#ifdef PACKED_IMP
#undef PACKED_IMP
#include "packed.cpp"
#endif
#endif
