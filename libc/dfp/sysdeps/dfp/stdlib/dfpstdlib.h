/* Internal function prototypes for string-to-decimal conversions
   Copyright (C) 2006 IBM Corporation.

   Author(s): Ryan S. Arnold <rsa@us.ibm.com>

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

#ifndef	_DFPSTDLIB_H
#define	_DFPSTDLIB_H	1

#include <features.h>

extern _Decimal32 __strtod32_internal (const char * __restrict nptr, char ** __restrict endptr, int group);
extern _Decimal64 __strtod64_internal (const char * __restrict nptr, char ** __restrict endptr, int group);
extern _Decimal128 __strtod128_internal (const char * __restrict nptr, char ** __restrict endptr, int group);

libc_hidden_proto (__strtod32_internal)
libc_hidden_proto (__strtod64_internal)
libc_hidden_proto (__strtod128_internal)

#endif /* _DFPSTDLIB_H  */
