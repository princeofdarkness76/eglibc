/* Copyright (C) 1991,92,96,97,99,2000,2001,2009 Free Software Foundation, Inc.
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

#ifndef	_STRINGS_H
#define	_STRINGS_H	1

/* We don't need and should not read this file if <string.h> was already
   read. The one exception being that if __USE_BSD isn't defined, then
   these aren't defined in string.h, so we need to define them here.  */
#if !defined _STRING_H || !defined __USE_BSD

# include <features.h>
# define __need_size_t
# include <stddef.h>

/* Tell the caller that we provide correct C++ prototypes.  */
# if defined __cplusplus && __GNUC_PREREQ (4, 4)
#  define __CORRECT_ISO_CPP_STRINGS_H_PROTO
# endif

__BEGIN_DECLS

/* Compare N bytes of S1 and S2 (same as memcmp).  */
extern int bcmp (__const void *__s1, __const void *__s2, size_t __n)
     __THROW __attribute_pure__;

/* Copy N bytes of SRC to DEST (like memmove, but args reversed).  */
extern void bcopy (__const void *__src, void *__dest, size_t __n) __THROW;

/* Set N bytes of S to 0.  */
extern void bzero (void *__s, size_t __n) __THROW;

/* Return the position of the first bit set in I, or 0 if none are set.
   The least-significant bit is position 1, the most-significant 32.  */
extern int ffs (int __i) __THROW __attribute__ ((const));

/* Find the first occurrence of C in S (same as strchr).  */
# ifdef __CORRECT_ISO_CPP_STRINGS_H_PROTO
extern "C++"
{
extern char *index (char *__s, int __c)
     __THROW __asm ("index") __attribute_pure__ __nonnull ((1));
extern __const char *index (__const char *__s, int __c)
     __THROW __asm ("index") __attribute_pure__ __nonnull ((1));

#  if defined __OPTIMIZE__ && !defined __CORRECT_ISO_CPP_STRING_H_PROTO
__extern_always_inline char *
index (char *__s, int __c) __THROW
{
  return __builtin_index (__s, __c);
}

__extern_always_inline __const char *
index (__const char *__s, int __c) __THROW
{
  return __builtin_index (__s, __c);
}
#  endif
}
# else
extern char *index (__const char *__s, int __c)
     __THROW __attribute_pure__ __nonnull ((1));
# endif

/* Find the last occurrence of C in S (same as strrchr).  */
# ifdef __CORRECT_ISO_CPP_STRINGS_H_PROTO
extern "C++"
{
extern char *rindex (char *__s, int __c)
     __THROW __asm ("rindex") __attribute_pure__ __nonnull ((1));
extern __const char *rindex (__const char *__s, int __c)
     __THROW __asm ("rindex") __attribute_pure__ __nonnull ((1));

#  if defined __OPTIMIZE__ && !defined __CORRECT_ISO_CPP_STRING_H_PROTO
__extern_always_inline char *
rindex (char *__s, int __c) __THROW
{
  return __builtin_rindex (__s, __c);
}

__extern_always_inline __const char *
rindex (__const char *__s, int __c) __THROW
{
  return __builtin_rindex (__s, __c);
}
#endif
}
# else
extern char *rindex (__const char *__s, int __c)
     __THROW __attribute_pure__ __nonnull ((1));
# endif

/* Compare S1 and S2, ignoring case.  */
extern int strcasecmp (__const char *__s1, __const char *__s2)
     __THROW __attribute_pure__;

/* Compare no more than N chars of S1 and S2, ignoring case.  */
extern int strncasecmp (__const char *__s1, __const char *__s2, size_t __n)
     __THROW __attribute_pure__;

__END_DECLS

#endif	/* string.h  */

#endif	/* strings.h  */
