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
#ifndef __TWL4030_REG_H_INCLUDED
#define  __TWL4030_REG_H_INCLUDED

/*
 * TWL4030 address and registers
 * TWL4030 production product name is 
 * TPS65950 OMAP Power Management
 */
#define TWL4030_ADDR_GRP1	0x4b
#define TWL4030_ADDR_GRP2	0x4a
#define TWL4030_ADDR_GRP3	0x48
#define TWL4030_ADDR_GRP4	0x49

/* Register offsets 
 *  Refer to TI TPS65950 OMAP Power Management
 */
#define TWL4030_LEDEN			0xEE
#define TWL4030_PWMAON			0xEF
#define TWL4030_PWMAOFF			0xF0
#define TWL4030_PWMBON			0xF1
#define TWL4030_PWMBOFF			0xF2

#define TWL4030_VAUX4_DEV_GRP		0x7E
#define TWL4030_VAUX4_DEDICATED		0x81
#define TWL4030_VPLL2_DEV_GRP		0x8E
#define TWL4030_VPLL2_DEDICATED		0x91

/* Values to set reisters to 
* TPS65950 TI OMAP Power Management 
 */
#define TWL4030_ENABLE_VAUX4_DEDICATED  0x05
#define TWL4030_ENABLE_VAUX4_DEV_GRP    0x20
#define TWL4030_ENABLE_VPLL2_DEDICATED  0x05
#define TWL4030_ENABLE_VPLL2_DEV_GRP    0xE0


/*
 *  Device to direct API calls to
 */
#define TWL4030_I2C_DEVICE		"/dev/i2c0"

/* TWL4030 I2S clock speed */
#define TWL4030_I2C_SPEED		100000

#endif

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/devg/omap35xx/twl4030_reg.h $ $Rev: 308279 $" )
