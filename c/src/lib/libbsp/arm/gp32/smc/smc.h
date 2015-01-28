/**
 * @file
 * @ingroup gp32_smc
 * @brief SMC disk driver initialization entry point
 */

#ifndef __SMC_H__
#define __SMC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>

#include "rtems/blkdev.h"

/**
 * @defgroup gp32_smc SMC Disk Driver
 * @ingroup arm_gp32
 * @brief SMC Disk Driver Support
 * @{
 */

/**
 * @brief smc_initialize
 *     SMC disk driver initialization entry point.
 */
rtems_device_driver
smc_initialize(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg);

#define SMC_DRIVER_TABLE_ENTRY \
    { smc_initialize, GENERIC_BLOCK_DEVICE_DRIVER_ENTRIES }

/** @} */

#ifdef __cplusplus
}
#endif

#endif
