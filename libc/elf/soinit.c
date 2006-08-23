/* Initializer module for building the ELF shared C library.  This file and
   sofini.c do the work normally done by crtbeginS.o and crtendS.o, to wrap
   the `.ctors' and `.dtors' sections so the lists are terminated, and
   calling those lists of functions.  */

#include <libc-internal.h>
#include <stdlib.h>

static void (*const __CTOR_LIST__[1]) (void)
     __attribute__ ((section (".ctors")))
     = { (void (*) (void)) -1 };
static void (*const __DTOR_LIST__[1]) (void)
     __attribute__ ((section (".dtors")))
     = { (void (*) (void)) -1 };

static inline void
run_hooks (void (*const list[]) (void))
{
  while (*++list)
    (**list) ();
}

static char __EH_FRAME_BEGIN__[]
     __attribute__ ((section (".eh_frame")))
     = { };

/* This function will be called from _init in init-first.c.  */
void
__libc_global_ctors (void)
{
  /* Call constructor functions.  */
  run_hooks (__CTOR_LIST__);
}


/* This function becomes the DT_FINI termination function
   for the C library.  */
#ifndef HAVE_INITFINI_ARRAY
void _fini (void) __attribute__ ((section (".fini"))); /* Just for kicks.  */
void
_fini (void)
#else
void
__libc_fini (void)
#endif
{
  /* Call destructor functions.  */
  run_hooks (__DTOR_LIST__);
}
#ifdef HAVE_INITFINI_ARRAY
void (*_fini_ptr) (void) __attribute__ ((section (".fini_array")))
     = &__libc_fini;
#endif
