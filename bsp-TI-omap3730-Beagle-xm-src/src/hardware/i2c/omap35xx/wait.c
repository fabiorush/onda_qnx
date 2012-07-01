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

#define OMAP_I2C_STAT_MASK \
	       (OMAP_I2C_STAT_BB | \
	       OMAP_I2C_STAT_RDR | \
	       OMAP_I2C_STAT_XDR | \
             OMAP_I2C_STAT_ROVR | \
             OMAP_I2C_STAT_AAS | \
             OMAP_I2C_STAT_GC | \
             OMAP_I2C_STAT_XRDY | \
             OMAP_I2C_STAT_RRDY | \
             OMAP_I2C_STAT_ARDY | \
             OMAP_I2C_STAT_NACK | \
             OMAP_I2C_STAT_AL)


int
omap_wait_bus_not_busy(omap_dev_t *dev, unsigned int stop)
{
    unsigned        tries = 1000000;

	if(dev->re_start) {
		if (stop){
			dev->re_start = 0;
		}
		return 0;
	}else {
		while (in16(dev->regbase + OMAP_I2C_STAT) & OMAP_I2C_STAT_BB){
			if (tries-- == 0){
				/* reset the controller to see if it's able to recover*/
				omap_i2c_reset(dev);
				//try again to see if it's OK now after reset
				if(in16(dev->regbase + OMAP_I2C_STAT) & OMAP_I2C_STAT_BB){
					delay(1);
					return -1;
				}else{
					break;
				}
			}
		}
		if(!stop) {
			dev->re_start = 1;
		}
		return 0;
	}

	return -1;
}

const struct sigevent *i2c_intr(void *area, int id)
{
	omap_dev_t *dev = area;
	int count = 0;
	uint16_t	stat; 
	
	while ((stat = in16(dev->regbase + OMAP_I2C_STAT)) & OMAP_I2C_STAT_MASK) {
		if (count++ == 100) {
			//Too much work in one IRQ
			break;
		}
		//clear the status
		out16(dev->regbase + OMAP_I2C_STAT, stat);

		if (stat & OMAP_I2C_STAT_NACK) {
			dev->status |= I2C_STATUS_NACK ;
			out16(dev->regbase + OMAP_I2C_CON, 
				   in16(dev->regbase + OMAP_I2C_CON) | OMAP_I2C_CON_STP);
		}
		if (stat & OMAP_I2C_STAT_AL) {
			dev->status |= I2C_STATUS_ARBL;
		}
		if ((stat & (OMAP_I2C_STAT_ARDY | OMAP_I2C_STAT_NACK |
					OMAP_I2C_STAT_AL)) && dev->intexpected){
			dev->status |= I2C_STATUS_DONE;
			dev->intexpected = 0;
			return &dev->intrevent;
		}
		if (stat & (OMAP_I2C_STAT_RRDY | OMAP_I2C_STAT_RDR)) {
			int num_bytes = 1;
			uint16_t tmp;
			if (dev->fifo_size) {
				if (stat & OMAP_I2C_STAT_RRDY)
					num_bytes = dev->fifo_size;
				else
					num_bytes = in16(dev->regbase+OMAP_I2C_BUFSTAT);
			}
			while (num_bytes) {
				num_bytes--;
				tmp = in16(dev->regbase + OMAP_I2C_DATA);
				if (dev->xlen) {
					*dev->buf++ = (uint8_t)(tmp & 0x00FF);;
					dev->xlen--;
				} else {
					break;
				}
			}
			out16(dev->regbase + OMAP_I2C_STAT,
				stat & (OMAP_I2C_STAT_RRDY | OMAP_I2C_STAT_RDR));
			continue;
		}
		if (stat & (OMAP_I2C_STAT_XRDY | OMAP_I2C_STAT_XDR)) {
			int num_bytes = 1;
			if (dev->fifo_size) {
				if (stat & OMAP_I2C_STAT_XRDY)
					num_bytes = dev->fifo_size;
				else
					num_bytes = in16(dev->regbase+OMAP_I2C_BUFSTAT);
			}
			while (num_bytes) {
				num_bytes--;
				if (dev->xlen) {
					out8(dev->regbase + OMAP_I2C_DATA, *dev->buf++);
					dev->xlen--;
				} else {
					break;
				}
			}
			out16(dev->regbase + OMAP_I2C_STAT,
				stat & (OMAP_I2C_STAT_XRDY | OMAP_I2C_STAT_XDR));
			continue;
		}
		if (stat & (OMAP_I2C_STAT_ROVR|OMAP_I2C_STAT_XUDF)) {
			dev->status |= I2C_STATUS_ERROR;
			out16(dev->regbase + OMAP_I2C_CON, 
				   in16(dev->regbase + OMAP_I2C_CON) | OMAP_I2C_CON_STP);
			break;
		}
	}

	if(dev->status && dev->intexpected){
		dev->status |= I2C_STATUS_DONE;
		dev->intexpected = 0;
		return &dev->intrevent;
	}
		
	return NULL;
}


uint32_t
omap_wait_status(omap_dev_t *dev)
{
	struct _pulse	pulse;
	uint64_t        ntime = 1e9;
	
	while (1) {
		ntime += dev->xlen * 1000 * 50;
		TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, NULL, &ntime, NULL);
		if (MsgReceivePulse(dev->chid, &pulse, sizeof(pulse), NULL) == -1){
			fprintf(stderr, "omap_i2c_wait_status: errono %s(%d), dev->status %x, stat reg %x\n", 
				strerror(errno), errno, dev->status, in16(dev->regbase + OMAP_I2C_STAT));
			omap_i2c_reset(dev);
			return (I2C_STATUS_DONE | I2C_STATUS_ERROR);
		}
		switch (pulse.code) {
			case OMAP_I2C_EVENT:
			{
				if(dev->status & I2C_STATUS_ARBL){
					omap_i2c_reset(dev);
				}
				return (dev->status);
			}
		}
	}

	return (I2C_STATUS_DONE | I2C_STATUS_ERROR);
}


__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/i2c/omap35xx/wait.c $ $Rev: 308279 $" );
