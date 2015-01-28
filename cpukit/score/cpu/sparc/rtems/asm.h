/**
 * @file
 *
 * @brief Address the Problems Caused by Incompatible Flavor of
 * Assemblers and Toolsets
 *
 * This include file attempts to address the problems
 * caused by incompatible flavors of assemblers and
 * toolsets.  It primarily addresses variations in the
 * use of leading underscores on symbols and the requirement
 * that register names be preceded by a %.
 *
 * NOTE: The spacing in the use of these macros
 *       is critical to them working as advertised.
 */

/*
 *  COPYRIGHT:
 *
 *  This file is based on similar code found in newlib available
 *  from ftp.cygnus.com.  The file which was used had no copyright
 *  notice.  This file is freely distributable as long as the source
 *  of the file is noted.
 */

#ifndef _RTEMS_ASM_H
#define _RTEMS_ASM_H

/*
 *  Indicate we are in an assembly file and get the basic CPU definitions.
 */

#ifndef ASM
#define ASM
#endif

#include <rtems/score/cpuopts.h>
#include <rtems/score/cpu.h>

/*
 *  Recent versions of GNU cpp define variables which indicate the
 *  need for underscores and percents.  If not using GNU cpp or
 *  the version does not support this, then you will obviously
 *  have to define these as appropriate.
 */

/* XXX __USER_LABEL_PREFIX__ and __REGISTER_PREFIX__ do not work on gcc 2.7.0 */
/* XXX The following ifdef magic fixes the problem but results in a warning   */
/* XXX when compiling assembly code.                                          */

#ifndef __USER_LABEL_PREFIX__
#define __USER_LABEL_PREFIX__ _
#endif

#ifndef __REGISTER_PREFIX__
#define __REGISTER_PREFIX__
#endif

#include <rtems/concat.h>

/* Use the right prefix for global labels.  */

#define SYM(x) CONCAT1 (__USER_LABEL_PREFIX__, x)

/* Use the right prefix for registers.  */

#define REG(x) CONCAT1 (__REGISTER_PREFIX__, x)

/*
 *  define macros for all of the registers on this CPU
 *
 *  EXAMPLE:     #define d0 REG (d0)
 */

/*
 *  Define macros to handle section beginning and ends.
 */


#define BEGIN_CODE_DCL .text
#define END_CODE_DCL
#define BEGIN_DATA_DCL .data
#define END_DATA_DCL
#define BEGIN_CODE .text
#define END_CODE
#define BEGIN_DATA
#define END_DATA
#define BEGIN_BSS
#define END_BSS
#define END

/*
 *  Following must be tailor for a particular flavor of the C compiler.
 *  They may need to put underscores in front of the symbols.
 */

#define PUBLIC(sym) .globl SYM (sym)
#define EXTERN(sym) .globl SYM (sym)

/*
 *  Entry for traps which jump to a programmer-specified trap handler.
 */

#define TRAP(_vector, _handler)  \
  mov   %psr, %l0 ; \
  sethi %hi(_handler), %l4 ; \
  jmp   %l4+%lo(_handler); \
  mov   _vector, %l3

/*
 *  Used for the reset trap to avoid a supervisor instruction
 */

#define RTRAP(_vector, _handler)  \
  mov   %g0, %l0 ; \
  sethi %hi(_handler), %l4 ; \
  jmp   %l4+%lo(_handler); \
  mov   _vector, %l3

#endif
