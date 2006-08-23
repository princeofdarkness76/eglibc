/* Set flags signalling availability of kernel features based on given
   kernel version number.
   Copyright (C) 2006 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

/* The utimes syscall was added before 2.6.1.  */
#if __LINUX_KERNEL_VERSION >= 132609
# define __ASSUME_UTIMES	1
#endif

/* The new getrlimit syscall was added sometime before 2.4.6.  */
#if __LINUX_KERNEL_VERSION >= 132102
#define __ASSUME_NEW_GETRLIMIT_SYSCALL	1
#endif

#include_next <kernel-features.h>
