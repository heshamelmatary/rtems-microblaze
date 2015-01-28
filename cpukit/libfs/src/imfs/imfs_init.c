/**
 * @file
 *
 * @ingroup LibFSIMFS
 *
 * @brief IMFS initialization.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

const rtems_filesystem_operations_table IMFS_ops = {
  .lock_h = rtems_filesystem_default_lock,
  .unlock_h = rtems_filesystem_default_unlock,
  .eval_path_h = IMFS_eval_path,
  .link_h = IMFS_link,
  .are_nodes_equal_h = rtems_filesystem_default_are_nodes_equal,
  .mknod_h = IMFS_mknod,
  .rmnod_h = IMFS_rmnod,
  .fchmod_h = IMFS_fchmod,
  .chown_h = IMFS_chown,
  .clonenod_h = IMFS_node_clone,
  .freenod_h = IMFS_node_free,
  .mount_h = IMFS_mount,
  .fsmount_me_h = IMFS_initialize,
  .unmount_h = IMFS_unmount,
  .fsunmount_me_h = IMFS_fsunmount,
  .utime_h = IMFS_utime,
  .symlink_h = IMFS_symlink,
  .readlink_h = IMFS_readlink,
  .rename_h = IMFS_rename,
  .statvfs_h = rtems_filesystem_default_statvfs
};

static const IMFS_node_control *const
  IMFS_node_controls [IMFS_TYPE_COUNT] = {
  [IMFS_DIRECTORY] = &IMFS_node_control_directory,
  [IMFS_DEVICE] = &IMFS_node_control_device,
  [IMFS_HARD_LINK] = &IMFS_node_control_hard_link,
  [IMFS_SYM_LINK] = &IMFS_node_control_sym_link,
  [IMFS_MEMORY_FILE] = &IMFS_node_control_memfile,
  [IMFS_LINEAR_FILE] = &IMFS_node_control_linfile,
  [IMFS_FIFO] = &IMFS_node_control_enosys
};

int IMFS_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  return IMFS_initialize_support(
    mt_entry,
    &IMFS_ops,
    IMFS_node_controls
  );
}
