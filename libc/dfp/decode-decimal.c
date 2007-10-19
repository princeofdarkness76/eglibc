/* Functions to print the HOST _Decimal information to use with printf %Z
   This is the default no-op version.
   Copyright (C) 2006, 2007 IBM Corporation.

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

#include <decode-decimal.h>

char * __decoded32 (_Decimal32 a, char * str)
{
    return str;
}
#if defined NOT_IN_libc
  weak_alias (__decoded32, decoded32)
#endif
libc_hidden_def(__decoded32)

char * __decoded64 ( _Decimal64 a, char * str )
{
    return str;
}
#if defined NOT_IN_libc
  weak_alias (__decoded64, decoded64)
#endif
libc_hidden_def(__decoded64)

char * __decoded128 ( _Decimal128 a, char * str )
{
    return str;
}
#if defined NOT_IN_libc
  weak_alias (__decoded128, decoded128)
#endif
libc_hidden_def(__decoded128)
