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

void omap3530_sdma_disablespi(omap3530_spi_t *omap3530)
{
	int     tx_idx = omap3530->sdma_tx_chid ;
	int     rx_idx = omap3530->sdma_rx_chid;
	
	omap3530->dma4->channel[tx_idx].ccr &= ~DMA4_CCR_ENABLE;
	omap3530->dma4->channel[rx_idx].ccr &= ~DMA4_CCR_ENABLE;
	
	/*Disable interrupt */
	omap3530->dma4->channel[tx_idx].cicr = 0;
	omap3530->dma4->channel[rx_idx].cicr = 0;
	
	/* Clear previous status of the channel */
	omap3530->dma4->channel[tx_idx].csr =DMA4_CSR_MSK;
	omap3530->dma4->channel[rx_idx].csr =DMA4_CSR_MSK;
	
}

int omap3530_setup_sdma(omap3530_spi_t *omap3530, int device, spi_dma_paddr_t *paddr, int len )
{
	int     tx_idx = omap3530->sdma_tx_chid ;
	int     rx_idx = omap3530->sdma_rx_chid;

	/********************
	 * Setup Tx channel *
	 ********************/
	/* Config DMA Channel for SPI transmit DMA request */
	omap3530->dma4->channel[tx_idx].ccr = ((omap3530->spi_tx_dma_req + 1) & 0x1f) 
		| ((((omap3530->spi_tx_dma_req +1) & 0x60) << 14)
//		| DMA4_CCR_FS
//		| DMA4_CCR_BS /* Packet transfer Mode*/
		| DMA4_CCR_SRC_POST_INCR
		| DMA4_CCR_DST_CONSTANT);

	omap3530->dma4->channel[tx_idx].cen = len; /* number of elements per frame (EN) */
	omap3530->dma4->channel[tx_idx].cfn = 1;  /*number of frames (FN) per block*/

	/* Set the Physical Address for the source-side of the DMA channel */
	if (paddr->wpaddr)
		omap3530->dma4->channel[tx_idx].cssa = (uint32_t)paddr->wpaddr;
	else
		omap3530->dma4->channel[tx_idx].cssa =  omap3530->pdmabuf;

	/* Set the Physical Address for the destination-side of the DMA channel */
	omap3530->dma4->channel[tx_idx].cdsa = omap3530->pbase+OMAP3530_MCSPI_CH1_TX_BUFFER_OFFSET + (0x14 * device) ; 

	/* Enable end of frame/fragment interrupt */
	omap3530->dma4->channel[tx_idx].cicr = DMA4_CICR_FRAME_IE;

	/********************
	 * Setup Rx channel *
	 ********************/
	/* Config DMA Channel for SPI receive DMA request */
	omap3530->dma4->channel[rx_idx].ccr = ((omap3530->spi_rx_dma_req + 1) & 0x1f) 
			| ((((omap3530->spi_rx_dma_req +1) & 0x60) << 14)
//			| DMA4_CCR_FS 
//			| DMA4_CCR_BS 
			| DMA4_CCR_SEL_SRC_DST_SYNC 
			| DMA4_CCR_SRC_CONSTANT 
			| DMA4_CCR_DST_POST_INCR);

	omap3530->dma4->channel[rx_idx].cen = len;	/* number of elements per frame (EN) */
	omap3530->dma4->channel[rx_idx].cfn = 1; /*number of frames (FN) per block*/
	
	/* Set the Physical Address for the source-side of the DMA channel */
	omap3530->dma4->channel[rx_idx].cssa = omap3530->pbase+OMAP3530_MCSPI_CH1_RX_BUFFER_OFFSET + (0x14 * device) ;

	/* Set the Physical Address for the destination-side of the DMA channel */
	if (paddr->rpaddr) 
		omap3530->dma4->channel[rx_idx].cdsa = (uint32_t)paddr->rpaddr;
	else 
		omap3530->dma4->channel[rx_idx].cdsa = omap3530->pdmabuf;

	/* Enable end of frame/fragment interrupt */
	omap3530->dma4->channel[rx_idx].cicr = DMA4_CICR_FRAME_IE;

	/* Enable SDMA */
	omap3530->dma4->channel[tx_idx].ccr |= DMA4_CCR_ENABLE;
	omap3530->dma4->channel[rx_idx].ccr |= DMA4_CCR_ENABLE;

	return 0;
}


void
omap3530_sdma_detach(omap3530_spi_t *omap3530)
{
	rsrc_request_t req = { 0 };

	if (omap3530->sdma_tx_chid != -1)
	{
		req.length = 1;
		req.start = omap3530->sdma_tx_chid;
		req.flags = RSRCDBMGR_DMA_CHANNEL;
		rsrcdbmgr_detach (&req, 1);
	}
	if (omap3530->sdma_rx_chid != -1)
	{
		memset (&req, 0, sizeof (req));
		req.length = 1;
		req.start = omap3530->sdma_rx_chid;
		req.flags = RSRCDBMGR_DMA_CHANNEL;
		rsrcdbmgr_detach (&req, 1);
	}
	munmap_device_memory (omap3530->dma4, sizeof (dma4_t));

}

int32_t
omap3530_sdma_attach(omap3530_spi_t *omap3530)
{
	int     tx_idx = omap3530->sdma_tx_chid = -1;
	int     rx_idx = omap3530->sdma_rx_chid = -1;
	
	rsrc_request_t req = { 0 };

	/* Map in DMA registers */
	if ((omap3530->dma4 = mmap_device_memory (0, sizeof (dma4_t),
				PROT_READ | PROT_WRITE | PROT_NOCACHE, 0, DMA4_BASE_ADDR)) == MAP_FAILED)
	{
		fprintf(stderr, "Unable to mmap DMA (%s) \n", strerror (errno));
		return (-1);
	}

	/*
	*********** SPI: transmit ****************
	*/
	req.length = 1;
	req.flags = RSRCDBMGR_DMA_CHANNEL;
	if (rsrcdbmgr_attach (&req, 1) == -1)
	{
			fprintf(stderr, "Unable to aquire DMA channels (%s) \n", strerror (errno));
			munmap_device_memory (omap3530->dma4, sizeof (dma4_t));
			return (-1);
	}
	 tx_idx = omap3530->sdma_tx_chid  = req.start;
	
	/*
	*********** SPI: receive ****************
	*/
	memset (&req, 0, sizeof (req));
	req.length = 1;
	req.flags = RSRCDBMGR_DMA_CHANNEL;
	if (rsrcdbmgr_attach (&req, 1) == -1)
	{
			fprintf(stderr, "Unable to aquire DMA channels (%s) \n", strerror (errno));
			omap3530_sdma_detach (omap3530);
			return (-1);
	}
	rx_idx = omap3530->sdma_rx_chid = req.start;

	// do some common initiailization
	omap3530->dma4->channel[tx_idx].cse = 1;
	omap3530->dma4->channel[tx_idx].cde = 1;
	omap3530->dma4->channel[rx_idx].cse = 1;
	omap3530->dma4->channel[rx_idx].cde = 1;
   	omap3530->dma4->channel[tx_idx].cdf = 0;	
   	omap3530->dma4->channel[rx_idx].csf = 0;
	omap3530->dma4->channel[tx_idx].csdp = DMA4_CSDP_DATA_TYPE_8BIT;
	omap3530->dma4->channel[rx_idx].csdp = DMA4_CSDP_DATA_TYPE_8BIT;

	return 0;
}

int omap3530_init_sdma(omap3530_spi_t *omap3530)
{
	/*
	 * Map in DMA buffer
	 */
	if (omap3530->sdma) {
			omap3530->dmabuf = mmap(0, OMAP3530_SPI_MAXDMALEN, PROT_READ|PROT_WRITE|PROT_NOCACHE, 
							MAP_ANON|MAP_PHYS|MAP_PRIVATE, NOFD, 0);
			if (omap3530->dmabuf != MAP_FAILED) {
				omap3530->pdmabuf =  mphys(omap3530->dmabuf);
				if (omap3530->pdmabuf == -1)
					omap3530->sdma = 0;
			}
			else 
					omap3530->sdma = 0;
	}

	if(omap3530_sdma_attach(omap3530))  return -1;
	
	omap3530_sdma_disablespi(omap3530);
	return 0;
}

__SRCVERSION( "$URL$ $Rev$" );
