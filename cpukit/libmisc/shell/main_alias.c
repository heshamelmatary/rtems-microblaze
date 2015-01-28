/*
 *  ALIAS Shell Command Implmentation
 *
 *  Author: Fernando RUIZ CASAS
 *  Work: fernando.ruiz@ctv.es
 *  Home: correo@fernando-ruiz.com
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_rtems_main_alias(int argc, char **argv)
{
  if (argc<3) {
    fprintf(stderr,"too few arguments\n");
    return 1;
  }

  if (!rtems_shell_alias_cmd(argv[1],argv[2])) {
    fprintf(stderr,"unable to make an alias(%s,%s)\n",argv[1],argv[2]);
  }
  return 0;
}

rtems_shell_cmd_t rtems_shell_ALIAS_Command = {
  .name = "alias",
  .usage = "alias old new",
  .topic = "misc",
  .command = rtems_shell_rtems_main_alias,
  .mode = S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH
};
