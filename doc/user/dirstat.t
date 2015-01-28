@c
@c  COPYRIGHT (c) 1989-2011.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Directive Status Codes

@section Introduction

@table @b
@item @code{@value{RPREFIX}SUCCESSFUL} - successful completion
@item @code{@value{RPREFIX}TASK_EXITTED} - returned from a task
@item @code{@value{RPREFIX}MP_NOT_CONFIGURED} - multiprocessing not configured
@item @code{@value{RPREFIX}INVALID_NAME} - invalid object name
@item @code{@value{RPREFIX}INVALID_ID} - invalid object id
@item @code{@value{RPREFIX}TOO_MANY} - too many
@item @code{@value{RPREFIX}TIMEOUT} - timed out waiting
@item @code{@value{RPREFIX}OBJECT_WAS_DELETED} - object was deleted while waiting
@item @code{@value{RPREFIX}INVALID_SIZE} - invalid specified size
@item @code{@value{RPREFIX}INVALID_ADDRESS} - invalid address specified
@item @code{@value{RPREFIX}INVALID_NUMBER} - number was invalid
@item @code{@value{RPREFIX}NOT_DEFINED} - item not initialized
@item @code{@value{RPREFIX}RESOURCE_IN_USE} - resources outstanding
@item @code{@value{RPREFIX}UNSATISFIED} - request not satisfied
@item @code{@value{RPREFIX}INCORRECT_STATE} - task is in wrong state
@item @code{@value{RPREFIX}ALREADY_SUSPENDED} - task already in state
@item @code{@value{RPREFIX}ILLEGAL_ON_SELF} - illegal for calling task
@item @code{@value{RPREFIX}ILLEGAL_ON_REMOTE_OBJECT} - illegal for remote object
@item @code{@value{RPREFIX}CALLED_FROM_ISR} - invalid environment
@item @code{@value{RPREFIX}INVALID_PRIORITY} - invalid task priority
@item @code{@value{RPREFIX}INVALID_CLOCK} - invalid time buffer
@item @code{@value{RPREFIX}INVALID_NODE} - invalid node id
@item @code{@value{RPREFIX}NOT_CONFIGURED} - directive not configured
@item @code{@value{RPREFIX}NOT_OWNER_OF_RESOURCE} - not owner of resource
@item @code{@value{RPREFIX}NOT_IMPLEMENTED} - directive not implemented
@item @code{@value{RPREFIX}INTERNAL_ERROR} - RTEMS inconsistency detected
@item @code{@value{RPREFIX}NO_MEMORY} - could not get enough memory
@end table

@section Directives

@page
@subsection STATUS_TEXT - Returns the enumeration name for a status code

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_status_text
@example
const char *rtems_status_text(
  rtems_status_code code
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES

The status code enumeration name or "?" in case the status code is invalid.

@subheading DESCRIPTION:

Returns the enumeration name for the specified status code.
