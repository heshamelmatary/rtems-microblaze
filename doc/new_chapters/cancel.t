@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Thread Cancellation Manager

@section Introduction

The 
thread cancellation manager is ...

The directives provided by the thread cancellation manager are:

@itemize @bullet
@item @code{pthread_cancel} - 
@item @code{pthread_setcancelstate} - 
@item @code{pthread_setcanceltype} - 
@item @code{pthread_testcancel} - 
@item @code{pthread_cleanup_push} - 
@end itemize

@section Background

There is currently no text in this section.

@section Operations

There is currently no text in this section.

@section Directives

This section details the thread cancellation manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection pthread_cancel - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pthread_cancel(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection pthread_setcancelstate - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pthread_setcancelstate(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection pthread_setcanceltype - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pthread_setcanceltype(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection pthread_testcancel - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pthread_testcancel(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection pthread_cleanup_push - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pthread_cleanup_push(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

