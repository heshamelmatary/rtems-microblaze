@c
@c  COPYRIGHT (c) 1988-1999.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Ada95 Interrupt Support

@section Introduction

This chapter describes what is required to enable Ada interrupt
and error exception handling when using GNAT over RTEMS.

The GNAT Ada95 interrupt support RTEMS was developed by 
Jiri Gaisler <jgais@@ws.estec.esa.nl> who also wrote this
chapter.

@section Mapping Interrupts to POSIX Signals

In Ada95, interrupts can be attached with the interrupt_attach pragma. 
For most systems, the gnat run-time will use POSIX signal to implement
the interrupt handling, mapping one signal per interrupt. For interrupts
to be propagated to the attached Ada handler, the corresponding signal
must be raised when the interrupt occurs. 

The same mechanism is used to generate Ada error exceptions. 
Three error exceptions are defined: program, constraint and storage
error. These are generated by raising the predefined signals: SIGILL, 
SIGFPE and SIGSEGV. These signals should be raised when a spurious
or erroneous trap occurs.

To enable gnat interrupt and error exception support for a particular 
bsp, the following has to be done:

@enumerate

@item Write an interrupt/trap handler that will raise the corresponding
signal depending on the interrupt/trap number.

@item Install the interrupt handler for all interrupts/traps that will be
handled by gnat (including spurious).

@item At startup, gnat calls @code{__gnat_install_handler()}. The bsp 
must provide this function which installs the interrupt/trap handlers.

@end enumerate

Which cpu-interrupt will generate which signal is implementation
defined. There are 32 POSIX signals (1 - 32), and all except the
three error signals (SIGILL, SIGFPE and SIGSEGV) can be used. I
would suggest to use the upper 16 (17 - 32) which do not
have an assigned POSIX name.

Note that the pragma interrupt_attach will only bind a signal
to a particular Ada handler - it will not unmask the
interrupt or do any other things to enable it. This have to be
done separately, typically by writing various device register.

@section Example Ada95 Interrupt Program

An example program (@code{irq_test}) is included in the
Ada examples package to show how interrupts can be handled
in Ada95. Note that generation of the test interrupt
(@code{irqforce.c}) is bsp specific and must be edited.

NOTE: The @code{irq_test} example was written for the SPARC/ERC32
BSP.

@section Version Requirements

With RTEMS 4.0, a patch was required to psignal.c in RTEMS
sources (to correct a bug associated to the default action of
signals 15-32).   The SPARC/ERC32 RTEMS BSP includes the
@code{gnatsupp} subdirectory that can be used as an example
for other BSPs.

With GNAT 3.11p, a patch is required for @code{a-init.c} to invoke
the BSP specific routine that installs the exception handlers.

