/* Raise given exceptions.
   Copyright (C) 1997,1999,2000,2001,2002,2006 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Richard Sandiford <richard@codesourcery.com>

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <fenv.h>
#include <float.h>
#include <math.h>

int
feraiseexcept (int excepts)
{
  /* Raise exceptions represented by EXCEPTS.  But we must raise only one
     signal at a time.  It is important that if the overflow/underflow
     exception and the divide by zero exception are given at the same
     time, the overflow/underflow exception follows the divide by zero
     exception.

     The Coldfire FPU allows an exception to be raised by asserting
     the associated EXC bit and then executing an arbitrary arithmetic
     instruction.  fmove.l is classified as an arithmetic instruction
     and suffices for this purpose.

     We therefore raise an exception by setting both the EXC and AEXC
     bit associated with the exception (the latter being 6 bits to the
     left of the former) and then loading the longword at (%sp) into a
     data register.  */

  inline void
  raise_one_exception (int mask)
  {
    if (excepts & mask)
      {
	int fpsr;
	double unused;

	asm volatile ("fmove%.l %/fpsr,%0" : "=d" (fpsr));
	fpsr |= (mask << 6) | mask;
	asm volatile ("fmove%.l %0,%/fpsr" :: "d" (fpsr));
	asm volatile ("fmove%.l (%%sp),%0" : "=f" (unused));
      }
  }

  raise_one_exception (FE_INVALID);
  raise_one_exception (FE_DIVBYZERO);
  raise_one_exception (FE_OVERFLOW);
  raise_one_exception (FE_UNDERFLOW);
  raise_one_exception (FE_INEXACT);

  /* Success.  */
  return 0;
}
libm_hidden_def (feraiseexcept)