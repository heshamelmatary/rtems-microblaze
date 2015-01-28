/**
 * @file
 *
 * @brief MSDOS File Handlers Implementation
 * @ingroup libfs
 */

/*
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>

#include "fat.h"
#include "fat_fat_operations.h"
#include "fat_file.h"

#include "msdos.h"

/* msdos_file_read --
 *     This routine read from file pointed to by file control block into
 *     the specified data buffer provided by user
 *
 * PARAMETERS:
 *     iop    - file control block
 *     buffer - buffer  provided by user
 *     count  - the number of bytes to read
 *
 * RETURNS:
 *     the number of bytes read on success, or -1 if error occured (errno set
 *     appropriately)
 */
ssize_t
msdos_file_read(rtems_libio_t *iop, void *buffer, size_t count)
{
    ssize_t            ret = 0;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info;
    fat_file_fd_t     *fat_fd = iop->pathinfo.node_access;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        rtems_set_errno_and_return_minus_one(EIO);

    ret = fat_file_read(&fs_info->fat, fat_fd, iop->offset, count,
                        buffer);
    if (ret > 0)
        iop->offset += ret;

    rtems_semaphore_release(fs_info->vol_sema);
    return ret;
}

/* msdos_file_write --
 *     This routine writes the specified data buffer into the file pointed to
 *     by file control block.
 *
 * PARAMETERS:
 *     iop    - file control block
 *     buffer - data to write
 *     count  - count of bytes to write
 *
 * RETURNS:
 *     the number of bytes written on success, or -1 if error occured
 *     and errno set appropriately
 */
ssize_t
msdos_file_write(rtems_libio_t *iop,const void *buffer, size_t count)
{
    ssize_t            ret = 0;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info;
    fat_file_fd_t     *fat_fd = iop->pathinfo.node_access;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        rtems_set_errno_and_return_minus_one(EIO);

    if ((iop->flags & LIBIO_FLAGS_APPEND) != 0)
        iop->offset = fat_fd->fat_file_size;

    ret = fat_file_write(&fs_info->fat, fat_fd, iop->offset, count,
                         buffer);
    if (ret < 0)
    {
        rtems_semaphore_release(fs_info->vol_sema);
        return -1;
    }

    /*
     * update file size in both fat-file descriptor and file control block if
     * file was extended
     */
    iop->offset += ret;
    if (iop->offset > fat_fd->fat_file_size)
        fat_file_set_file_size(fat_fd, (uint32_t) iop->offset);

    if (ret > 0)
        fat_file_set_ctime_mtime(fat_fd, time(NULL));

    rtems_semaphore_release(fs_info->vol_sema);
    return ret;
}

/* msdos_file_stat --
 *
 * PARAMETERS:
 *     loc - node description
 *     buf - stat buffer provided by user
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set appropriately)
 */
int
msdos_file_stat(
    const rtems_filesystem_location_info_t *loc,
    struct stat *buf
)
{
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = loc->mt_entry->fs_info;
    fat_file_fd_t     *fat_fd = loc->node_access;
    uint32_t           cl_mask = fs_info->fat.vol.bpc - 1;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        rtems_set_errno_and_return_minus_one(EIO);

    buf->st_dev = rtems_disk_get_device_identifier(fs_info->fat.vol.dd);
    buf->st_ino = fat_fd->ino;
    buf->st_mode  = S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO;
    buf->st_rdev = 0ll;
    buf->st_size = fat_fd->fat_file_size;
    buf->st_blocks = ((fat_fd->fat_file_size + cl_mask) & ~cl_mask)
      >> FAT_SECTOR512_BITS;
    buf->st_blksize = fs_info->fat.vol.bpc;
    buf->st_atime = fat_fd->mtime;
    buf->st_ctime = fat_fd->ctime;
    buf->st_mtime = fat_fd->mtime;

    rtems_semaphore_release(fs_info->vol_sema);
    return RC_OK;
}

/* msdos_file_ftruncate --
 *     Truncate the file.
 *
 * PARAMETERS:
 *     iop    - file control block
 *     length - new length
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set appropriately).
 */
int
msdos_file_ftruncate(rtems_libio_t *iop, off_t length)
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info;
    fat_file_fd_t     *fat_fd = iop->pathinfo.node_access;
    uint32_t old_length;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        rtems_set_errno_and_return_minus_one(EIO);

    old_length = fat_fd->fat_file_size;
    if (length < old_length) {
        rc = fat_file_truncate(&fs_info->fat, fat_fd, length);
    } else {
        uint32_t new_length;

        rc = fat_file_extend(&fs_info->fat,
                             fat_fd,
                             true,
                             length,
                             &new_length);
        if (rc == RC_OK && length != new_length) {
            fat_file_truncate(&fs_info->fat, fat_fd, old_length);
            errno = ENOSPC;
            rc = -1;
        }
    }

    if (rc == RC_OK)
    {
        fat_file_set_file_size(fat_fd, length);
        fat_file_set_ctime_mtime(fat_fd, time(NULL));
    }

    rtems_semaphore_release(fs_info->vol_sema);

    return rc;
}

/* msdos_file_sync --
 *     Synchronize file - synchronize file data and if file is not removed
 *     synchronize file metadata.
 *
 * PARAMETERS:
 *     iop - file control block
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set appropriately)
 */
int
msdos_file_sync(rtems_libio_t *iop)
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info;
    fat_file_fd_t     *fat_fd = iop->pathinfo.node_access;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        rtems_set_errno_and_return_minus_one(EIO);

    rc = fat_file_update(&fs_info->fat, fat_fd);
    if (rc != RC_OK)
    {
        rtems_semaphore_release(fs_info->vol_sema);
        return rc;
    }

    rc = fat_sync(&fs_info->fat);

    rtems_semaphore_release(fs_info->vol_sema);
    if ( rc != 0 )
      rtems_set_errno_and_return_minus_one(EIO);

    return RC_OK;
}
