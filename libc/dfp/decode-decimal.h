/* Prototype for converting a _Decimal* number into a string

   Copyright (C) 2007 IBM Corporation.
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

#ifndef _DISP_DECIMAL_H
#define _DISP_DECIMAL_H 1

#include <features.h>

__BEGIN_DECLS

/* char * should ref 14 bytes, +0,000,000E+0\0  */
extern char * __decoded32 (_Decimal32, char*);
/* char * should ref 26 bytes, +0,000,000,000,000,000E+0\0  */
extern char * __decoded64 (_Decimal64, char*);
/* char * should ref 50 bytes, * +0,000,000,000,000,000,000,000,000,000,000,000E+0\0  */
extern char * __decoded128 (_Decimal128, char*);

libc_hidden_proto(__decoded32)
libc_hidden_proto(__decoded64)
libc_hidden_proto(__decoded128)

__END_DECLS

#endif
