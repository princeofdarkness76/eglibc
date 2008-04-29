/* Number of digits functions, optimized for Power6.

   Copyright (C) 2006, 2007, 2008 IBM Corporation.
   Author(s): Pete Eberlein <eberlein@us.ibm.com>

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


#ifndef _NUMDIGITS_H
#define _NUMDIGITS_H 1


#define NUMDIGITS_SUPPORT 1

#include "dpd-private.h"

#if _DECIMAL_SIZE == 32
// DECIMAL32 gets widened to DECIMAL64, so it ought to use DECIMAL64 bias
#  define DECIMAL_BIAS (101+297)
#elif _DECIMAL_SIZE == 64
#  define DECIMAL_BIAS 398
#elif _DECIMAL_SIZE == 128
#  define DECIMAL_BIAS 6176
#endif


static inline int
getexp (DEC_TYPE x)
{
  union {
    double f;
    int i[2];
  } result;

#if _DECIMAL_SIZE == 32
  asm ("dctdp %0,%1\n\t"
       "dxex %0,%0\n\t" : "=f"(result.f) : "f"(x));
#elif _DECIMAL_SIZE == 64
  asm ("dxex %0,%1" : "=f"(result.f) : "f"(x));
#elif _DECIMAL_SIZE == 128
  /* dec quad needs to be in an even-odd fr pair */
  register DEC_TYPE tmp asm ("fr0") = x;
  asm ("dxexq %0,%1" : "=f"(result.f) : "f"(tmp));
#endif

  return result.i[1] - DECIMAL_BIAS;
}


static inline DEC_TYPE
setexp (DEC_TYPE x, int exp)
{
#if _DECIMAL_SIZE == 128
  register DEC_TYPE tmp asm ("fr0") = x;
#else
  register DEC_TYPE tmp = x;
#endif
  union {
    int i[2];
    double f;
  } e;

  e.i[0] = 0;
  e.i[1] = exp + DECIMAL_BIAS;

  asm (
#if _DECIMAL_SIZE == 32
   "dctdp %0,%0\n\t"
   "diex %0,%1,%0\n\t"
   "drsp %0,%0\n\t" 
#elif _DECIMAL_SIZE == 64
   "diex %0,%1,%0\n\t"
#elif _DECIMAL_SIZE == 128
    "diexq %0,%1,%0\n\t"
#endif
    : "=f"(tmp) : "f"(e.f), "0"(tmp));
  return tmp;
}



static inline int
numdigits (DEC_TYPE x)
{
#if _DECIMAL_SIZE == 128
  register DEC_TYPE tmp asm ("fr0") = x;
#else
  register DEC_TYPE tmp = x;
#endif
  union {
    int i[2];
    double f;
  } a, b, one;
  
  one.i[0] = 0;
  one.i[1] = 1;
  
  asm (
#if _DECIMAL_SIZE == 32
    "dctdp %0,%0\n\t"
#endif
#if _DECIMAL_SIZE != 128
    "dxex %1,%0\n\t"
    "drrnd %0,%4,%0,1\n\t"
    "dxex %2,%0\n\t"
#else /* _DECIMAL_SIZE == 128 */
    "dxexq %1,%0\n\t"
    "drrndq %0,%4,%0,1\n\t"
    "dxexq %2,%0\n\t"
#endif
   : "=f"(tmp), "=f"(a.f), "=f"(b.f) : "0"(tmp), "f"(one.f));
//  printf("a: %lld  b: %lld\n", a.i, b.i);
  return b.i[1] - a.i[1] + 1;
}


static DEC_TYPE
left_justify (DEC_TYPE x)
{
#if _DECIMAL_SIZE == 128
  register DEC_TYPE tmp asm ("fr0") = x, rnd asm ("fr2");
#else
  register DEC_TYPE tmp = x, rnd;
#endif
  union {
    int i[2];
    double f;
  } d;

  d.i[0] = 0;
  d.i[1] = 1;

#if _DECIMAL_SIZE == 32
  asm ("dctdp %0,%0\n\t" : "=f"(tmp) : "0"(tmp));
#endif  

#if _DECIMAL_SIZE != 128
  asm ("drrnd %0,%1,%2,1" : "=f"(rnd) : "f"(d.f), "f"(tmp));
  asm ("dxex %0,%1\n\t" : "=f"(d.f) : "f"(rnd));
  d.i[1] -= (_DECIMAL_SIZE==32) ? 6 : 15;
  asm ("diex %0,%1,%0\n\t" : "=f"(rnd) : "f"(d.f), "0"(rnd));
  asm ("dqua %0,%0,%2,1\n\t" : "=f"(rnd) : "0"(rnd), "f"(tmp));

#else /* _DECIMAL_SIZE == 128 */
  asm ("drrndq %0,%1,%2,1" : "=f"(rnd) : "f"(d.f), "f"(tmp));
  asm ("dxexq %0,%1\n\t" : "=f"(d.f) : "f"(rnd));
  d.i[1] -= 33;
  asm ("diexq %0,%1,%0\n\t" : "=f"(rnd) : "f"(d.f), "0"(rnd));
  asm ("dquaq %0,%0,%2,1\n\t" : "=f"(rnd) : "0"(rnd), "f"(tmp));
#endif

#if _DECIMAL_SIZE == 32
  asm ("drsp %0,%0\n\t" : "=f"(rnd) : "0"(rnd));
#endif  
  return rnd;
}


#endif /* _NUMDIGITS_H */
