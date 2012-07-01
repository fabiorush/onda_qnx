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


int omap3530_wait(omap3530_spi_t *dev, int len)
{
	struct _pulse	 pulse;
	int rx_idx = dev->sdma_rx_chid;
	
	while (1) {
		if (len) {
			uint64_t	to = dev->dtime * 1000 * 50; /* 50 times for time out */
			to *= len ;	
			TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, NULL, &to, NULL);
		}
		if (MsgReceivePulse(dev->chid, &pulse, sizeof(pulse), NULL) == -1){
			fprintf(stderr, "omap3530_wait: errono %s(%d), status %x\n", strerror(errno), errno, in32(dev->vbase+ OMAP3530_MCSPI_IRQ_STATUS_OFFSET));
			return -1;
		}

		switch (pulse.code) {
			case OMAP3530_SPI_EVENT:
					return 0;
			case OMAP3530_SDMA_EVENT:
				{
					/* Unmask the Interrupt */
					InterruptUnmask(dev->irq_sdma+rx_idx, dev->iid_sdma);
					if ((dev->dma4->channel[rx_idx].csr & DMA4_CSR_FRAME)){
						/* clear interrupt status line 0 for transmit channel */
						dev->dma4->channel[rx_idx].csr |= DMA4_CSR_FRAME;
						return 0;
					}else {
						  continue;
					}
				}
		}
	}

	return 0;
}


__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/spi/omap3530/wait.c $ $Rev: 308279 $" );
