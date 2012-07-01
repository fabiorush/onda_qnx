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

#include <stdio.h>
#include <devctl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <hw/i2c.h>
#include <sys/mman.h>


/*
 * TWL4030 address and registers
 * TWL4030 production product name is 
 * TPS65950 OMAP Power Management
 */


/*
 *  Device to direct API calls to
 */
#define TWL4030_I2C_DEVICE    "/dev/i2c0"

/* TWL4030 I2S clock speed */
#define TWL4030_I2C_SPEED     100000

#define TWL4030_ADDR_GRP1	0x4b

/* Voltage regulator: VMMC1 device group register (VRRTC domain) */
#define TWL4030_VMMC1_DEV_GRP	  		   0x82

/* Voltage regulator: VMMC1 basic settings */
#define TWL4030_VMMC1_DEDICATED			0x85


/*  Belongs to P1 device group */
#define TWL4030_DEV_GRP 0x20

/*
 * VSEL Select LDO output voltage. RW
 *          00 1.85
 *          01 2.85
 *          10 3.00
 *          11 3.15
 */
#define TWL4030_VSEL 0x02

/* i2c descriptor */
static int fd;
 
 /*
 * TWL4030 function definitions
 */
static int
twl4030_i2c_write (uint8_t addr, uint8_t reg, uint8_t val)
{
    iov_t           siov[3];
    i2c_send_t      hdr;

    hdr.slave.addr = addr;
    hdr.slave.fmt = I2C_ADDRFMT_7BIT;
    hdr.len = 2;
    hdr.stop = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &reg, sizeof(reg));
    SETIOV(&siov[2], &val, 1);

    if (devctlv(fd, DCMD_I2C_SEND, 3, 0, siov, NULL, NULL) != EOK) {

		return -1;
    }
	return 0;
}


int twl4030_setup_sdio()
{
	unsigned speed;
	int err;
	/*
	 * Open the I2C controller device for the codec
	 */
	fd = open(TWL4030_I2C_DEVICE, O_RDWR);
	if (fd == NULL) {
		perror("open");
		return -1;
	}
	
	/*
	 * Set the I2C speed
	 */
	speed = TWL4030_I2C_SPEED;
	err = devctl(fd, DCMD_I2C_SET_BUS_SPEED, &speed, sizeof(speed), NULL);
	if (err != EOK) {
		return -1;
	}
	
	/* power down to reset card */
	twl4030_i2c_write(TWL4030_ADDR_GRP1, TWL4030_VMMC1_DEV_GRP, 0);
	twl4030_i2c_write(TWL4030_ADDR_GRP1, TWL4030_VMMC1_DEDICATED, 0);
	delay(100);
	/* power up mmcsd 3.0v  */
	twl4030_i2c_write(TWL4030_ADDR_GRP1, TWL4030_VMMC1_DEV_GRP, TWL4030_DEV_GRP);
	twl4030_i2c_write(TWL4030_ADDR_GRP1, TWL4030_VMMC1_DEDICATED, TWL4030_VSEL);
	
	
	return 0;
}

__SRCVERSION("$URL$ $Rev$")
