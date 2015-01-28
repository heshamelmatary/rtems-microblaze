/**
 * @file
 *
 * @brief FIFO Support
 * @ingroup IMFS
 */

/*
 * Author: Wei Shen <cquark@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

#define JNODE2PIPE(_jnode)  ( ((IMFS_fifo_t *)(_jnode))->pipe )

#define LIBIO2PIPE(_iop)  ( JNODE2PIPE((IMFS_jnode_t *)(_iop)->pathinfo.node_access) )

/* Set errno and return -1 if error, else return _err */
#define IMFS_FIFO_RETURN(_err) \
do {  \
  if (_err < 0)	\
    rtems_set_errno_and_return_minus_one(-_err); \
  return _err; \
} while (0)

static int IMFS_fifo_open(
  rtems_libio_t *iop,
  const char    *pathname,
  int            oflag,
  mode_t         mode
)
{
  IMFS_jnode_t *jnode = iop->pathinfo.node_access;

  int err = fifo_open(&JNODE2PIPE(jnode), iop);
  IMFS_FIFO_RETURN(err);
}

static int IMFS_fifo_close(
  rtems_libio_t *iop
)
{
  int err = 0;
  IMFS_jnode_t *jnode = iop->pathinfo.node_access;

  pipe_release(&JNODE2PIPE(jnode), iop);

  IMFS_FIFO_RETURN(err);
}

static ssize_t IMFS_fifo_read(
  rtems_libio_t *iop,
  void          *buffer,
  size_t         count
)
{
  IMFS_jnode_t *jnode = iop->pathinfo.node_access;

  int err = pipe_read(JNODE2PIPE(jnode), buffer, count, iop);
  if (err > 0)
    IMFS_update_atime(jnode);

  IMFS_FIFO_RETURN(err);
}

static ssize_t IMFS_fifo_write(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
)
{
  IMFS_jnode_t *jnode = iop->pathinfo.node_access;

  int err = pipe_write(JNODE2PIPE(jnode), buffer, count, iop);
  if (err > 0) {
    IMFS_mtime_ctime_update(jnode);
  }

  IMFS_FIFO_RETURN(err);
}

static int IMFS_fifo_ioctl(
  rtems_libio_t   *iop,
  ioctl_command_t  command,
  void            *buffer
)
{
  int err;

  if (command == FIONBIO) {
    if (buffer == NULL)
      err = -EFAULT;
    else {
      if (*(int *)buffer)
        iop->flags |= LIBIO_FLAGS_NO_DELAY;
      else
        iop->flags &= ~LIBIO_FLAGS_NO_DELAY;
      return 0;
    }
  }
  else
    err = pipe_ioctl(LIBIO2PIPE(iop), command, buffer, iop);

  IMFS_FIFO_RETURN(err);
}

static const rtems_filesystem_file_handlers_r IMFS_fifo_handlers = {
  .open_h = IMFS_fifo_open,
  .close_h = IMFS_fifo_close,
  .read_h = IMFS_fifo_read,
  .write_h = IMFS_fifo_write,
  .ioctl_h = IMFS_fifo_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = IMFS_stat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

const IMFS_node_control IMFS_node_control_fifo = {
  .handlers = &IMFS_fifo_handlers,
  .node_size = sizeof(IMFS_fifo_t),
  .node_initialize = IMFS_node_initialize_default,
  .node_remove = IMFS_node_remove_default,
  .node_destroy = IMFS_node_destroy_default
};
