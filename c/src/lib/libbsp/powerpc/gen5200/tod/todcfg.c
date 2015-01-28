/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.org/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file configures the pcf8563 RTC for a PM520 board          |
\*===============================================================*/
/*
 * This file contains the RTC driver table for Motorola MCF5206eLITE
 * ColdFire evaluation board.
 *
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include "../include/i2c.h"
#include <libchip/rtc.h>
#include "../tod/pcf8563.h"

/* Forward function declaration */
bool mpc5200_pcf8563_probe(int minor);

extern rtc_fns pcf8563_fns;

/* The following table configures the RTC drivers used in this BSP */
rtc_tbl RTC_Table[] = {
    {
        "/dev/rtc",                /* sDeviceName */
        RTC_CUSTOM,                /* deviceType */
        &pcf8563_fns,              /* pDeviceFns */
        mpc5200_pcf8563_probe,     /* deviceProbe */
        NULL,                      /* pDeviceParams */
        0x01,                      /* ulCtrlPort1, for PCF8563-I2C bus number */
        PCF8563_I2C_ADDRESS,       /* ulDataPort, for PCF8563-I2C device addr */
        NULL,                      /* getRegister - not applicable to PCF8563 */
        NULL                       /* setRegister - not applicable to PCF8563 */
    }
};

/* Some information used by the RTC driver */

#define NUM_RTCS (sizeof(RTC_Table)/sizeof(rtc_tbl))

size_t RTC_Count = NUM_RTCS;

rtems_device_minor_number RTC_Minor;

/* mpc5200_pcf8563_probe --
 *     RTC presence probe function. Return TRUE, if device is present.
 *     Device presence checked by probe access to RTC device over I2C bus.
 *
 * PARAMETERS:
 *     minor - minor RTC device number
 *
 * RETURNS:
 *     TRUE, if RTC device is present
 */
bool
mpc5200_pcf8563_probe(int minor)
{
    int try = 0;
    i2c_message_status status;
    rtc_tbl *rtc;
    i2c_bus_number bus;
    i2c_address addr;

    if (minor >= NUM_RTCS)
        return false;

    rtc = RTC_Table + minor;

    bus = rtc->ulCtrlPort1;
    addr = rtc->ulDataPort;
    do {
        status = i2c_wrbyte(bus, addr, 0);
        if (status == I2C_NO_DEVICE)
            return false;
        try++;
    } while ((try < 15) && (status != I2C_SUCCESSFUL));
    if (status == I2C_SUCCESSFUL)
        return true;
    else
        return false;
}
