@c
@c  COPYRIGHT (c) 1988-1999.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Clock Driver

@section Introduction

The purpose of the clock driver is to provide a steady time
basis to the kernel, so that the RTEMS primitives that need
a clock tick work properly.  See the @code{Clock Manager} chapter
of the @b{RTEMS Application C User's Guide} for more details.

The clock driver is located in the @code{clock} directory of the BSP. 

@section Clock Driver Global Variables

This section describes the global variables expected to be provided by
this driver.

@subsection Major and Minor Number

The major and minor numbers of the clock driver are made available via 
the following variables.  

@itemize @bullet
@item rtems_device_major_number rtems_clock_major;
@item rtems_device_minor_number rtems_clock_minor;
@end itemize

The clock device driver is responsible for declaring and
initializing these variables.  These variables are used
by other RTEMS components -- notably the Shared Memory Driver. 

@b{NOTE:} In a future RTEMS version, these variables may be replaced
with the clock device driver registering @b{/dev/clock}.

@subsection Ticks Counter

Most of the clock device drivers provide a global variable
that is simply a count of the number of clock driver interrupt service
routines that have occured.  This information is valuable when debugging
a system.  This variable is declared as follows:

@example
volatile rtems_unsigned32 Clock_driver_ticks;
@end example

@section Initialization

The initialization routine is responsible for
programming the hardware that will periodically
generate an interrupt.  A programmable interval timer is commonly
used as the source of the clock tick.

The device should be programmed such that an interrupt is generated
every @i{m} microseconds, where @i{m} is equal to 
@code{BSP_Configuration.microseconds_per_tick}. Sometimes the periodic interval
timer can use a prescaler so you have to look carefully at your user's
manual to determine the correct value. 

You must use the RTEMS primitive @code{rtems_interrupt_catch} to install
your clock interrupt service routine:

@example
rtems_interrupt_catch (Clock_ISR, CLOCK_VECTOR, &old_handler); 
@end example

Since there is currently not a driver entry point invoked at system
shutdown, many clock device drivers use the @code{atexit} routine
to schedule their @code{Clock_exit} routine to execute when the
system is shutdown.

By convention, many of the clock drivers do not install the clock
tick if the @code{ticks_per_timeslice} field of the Configuration
Table is 0.

@section System shutdown

Many drivers provide the routine @code{Clock_exit} that is scheduled
to be run during system shutdown via the @code{atexit} routine.
The @code{Clock_exit} routine will disable the clock tick source
if it was enabled.  This can be used to prevent clock ticks after the
system is shutdown.

@section Clock Interrupt Subroutine

It only has to inform the kernel that a ticker has elapsed, so call : 

@example
@group
rtems_isr Clock_isr( rtems_vector_number vector )
@{
  invoke the rtems_clock_tick() directive to announce the tick
  if necessary for this hardware
    reload the programmable timer
@}
@end group
@end example

@section IO Control

The clock driver must supply a handler for the IO control device driver
entry point.  This functionality is used by other components -- notably
the Shared Memory Driver to install a wrapper for the clock interrupt
service routine.  The following shows the functionality required:

@example
@group
rtems_device_driver Clock_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
@{
  error check the argument pointer parameter

  if the command is "ISR"
    invoke the clock interrupt service routine
  else if the command is "NEW"
    install the requested handler
@}
@end group
@end example




