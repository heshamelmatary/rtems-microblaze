/*
 *  Copyright (c) 2015, Hesham Almatary
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*  
 *  This file sets up basic CPU dependency settings based on
 *  compiler settings.  For example, it can determine if
 *  floating point is available.  This particular implementation
 *  is specified to the NO CPU port.
 *
 *  $Id: no_cpu.h,v 1.9 2009/12/02 09:48:25 ralf Exp $
 *
 */


#ifndef _RTEMS_SCORE_NO_CPU_H
#define _RTEMS_SCORE_NO_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the NO CPU family.
 *  It does this by setting variables to indicate which
 *  implementation dependent features are present in a particular
 *  member of the family.
 *
 *  This is a good place to list all the known CPU models
 *  that this port supports and which RTEMS CPU model they correspond
 *  to.
 */

#if defined(rtems_multilib)
/*
 *  Figure out all CPU Model Feature Flags based upon compiler
 *  predefines.
 */

#define CPU_MODEL_NAME  "rtems_multilib"
#define NOCPU_HAS_FPU     1

#else
/* if defined(__MICROBLAZE__) */

#define CPU_MODEL_NAME  "MicroBlaze"
#define NOCPU_HAS_FPU     1

/*
#else

#error "Unsupported CPU Model"
*/

#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "MicroBlaze CPU"

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_NO_CPU_H */
