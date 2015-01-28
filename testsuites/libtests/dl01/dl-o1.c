/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <stdio.h>
#include <stdlib.h>

/**
 * Hello World as a loadable module.
 */

#include <stdio.h>

/*
 * Yes a decl in the source. This is a modules main and I could not find which
 * header main is defined in.
 */
int rtems_main (int argc, char* argv[]);

int rtems_main (int argc, char* argv[])
{
  int arg;
  printf("Loaded module: argc:%d [%s]\n", argc, __FILE__);
  for (arg = 0; arg < argc; ++arg)
    printf("  %d: %s\n", arg, argv[arg]);
  return argc;
}
