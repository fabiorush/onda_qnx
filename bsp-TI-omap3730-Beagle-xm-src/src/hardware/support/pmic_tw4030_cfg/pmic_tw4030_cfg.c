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


#ifdef __USAGE
%C - TW4030 power management chip configuration utility via I2C bus.

#endif



#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <hw/inout.h>
#include <sys/mman.h>
#include <sys/resmgr.h>
#include <sys/neutrino.h>
#include <sys/rsrcdbmgr.h>
#include <hw/sysinfo.h>
#include <hw/i2c.h>
#include <confname.h>

#include "twl4030_reg.h"

/* i2c descriptor */
static int fd;

////////////////////////////////////////////////////////////////////////////////
//                            PRIVATE FUNCTIONS                               //
////////////////////////////////////////////////////////////////////////////////

/* OMAP3530 Beagleboard write tw4030 */
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
		printf("twl4030_i2c_write fail");
		return -1;
    }
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//                                 MAIN                                       //
////////////////////////////////////////////////////////////////////////////////

#define MACHINE_NAME_MAX_LEN 256

int main( int argc, char *argv[] )
{

	unsigned speed;
	int err;
        char machine_name[MACHINE_NAME_MAX_LEN];

	/*
	 * Open the I2C controller device for the codec
	 */
	fd = open(TWL4030_I2C_DEVICE, O_RDWR);
	if (fd == NULL) {
		perror("open");
		return -1;
	}
	
	/*
	 * Set the I2C speed for the codec
	 */
	speed = TWL4030_I2C_SPEED;
	err = devctl(fd, DCMD_I2C_SET_BUS_SPEED, &speed, sizeof(speed), NULL);
	if (err != EOK) {
		return -1;
	}
	
	/* MMC SD power */
	twl4030_i2c_write( TWL4030_ADDR_GRP1, TWL4030_NRESPWRON_DEV_GRP, 0x20);
	twl4030_i2c_write( TWL4030_ADDR_GRP1, TWL4030_CLKEN_DEV_GRP, 0x2);

	/* Audio*/
	twl4030_i2c_write( TWL4030_ADDR_GRP1, 0x7A, 0x20);
	twl4030_i2c_write( TWL4030_ADDR_GRP1, 0x7D, 0x03);
	twl4030_i2c_write( TWL4030_ADDR_GRP1, 0x8E, 0xE0);
	twl4030_i2c_write( TWL4030_ADDR_GRP1, 0x91, 0x05);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x01, 0x03);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x02, 0xc0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x03, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x04, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x05, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x06, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x07, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x08, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x09, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x0a, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x0b, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x0c, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x0d, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x0e, 0x01);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x0f, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x10, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x11, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x12, 0x6c);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x13, 0x6c);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x14, 0x03);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x15, 0x03);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x16, 0x03);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x17, 0x0c);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x18, 0x03);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x19, 0x03);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x1a, 0x03);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x1b, 0x2b);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x1c, 0x2b);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x1d, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x1e, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x1f, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x20, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x21, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x22, 0x24);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x23, 0x0a);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x24, 0x42);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x25, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x26, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x27, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x28, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x29, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x2a, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x2b, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x2c, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x2d, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x2e, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x2f, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x30, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x31, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x32, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x33, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x34, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x35, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x36, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x37, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x38, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x39, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x3a, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x3b, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x3c, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x3d, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x3e, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x3f, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x40, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x41, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x42, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x43, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x44, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x45, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x46, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x47, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x48, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x49, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x4a, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x4b, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x4c, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x4d, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x4e, 0x0);
	twl4030_i2c_write( TWL4030_ADDR_GRP4, 0x4f, 0x0);

	/*misc_init_r*/
	twl4030_i2c_write( TWL4030_ADDR_GRP1, 0x7a, 0x20);
	twl4030_i2c_write( TWL4030_ADDR_GRP1, 0x7d, 0x03);
	twl4030_i2c_write( TWL4030_ADDR_GRP1, 0x8E, 0xe0);
	twl4030_i2c_write( TWL4030_ADDR_GRP1, 0x91, 0x05);
	twl4030_i2c_write( TWL4030_ADDR_GRP1, 0x96, 0x20);
	twl4030_i2c_write( TWL4030_ADDR_GRP1, 0x99, 0x03);


	// Enable LEDB to blink on the xM
	twl4030_i2c_write( TWL4030_ADDR_GRP2, TWL4030_LEDEN, 0x23);

	/* turn on LCD backlight */
    twl4030_i2c_write(TWL4030_ADDR_GRP2, TWL4030_PWMBON, 0x3F);
    twl4030_i2c_write(TWL4030_ADDR_GRP2, TWL4030_PWMBOFF, 0x7F);
    
    /* configure VPLL2 for graphics */
	twl4030_i2c_write(TWL4030_ADDR_GRP1, TWL4030_VAUX4_DEDICATED, TWL4030_ENABLE_VAUX4_DEDICATED);
	twl4030_i2c_write(TWL4030_ADDR_GRP1, TWL4030_VAUX4_DEV_GRP,   TWL4030_ENABLE_VAUX4_DEV_GRP);
	twl4030_i2c_write(TWL4030_ADDR_GRP1, TWL4030_VPLL2_DEDICATED, TWL4030_ENABLE_VPLL2_DEDICATED);
	twl4030_i2c_write(TWL4030_ADDR_GRP1, TWL4030_VPLL2_DEV_GRP,   TWL4030_ENABLE_VPLL2_DEV_GRP);

    /* check if we have beagle C4 */
    confstr(_CS_MACHINE, machine_name, MACHINE_NAME_MAX_LEN);
    if(!strcmp(machine_name, "OMAP3530_BEAGLE_Rev_c4"))
    {
      // enable VDD_EHCI on VAUX2
      twl4030_i2c_write(TWL4030_ADDR_GRP1, TWL4030_VAUX2_DEDICATED, TWL4030_ENABLE_VAUX2_DEDICATED);
      twl4030_i2c_write(TWL4030_ADDR_GRP1, TWL4030_VAUX2_DEV_GRP, TWL4030_ENABLE_VAUX2_DEV_GRP);
    }
    if(!strcmp(machine_name, "OMAP3730_BEAGLE_Rev_xm_b"))
    {
      // enable VDD_EHCI on VAUX2
      twl4030_i2c_write(TWL4030_ADDR_GRP1, TWL4030_VAUX2_DEDICATED, TWL4030_ENABLE_VAUX2_DEDICATED);
      twl4030_i2c_write(TWL4030_ADDR_GRP1, TWL4030_VAUX2_DEV_GRP, TWL4030_ENABLE_VAUX2_DEV_GRP);

      printf("xM enabling hub  \n");

      twl4030_i2c_write(TWL4030_ADDR_GRP2, TWL4030_PWMBON, 0x0);
      twl4030_i2c_write(TWL4030_ADDR_GRP2, TWL4030_PWMBOFF, 0x0);

      // We should really do a read/mody/write here not just a straight write
      twl4030_i2c_write(TWL4030_ADDR_GRP2, TWL4030_LEDEN, 0x32);

    }
    else if(!strcmp(machine_name, "OMAP3730_BEAGLE_Rev_xm_c"))
    {
          // enable VDD_EHCI on VAUX2
          twl4030_i2c_write(TWL4030_ADDR_GRP1, TWL4030_VAUX2_DEDICATED, TWL4030_ENABLE_VAUX2_DEDICATED);
          twl4030_i2c_write(TWL4030_ADDR_GRP1, TWL4030_VAUX2_DEV_GRP, TWL4030_ENABLE_VAUX2_DEV_GRP);

          printf("xM enabling hub  \n");

          twl4030_i2c_write(TWL4030_ADDR_GRP2, TWL4030_PWMBON, 0x0);
          twl4030_i2c_write(TWL4030_ADDR_GRP2, TWL4030_PWMBOFF, 0x0);

          // We should really do a read/mody/write here not just a straight write
          twl4030_i2c_write(TWL4030_ADDR_GRP2, TWL4030_LEDEN, 0x33);

    }
    else if (!strcmp(machine_name, "OMAP3530_BEAGLE_Rev_xx"))
    {
      fprintf(stderr,"Warning: unrecognized BEAGLE revision.\n");
      fprintf(stderr,"         VAUX2 is left at default and so EHCI may not work.\n");
    }
		
	close (fd);
	exit( EXIT_SUCCESS );
}


__SRCVERSION( "$URL$ $Rev$" );
