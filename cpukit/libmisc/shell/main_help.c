/*
 *
 *  Shell Help Command
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/system.h>
#include <rtems/shell.h>

#include "internal.h"
#include <string.h>

/*
 * show the help for one command.
 */
static int rtems_shell_help_cmd(
  const rtems_shell_cmd_t *shell_cmd
)
{
  const char * pc;
  int    col,line;

  if (!rtems_shell_can_see_cmd(shell_cmd)) {
    return 0;
  }

  printf("%-12.12s - ",shell_cmd->name);
  col = 14;
  line = 1;
  if (shell_cmd->alias) {
    printf("is an <alias> for command '%s'",shell_cmd->alias->name);
  } else if (shell_cmd->usage) {
    pc = shell_cmd->usage;
    while (*pc) {
      switch(*pc) {
        case '\r':
          break;
        case '\n':
          putchar('\n');
          col = 0;
          break;
        default:
          putchar(*pc);
          col++;
          break;
      }
      pc++;
      if (col>78) { /* What daring... 78?*/
        if (*pc) {
          putchar('\n');
          col = 0;
        }
      }
      if (!col && *pc) {
        printf("            ");
        col = 12;line++;
      }
    }
  }
  puts("");
  return line;
}

/*
 * show the help. The first command implemented.
 * Can you see the header of routine? Known?
 * The same with all the commands....
 */
static int rtems_shell_help(
  int argc,
  char * argv[]
)
{
  int col,line,lines,arg;
  char* lines_env;
  rtems_shell_topic_t *topic;

  lines_env = getenv("SHELL_LINES");
  if (lines_env)
    lines = strtol(lines_env, 0, 0);
  else
    lines = 16;

  if (argc<2) {
    printf("help: The topics are\n");
    topic = rtems_shell_first_topic;
    col = printf("  all");
    while (topic) {
      if (!col){
        col = printf("  %s",topic->topic);
      } else {
        if ((col+strlen(topic->topic)+2)>78){
          printf("\n");
          col = printf("  %s",topic->topic);
        } else {
          col+= printf(", %s",topic->topic);
        }
      }
      topic = topic->next;
    }
    printf("\n");
    return 1;
  }
  line = 0;
  for (arg = 1;arg<argc;arg++) {
    const char *cur = argv[arg];
    rtems_shell_cmd_t *shell_cmd;

    if (lines && (line > lines)) {
      printf("Press any key to continue...");getchar();
      printf("\n");
      line = 0;
    }
    topic  =  rtems_shell_lookup_topic(cur);
    if (topic == NULL) {
      if ((shell_cmd = rtems_shell_lookup_cmd(cur)) == NULL) {
        if (strcmp(cur, "all") != 0) {
          printf(
            "help: topic or cmd '%s' not found. Try <help> alone for a list\n",
            cur
          );
          line++;
          continue;
        }
      } else {
        line+= rtems_shell_help_cmd(shell_cmd);
        continue;
      }
    }
    printf("help: list for the topic '%s'\n",cur);
    line++;
    shell_cmd = rtems_shell_first_cmd;
    while (shell_cmd) {
      if (topic == NULL || !strcmp(topic->topic,shell_cmd->topic))
        line+= rtems_shell_help_cmd(shell_cmd);
      if (lines && (line > lines)) {
        printf("Press any key to continue...");
        getchar();
        printf("\n");
        line = 0;
      }
      shell_cmd = shell_cmd->next;
    }
  }
  puts("");
  return 0;
}

rtems_shell_cmd_t rtems_shell_HELP_Command  =  {
  .name = "help",
  .usage = "help [topic] # list of usage of commands",
  .topic = "help",
  .command = rtems_shell_help,
  .mode = S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH
};
