/*
 * $QNXLicenseC:
 * Copyright 2006-2008 QNX Software Systems.
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
	
#ifndef OMAP_DMA__H_
#define OMAP_DMA__H_
	
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <atomic.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <sys/rsrcdbmgr.h>
#include <sys/rsrcdbmsg.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/slog.h>
	
#define DMA4_BASE_ADDR	0x48056000
#define SDMA_BASE_IRQ 256

/* SDMA Mapping Requests */ 
#define S_DMA_30    30	/* MCBSP1 TX */
#define S_DMA_31    31	/* MCBSP1 RX */
#define S_DMA_32    32	/* MCBSP2 TX */
#define S_DMA_33    33	/* MCBSP2 RX */
#define S_DMA_16    16	/* MCBSP3 TX */
#define S_DMA_17    17	/* MCBSP3 RX */
#define S_DMA_18    18	/* MCBSP4 TX */
#define S_DMA_19    19	/* MCBSP4 RX */
#define S_DMA_20    20	/* MCBSP5 TX */
#define S_DMA_21    21	/* MCBSP5 RX */
	
#define GCR_ARBITRATION_RATE			0xff0000
#define DMA_DEFAULT_FIFO_DEPTH     		0x10
	
#define OMAP_DMA4_CHANNEL_COUNT         32	
	
/*Bit definitions for DMA4_CSDP*/ 
#define DMA4_CSDP_SRC_ENDIAN 			(1<<21)
#define DMA4_CSDP_SRC_ENDIAN_LOCK		(1<<20)
#define DMA4_CSDP_DST_ENDIAN			(1<<19)
#define DMA4_CSDP_DST_ENDIAN_LOCK		(1<<18)
#define DMA4_CSDP_WRITE_MODE_1			(1<<17)
#define DMA4_CSDP_WRITE_MODE_0			(1<<16)
#define DMA4_CSDP_DST_BURST_EN_1		(1<<15)
#define DMA4_CSDP_DST_BURST_EN_0		(1<<14)
#define DMA4_CSDP_DST_PACKED			(1<<13)
#define DMA4_CSDP_SRC_BURST_EN_1		(1<<8)
#define DMA4_CSDP_SRC_BURST_EN_0		(1<<7)
#define DMA4_CSDP_SRC_PACKED			(1<<6)
#define DMA4_CSDP_DATA_TYPE_32BIT       (1<<1)
#define DMA4_CSDP_DATA_TYPE_16BIT       (1<<0)
#define DMA4_CSDP_DATA_TYPE_1			(1<<1)
#define DMA4_CSDP_DATA_TYPE_0			(1<<0)
	
/*Bit definitions for DMA4_CCR*/ 
#define DMA4_CCR_WRITE_PRIORITY			(1<<26)
#define DMA4_CCR_BUFFERING_DISABLE		(1<<25)
#define DMA4_CCR_SEL_SRC_DST_SYNC		(1<<24)
#define DMA4_CCR_PREFETCH				(1<<23)
#define DMA4_CCR_SUPERVISOR 			(1<<22)
#define DMA4_CCR_SECURE					(1<<21)
#define DMA4_CCR_BS						(1<<18)
#define DMA4_CCR_TRANSPARENT_COPY_ENABLE	(1<<17)
#define DMA4_CCR_CONST_FILL_ENABLE		(1<<16)
#define DMA4_CCR_DST_CONSTANT       	(0)
#define DMA4_CCR_DST_POST_INCR      	(1<<14)
#define DMA4_CCR_DST_AMODE_1			(1<<15)
#define DMA4_CCR_DST_AMODE_0			(1<<14)
#define DMA4_CCR_SRC_CONSTANT       	(0)
#define DMA4_CCR_SRC_SINGLE_IDX      	(1<<13)
#define DMA4_CCR_SRC_POST_INCR      	(1<<12)
#define DMA4_CCR_SRC_AMODE_1			(1<<13)
#define DMA4_CCR_SRC_AMODE_0			(1<<12)
#define DMA4_CCR_WR_ACTIVE				(1<<10)
#define DMA4_CCR_RD_ACTIVE				(1<<9)
#define DMA4_CCR_SUSPEND_SENSITIVE		(1<<8)
#define DMA4_CCR_ENABLE					(1<<7)
#define DMA4_CCR_READ_PRIORITY			(1<<6)
#define DMA4_CCR_FS						(1<<5)
	
/*Bit definitions for DMA4_CLNK_CTRL*/ 
#define DMA4_CLNK_CTRL_ENABLE_LNK		(1<<15)
#define DMA4_CICR_FRAME_IE  			(1<<3)
#define DMA4_CSR_FRAME      			(1<<3)
#define DMA4_CSR_MSK					(0x1ffe)
	
#define CONSTANT_ADDRESS_MODE			0	
#define POST_INCREMENTED_ADDRESS_MODE	1
#define SINGLE_INDEX_ADDRESS_MODE		2
#define DOUBLE_INDEX_ADDRESS_MODE		3	
	
#endif
	

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/deva/ctrl/omap35xx/sdma.h $ $Rev: 308279 $" )
