/**
 * @file
 *
 * @ingroup arm_gdbarmsim
 *
 * @brief Global BSP definitions.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _BSP_H
#define _BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup arm_gdbarmsim GDBARMSIM Support
 *
 * @ingroup bsp_arm
 *
 * @brief GDBARMSIM support package.
 *
 * @{
 */

//#define BSP_GET_WORK_AREA_DEBUG 1

/**
 * @brief Support for simulated clock tick
 */
Thread clock_driver_sim_idle_body(uintptr_t);
#define BSP_IDLE_TASK_BODY clock_driver_sim_idle_body

/*
 * Access to the GDB simulator.
 */
int     gdbarmsim_system(const char *);
int     gdbarmsim_rename(const char *, const char *);
int     gdbarmsim__isatty(int);
clock_t gdbarmsim_times(struct tms *);
int     gdbarmsim_gettimeofday(struct timeval *, void *);
int     gdbarmsim_unlink(const char *);
int     gdbarmsim_link(void);
int     gdbarmsim_stat(const char *, struct stat *);
int     gdbarmsim_fstat(int, struct stat *);
int	    gdbarmsim_swistat(int fd, struct stat * st);
int     gdbarmsim_close(int);
clock_t gdbarmsim_clock(void);
int     gdbarmsim_swiclose(int);
int     gdbarmsim_open(const char *, int, ...);
int     gdbarmsim_swiopen(const char *, int);
int     gdbarmsim_writec(const char c);
int     gdbarmsim_write(int, char *, int);
int     gdbarmsim_swiwrite(int, char *, int);
int     gdbarmsim_lseek(int, int, int);
int     gdbarmsim_swilseek(int, int, int);
int     gdbarmsim_read(int, char *, int);
int     gdbarmsim_swiread(int, char *, int);
void    initialise_monitor_handles(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _BSP_H */

