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

#ifndef __PROTO_H_INCLUDED
#define __PROTO_H_INCLUDED

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <hw/i2c.h>
#include <arm/omap.h>
#include <arm/omap3530.h>
#include "offsets.h"

typedef struct _omap_dev {
    unsigned            reglen;
    uintptr_t           regbase;
    unsigned            physbase;

	unsigned			re_start;
    int                 intr;
    int                 iid;
	int					fifo_size;
	int					chid;
	int					coid;
	int					xlen;
	uint8_t 			*buf;
	unsigned int		speed;
	volatile int		intexpected;
	volatile uint32_t	status;
    struct sigevent     intrevent;

    unsigned            own_addr;
    unsigned            slave_addr;
    unsigned            options;
    struct {
        unsigned char   major;
        unsigned char   minor;
    } rev;
} omap_dev_t;

#define OMAP_OPT_VERBOSE        0x00000002

#define OMAP_I2C_PCLK           12000000UL  /* peripheral clock */
#define OMAP_I2C_ICLK           12000000UL  /* internal clock */
#define OMAP_I2C_ADDRESS        1
#define OMAP_I2C_STATUS_TIMEDOUT 0xFFFF
#define OMAP_I2C_BUFSTAT		0x40
#define OMAP_I2C_IE_RDR			(1<<13)
#define OMAP_I2C_IE_XDR			(1<<14)
#define OMAP_I2C_STAT_RDR		(1<<13)
#define OMAP_I2C_STAT_XDR		(1<<14)
#define OMAP_I2C_BUF_RXFIF_CLR	(1<<14)
#define OMAP_I2C_BUF_TXFIF_CLR	(1<<6)
#define OMAP_I2C_IE_MASK		(OMAP_I2C_IE_AL |OMAP_I2C_IE_NACK |OMAP_I2C_IE_ARDY \
								|OMAP_I2C_IE_RRDY |OMAP_I2C_IE_XRDY \
								|OMAP_I2C_IE_RDR | OMAP_I2C_IE_XDR)

#define OMAP_I2C_EVENT			1
#define TWL4030_AUDIO_SLAVE_ADDRESS 0x49

void *omap_init(int argc, char *argv[]);
void omap_fini(void *hdl);
int omap_options(omap_dev_t *dev, int argc, char *argv[]);

int omap_set_slave_addr(void *hdl, unsigned int addr, i2c_addrfmt_t fmt);
int omap_set_bus_speed(void *hdl, unsigned int speed, unsigned int *ospeed);
int omap_version_info(i2c_libversion_t *version);
int omap_driver_info(void *hdl, i2c_driver_info_t *info);
int omap_devctl(void *hdl, int cmd, void *msg, int msglen, 
        int *nbytes, int *info);
i2c_status_t omap_recv(void *hdl, void *buf, 
        unsigned int len, unsigned int stop);
i2c_status_t omap_send(void *hdl, void *buf, 
        unsigned int len, unsigned int stop);

int omap_wait_bus_not_busy(omap_dev_t *dev,  unsigned int stop);
uint32_t omap_wait_status(omap_dev_t *dev);
const struct sigevent *i2c_intr(void *area, int id);
int omap_i2c_reset(omap_dev_t *dev);

#endif

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/i2c/omap35xx/proto.h $ $Rev: 308279 $" )
