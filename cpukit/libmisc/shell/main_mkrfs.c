/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/stringto.h>
#include <rtems/shellconfig.h>
#include <rtems/rtems-rfs-shell.h>
#include <rtems/fsmount.h>
#include "internal.h"

#define OPTIONS "[-v] [-s blksz] [-b grpblk] [-i grpinode] [-I] [-o %inode]"

rtems_shell_cmd_t rtems_shell_MKRFS_Command = {
  "mkrfs",                                   /* name */
  "mkrfs " OPTIONS " dev",                   /* usage */
  "files",                                   /* topic */
  rtems_shell_rfs_format,                    /* command */
  NULL,                                      /* alias */
  NULL                                       /* next */
};
