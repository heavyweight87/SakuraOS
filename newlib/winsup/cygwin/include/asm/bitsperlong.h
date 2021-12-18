/* asm/bitsperlong.h

This file is part of Cygwin.

This software is a copyrighted work licensed under the terms of the
Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
details. */

#ifndef __ASM_BITSPERLONG_H
#define __ASM_BITSPERLONG_H

#ifdef __x86_64__
#define __BITS_PER_LONG 64
#else
#define __BITS_PER_LONG 32
#endif

#endif /* __ASM_BITSPERLONG_H */
