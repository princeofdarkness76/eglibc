/* Break floating point number into normalized fraction and integral power of two
   Copyright (C) 2006 IBM Corporation.

   Author(s): Joseph Kerian <jkerian@us.ibm.com>

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

#ifndef _DECIMAL_SIZE
#  define _DECIMAL_SIZE 32
#  include <decimal32.h>
#endif

#include <decContext.h>
#include <decNumber.h>
#include <math.h>

#include <decNumberMath.h>

#define FUNCTION_NAME frexp

#include <dfpmacro.h>
#include <numdigits.h>

DEC_TYPE
INTERNAL_FUNCTION_NAME (DEC_TYPE x, int *y)
{
  DEC_TYPE result;
  int digits, exponent;
 
  if (isinf(x) || isnan(x))
    return x+x;
  
  digits = numdigits(x);
  exponent = getexp(x);
  *y = digits + exponent;

  result = x;
  setexp(&result, -digits);

  return result;
  
/* old decnumber implementation */  
/*  decNumber dn_x;
  decContext context;

  *y = 0;
  FUNC_CONVERT_TO_DN(&x, &dn_x);
  if (___decNumberIsNaN (&dn_x) || ___decNumberIsInfinite (&dn_x)
	|| ___decNumberIsZero (&dn_x))
    return x+x;

  *y = dn_x.digits + dn_x.exponent;
  dn_x.exponent = -dn_x.digits;

  ___decContextDefault(&context, DEFAULT_CONTEXT);
  FUNC_CONVERT_FROM_DN (&dn_x, &result, &context);
  
  return result;*/
}

weak_alias (INTERNAL_FUNCTION_NAME, EXTERNAL_FUNCTION_NAME)
