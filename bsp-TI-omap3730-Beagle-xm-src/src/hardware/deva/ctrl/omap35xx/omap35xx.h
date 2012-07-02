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

#ifndef __OMAP35XX_MCBSP2_H
#define __OMAP35XX_MCBSP2_H

struct omap35xx_card;

#define  HW_CONTEXT_T struct omap35xx_card
#define  PCM_SUBCHN_CONTEXT_T   ado_pcm_subchn_t

#define MCBSP1_BASE_ADDR 0x48074000
#define MCBSP2_BASE_ADDR 0x49022000
#define MCBSP3_BASE_ADDR 0x49024000
#define MCBSP4_BASE_ADDR 0x49026000
#define MCBSP5_BASE_ADDR 0x48096000

#define ENABLED 	1
#define DISABLED	0

#define MASTER   	1
#define SLAVE   	0

#define SAMPLE_SIZE	2	/* Sample size in bytes */

#define PROTOCOL_I2S 0
#define PROTOCOL_PCM 1
#define PROTOCOL_TDM 2

#include <audio_driver.h>
#include "sdma.h"
#include "variant.h"

typedef volatile uint32_t vuint32_t;
typedef volatile uint16_t vuint16_t;
typedef volatile uint8_t vuint8_t;

typedef struct mcbsp
{
	vuint32_t mcbsplp_drr_reg;			/* (MCBSP_BASE + 0x00000000) */
	vuint32_t mcbsplp_reserved_1_reg;
	vuint32_t mcbsplp_dxr_reg;			/* (MCBSP_BASE + 0x00000008) */
	vuint32_t mcbsplp_reserved_2_reg;
	vuint32_t mcbsplp_spcr2_reg;		/* (MCBSP_BASE + 0x00000010) */
	vuint32_t mcbsplp_spcr1_reg;		/* (MCBSP_BASE + 0x00000014) */
	vuint32_t mcbsplp_rcr2_reg;			/* (MCBSP_BASE + 0x00000018) */
	vuint32_t mcbsplp_rcr1_reg;			/* (MCBSP_BASE + 0x0000001c) */
	vuint32_t mcbsplp_xcr2_reg;			/* (MCBSP_BASE + 0x00000020) */
	vuint32_t mcbsplp_xcr1_reg;			/* (MCBSP_BASE + 0x00000024) */
	vuint32_t mcbsplp_srgr2_reg;		/* (MCBSP_BASE + 0x00000028) */
	vuint32_t mcbsplp_srgr1_reg;		/* (MCBSP_BASE + 0x0000002c) */
	vuint32_t mcbsplp_mcr2_reg;			/* (MCBSP_BASE + 0x00000030) */
	vuint32_t mcbsplp_mcr1_reg;			/* (MCBSP_BASE + 0x00000034) */
	vuint32_t mcbsplp_rcera_reg;		/* (MCBSP_BASE + 0x00000038) */
	vuint32_t mcbsplp_rcerb_reg;		/* (MCBSP_BASE + 0x0000003c) */
	vuint32_t mcbsplp_xcera_reg;		/* (MCBSP_BASE + 0x00000040) */
	vuint32_t mcbsplp_xcerb_reg;		/* (MCBSP_BASE + 0x00000044) */
	vuint32_t mcbsplp_pcr_reg;			/* (MCBSP_BASE + 0x00000048) */
	vuint32_t mcbsplp_rcerc_reg;		/* (MCBSP_BASE + 0x0000004c) */
	vuint32_t mcbsplp_rcerd_reg;		/* (MCBSP_BASE + 0x00000050) */
	vuint32_t mcbsplp_xcerc_reg;		/* (MCBSP_BASE + 0x00000054) */
	vuint32_t mcbsplp_xcerd_reg;		/* (MCBSP_BASE + 0x00000058) */
	vuint32_t mcbsplp_rcere_reg;		/* (MCBSP_BASE + 0x0000005c) */
	vuint32_t mcbsplp_rcerf_reg;		/* (MCBSP_BASE + 0x00000060) */
	vuint32_t mcbsplp_xcere_reg;		/* (MCBSP_BASE + 0x00000064) */
	vuint32_t mcbsplp_xcerf_reg;		/* (MCBSP_BASE + 0x00000068) */
	vuint32_t mcbsplp_rcerg_reg;		/* (MCBSP_BASE + 0x0000006c) */
	vuint32_t mcbsplp_rcerh_reg;		/* (MCBSP_BASE + 0x00000070) */
	vuint32_t mcbsplp_xcerg_reg;		/* (MCBSP_BASE + 0x00000074) */
	vuint32_t mcbsplp_xcerh_reg;		/* (MCBSP_BASE + 0x00000078) */
	vuint32_t mcbsplp_reserved_3_reg;
	vuint32_t mcbsplp_rintclr_reg;		/* (MCBSP_BASE + 0x00000080) */
	vuint32_t mcbsplp_xintclr_reg;		/* (MCBSP_BASE + 0x00000084) */
	vuint32_t mcbsplp_rovflclr_reg;		/* (MCBSP_BASE + 0x00000088) */
	vuint32_t mcbsplp_sysconfig_reg;	/* (MCBSP_BASE + 0x0000008c) */
	vuint32_t mcbsplp_thrsh2_reg;		/* (MCBSP_BASE + 0x00000090) */
	vuint32_t mcbsplp_thrsh1_reg;		/* (MCBSP_BASE + 0x00000094) */
	vuint32_t mcbsplp_reserved_4_reg;
	vuint32_t mcbsplp_reserved_5_reg;
	vuint32_t mcbsplp_irqstatus_reg;	/* (MCBSP_BASE + 0x000000a0) */
	vuint32_t mcbsplp_irqenable_reg;	/* (MCBSP_BASE + 0x000000a4) */
	vuint32_t mcbsplp_wakeupen_reg;		/* (MCBSP_BASE + 0x000000a8) */
	vuint32_t mcbsplp_xccr_reg;			/* (MCBSP_BASE + 0x000000ac) */
	vuint32_t mcbsplp_rccr_reg;			/* (MCBSP_BASE + 0x000000b0) */
	vuint32_t mcbsplp_xbuffstat_reg;	/* (MCBSP_BASE + 0x000000b4) */
	vuint32_t mcbsplp_rbuffstat_reg;	/* (MCBSP_BASE + 0x000000b8) */
	vuint32_t mcbsplp_sselcr_reg;		/* (MCBSP_BASE + 0x000000bc) */
	vuint32_t mcbsplp_status_reg;		/* (MCBSP_BASE + 0x000000c0) */
} mcbsp_t;

typedef struct dma4
{
	vuint32_t resv0[2];
	vuint32_t irqstatus[4];
	vuint32_t irqenable[4];
	vuint32_t sysstatus;
	vuint32_t ocp_sysconfig;
	vuint32_t resv1[13];
	vuint32_t caps_0;
	vuint32_t resv2;
	vuint32_t caps_2;
	vuint32_t caps_3;
	vuint32_t caps_4;
	vuint32_t gcr;
	vuint32_t resv3;
	struct dma4_channel
	{
		vuint32_t ccr;
		vuint32_t clnk_ctrl;
		vuint32_t cicr;
		vuint32_t csr;
		vuint32_t csdp;
		vuint32_t cen;
		vuint32_t cfn;
		vuint32_t cssa;
		vuint32_t cdsa;
		vuint32_t cse;
		vuint32_t csf;
		vuint32_t cde;
		vuint32_t cdf;
		vuint32_t csac;
		vuint32_t cdac;
		vuint32_t ccen;
		vuint32_t ccfn;
		vuint32_t color;
		vuint32_t resv4[6];
	} channel[32];
} dma4_t;

struct omap35xx_stream
{
	ado_pcm_subchn_t *pcm_subchn;
	ado_pcm_cap_t pcm_caps;
	ado_pcm_hw_t pcm_funcs;
	ado_pcm_config_t *pcm_config;
	int     dma_chan_idx;
	int     voices;
	int     aquired;		
};
typedef struct omap35xx_stream omap35xx_strm_t;

struct omap35xx_card
{
	ado_mutex_t hw_lock;
	ado_pcm_t *pcm1;
	ado_mixer_t *mixer;
	omap35xx_strm_t play_strm;
	omap35xx_strm_t cap_strm;
	mcbsp_t *mcbsp;
	dma4_t *dma4;
	int     codec_i2c_addr;
	int     en_power_amp;
	int     clk_mode;
	uint32_t sample_rate;
	uint32_t rate_min;
	uint32_t rate_max;
	uint64_t mcbsp_addr;
	uint8_t mcbsp_tx_dma_req;
	uint8_t mcbsp_rx_dma_req;
	int	pp_connect;
	int protocol;
	int	xclk_pol;
	int tdm_slot_size;
	int tdm_nslots;
	int sample_size;
};
typedef struct omap35xx_card omap35xx_t;

/* Bit defination for MCBSPLP_SPCR2_REG register */
#define MCBSPLP_SPCR2_REG_FREE			(1<<9)
#define MCBSPLP_SPCR2_REG_SOFT			(1<<8)
#define MCBSPLP_SPCR2_REG_FRST			(1<<7)
#define MCBSPLP_SPCR2_REG_GRST			(1<<6)
#define MCBSPLP_SPCR2_REG_XINTM_1		(1<<5)
#define MCBSPLP_SPCR2_REG_XINTM_0		(1<<4)
#define MCBSPLP_SPCR2_REG_XSYNCERR		(1<<3)
#define MCBSPLP_SPCR2_REG_XEMPTY		(1<<2)
#define MCBSPLP_SPCR2_REG_XRDY			(1<<1)
#define MCBSPLP_SPCR2_REG_XRST			(1<<0)

/* Bit defination for MCBSPLP_SPCR1_REG register */
#define MCBSPLP_SPCR1_REG_ALB			(1<<15)
#define MCBSPLP_SPCR1_REG_RJUST_1		(1<<14)
#define MCBSPLP_SPCR1_REG_RJUST_0		(1<<13)
#define MCBSPLP_SPCR1_REG_DXENA			(1<<7)
#define MCBSPLP_SPCR1_REG_RINTM_1		(1<<5)
#define MCBSPLP_SPCR1_REG_RINTM_0		(1<<4)
#define MCBSPLP_SPCR1_REG_RSYNCERR		(1<<3)
#define MCBSPLP_SPCR1_REG_RFULL			(1<<2)
#define MCBSPLP_SPCR1_REG_RRDY			(1<<1)
#define MCBSPLP_SPCR1_REG_RRST			(1<<0)

/* Bit defination for MCBSPLP_PCR_REG register */
#define MCBSPLP_PCR_REG_IDLE_EN			(1<<14)
#define MCBSPLP_PCR_REG_XIOEN			(1<<13)
#define MCBSPLP_PCR_REG_RIOEN			(1<<12)
#define MCBSPLP_PCR_REG_FSXM			(1<<11)
#define MCBSPLP_PCR_REG_FSRM			(1<<10)
#define MCBSPLP_PCR_REG_CLKXM			(1<<9)
#define MCBSPLP_PCR_REG_CLKRM			(1<<8)
#define MCBSPLP_PCR_REG_SCLKME			(1<<7)
#define MCBSPLP_PCR_REG_CLKS_STAT		(1<<6)
#define MCBSPLP_PCR_REG_DX_STAT			(1<<5)
#define MCBSPLP_PCR_REG_DR_STAT			(1<<4)
#define MCBSPLP_PCR_REG_FSXP			(1<<3)
#define MCBSPLP_PCR_REG_FSRP			(1<<2)
#define MCBSPLP_PCR_REG_CLKXP			(1<<1)
#define MCBSPLP_PCR_REG_CLKRP			(1<<0)

/* Bit defination for MCBSPLP_MCR2_REG register */
#define MCBSPLP_MCR2_REG_XMCME			(1<<9)
#define MCBSPLP_MCR2_REG_XMCM			(1<<0)

/* Bit defination for MCBSPLP_MCR1_REG register */
#define MCBSPLP_MCR1_REG_RMCME			MCBSPLP_MCR2_REG_XMCME
#define MCBSPLP_MCR1_REG_RMCM           MCBSPLP_MCR2_REG_XMCM

/* Bit defination for register MCBSPLP_XCR2_REG */
#define MCBSPLP_XCR_XPHASE              (0x01<<15)
#define MCBSPLP_XCR_XFRLEN_8            (0x07<<8)
#define MCBSPLP_XCR_XFRLEN_6            (0x05<<8)
#define MCBSPLP_XCR_XFRLEN_4            (0x03<<8)
#define MCBSPLP_XCR_XFRLEN_3            (0x02<<8)
#define MCBSPLP_XCR_XFRLEN_2            (0x01<<8)
#define MCBSPLP_XCR_XFRLEN_MSK          (0x7f<<8)
#define MCBSPLP_XCR_XWDLEN_MSK          (0x07<<5)
#define MCBSPLP_XCR_XWDLEN_8            (0)
#define MCBSPLP_XCR_XWDLEN_16           (0x02<<5)
#define MCBSPLP_XCR_XWDLEN_32           (0x05<<5)
#define MCBSPLP_XCR_XREVERSE_MSK        (0x03<<3)
#define MCBSPLP_XCR_XREVERSE_MSB        (0)
#define MCBSPLP_XCR_XREVERSE_LSB        (0x01<<3)
#define MCBSPLP_XCR_XDATDLY_MSK         (0x03<<0)
#define MCBSPLP_XCR_XDATDLY_0           (0)
#define MCBSPLP_XCR_XDATDLY_1           (0x01<<0)
#define MCBSPLP_XCR_XDATDLY_2           (0x02<<0)

/* Bit defination for register MCBSPLP_IRQSTATUS_REG*/
#define MCBSPLP_IRQSTATUS_REG_XEMPTYEOF		(1<<14)
#define MCBSPLP_IRQSTATUS_REG_XOVFLSTAT		(1<<12)
#define MCBSPLP_IRQSTATUS_REG_XUNDFLSTAT	(1<<11)
#define MCBSPLP_IRQSTATUS_REG_XRDY			(1<<10)
#define MCBSPLP_IRQSTATUS_REG_XEOF			(1<<9)
#define MCBSPLP_IRQSTATUS_REG_XFSX			(1<<8)
#define MCBSPLP_IRQSTATUS_REG_XSYNCERR		(1<<7)
#define MCBSPLP_IRQSTATUS_REG_ROVFLSTAT		(1<<5)
#define MCBSPLP_IRQSTATUS_REG_RUNDFLSTAT	(1<<4)
#define MCBSPLP_IRQSTATUS_REG_RRDY			(1<<3)
#define MCBSPLP_IRQSTATUS_REG_REOF			(1<<2)
#define MCBSPLP_IRQSTATUS_REG_RFSR			(1<<1)
#define MCBSPLP_IRQSTATUS_REG_RSYNCERR		(1<<0)

/* Bit defination for register MCBSPLP_IRQENABLE_REG */
#define MCBSPLP_IRQENABLE_REG_XEMPTYEOFEN	(1<<14)
#define MCBSPLP_IRQENABLE_REG_XOVFLEN		(1<<12)
#define MCBSPLP_IRQENABLE_REG_XUNDFLEN		(1<<11)
#define MCBSPLP_IRQENABLE_REG_XRDYEN		(1<<10)
#define MCBSPLP_IRQENABLE_REG_XEOFEN		(1<<9)
#define MCBSPLP_IRQENABLE_REG_XFSXEN		(1<<8)
#define MCBSPLP_IRQENABLE_REG_XSYNCERREN	(1<<7)
#define MCBSPLP_IRQENABLE_REG_ROVFLEN		(1<<5)
#define MCBSPLP_IRQENABLE_REG_RUNDFLEN		(1<<4)
#define MCBSPLP_IRQENABLE_REG_RRDYEN		(1<<3)
#define MCBSPLP_IRQENABLE_REG_REOFEN		(1<<2)
#define MCBSPLP_IRQENABLE_REG_RFSREN		(1<<1)
#define MCBSPLP_IRQENABLE_REG_RSYNCERREN	(1<<0)

/* Bit defination for register MCBSPLP_SRGR2_REG */
#define MCBSPLP_SRGR2_REG_GSYNC			(1<<15)
#define MCBSPLP_SRGR2_REG_CLKSP			(1<<14)
#define MCBSPLP_SRGR2_REG_CLKSM			(1<<13)
#define MCBSPLP_SRGR2_REG_FSGM			(1<<12)
#define MCBSPLP_SRGR2_REG_FPER_MSK      (0xFFF)

/* Bit defination for register MCBSPLP_XCCR_REG*/
#define MCBSPLP_XCCR_REG_EXTCLKGATE		(1<<15)
#define MCBSPLP_XCCR_REG_PPCONNECT		(1<<14)
#define MCBSPLP_XCCR_REG_DXENDLY_1		(1<<13)
#define MCBSPLP_XCCR_REG_DXENDLY_0		(1<<12)
#define MCBSPLP_XCCR_REG_XFULL_CYCLE	(1<<11)
#define MCBSPLP_XCCR_REG_DLB			(1<<5)
#define MCBSPLP_XCCR_REG_XDMAEN			(1<<3)
#define MCBSPLP_XCCR_REG_XDISABLE		(1<<0)

/* Bit defination for register MCBSPLP_RCCR_REG*/
#define MCBSPLP_RCCR_REG_RDISABLE		MCBSPLP_XCCR_REG_XDISABLE

/* Bit defination for register MCBSPLP_RCR2_REG */
#define MCBSPLP_RCR_RPHASE             MCBSPLP_XCR_XPHASE
#define MCBSPLP_RCR_RFRLEN_8           MCBSPLP_XCR_XFRLEN_8
#define MCBSPLP_RCR_RFRLEN_4           MCBSPLP_XCR_XFRLEN_4
#define MCBSPLP_RCR_RFRLEN_3           MCBSPLP_XCR_XFRLEN_3
#define MCBSPLP_RCR_RFRLEN_2           MCBSPLP_XCR_XFRLEN_2
#define MCBSPLP_RCR_RFRLEN_MSK         MCBSPLP_XCR_XFRLEN_MSK
#define MCBSPLP_RCR_RWDLEN_MSK         MCBSPLP_XCR_XWDLEN_MSK
#define MCBSPLP_RCR_RWDLEN_8           MCBSPLP_XCR_XWDLEN_8
#define MCBSPLP_RCR_RWDLEN_16          MCBSPLP_XCR_XWDLEN_16
#define MCBSPLP_RCR_RWDLEN_32          MCBSPLP_XCR_XWDLEN_32
#define MCBSPLP_RCR_RREVERSE_MSK       MCBSPLP_XCR_XREVERSE_MSK
#define MCBSPLP_RCR_RREVERSE_MSB       MCBSPLP_XCR_XREVERSE_MSB
#define MCBSPLP_RCR_RREVERSE_LSB       MCBSPLP_XCR_XREVERSE_LSB
#define MCBSPLP_RCR_RDATDLY_MSK        MCBSPLP_XCR_XDATDLY_MSK
#define MCBSPLP_RCR_RDATDLY_0          MCBSPLP_XCR_XDATDLY_0
#define MCBSPLP_RCR_RDATDLY_1          MCBSPLP_XCR_XDATDLY_1
#define MCBSPLP_RCR_RDATDLY_2          MCBSPLP_XCR_XDATDLY_2

#endif

__SRCVERSION
	("$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/deva/ctrl/omap35xx/omap35xx.h $ $Rev: 308279 $")
