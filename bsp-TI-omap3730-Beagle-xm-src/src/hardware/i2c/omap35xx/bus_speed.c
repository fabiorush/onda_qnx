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

#include "proto.h"

int
omap_set_bus_speed(void *hdl, unsigned int speed, unsigned int *ospeed)
{
    omap_dev_t      *dev = hdl;
    unsigned long   iclk;
    unsigned        scll;

    if (speed > 400000) {
        errno = EINVAL;
        return -1;
    }

	iclk = OMAP_I2C_ICLK;

	/* OMAP_I2C_PSC is already set */
    /* Set clock for "speed" bps */
	scll = iclk/(speed << 1) - 7;

    out16(dev->regbase + OMAP_I2C_SCLL, scll);
    out16(dev->regbase + OMAP_I2C_SCLH, scll + 2);

	if (ospeed)
        *ospeed = iclk / ((scll + 7) << 1);
	dev->speed = speed;
	//fprintf(stderr, "omap_i2c_set_bus_speed: speed=%d ospeed=%d iclk=%d scll=%d\n", speed, iclk / ((scll + 7) << 1), iclk, scll);

    return 0;
}

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/i2c/omap35xx/bus_speed.c $ $Rev: 308279 $" );
