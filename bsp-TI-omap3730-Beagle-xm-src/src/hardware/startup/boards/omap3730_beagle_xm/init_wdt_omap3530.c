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
 * Support OMAP3530 Watchdog timer
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
#include <drvr/hwinfo.h>                // for hwi support routines in libdrvr
#include <hwinfo_private.h>


void init_wdt_omap3530(void)
{
    unsigned hwi_off;
    hwiattr_timer_t attr = HWIATTR_TIMER_T_INITIALIZER;
    const struct hwi_inputclk clksrc_kick = { .clk = 10, .div = 1 };

    /* add the WATCHDOG device */
    kprintf("Adding OMAP3530 watchdog timer\n");

    HWIATTR_TIMER_SET_NUM_CLK(&attr, 1);

    HWIATTR_TIMER_SET_LOCATION(&attr, OMAP3530_WDT2_BASE, OMAP3530_WDT_SIZE, 0, hwi_find_as(
            OMAP3530_WDT2_BASE, 1));

    hwi_off = hwidev_add_timer("wdog", &attr, HWI_NULL_OFF);

    ASSERT(hwi_off != HWI_NULL_OFF);

    hwitag_set_inputclk(hwi_off, 0, (struct hwi_inputclk *) &clksrc_kick);

}

__SRCVERSION( "$URL$ $Rev$" )
