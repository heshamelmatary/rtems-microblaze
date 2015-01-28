/*
 *  PWD Shell Command Implmentation
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
#include <unistd.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_pwd(
  int   argc __attribute__((unused)),
  char *argv[] __attribute__((unused))
)
{
  char dir[1024];

  getcwd(dir,1024);
  puts(dir);
  return 0;
}

rtems_shell_cmd_t rtems_shell_PWD_Command = {
  "pwd",                                        /* name */
  "pwd          # print work directory",        /* usage */
  "files",                                      /* topic */
  rtems_shell_main_pwd,                         /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
