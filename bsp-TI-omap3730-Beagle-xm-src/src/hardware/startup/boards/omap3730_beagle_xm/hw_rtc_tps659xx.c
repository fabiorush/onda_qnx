/*
 * $QNXLicenseC:
 * Copyright 2009, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

/*
 * Support RTC (TPS659xx)
 */

#include "startup.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <hw/i2c.h>
#include <arm/omap.h>
#include <arm/omap3530.h>

/*
 * TPS659xx register offsets
 */

/*
 * TPS659xx registers
 */
#define TPS659xx_SECONDS_REG        0x1c
#define TPS659xx_MINUTES_REG        0x1d
#define TPS659xx_HOURS_REG          0x1e
#define TPS659xx_DAYS_REG           0x1f
#define TPS659xx_MONTHS_REG         0x20
#define TPS659xx_YEARS_REG          0x21
#define TPS659xx_WEEKS_REG          0x22
#define TPS659xx_ALRM_SECS_REG      0x23
#define TPS659xx_ALRM_MINS_REG      0x24
#define TPS659xx_ALRM_HRS_REG       0x25
#define TPS659xx_ALRM_DAYS_REG      0x26
#define TPS659xx_ALRM_MNTHS_REG     0x27
#define TPS659xx_ALRM_YRS_REG       0x28
#define TPS659xx_RTC_CTRL_REG       0x29
#define TPS659xx_RTC_STATUS_REG     0x2a
#define TPS659xx_RTC_INTS_REG       0x2b
#define TPS659xx_RTC_COMP_LSB_REG   0x2c
#define TPS659xx_RTC_COMP_MSB_REG   0x2d

/*
 * TPS659xx register offsets
 */
#define TPS659xx_SECONDS_REG_OFFSET         0x00
#define TPS659xx_MINUTES_REG_OFFSET         0x01
#define TPS659xx_HOURS_REG_OFFSET           0x02
#define TPS659xx_DAYS_REG_OFFSET            0x03
#define TPS659xx_MONTHS_REG_OFFSET          0x04
#define TPS659xx_YEARS_REG_OFFSET           0x05
#define TPS659xx_WEEKS_REG_OFFSET           0x06
#define TPS659xx_ALRM_SECS_REG_OFFSET       0x07
#define TPS659xx_ALRM_MINS_REG_OFFSET       0x08
#define TPS659xx_ALRM_HRS_REG_OFFSET        0x09
#define TPS659xx_ALRM_DAYS_REG_OFFSET       0x0a
#define TPS659xx_ALRM_MNTHS_REG_OFFSET      0x0b
#define TPS659xx_ALRM_YRS_REG_OFFSET        0x0c
#define TPS659xx_RTC_CTRL_REG_OFFSET        0x0d
#define TPS659xx_RTC_STATUS_REG_OFFSET      0x0e
#define TPS659xx_RTC_INTS_REG_OFFSET        0x0f
#define TPS659xx_RTC_COMP_LSB_REG_OFFSET    0x10
#define TPS659xx_RTC_COMP_MSB_REG_OFFSET    0x11

/*  AM/PM bit in status register */
#define TPS659xx_RTD_PM		    0x08
#define TPS659xx_RESET_EVENT    0x80

#define TPS659xx_I2C_ADDRESS        0x4b   
#define TPS659XX_I2C_DEVNAME        "/dev/i2c0"
#define TPS659xx_CTRL_REG_ADDR      (TPS659xx_I2C_ADDRESS + TPS659xx_RTC_CTRL_REG)
#define TPS659xx_STATUS_REG_ADDR    (TPS659xx_I2C_ADDRESS + TPS659xx_RTC_STATUS_REG)
#define TPS659xx_I2C_SEC_REG_ADDR   (TPS659xx_I2C_ADDRESS + TPS659xx_SECONDS_REG)

#define OMAP_I2C_PCLK               12000000UL  // peripheral clock
#define OMAP_I2C_ICLK               12000000UL  // internal clock
#define OMAP_I2C_ADDRESS            1

#define OMAP_OPT_VERBOSE            0x00000002

#define OMAP_I2C_STAT_RDR           (1<<13)
#define OMAP_I2C_STAT_XDR           (1<<14)

#define OMAP_I2C_STAT_MASK        \
    (OMAP_I2C_STAT_BB   | \
    OMAP_I2C_STAT_RDR  | \
    OMAP_I2C_STAT_XDR  | \
    OMAP_I2C_STAT_ROVR | \
    OMAP_I2C_STAT_AAS  | \
    OMAP_I2C_STAT_GC   | \
    OMAP_I2C_STAT_XRDY | \
    OMAP_I2C_STAT_RRDY | \
    OMAP_I2C_STAT_ARDY | \
    OMAP_I2C_STAT_NACK | \
    OMAP_I2C_STAT_AL)

typedef struct _omap_dev
{
    void *buf;
    unsigned buflen;
    unsigned reglen;
    uintptr_t regbase;
    unsigned physbase;
    unsigned re_start;
    unsigned own_addr;
    unsigned slave_addr;
    unsigned options;

} omap_dev_t;

unsigned long rtc_time_tps659xx(void)
{
    struct tm tm;

    /* add the TPS659xx RTC device in syspage */
    hwi_add_device(HWI_ITEM_BUS_UNKNOWN, HWI_ITEM_DEVCLASS_RTC, "tps659xx", 0);

    // Now just set a default value into the system clock since the xM doesn't have
    // a battery backed-RTC anyway.
    tm.tm_sec = 0x1;
    tm.tm_min = 0x1;
    tm.tm_hour = 0x1;
    tm.tm_mday = 0x1;
    tm.tm_mon = 0x1;
    tm.tm_year = 0x99;

    kprintf("RTC_Time_TPS659xx: Returning default RTC value\n");

    return (calc_time_t(&tm));
}

__SRCVERSION( "$URL$ $Rev$" )
