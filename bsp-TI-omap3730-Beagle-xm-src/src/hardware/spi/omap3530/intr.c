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


#include "omap3530spi.h"



/*
 * We use the same buffer for transmit and receive
 * For exchange, that's exactly what we wanted
 * For Read, it doesn't matter what we write to SPI, so we are OK.
 * For transmit, the receive data is put at the buffer we just transmitted, we are still OK.
 */

static const struct sigevent *spi_intr(void *area, int id)
{
	int channel_id = 0;
	uint32_t reg_value = 0;
	uintptr_t	base;
	int 		i, expected;
	omap3530_spi_t	*dev = area;
	base = dev->vbase;
	
	reg_value = in32(base + OMAP3530_MCSPI_IRQ_STATUS_OFFSET);

	/* The code below detects on which channel the interrupt
	 * has occured.
	 */
	if(reg_value & 0x0F)
		channel_id = OMAP3530_SPI_MASTER_CHANNEL_ONE;
	else {
		if(reg_value & 0x70)
			channel_id = OMAP3530_SPI_MASTER_CHANNEL_TWO;
		else
		{
			if(reg_value & 0x700)
				channel_id = OMAP3530_SPI_MASTER_CHANNEL_THREE;
			else 
				channel_id = OMAP3530_SPI_MASTER_CHANNEL_FOUR;
		}
	}
	if(channel_id>=NUM_OF_SPI_DEVS){
		out32(base + OMAP3530_MCSPI_IRQ_STATUS_OFFSET, reg_value);
		return NULL;
	}

	// Clear the interupt
	out32(base + OMAP3530_MCSPI_IRQ_STATUS_OFFSET, reg_value);

	// The remaining bytes are read from the omap3530_xfer function
	if (reg_value & INTR_TYPE_EOWKE)
		return (&dev->spievent);

#ifndef CPU_OMAP4430
	// Is Rx Full?
	if(reg_value & (INTR_TYPE_RX0_FULL << (channel_id * OMAP3530_INTERRUPT_BITS_PER_SPI_CHANNEL))) {
			expected = min(OMAP3530_SPI_FIFOLEN, dev->tlen - dev->rlen);
			// Read received block
			for(i = 0 ; i < expected; i++) {
					dev->pbuf[dev->rlen++] = in32(base + OMAP3530_MCSPI_CH1_RX_BUFFER_OFFSET + (OMAP3530_SPI_DEVICE_OFFSET * channel_id));
			}
	}

	// Is Tx empty?
	if(reg_value & (INTR_TYPE_TX0_EMPTY << (channel_id * OMAP3530_INTERRUPT_BITS_PER_SPI_CHANNEL))) { 
		expected = min(OMAP3530_SPI_FIFOLEN, dev->xlen - dev->tlen);
		if(expected) {
			// Start Tx again
			for(i = 0; i < expected; ++i) {
				out32(base + OMAP3530_MCSPI_CH1_TX_BUFFER_OFFSET + (OMAP3530_SPI_DEVICE_OFFSET * channel_id), dev->pbuf[dev->tlen++]);
			}			
		}
	}
#else
	// Is Rx Full?
	if(reg_value & (INTR_TYPE_RX0_FULL << (channel_id * OMAP3530_INTERRUPT_BITS_PER_SPI_CHANNEL))) {
			expected = min(dev->fifo_len, dev->tlen - dev->rlen);
			// Read received block
			for(i = 0 ; i < expected; i++) {
				dev->pbuf[dev->rlen++] = in32(base + OMAP3530_MCSPI_CH1_RX_BUFFER_OFFSET + (OMAP3530_SPI_DEVICE_OFFSET * channel_id));
			}
	}

	// Is Tx empty?
	if(reg_value & (INTR_TYPE_TX0_EMPTY << (channel_id * OMAP3530_INTERRUPT_BITS_PER_SPI_CHANNEL))) { 
		expected = min(dev->fifo_len, dev->xlen - dev->tlen);
		if(expected) {
			// Start Tx again
			for(i = 0; i < expected; ++i) {
				out32(base + OMAP3530_MCSPI_CH1_TX_BUFFER_OFFSET + (OMAP3530_SPI_DEVICE_OFFSET * channel_id), dev->pbuf[dev->tlen++]);
			}			
		}
	}
#endif
	return NULL;
}


int omap3530_attach_intr(omap3530_spi_t *omap3530)
{
	if ((omap3530->chid = ChannelCreate(_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK)) == -1)
		return -1;

	if ((omap3530->coid = ConnectAttach(0, 0, omap3530->chid, _NTO_SIDE_CHANNEL, 0)) == -1) 
		goto fail0;

	omap3530->spievent.sigev_notify   = SIGEV_PULSE;
	omap3530->spievent.sigev_coid     = omap3530->coid;
	omap3530->spievent.sigev_code     = OMAP3530_SPI_EVENT;
	omap3530->spievent.sigev_priority = OMAP3530_SPI_PRIORITY;

	/*
	 * Attach SPI interrupt
	 */
	omap3530->iid_spi = InterruptAttach(omap3530->irq_spi, spi_intr, omap3530, 0, _NTO_INTR_FLAGS_TRK_MSK);

	if (omap3530->iid_spi != -1)
		return 0;

	ConnectDetach(omap3530->coid);
fail0:
	ChannelDestroy(omap3530->chid);

	return -1;
}


int omap3530_sdma_attach_intr(omap3530_spi_t *omap3530)
{
	int rx_idx = omap3530->sdma_rx_chid ;

	if ((omap3530->sdma_coid = ConnectAttach(0, 0, omap3530->chid, _NTO_SIDE_CHANNEL, 0)) == -1) 
		goto fail0;
	
	omap3530->sdmaevent.sigev_notify   = SIGEV_PULSE;
	omap3530->sdmaevent.sigev_coid     = omap3530->sdma_coid;
	omap3530->sdmaevent.sigev_code     = OMAP3530_SDMA_EVENT;
	omap3530->sdmaevent.sigev_priority = OMAP3530_SPI_PRIORITY+1;

	omap3530->iid_sdma = InterruptAttachEvent(omap3530->irq_sdma+rx_idx, &omap3530->sdmaevent, _NTO_INTR_FLAGS_TRK_MSK);
	
	if (omap3530->iid_sdma != -1)
		return 0;

	ConnectDetach(omap3530->sdma_coid);
fail0:

	return -1;
}


__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/spi/omap3530/intr.c $ $Rev: 308279 $" );
