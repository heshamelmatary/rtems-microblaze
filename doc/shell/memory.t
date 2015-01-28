@c
@c  COPYRIGHT (c) 1988-2012.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c

@chapter Memory Commands

@section Introduction

The RTEMS shell has the following memory commands:

@itemize @bullet

@item @code{mdump} - Display contents of memory
@item @code{wdump} - Display contents of memory (word)
@item @code{ldump} - Display contents of memory (longword)
@item @code{medit} - Modify contents of memory
@item @code{mfill} - File memory with pattern
@item @code{mmove} - Move contents of memory
@item @code{malloc} - Obtain information on C Program Heap

@end itemize

@section Commands

This section details the Memory Commands available.  A
subsection is dedicated to each of the commands and
describes the behavior and configuration of that
command as well as providing an example usage.

@c
@c
@c
@page
@subsection mdump - display contents of memory

@pgindex mdump

@subheading SYNOPSYS:

@example
mdump [address [length [size]]]
@end example

@subheading DESCRIPTION:

This command displays the contents of memory at the @code{address}
and @code{length} in @code{size} byte units specified on the command line.

When @code{size} is not provided, it defaults to @code{1} byte units.
Values of @code{1}, @code{2}, and @code{4} are valid; all others will
cause an error to be reported.

When @code{length} is not provided, it defaults to @code{320} which
is twenty lines of output with sixteen bytes of output per line.

When @code{address} is not provided, it defaults to @code{0x00000000}.

@subheading EXIT STATUS:

This command always returns 0 to indicate success.

@subheading NOTES:

Dumping memory from a non-existent address may result in an unrecoverable
program fault.

@subheading EXAMPLES:

The following is an example of how to use @code{mdump}:

@smallexample
SHLL [/] $ mdump 0x10000 32
0x0001000000 00 00 00 00 00 00 00-00 00 00 00 00 00 00 00 ................
0x0001001000 00 00 00 00 00 00 00-00 00 00 00 00 00 00 00 ................
SHLL [/] $ mdump 0x02000000 32
0x02000000A1 48 00 00 29 00 80 33-81 C5 22 BC A6 10 21 00 .H..)..3.."...!.
0x02000010A1 48 00 00 29 00 80 33-81 C5 22 BC A6 10 21 01 .H..)..3.."...!.
SHLL [/] $ mdump 0x02001000 32
0x0200100003 00 80 00 82 10 60 00-81 98 40 00 83 48 00 00 ......`...@..H..
0x0200101084 00 60 01 84 08 A0 07-86 10 20 01 87 28 C0 02 ..`....... ..(..
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MDUMP
@findex CONFIGURE_SHELL_COMMAND_MDUMP

This command is included in the default shell command set.
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_MDUMP} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_MDUMP} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mdump

The @code{mdump} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_mdump(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{mdump} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MDUMP_Command;
@end example

@c
@c
@c
@page
@subsection wdump - display contents of memory (word)

@pgindex wdump

@subheading SYNOPSYS:

@example
wdump [address [length]]
@end example

@subheading DESCRIPTION:

This command displays the contents of memory at the @code{address}
and @code{length} in bytes specified on the command line.

This command is equivalent to @code{mdump address length 2}.

When @code{length} is not provided, it defaults to @code{320} which
is twenty lines of output with eight words of output per line.

When @code{address} is not provided, it defaults to @code{0x00000000}.

@subheading EXIT STATUS:

This command always returns 0 to indicate success.

@subheading NOTES:

Dumping memory from a non-existent address may result in an unrecoverable
program fault.

@subheading EXAMPLES:

The following is an example of how to use @code{wdump}:

@smallexample
SHLL [/] $ wdump 0x02010000 32
0x02010000 0201 08D8 0201 08C0-0201 08AC 0201 0874 ...............t
0x02010010 0201 0894 0201 0718-0201 0640 0201 0798 ...........@....
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_WDUMP
@findex CONFIGURE_SHELL_COMMAND_WDUMP

This command is included in the default shell command set.
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_WDUMP} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_WDUMP} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_wdump

The @code{wdump} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_wdump(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{wdump} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_WDUMP_Command;
@end example

@c
@c
@c
@page
@subsection ldump - display contents of memory (longword)

@pgindex ldump

@subheading SYNOPSYS:

@example
ldump [address [length]]
@end example

@subheading DESCRIPTION:

This command displays the contents of memory at the @code{address}
and @code{length} in bytes specified on the command line.

This command is equivalent to @code{mdump address length 4}.

When @code{length} is not provided, it defaults to @code{320} which
is twenty lines of output with four longwords of output per line.

When @code{address} is not provided, it defaults to @code{0x00000000}.

@subheading EXIT STATUS:

This command always returns 0 to indicate success.

@subheading NOTES:

Dumping memory from a non-existent address may result in an unrecoverable
program fault.

@subheading EXAMPLES:

The following is an example of how to use @code{ldump}:

@smallexample
SHLL [/] $ ldump 0x02010000 32
0x02010000 020108D8 020108C0-020108AC 02010874 ...............t
0x02010010 020 0894 02010718-02010640 02010798 ...........@....
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_LDUMP
@findex CONFIGURE_SHELL_COMMAND_LDUMP

This command is included in the default shell command set.
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_LDUMP} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_LDUMP} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_ldump

The @code{ldump} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_ldump(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{ldump} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_LDUMP_Command;
@end example

@c
@c
@c
@page
@subsection medit - modify contents of memory

@pgindex medit

@subheading SYNOPSYS:

@example
medit address value1 [value2 ... valueN]
@end example

@subheading DESCRIPTION:

This command is used to modify the contents of the memory starting
at @code{address} using the octets specified by the parameters
@code{value1} through @code{valueN}.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

Dumping memory from a non-existent address may result in an unrecoverable
program fault.

@subheading EXAMPLES:

The following is an example of how to use @code{medit}:

@smallexample
SHLL [/] $ mdump 0x02000000 32
0x02000000 A1 48 00 00 29 00 80 33-81 C5 22 BC A6 10 21 00 .H..)..3.."...!.
0x02000010 A1 48 00 00 29 00 80 33-81 C5 22 BC A6 10 21 01 .H..)..3.."...!.
SHLL [/] $  medit 0x02000000 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09
SHLL [/] $ mdump 0x02000000 32
0x02000000 01 02 03 04 05 06 07 08-09 00 22 BC A6 10 21 00 .........."...!.
0x02000010 A1 48 00 00 29 00 80 33-81 C5 22 BC A6 10 21 01 .H..)..3.."...!.
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MEDIT
@findex CONFIGURE_SHELL_COMMAND_MEDIT

This command is included in the default shell command set.
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_MEDIT} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_MEDIT} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_medit

The @code{medit} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_medit(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{medit} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MEDIT_Command;
@end example

@c
@c
@c
@page
@subsection mfill - file memory with pattern

@pgindex mfill

@subheading SYNOPSYS:

@example
mfill address length value
@end example

@subheading DESCRIPTION:

This command is used to fill the memory starting at @code{address}
for the specified @code{length} in octets when the specified at
@code{value}.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

Filling a non-existent address range may result in an unrecoverable
program fault.  Similarly overwriting interrupt vector tables, code
space or critical data areas can be fatal as shown in the example.

@subheading EXAMPLES:

In this example, the address used (@code{0x23d89a0}) as the base
address of the filled area is the end of the stack for the
Idle thread.  This address was determined manually using gdb and
is very specific to this application and BSP.  The first command
in this example is an @code{mdump} to display the initial contents
of this memory.  We see that the first 8 bytes are 0xA5 which is
the pattern used as a guard by the Stack Checker.  On
the first context switch after the pattern is overwritten
by the  @code{mfill} command, the Stack Checker detect the pattern
has been corrupted and generates a fatal error.

@smallexample
SHLL [/] $ mdump 0x23d89a0 16
0x023D89A0 A5 A5 A5 A5 A5 A5 A5 A5-FE ED F0 0D 0B AD 0D 06 ................
SHLL [/] $ mfill 0x23d89a0 13 0x5a
SHLL [/] $ BLOWN STACK!!! Offending task(0x23D4418): id=0x09010001; name=0x0203D908
  stack covers range 0x23D89A0 - 0x23D99AF (4112 bytes)
  Damaged pattern begins at 0x023D89A8 and is 16 bytes long
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MFILL
@findex CONFIGURE_SHELL_COMMAND_MFILL

This command is included in the default shell command set.
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_MFILL} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_MFILL} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mfill

The @code{mfill} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_mfill(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{mfill} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MFILL_Command;
@end example

@c
@c
@c
@page
@subsection mmove - move contents of memory

@pgindex mmove

@subheading SYNOPSYS:

@example
mmove dst src length
@end example

@subheading DESCRIPTION:

This command is used to copy the contents of the memory
starting at @code{src} to the memory located at @code{dst}
for the specified @code{length} in octets.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{mmove}:

@smallexample
SHLL [/] $ mdump 0x023d99a0 16
0x023D99A0 A5 A5 A5 A5 A5 A5 A5 A5-A5 A5 A5 A5 A5 A5 A5 A5 ................
SHLL [/] $ mdump 0x02000000 16
0x02000000 A1 48 00 00 29 00 80 33-81 C5 22 BC A6 10 21 00 .H..)..3.."...!.
SHLL [/] $ mmove 0x023d99a0 0x02000000 13
SHLL [/] $ mdump 0x023d99a0 16
0x023D99A0 A1 48 00 00 29 00 80 33-81 C5 22 BC A6 A5 A5 A5 .H..)..3..".....
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MMOVE
@findex CONFIGURE_SHELL_COMMAND_MMOVE

This command is included in the default shell command set.
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_MMOVE} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_MMOVE} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_mmove

The @code{mmove} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_mmove(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{mmove} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MMOVE_Command;
@end example

@c
@c
@c
@page
@subsection malloc - obtain information on C program heap

@pgindex malloc

@subheading SYNOPSYS:

@example
malloc [walk]
@end example

@subheading DESCRIPTION:

This command prints information about the current state of the C Program Heap
used by the @code{malloc()} family of calls if no or invalid options are passed
to the command.  This includes the following information:

@itemize @bullet
@item Number of free blocks
@item Largest free block
@item Total bytes free
@item Number of used blocks
@item Largest used block
@item Total bytes used
@item Size of the allocatable area in bytes
@item Minimum free size ever in bytes
@item Maximum number of free blocks ever
@item Maximum number of blocks searched ever
@item Lifetime number of bytes allocated
@item Lifetime number of bytes freed
@item Total number of searches
@item Total number of successful allocations
@item Total number of failed allocations
@item Total number of successful frees
@item Total number of successful resizes
@end itemize

When the subcommand @code{walk} is specified, then a heap walk will be
performed and information about each block is printed out.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use the @code{malloc} command.

@example
SHLL [/] $ malloc
C Program Heap and RTEMS Workspace are the same.
Number of free blocks:                               2
Largest free block:                          266207504
Total bytes free:                            266208392
Number of used blocks:                             167
Largest used block:                              16392
Total bytes used:                                83536
Size of the allocatable area in bytes:       266291928
Minimum free size ever in bytes:             266207360
Maximum number of free blocks ever:                  6
Maximum number of blocks searched ever:              5
Lifetime number of bytes allocated:              91760
Lifetime number of bytes freed:                   8224
Total number of searches:                          234
Total number of successful allocations:            186
Total number of failed allocations:                  0
Total number of successful frees:                   19
Total number of successful resizes:                  0
SHLL [/] $ malloc walk
malloc walk
PASS[0]: page size 8, min block size 48
        area begin 0x00210210, area end 0x0FFFC000
        first block 0x00210214, last block 0x0FFFBFDC
        first free 0x00228084, last free 0x00228354
PASS[0]: block 0x00210214: size 88
...
PASS[0]: block 0x00220154: size 144
PASS[0]: block 0x002201E4: size 168, prev 0x002205BC, next 0x00228354 (= last free)
PASS[0]: block 0x0022028C: size 168, prev_size 168
...
PASS[0]: block 0x00226E7C: size 4136
PASS[0]: block 0x00227EA4: size 408, prev 0x00228084 (= first free), next 0x00226CE4
PASS[0]: block 0x0022803C: size 72, prev_size 408
PASS[0]: block 0x00228084: size 648, prev 0x0020F75C (= head), next 0x00227EA4
PASS[0]: block 0x0022830C: size 72, prev_size 648
PASS[0]: block 0x00228354: size 266157192, prev 0x002201E4, next 0x0020F75C (= tail)
PASS[0]: block 0x0FFFBFDC: size 4028711480, prev_size 266157192
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_MALLOC
@findex CONFIGURE_SHELL_COMMAND_MALLOC

This command is included in the default shell command set.
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_MALLOC} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_MALLOC} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_malloc

The @code{malloc} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_malloc(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{malloc} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_MALLOC_Command;
@end example

