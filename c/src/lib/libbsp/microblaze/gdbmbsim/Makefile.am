##
## $Id: Makefile.am,v 1.2 2011/01/31 17:41:09 joel Exp $
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

nodist_include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c ../../shared/bsppretaskinghook.c \
    ../../shared/bspgetworkarea.c ../../shared/bsppost.c \
    ../../shared/bspstart.c ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c ../../shared/bspreset.c \
    startup/_exception_handler.S startup/_hw_exception_handler.S \
    startup/_interrupt_handler.S startup/sim-crtinit.S

clock_SOURCES = ../../shared/clock_driver_simidle.c
console_SOURCES = ../../shared/console-polled.c console/console-io.c
timer_SOURCES = ../../shared/timerstub.c

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(timer_SOURCES)

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
