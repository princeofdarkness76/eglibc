/* ------------------------------------------------------------------ */
/* Decimal 128-bit format module                                      */
/* ------------------------------------------------------------------ */
/* Copyright (C) 2006,2007 IBM Corporation.
   Contributed by IBM Corporation.

   The Decimal Floating Point C Library is free software; you can
   redistribute it and/or modify it under the terms of the GNU Lesser
   General Public License version 2.1.

   The Decimal Floating Point C Library is distributed in the hope that
   it will be useful, but WITHOUT ANY WARRANTY; without even the implied
   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
   the GNU Lesser General Public License version 2.1 for more details.

   You should have received a copy of the GNU Lesser General Public
   License version 2.1 along with the Decimal Floating Point C Library;
   if not, write to the Free Software Foundation, Inc., 59 Temple Place,
   Suite 330, Boston, MA 02111-1307 USA.

   Please see dfp/COPYING.txt for more information.  */

/* ------------------------------------------------------------------ */
/* This module comprises the routines for decimal128 format numbers.  */
/* Conversions are supplied to and from decNumber and String.         */
/*                                                                    */
/* No arithmetic routines are included; decNumber provides these.     */
/*                                                                    */
/* Error handling is the same as decNumber (qv.).                     */
/* ------------------------------------------------------------------ */
#include <string.h>           // [for memset/memcpy]
#include <stdio.h>            // [for printf]

#define  DECNUMDIGITS 34      // make decNumbers with space for 34
#include "decNumber.h"        // base number library
#include "decNumberLocal.h"   // decNumber local types, etc.
#include "decimal128.h"       // our primary include

/* Utility routines and tables [in decimal64.c] */
extern const uInt   COMBEXP[32], COMBMSD[32];
libc_hidden_proto (COMBEXP)
libc_hidden_proto (COMBMSD)

extern const uShort DPD2BIN[1024];
/*libc_hidden_proto (DPD2BIN) */
extern const uShort __BIND2DPD[1000];      // [not used]
/*libc_hidden_proto (__BIND2DPD) */
extern const uByte  BIN2CHAR[4001];
/*libc_hidden_proto (BIN2CHAR) */

#if defined NOT_IN_libc /* Don't build this into libc  */
extern void ___decDigitsToDPD(const decNumber *, uInt *, Int);
#endif

extern void ___decDigitsFromDPD(decNumber *, const uInt *, Int);
libc_hidden_proto (___decDigitsFromDPD)

#if defined NOT_IN_libc /* We don't want this in libc. */
#if DECTRACE || DECCHECK
void ___decimal128Show(const decimal128 *);          // for debug
extern void ___decNumberShow(const decNumber *);     // ..
#endif /* DECTRACE || DECCHECK  */
#endif /* NOT_IN_libc  */

/* compile-time endian tester [assumes sizeof(int)>1] */
static  const  Int mfcone=1;                 // constant 1
/*libc_hidden_data_def (mfcone) */
static  const  Flag *mfctop=(Flag *)&mfcone; // -> top byte
/* libc_hidden_data_def (mfctop) */
#define LITEND mfctop[0]           // named flag; 1=little-endian

/* Useful macro */
// Clear a structure (e.g., a decNumber)
#define DEC_clear(d) memset(d, 0, sizeof(*d))

/* ------------------------------------------------------------------ */
/* ___decimal128FromNumber -- convert decNumber to decimal128          */
/*                                                                    */
/*   ds is the target decimal128                                      */
/*   dn is the source number (assumed valid)                          */
/*   set is the context, used only for reporting errors               */
/*                                                                    */
/* The set argument is used only for status reporting and for the     */
/* rounding mode (used if the coefficient is more than DECIMAL128_Pmax*/
/* digits or an overflow is detected).  If the exponent is out of the */
/* valid range then Overflow or Underflow will be raised.             */
/* After Underflow a subnormal result is possible.                    */
/*                                                                    */
/* DEC_Clamped is set if the number has to be 'folded down' to fit,   */
/* by reducing its exponent and multiplying the coefficient by a      */
/* power of ten, or if the exponent on a zero had to be clamped.      */
/* ------------------------------------------------------------------ */
#if defined NOT_IN_libc /* Don't build this into libc  */
decimal128 * ___decimal128FromNumber(decimal128 *d128, const decNumber *dn,
                                    decContext *set) {
  uInt status=0;                   // status accumulator
  Int ae;                          // adjusted exponent
  decNumber  dw;                   // work
  decContext dc;                   // ..
  uInt *pu;                        // ..
  uInt comb, exp;                  // ..
  uInt targar[4]={0,0,0,0};        // target 128-bit
  #define targhi targar[3]         // name the word with the sign
  #define targmh targar[2]         // name the words
  #define targml targar[1]         // ..
  #define targlo targar[0]         // ..

  // If the number has too many digits, or the exponent could be
  // out of range then reduce the number under the appropriate
  // constraints.  This could push the number to Infinity or zero,
  // so this check and rounding must be done before generating the
  // decimal128]
  ae=dn->exponent+dn->digits-1;              // [0 if special]
  if (dn->digits>DECIMAL128_Pmax             // too many digits
   || ae>DECIMAL128_Emax                     // likely overflow
   || ae<DECIMAL128_Emin) {                  // likely underflow
    ___decContextDefault(&dc, DEC_INIT_DECIMAL128); // [no traps]
    dc.round=set->round;                     // use supplied rounding
    ___decNumberPlus(&dw, dn, &dc);           // (round and check)
    // [this changes -0 to 0, so enforce the sign...]
    dw.bits|=dn->bits&DECNEG;
    status=dc.status;                        // save status
    dn=&dw;                                  // use the work number
    } // maybe out of range

  if (dn->bits&DECSPECIAL) {                      // a special value
    if (dn->bits&DECINF) targhi=DECIMAL_Inf<<24;
     else {                                       // sNaN or qNaN
      if ((*dn->lsu!=0 || dn->digits>1)           // non-zero coefficient
       && (dn->digits<DECIMAL128_Pmax)) {         // coefficient fits
        ___decDigitsToDPD(dn, targar, 0);
        }
      if (dn->bits&DECNAN) targhi|=DECIMAL_NaN<<24;
       else targhi|=DECIMAL_sNaN<<24;
      } // a NaN
    } // special

   else { // is finite
    if (___decNumberIsZero(dn)) {             // is a zero
      // set and clamp exponent
      if (dn->exponent<-DECIMAL128_Bias) {
        exp=0;                               // low clamp
        status|=DEC_Clamped;
        }
       else {
        exp=dn->exponent+DECIMAL128_Bias;    // bias exponent
        if (exp>DECIMAL128_Ehigh) {          // top clamp
          exp=DECIMAL128_Ehigh;
          status|=DEC_Clamped;
          }
        }
      comb=(exp>>9) & 0x18;             // msd=0, exp top 2 bits ..
      }
     else {                             // non-zero finite number
      uInt msd;                         // work
      Int pad=0;                        // coefficient pad digits

      // the dn is known to fit, but it may need to be padded
      exp=(uInt)(dn->exponent+DECIMAL128_Bias);    // bias exponent
      if (exp>DECIMAL128_Ehigh) {                  // fold-down case
        pad=exp-DECIMAL128_Ehigh;
        exp=DECIMAL128_Ehigh;                      // [to maximum]
        status|=DEC_Clamped;
        }

      // [fastpath for common case is not a win, here]
      ___decDigitsToDPD(dn, targar, pad);
      // save and clear the top digit
      msd=targhi>>14;
      targhi&=0x00003fff;

      // create the combination field
      if (msd>=8) comb=0x18 | ((exp>>11) & 0x06) | (msd & 0x01);
             else comb=((exp>>9) & 0x18) | msd;
      }
    targhi|=comb<<26;              // add combination field ..
    targhi|=(exp&0xfff)<<14;       // .. and exponent continuation
    } // finite

  if (dn->bits&DECNEG) targhi|=0x80000000; // add sign bit

  // now write to storage; this may be endian, or not
  #if DECENDIAN
  // DECENDIAN -- direct store, in the right order
  pu=(uInt *)d128->bytes;          // overlay
  if (LITEND) {
    pu[0]=targlo;                  // directly store the low int
    pu[1]=targml;                  // then the mid-low
    pu[2]=targmh;                  // then the mid-high
    pu[3]=targhi;                  // then the high int
    }
   else {
    pu[0]=targhi;                  // directly store the high int
    pu[1]=targmh;                  // then the mid-high
    pu[2]=targml;                  // then the mid-low
    pu[3]=targlo;                  // then the low int
    }
  #else
  // not DECENDIAN -- use network byte order
  if (LITEND) {                    // little-endian needs reversal
    uByte *pb;                     // work
    Int off;                       // ..
    for (pb=&d128->bytes[15]; pb>=d128->bytes; pb--) {
      off=3-((pb-d128->bytes)>>2); // 0, then 1, 2, 3
      *pb=(uByte)(targar[off]&0xff);
      targar[off]>>=8;
      } // i
    }
   else { // big-endian; it's the right way round already
    pu=(uInt *)d128->bytes;        // overlay
    pu[0]=targhi;                  // directly store the high int
    pu[1]=targmh;                  // then the mid-high
    pu[2]=targml;                  // then the mid-low
    pu[3]=targlo;                  // then the low int
    }
  #endif

  if (status!=0) ___decContextSetStatus(set, status); // pass on status
  // ___decimal128Show(d128);
  return d128;
} /* ___decimal128FromNumber  */
#endif /* NOT_IN_libc  */

/* ------------------------------------------------------------------ */
/* ___decimal128ToNumber -- convert decimal128 to decNumber            */
/*   d128 is the source decimal128                                    */
/*   dn is the target number, with appropriate space                  */
/* No error is possible.                                              */
/* ------------------------------------------------------------------ */
decNumber * ___decimal128ToNumber(const decimal128 *d128, decNumber *dn) {
  uInt msd;                        // coefficient MSD
  uInt exp;                        // exponent top two bits
  uInt comb;                       // combination field
  uInt *pu;                        // work
  Int  need;                       // ..
  uInt sourar[4];                  // source 128-bit
  #define sourhi sourar[3]         // name the word with the sign
  #define sourmh sourar[2]         // and the mid-high word
  #define sourml sourar[1]         // and the mod-low word
  #define sourlo sourar[0]         // and the lowest word

  // load source from storage; this may be endian, or not
  #if DECENDIAN
  // DECENDIAN -- direct load, in the right order
  pu=(uInt *)d128->bytes;          // overlay
  if (LITEND) {
    sourlo=pu[0];                  // directly load the low int
    sourml=pu[1];                  // then the mid-low
    sourmh=pu[2];                  // then the mid-high
    sourhi=pu[3];                  // then the high int
    }
   else {
    sourhi=pu[0];                  // directly load the high int
    sourmh=pu[1];                  // then the mid-high
    sourml=pu[2];                  // then the mid-low
    sourlo=pu[3];                  // then the low int
    }
  #else
  // not DECENDIAN -- use network byte order
  if (LITEND) {                    // little-endian needs reversal
    const uByte *pb;               // work
    Int off;                       // ..
    for (pb=d128->bytes; pb<=&d128->bytes[15]; pb++) {
      off=3-((pb-d128->bytes)>>2); // 3, then 2, 1, 0
      sourar[off]<<=8;
      sourar[off]|=*pb;
      } // i
    }
   else { // big-endian; it's the right way round already
    pu=(uInt *)d128->bytes;        // overlay
    sourhi=pu[0];                  // directly load the high int
    sourmh=pu[1];                  // then the mid-high
    sourml=pu[2];                  // then the mid-low
    sourlo=pu[3];                  // then the low int
    }
  #endif

  comb=(sourhi>>26)&0x1f;          // combination field

  ___decNumberZero(dn);             // clean number
  if (sourhi&0x80000000) dn->bits=DECNEG; // set sign if negative

  msd=COMBMSD[comb];               // decode the combination field
  exp=COMBEXP[comb];               // ..

  if (exp==3) {                    // is a special
    if (msd==0) {
      dn->bits|=DECINF;
      return dn;                   // no coefficient needed
      }
    else if (sourhi&0x02000000) dn->bits|=DECSNAN;
    else dn->bits|=DECNAN;
    msd=0;                         // no top digit
    }
   else {                          // is a finite number
    dn->exponent=(exp<<12)+((sourhi>>14)&0xfff)-DECIMAL128_Bias; // unbiased
    }

  // get the coefficient
  sourhi&=0x00003fff;              // clean coefficient continuation
  if (msd) {                       // non-zero msd
    sourhi|=msd<<14;               // prefix to coefficient
    need=12;                       // process 12 declets
    }
   else { // msd=0
    if (sourhi) need=11;           // declets to process
     else if (sourmh) need=10;
     else if (sourml) need=7;
     else if (sourlo) need=4;
     else return dn;               // easy: coefficient is 0
    } //msd=0

  ___decDigitsFromDPD(dn, sourar, need);   // process declets
  // ___decNumberShow(dn);
  return dn;
} /* ___decimal128ToNumber  */
libc_hidden_def (___decimal128ToNumber)

/* ------------------------------------------------------------------ */
/* to-scientific-string -- conversion to numeric string               */
/* to-engineering-string -- conversion to numeric string              */
/*                                                                    */
/*   ___decimal128ToString(d128, string);                              */
/*   ___decimal128ToEngString(d128, string);                           */
/*                                                                    */
/*  d128 is the decimal128 format number to convert                   */
/*  string is the string where the result will be laid out            */
/*                                                                    */
/*  string must be at least 24 characters                             */
/*                                                                    */
/*  No error is possible, and no status can be set.                   */
/* ------------------------------------------------------------------ */
char * ___decimal128ToEngString(const decimal128 *d128, char *string){
  decNumber dn;                         // work
  ___decimal128ToNumber(d128, &dn);
  ___decNumberToEngString(&dn, string);
  return string;
} /* ___decimal128ToEngString  */
libc_hidden_def (___decimal128ToEngString) /* Used internally in libc for printf.  */

char * ___decimal128ToString(const decimal128 *d128, char *string){
  uInt msd;                        // coefficient MSD
  Int  exp;                        // exponent top two bits or full
  uInt comb;                       // combination field
  char *cstart;                    // coefficient start
  char *c;                         // output pointer in string
  uInt *pu;                        // work
  char *s, *t;                     // .. (source, target)
  Int  dpd;                        // ..
  Int  pre, e;                     // ..
  const uByte *u;                  // ..

  uInt sourar[4];                  // source 128-bit
  #define sourhi sourar[3]         // name the word with the sign
  #define sourmh sourar[2]         // and the mid-high word
  #define sourml sourar[1]         // and the mod-low word
  #define sourlo sourar[0]         // and the lowest word

  // load source from storage; this may be endian, or not
  #if DECENDIAN
  // DECENDIAN -- direct load, in the right order
  pu=(uInt *)d128->bytes;          // overlay
  if (LITEND) {
    sourlo=pu[0];                  // directly load the low int
    sourml=pu[1];                  // then the mid-low
    sourmh=pu[2];                  // then the mid-high
    sourhi=pu[3];                  // then the high int
    }
   else {
    sourhi=pu[0];                  // directly load the high int
    sourmh=pu[1];                  // then the mid-high
    sourml=pu[2];                  // then the mid-low
    sourlo=pu[3];                  // then the low int
    }
  #else
  // not DECENDIAN -- use network byte order
  if (LITEND) {                    // little-endian needs reversal
    const uByte *pb;               // work
    Int off;                       // ..
    for (pb=d128->bytes; pb<=&d128->bytes[15]; pb++) {
      off=3-((pb-d128->bytes)>>2); // 3, then 2, 1, 0
      sourar[off]<<=8;
      sourar[off]|=*pb;
      } // i
    }
   else { // big-endian; it's the right way round already
    pu=(uInt *)d128->bytes;        // overlay
    sourhi=pu[0];                  // directly load the high int
    sourmh=pu[1];                  // then the mid-high
    sourml=pu[2];                  // then the mid-low
    sourlo=pu[3];                  // then the low int
    }
  #endif

  c=string;                        // where result will go
  if (((Int)sourhi)<0) *c++='-';   // handle sign

  comb=(sourhi>>26)&0x1f;          // combination field
  msd=COMBMSD[comb];               // decode the combination field
  exp=COMBEXP[comb];               // ..

  if (exp==3) {
    if (msd==0) {                  // infinity
      strcpy(c, "Infinity");
      return string;               // easy
      }
    if (sourhi&0x02000000) *c++='s'; // sNaN
    strcpy(c, "NaN");              // complete word
    c+=3;                          // step past
    if (sourlo==0 && sourml==0 && sourmh==0
     && (sourhi&0x0003ffff)==0) return string; // zero payload
    // otherwise drop through to add integer; set correct exp
    exp=0; msd=0;                  // setup for following code
    }
   else exp=(exp<<12)+((sourhi>>14)&0xfff)-DECIMAL128_Bias; // unbiased

  // convert 34 digits of significand to characters
  cstart=c;                        // save start of coefficient
  if (msd) *c++='0'+(char)msd;     // non-zero most significant digit

  // Now decode the declets.  After extracting each one, it is
  // decoded to binary and then to a 4-char sequence by table lookup;
  // the 4-chars are a 1-char length (significant digits, except 000
  // has length 0).  This allows us to left-align the first declet
  // with non-zero content, then remaining ones are full 3-char
  // length.  We use fixed-length memcpys because variable-length
  // causes a subroutine call in GCC.  (These are length 4 for speed
  // and are safe because the array has an extra terminator byte.)
  #define dpd2char u=&BIN2CHAR[DPD2BIN[dpd]*4];                   \
                   if (c!=cstart) {memcpy(c, u+1, 4); c+=3;}      \
                    else if (*u)  {memcpy(c, u+4-*u, 4); c+=*u;}
  dpd=(sourhi>>4)&0x3ff;                     // declet 1
  dpd2char;
  dpd=((sourhi&0xf)<<6) | (sourmh>>26);      // declet 2
  dpd2char;
  dpd=(sourmh>>16)&0x3ff;                    // declet 3
  dpd2char;
  dpd=(sourmh>>6)&0x3ff;                     // declet 4
  dpd2char;
  dpd=((sourmh&0x3f)<<4) | (sourml>>28);     // declet 5
  dpd2char;
  dpd=(sourml>>18)&0x3ff;                    // declet 6
  dpd2char;
  dpd=(sourml>>8)&0x3ff;                     // declet 7
  dpd2char;
  dpd=((sourml&0xff)<<2) | (sourlo>>30);     // declet 8
  dpd2char;
  dpd=(sourlo>>20)&0x3ff;                    // declet 9
  dpd2char;
  dpd=(sourlo>>10)&0x3ff;                    // declet 10
  dpd2char;
  dpd=(sourlo)&0x3ff;                        // declet 11
  dpd2char;

  if (c==cstart) *c++='0';         // all zeros -- make 0

  if (exp==0) {                    // integer or NaN case -- easy
    *c='\0';                       // terminate
    return string;
    }

  /* non-0 exponent */
  e=0;                             // assume no E
  pre=c-cstart+exp;
  // [here, pre-exp is the digits count (==1 for zero)]
  if (exp>0 || pre<-5) {           // need exponential form
    e=pre-1;                       // calculate E value
    pre=1;                         // assume one digit before '.'
    } // exponential form

  /* modify the coefficient, adding 0s, '.', and E+nn as needed */
  s=c-1;                           // source (LSD)
  if (pre>0) {                     // ddd.ddd (plain), perhaps with E
    char *dotat=cstart+pre;
    if (dotat<c) {                 // if embedded dot needed...
      t=c;                              // target
      for (; s>=dotat; s--, t--) *t=*s; // open the gap; leave t at gap
      *t='.';                           // insert the dot
      c++;                              // length increased by one
      }

    // finally add the E-part, if needed; it will never be 0, and has
    // a maximum length of 4 digits
    if (e!=0) {
      *c++='E';                    // starts with E
      *c++='+';                    // assume positive
      if (e<0) {
        *(c-1)='-';                // oops, need '-'
        e=-e;                      // uInt, please
        }
      if (e<1000) {                // 3 (or fewer) digits case
        u=&BIN2CHAR[e*4];          // -> length byte
        memcpy(c, u+4-*u, 4);      // copy fixed 4 characters [is safe]
        c+=*u;                     // bump pointer appropriately
        }
       else {                      // 4-digits
        Int thou=((e>>3)*1049)>>17; // e/1000
        Int rem=e-(1000*thou);      // e%1000
        *c++='0'+(char)thou;
        u=&BIN2CHAR[rem*4];        // -> length byte
        memcpy(c, u+1, 4);         // copy fixed 3+1 characters [is safe]
        c+=3;                      // bump pointer, always 3 digits
        }
      }
    *c='\0';                       // add terminator
    //printf("res %s\n", string);
    return string;
    } // pre>0

  /* -5<=pre<=0: here for plain 0.ddd or 0.000ddd forms (can never have E) */
  t=c+1-pre;
  *(t+1)='\0';                          // can add terminator now
  for (; s>=cstart; s--, t--) *t=*s;    // shift whole coefficient right
  c=cstart;
  *c++='0';                             // always starts with 0.
  *c++='.';
  for (; pre<0; pre++) *c++='0';        // add any 0's after '.'
  //printf("res %s\n", string);
  return string;
} /* ___decimal128ToString  */
libc_hidden_def (___decimal128ToString) /* Used internally in libc for printf.  */

/* ------------------------------------------------------------------ */
/* to-number -- conversion from numeric string                        */
/*                                                                    */
/*   ___decimal128FromString(result, string, set);                     */
/*                                                                    */
/*  result  is the decimal128 format number which gets the result of  */
/*          the conversion                                            */
/*  *string is the character string which should contain a valid      */
/*          number (which may be a special value)                     */
/*  set     is the context                                            */
/*                                                                    */
/* The context is supplied to this routine is used for error handling */
/* (setting of status and traps) and for the rounding mode, only.     */
/* If an error occurs, the result will be a valid decimal128 NaN.     */
/* ------------------------------------------------------------------ */
#if defined NOT_IN_libc /* Don't build this into libc  */
decimal128 * ___decimal128FromString(decimal128 *result, const char *string,
                                    decContext *set) {
  decContext dc;                             // work
  decNumber dn;                              // ..

  ___decContextDefault(&dc, DEC_INIT_DECIMAL128); // no traps, please
  dc.round=set->round;                       // use supplied rounding

  ___decNumberFromString(&dn, string, &dc);   // will round if needed
  ___decimal128FromNumber(result, &dn, &dc);
  if (dc.status!=0) {                        // something happened
    ___decContextSetStatus(set, dc.status);   // .. pass it on
    }
  return result;
} /* ___decimal128FromString  */
#endif

//#if DECTRACE || DECCHECK
/* ------------------------------------------------------------------ */
/* ___decimal128Show -- display a decimal128 in hexadecimal [debug aid]*/
/*   d128 -- the number to show                                       */
/* ------------------------------------------------------------------ */
// Also shows sign/cob/expconfields extracted
#if defined NOT_IN_libc /* We don't want this in libc.  */
void ___decimal128Show(const decimal128 *d128) {
  char buf[DECIMAL128_Bytes*2+1];
  Int i, j=0;

  #if DECENDIAN
  if (LITEND) {
    for (i=0; i<DECIMAL128_Bytes; i++, j+=2) {
      sprintf(&buf[j], "%02x", d128->bytes[15-i]);
      }
    printf(" D128> %s [S:%d Cb:%02x Ec:%02x] LittleEndian\n", buf,
           d128->bytes[15]>>7, (d128->bytes[15]>>2)&0x1f,
           ((d128->bytes[15]&0x3)<<10)|(d128->bytes[14]<<2)|
           (d128->bytes[13]>>6));
    }
   else {
  #endif
    for (i=0; i<DECIMAL128_Bytes; i++, j+=2) {
      sprintf(&buf[j], "%02x", d128->bytes[i]);
      }
    printf(" D128> %s [S:%d Cb:%02x Ec:%02x] BigEndian\n", buf,
           ___decimal128Sign(d128), ___decimal128Comb(d128),
           ___decimal128ExpCon(d128));
  #if DECENDIAN
    }
  #endif
} /* ___decimal128Show */
#endif /* NOT_IN_libc  */
