/*
 * $QNXLicenseC:
 * Copyright 2010, QNX Software Systems.
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
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resmgr.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <errno.h>
#include <sys/procmgr.h>
#include <drvr/hwinfo.h>
#include <arm/omap3530.h>
#include <fcntl.h>
#include <hw/i2c.h>

// Defines

/*
 * TPS659xx Watchdog driver
 *
 *  NOTE:  This driver assumes UNSECURE mode!
 */

/*
 * TPS659xx registers
 */
#define TPS659xx_CFG_P1_TRANS_REG   0x36
#define TPS659xx_CFG_P2_TRANS_REG   0x37
#define TPS659xx_CFG_P3_TRANS_REG   0x38
#define TPS659xx_PWR_PRT_KEY_REG    0x44    // Power registers PROTECT_KEY reg
#define TPS659xx_WDT_CFG_REG        0x5e

// Default prescaler value
#define TWL4030_MAX_TIMEOUT     30 // 30 seconds is max possible period
#define TWL4030_DEFAULT_TIMEOUT 20  // 20 seconds
#define TWL4030_DEFAULT_KICK        15 // 15 seconds

#define TPS659xx_STARTON_SWBUG_BIT  0x1<<7  // Enable power on bit for watchdog
#define TPS659xx_PWR_KEY1_VAL   0xc0
#define TPS659xx_PWR_KEY2_VAL   0x0c    // Writing key 1 then 2 enables write access
#define TPS659XX_I2C_DEVNAME    "/dev/i2c0"

#define TWL4030_ADDR_GRP1   0x4b

/* TWL4030 I2S clock speed */
#define TWL4030_I2C_SPEED     100000

// Global file descriptor
static int fd = -1;


/*************************************************************************
 *  tps659xx_i2c_write
 *************************************************************************/
static int tps659xx_i2c_write(uint8_t addr, uint8_t reg, uint8_t val)
{
    int error;
    iov_t siov[3];
    i2c_send_t hdr;

    hdr.slave.addr = addr;
    hdr.slave.fmt = I2C_ADDRFMT_7BIT;
    hdr.len = 2;
    hdr.stop = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &reg, sizeof(reg));
    SETIOV(&siov[2], &val, 1);

    //fprintf(stdout, "RTC, writing to %x, reg %x, data %x \n", addr, reg, val);

    error = devctlv( fd, DCMD_I2C_SEND, 3, 0, siov, NULL, NULL);
    if (error != EOK)
    {
        fprintf( stderr, "TWL4030 Watchdog: twl4030_i2c_write fail\n" );
        fprintf( stderr, "RTC, error means %s \n", strerror( error ) );
        return error;
    }

    return 0;
}

//-----------------------------------------------------
int twl4030_wdt_settimeout(int timeval)
{
    //fprintf( stdout, "TWL4030 Watchdog setting timeout, calc val = %x \n", timeval );

    // Set the value into the watchdog register
    if (tps659xx_i2c_write( TWL4030_ADDR_GRP1, TPS659xx_WDT_CFG_REG, timeval ) != EOK)
    {
        fprintf( stderr, "TWL4030 Watchdog: Unable to write seconds data to I2C device\n" );
        return -1;
    }

    return EOK;
}

//-----------------------------------------------------
int twl4030_wdt_setreset(void)
{
    // Sets the correct power reset bits and access to PM regsiters on the TPS659xx

    // Set Key 1 to enable write access
    if (tps659xx_i2c_write( TWL4030_ADDR_GRP1, TPS659xx_PWR_PRT_KEY_REG, TPS659xx_PWR_KEY1_VAL)
            != EOK)
    {
        fprintf( stderr, "TWL4030 Watchdog: Unable to write seconds data to I2C device\n" );
        return -1;
    }
    // Set Key 2 to enable write access
    if (tps659xx_i2c_write( TWL4030_ADDR_GRP1, TPS659xx_PWR_PRT_KEY_REG, TPS659xx_PWR_KEY2_VAL)
            != EOK)
    {
        fprintf( stderr, "TWL4030 Watchdog: Unable to write seconds data to I2C device\n" );
        return -1;
    }

    // PM registers of the TPS659xx should now be write-able.
    // Set P1 transition to enable reset on watchdog
    if (tps659xx_i2c_write( TWL4030_ADDR_GRP1, TPS659xx_CFG_P1_TRANS_REG,
            TPS659xx_STARTON_SWBUG_BIT) != EOK)
    {
        fprintf( stderr, "TWL4030 Watchdog: Unable to write seconds data to I2C device\n" );
        return -1;
    }
    // Set P2 transition to enable reset on watchdog
    if (tps659xx_i2c_write( TWL4030_ADDR_GRP1, TPS659xx_CFG_P2_TRANS_REG,
            TPS659xx_STARTON_SWBUG_BIT) != EOK)
    {
        fprintf( stderr, "TWL4030 Watchdog: Unable to write seconds data to I2C device\n" );
        return -1;
    }
    // Set P3 transition to enable reset on watchdog
    if (tps659xx_i2c_write( TWL4030_ADDR_GRP1, TPS659xx_CFG_P3_TRANS_REG,
            TPS659xx_STARTON_SWBUG_BIT) != EOK)
    {
        fprintf( stderr, "TWL4030 Watchdog: Unable to write seconds data to I2C device\n" );
        return -1;
    }

    return EOK;
}

//-----------------------------------------------------
int main(int argc, char *argv[])
{
    int opt, curr;
    int priority = 10; /*default priority:default 10 */
    long long kicktime = -1; /*default time for watchdog timer kick*/
    long long timeout = -1;
    unsigned speed;

    fprintf( stdout, "OMAP3530 TWL4030 Watchdog Init \n" );

    /* Process dash options.*/
    while ((opt = getopt( argc, argv, "p:t:k:" )) != -1)
    {
        switch (opt) {
            case 'p': // priority
                priority = strtoul( optarg, NULL, 0 );
                break;
            case 't': // watchdog timeout in milliseconds
                timeout = strtoull( optarg, NULL, 0 );
                break;
            case 'k': // kick time period in milliseconds
                kicktime = strtoull( optarg, NULL, 0 );
                break;
        }
    }

    /*check if the params are valid*/
    if (kicktime == -1)
    {
        slogf( _SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                "wdtkick error : Invalid default time for watchdog timer kick. Please check the command line." );
        return EXIT_FAILURE;
    }

    // Enable IO capability.
    if (ThreadCtl( _NTO_TCTL_IO, NULL ) == -1)
    {
        slogf( _SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "netio:  ThreadCtl" );
        return EXIT_FAILURE;
    }

    //run in the background
    if (procmgr_daemon( EXIT_SUCCESS, PROCMGR_DAEMON_NOCLOSE | PROCMGR_DAEMON_NODEVNULL ) == -1)
    {
        slogf( _SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "%s:  procmgr_daemon", argv[0] );
        return EXIT_FAILURE;
    }

    // If requested: Change priority.
    curr = getprio( 0 );
    if (priority != curr && setprio( 0, priority ) == -1)
        slogf( _SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "WDT:  can't change priority" );

    // No timeout specified; use default
    if (timeout == -1)
    {
        fprintf( stdout, "TWL4030 Watchdog No timeout specified \n" );
        timeout = TWL4030_DEFAULT_TIMEOUT;
    }
    else if (timeout > TWL4030_MAX_TIMEOUT)
    {
        // TPS659xx max timeout value is 30 seconds
        fprintf( stdout, "TWL4030 Watchdog:  Timeout requested exceeds max of 30s.  Setting to 30s. \n" );
                timeout = TWL4030_MAX_TIMEOUT;
    }

    fd = open( TPS659XX_I2C_DEVNAME, O_RDWR);
    if (fd < 0)
    {
        fprintf( stderr, "TWL4030 Watchdog:  Unable to open I2C device\n" );
        return -1;
    }

    /*
     * Set the I2C speed
     */
    speed = TWL4030_I2C_SPEED;
    if (devctl( fd, DCMD_I2C_SET_BUS_SPEED, &speed, sizeof(speed), NULL) != EOK)
    {
        fprintf( stderr, "TWL4030 Watchdog:  Unable to set I2C speed\n" );
        return -1;
    }

    // Setup the power reset and enable write-access to PM registers on the TPS659xx
    twl4030_wdt_setreset();

    // Now program the value (which starts the clock)
    twl4030_wdt_settimeout( timeout );

    //kick the watchdog timer with kicktime interval
    while (1)
    {
        // this "kicks or clears" the watchdog

        //fprintf( stdout, "TWL4030 Watchdog kicking timer \n" );
        // Just reload the timeout value
        twl4030_wdt_settimeout( timeout );
        // Delay is in MS while TPS659xx register is in seconds (as are args)
        delay( (kicktime * 1000) );
    }

    return EXIT_SUCCESS;
}
