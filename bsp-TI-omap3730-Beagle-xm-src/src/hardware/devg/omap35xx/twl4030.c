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

#include <stdlib.h>
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

#include "omap.h"
#include "twl4030_reg.h"

/* i2c descriptor */
static int fd;
 
 /*
 * TWL4030 function definitions
 */
static int
twl4030_i2c_write (disp_adapter_t *adapter, uint8_t addr, uint8_t reg, uint8_t val)
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
		disp_printf(adapter, "twl4030_i2c_write fail");
		return -1;
    }
	return 0;
}

static int twl4030_i2c_read(uint8_t addr, uint8_t reg, uint8_t * val)
{
	struct send_recv
	{
		i2c_sendrecv_t hdr;
		uint8_t buf[2];
	} twl4030_rd_data;

	/*Read the Registers Current Value */
	twl4030_rd_data.buf[0] = reg;
	twl4030_rd_data.hdr.send_len = 1;
	twl4030_rd_data.hdr.recv_len = 1;

	twl4030_rd_data.hdr.slave.addr = addr;
	twl4030_rd_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	twl4030_rd_data.hdr.stop = 1;

	if (devctl(fd, DCMD_I2C_SENDRECV, &twl4030_rd_data, sizeof(twl4030_rd_data), NULL))
	{
		slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "BEAGLE MMCSD: twl4030_i2c_read fail");
		return -1;
	}

	*val = twl4030_rd_data.buf[0];

	return 0;
}


int twl4030_enable_dvi(disp_adapter_t *adapter)
{
	unsigned speed;
	unsigned char b;
	int err;
	/*
	 * Open the I2C controller device for the codec
	 */
	fd = open(TWL4030_I2C_DEVICE, O_RDWR);
	if (fd == NULL) {
		perror("open");
		return -1;
	}

	printf("\n Enable DVI output on BeagleBoard-xM\n");
	slogf(21,0,"Enable DVI output on BeagleBoard-xM\n" );

	/*
	 * Set the I2C speed for the codec
	 */
	speed = TWL4030_I2C_SPEED;
	err = devctl(fd, DCMD_I2C_SET_BUS_SPEED, &speed, sizeof(speed), NULL);
	if (err != EOK) {
		return -1;
	}

	/* Set GPOI2 (DVI_PU) high, to power on the DVI */

	// First set the direction. Read in the existing direction value
	if (twl4030_i2c_read(TWL4030_ADDR_GRP4, 0x9B, &b))
		return -1; // bail if the access fails
	b |= (1 << 2);
	twl4030_i2c_write(adapter, TWL4030_ADDR_GRP4, 0x9B, b);
	
	// Now set the value high
	twl4030_i2c_write(adapter, TWL4030_ADDR_GRP4, 0xA4, (unsigned char)(1<<2));

	return 0;
}


int twl4030_setup_graphics(disp_adapter_t *adapter)
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
	
	printf("\n Setting up graphics \n");
	slogf(21,0,"setting up graphics \n" );

	/*
	 * Set the I2C speed for the codec
	 */
	speed = TWL4030_I2C_SPEED;
	err = devctl(fd, DCMD_I2C_SET_BUS_SPEED, &speed, sizeof(speed), NULL);
	if (err != EOK) {
		return -1;
	}
	
	/* turn on LCD backlight */
	//twl4030_i2c_write(adapter, TWL4030_ADDR_GRP2, TWL4030_LEDEN, 0x33);
	//twl4030_i2c_write(adapter, TWL4030_ADDR_GRP2, TWL4030_LEDEN, 0x32);
	twl4030_i2c_write(adapter, TWL4030_ADDR_GRP2, TWL4030_PWMAON, 0x7F);
    twl4030_i2c_write(adapter, TWL4030_ADDR_GRP2, TWL4030_PWMAOFF, 0x7F);	
    twl4030_i2c_write(adapter, TWL4030_ADDR_GRP2, TWL4030_PWMBON, 0x7F);
    twl4030_i2c_write(adapter, TWL4030_ADDR_GRP2, TWL4030_PWMBOFF, 0x7F);
	
	/* configure VPLL2 for graphics */
	twl4030_i2c_write(adapter, TWL4030_ADDR_GRP1, TWL4030_VAUX4_DEDICATED, TWL4030_ENABLE_VAUX4_DEDICATED);
	twl4030_i2c_write(adapter, TWL4030_ADDR_GRP1, TWL4030_VAUX4_DEV_GRP,   TWL4030_ENABLE_VAUX4_DEV_GRP);
	twl4030_i2c_write(adapter, TWL4030_ADDR_GRP1, TWL4030_VPLL2_DEDICATED, TWL4030_ENABLE_VPLL2_DEDICATED);
	twl4030_i2c_write(adapter, TWL4030_ADDR_GRP1, TWL4030_VPLL2_DEV_GRP,   TWL4030_ENABLE_VPLL2_DEV_GRP);
	
	
	
	return 0;
}



__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/devg/omap35xx/twl4030.c $ $Rev: 308279 $" );
