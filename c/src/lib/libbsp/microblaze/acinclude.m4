# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  gdbmbsim )
    AC_CONFIG_SUBDIRS([gdbmbsim]);;
  microblaze_fpga )
    AC_CONFIG_SUBDIRS([microblaze_fpga]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
