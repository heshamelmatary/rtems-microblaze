@c
@c  COPYRIGHT (c) 1988-1999.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Fatal Error Manager

@cindex fatal errors

@section Introduction

The fatal error manager processes all fatal or
irrecoverable errors.  The directive provided by the fatal error
manager is:

@itemize @bullet
@item @code{@value{DIRPREFIX}fatal_error_occurred} - Invoke the fatal error handler
@end itemize

@section Background

@cindex fatal error detection
@cindex fatal error processing
@cindex fatal error user extension

The fatal error manager is called upon detection of
an irrecoverable error condition by either RTEMS or the
application software.  Fatal errors can be detected from three
sources:

@itemize @bullet
@item the executive (RTEMS)
@item user system code
@item user application code
@end itemize

RTEMS automatically invokes the fatal error manager
upon detection of an error it considers to be fatal.  Similarly,
the user should invoke the fatal error manager upon detection of
a fatal error.

Each status or dynamic user extension set may include
a fatal error handler.  The fatal error handler in the static
extension set can be used to provide access to debuggers and
monitors which may be present on the target hardware.  If any
user-supplied fatal error handlers are installed, the fatal
error manager will invoke them.  If no user handlers are
configured or if all the user handler return control to the
fatal error manager, then the RTEMS default fatal error handler
is invoked.  If the default fatal error handler is invoked, then
the system state is marked as failed.

Although the precise behavior of the default fatal
error handler is processor specific, in general, it will disable
all maskable interrupts, place the error code in a known
processor dependent place (generally either on the stack or in a
register), and halt the processor.  The precise actions of the
RTEMS fatal error are discussed in the Default Fatal Error
Processing chapter of the Applications Supplement document for
a specific target processor.

@section Operations

@subsection Announcing a Fatal Error

The @code{@value{DIRPREFIX}fatal_error_occurred} directive is invoked when a
fatal error is detected.  Before invoking any user-supplied
fatal error handlers or the RTEMS fatal error handler, the
@code{@value{DIRPREFIX}fatal_error_occurred}
directive stores useful information in the
variable @code{_Internal_errors_What_happened}.  This @value{STRUCTURE}
contains three pieces of information:

@itemize @bullet
@item the source of the error (API or executive core),

@item whether the error was generated internally by the
executive, and a

@item a numeric code to indicate the error type.
@end itemize

The error type indicator is dependent on the source
of the error and whether or not the error was internally
generated by the executive.  If the error was generated
from an API, then the error code will be of that API's
error or status codes.  The status codes for the RTEMS
API are in c/src/exec/rtems/headers/status.h.  Those
for the POSIX API can be found in <errno.h>.

The @code{@value{DIRPREFIX}fatal_error_occurred} directive is responsible
for invoking an optional user-supplied fatal error handler
and/or the RTEMS fatal error handler.  All fatal error handlers
are passed an error code to describe the error detected.

Occasionally, an application requires more
sophisticated fatal error processing such as passing control to
a debugger.  For these cases, a user-supplied fatal error
handler can be specified in the RTEMS configuration table.  The
User Extension Table field fatal contains the address of the
fatal error handler to be executed when the
@code{@value{DIRPREFIX}fatal_error_occurred}
directive is called.  If the field is set to NULL or if the
configured fatal error handler returns to the executive, then
the default handler provided by RTEMS is executed.  This default
handler will halt execution on the processor where the error
occurred.

@section Directives

This section details the fatal error manager's
directives.  A subsection is dedicated to each of this manager's
directives and describes the calling sequence, related
constants, usage, and status codes.

@c
@c
@c
@page
@subsection FATAL_ERROR_OCCURRED - Invoke the fatal error handler

@cindex announce fatal error
@cindex fatal error, announce

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_fatal_error_occurred
@example
void volatile rtems_fatal_error_occurred(
  rtems_unsigned32        the_error
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Fatal_Error_Occurred (
   The_Error : in     RTEMS.Unsigned32
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

NONE

@subheading DESCRIPTION:

This directive processes fatal errors.  If the FATAL
error extension is defined in the configuration table, then the
user-defined error extension is called.  If configured and the
provided FATAL error extension returns, then the RTEMS default
error handler is invoked.  This directive can be invoked by
RTEMS or by the user's application code including initialization
tasks, other tasks, and ISRs.

@subheading NOTES:

This directive supports local operations only.

Unless the user-defined error extension takes special
actions such as restarting the calling task, this directive WILL
NOT RETURN to the caller.

The user-defined extension for this directive may
wish to initiate a global shutdown.
