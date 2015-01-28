/**
 * @file
 *
 * @ingroup i386_pc386
 *
 * @brief Sizes of Global and Interrupt descriptor tables.
 */

/*
 * This header file is also used in assembler modules.
 *
 * Copyright (C) 2014  Jan Doležal (dolezj21@fel.cvut.cz)
 *                     CTU in Prague.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bspopts.h>

#define IDT_SIZE (256)
#define GDT_SIZE (3 + NUM_APP_DRV_GDT_DESCRIPTORS)
