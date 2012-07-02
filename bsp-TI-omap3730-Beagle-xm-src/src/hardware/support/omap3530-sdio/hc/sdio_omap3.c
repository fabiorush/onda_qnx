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

#include	<time.h>
#include	<stdio.h>
#include	<atomic.h>
#include	<stdlib.h>
#include	<stddef.h>
#include	<unistd.h>
#include	<string.h>
#include	<inttypes.h>
#include	<sys/mman.h>
#include	<sys/slog.h>
#include	<sys/types.h>
#include	<hw/pci.h>

#include	<sdio/sdio.h>

#include	"proto.h"

#include	"sdio_omap3.h"

int twl4030_setup_sdio();

//#define SDIO_TRACE_DEBUG
#if defined(SDIO_TRACE_DEBUG)
#define TRACE  slogf(99,1, "TRACE [%s]", __FUNCTION__)
#define TRACE_ENTER slogf(99,1, "%s enter", __FUNCTION__)
#define TRACE_EXIT slogf(99,1, "%s exit", __FUNCTION__)
#define DEBUG_MSG(x) slogf(99,1, "%s %s", __FUNCTION__, x)
#define DEBUG_CMD(x)  x
#else
#define TRACE
#define TRACE_ENTER
#define TRACE_EXIT 
#define DEBUG_MSG(x) 
#define DEBUG_CMD(x)  
#endif


#define _out32(x,y) (*((volatile unsigned int *)(x))=y)
#define _in32(x) (*((volatile unsigned int *)(x)))



static int _omap_setup_pio(void *hdl, char *buf, int len, int dir);

static int _omap_detect(void *hdl)
{
	TRACE;

	/*  need card detection code */
	return (MMC_SUCCESS);
}


static int _omap_ienable(void *hdl, int enable)
{
	omap3_ext_t		*ctx = (omap3_ext_t *)hdl;
	uintptr_t		base  = ctx->mmc_base;

/* Note: This can be called from an interrupt isr */
   //InterruptDisable();
	if (enable) {
		 _out32(base + OMAP3_MMCHS_IE,_in32(base + OMAP3_MMCHS_IE) & ~INTR_CIRQ);
		 _out32(base + OMAP3_MMCHS_ISE,_in32(base + OMAP3_MMCHS_ISE) | INTR_CIRQ);
		 _out32(base + OMAP3_MMCHS_IE, _in32(base + OMAP3_MMCHS_IE)  | INTR_CIRQ);
		DEBUG_CMD(slogf(99,1,"%s ise %x ie %x stat %x", __FUNCTION__, _in32(base + OMAP3_MMCHS_ISE), _in32(base + OMAP3_MMCHS_IE), _in32(base + OMAP3_MMCHS_STAT));)
	 } else {
		 //_out32(base + OMAP3_MMCHS_IE,_in32(base + OMAP3_MMCHS_IE) & ~INTR_CIRQ);
		 //_out32(base + OMAP3_MMCHS_IE,_in32(base + OMAP3_MMCHS_IE) | INTR_CIRQ);
		 _out32(base + OMAP3_MMCHS_ISE,_in32(base + OMAP3_MMCHS_ISE) & ~INTR_CIRQ);
	 }
	//InterruptEnable();

	return (MMC_SUCCESS);
}



/*
 * Interrupt validate
 */
static int _omap_ivalidate(void *hdl, int irq, int busy)
{
	omap3_ext_t  *ctx = (omap3_ext_t *)hdl;
	uintptr_t		base  = ctx->mmc_base;
	uint32_t		sts;
	int ret = SDIO_INTR_SDIO;
	
	sts = _in32(base + OMAP3_MMCHS_STAT);

	// not interrupt
	if (!sts)
		return (SDIO_INTR_NONE);	


	sts &= _in32(base + OMAP3_MMCHS_ISE) | INTR_ERRI ;
 	/*
    * Check card interrupt
    */
	if (sts & INTR_CIRQ) {
		_out32(base + OMAP3_MMCHS_ISE, ~(INTR_CIRQ | INTR_ERRI)); 
		_out32(base + OMAP3_MMCHS_IE, ~(INTR_CIRQ | INTR_ERRI)); 

      ctx->mask = 0;
      if (!busy)
         return (SDIO_INTR_CARD);

      /*
       * Client can't accept event now, so we set the event flag, 
       * return a fake SDIO interrupt, SDIO thread will queue the event
       */
      atomic_set(&ctx->sts, INTR_CIRQ);
      return (SDIO_INTR_SDIO);
   }
	
	_out32(base + OMAP3_MMCHS_STAT, sts  & ~(INTR_CIRQ | INTR_ERRI));

	ctx->sts = sts;

	return (ret);
}


/*
 * Interrupt process,
 */
static int _omap_iprocess(void *hdl, sdio_cmd_t *cmd)
{
	omap3_ext_t   *ctx = (omap3_ext_t *)hdl;
	uintptr_t		base  = ctx->mmc_base;
   uint32_t       ests, nsts;
	int				intr = 0;
	TRACE;


	DEBUG_CMD(slogf(99,1,"%s ise %x ie %x stat %x", __FUNCTION__, _in32(base + OMAP3_MMCHS_ISE), _in32(base + OMAP3_MMCHS_IE), _in32(base + OMAP3_MMCHS_STAT));)
   ests = ctx->sts & OMAP3_MMCHS_ERRORS_FLAGS;
   nsts = ests ^ ctx->sts;
	DEBUG_CMD(slogf(99,1,"%s sts %x ests %x nsts %x", __FUNCTION__, ctx->sts, ests, nsts));

  // Check SDIO interrupt
   if (nsts & INTR_CIRQ) {
      atomic_clr(&ctx->sts, INTR_CIRQ);
      intr |= SDMMC_INT_SERVICE;
      nsts &= ~INTR_CIRQ;
   }

	if (ests) {	
		if (ests & INTR_DTO) {
			intr |= SDMMC_INT_ERRDT;
			_out32(base + OMAP3_MMCHS_SYSCTL, _in32(base + OMAP3_MMCHS_SYSCTL) | SYSCTL_SRD);
			while (_in32(base + OMAP3_MMCHS_SYSCTL) & SYSCTL_SRD)
				;
		}
		if (ests & INTR_DCRC) {
			intr |= SDMMC_INT_ERRDC;
		}
		if (ests & INTR_DEB) {
			intr |= SDMMC_INT_ERRDE;
		}

		if (ests & INTR_CTO) {
			intr |= SDMMC_INT_ERRCT;
			_out32(base + OMAP3_MMCHS_SYSCTL, _in32(base + OMAP3_MMCHS_SYSCTL) | SYSCTL_SRC);
			while (_in32(base + OMAP3_MMCHS_SYSCTL) & SYSCTL_SRC);
		}
		if (ests & INTR_CCRC) {
			intr |= SDMMC_INT_ERRCC;
		}
		if (ests & INTR_CEB) {
			intr |= SDMMC_INT_ERRCE;
		}
		if (ests & INTR_CIE) {
			intr |= SDMMC_INT_ERRCI;
		}
		atomic_clr(&ctx->sts, ests);
	} 
	else {
		if (nsts & INTR_CC) {
			intr |= SDMMC_INT_COMMAND;
			atomic_clr(&ctx->sts, INTR_CC);
			if (cmd) {
				if (cmd->rsptype & MMC_RSP_136) {
					cmd->resp[3] = _in32(base + OMAP3_MMCHS_RSP76);
					cmd->resp[2] = _in32(base + OMAP3_MMCHS_RSP54);
					cmd->resp[1] = _in32(base + OMAP3_MMCHS_RSP32);
					cmd->resp[0] = _in32(base + OMAP3_MMCHS_RSP10);
				} else if (cmd->rsptype & MMC_RSP_PRESENT) {
					cmd->resp[0] = _in32(base + OMAP3_MMCHS_RSP10);
				}
			}

			if (cmd->rsptype & MMC_RSP_BUSY) {
				int		i;

				for (i = 1024 * 256; i > 0; i--) {
					if (_in32(base + OMAP3_MMCHS_PSTATE) & PSTATE_DLA) {
						nanospin_ns(1024);
						continue;
					}
					break;
				}
				if (i <= 0) {
					intr |= SDMMC_INT_ERROR;
				}
				
			}
		}

		if (nsts & (INTR_TC | INTR_BWR | INTR_BRR)) {
			if (nsts & INTR_TC) {
				intr |= SDMMC_INT_DATA;
			}
			if (nsts & INTR_BRR) {
				intr |= SDMMC_INT_RBRDY;
			}
			if (nsts & INTR_BWR) {
				intr |= SDMMC_INT_WBRDY;
			}

		}
	}

	if (intr)
		_out32(base + OMAP3_MMCHS_IE,  (_in32(base + OMAP3_MMCHS_IE) & INTR_CIRQ) );

	return intr;
}

/*
 * setup DMA transfer
 */
static int _omap_setup_dma(void *hdl, paddr_t paddr, int len, int dir)
{
	omap3_ext_t		*ctx = (omap3_ext_t *)hdl;
	dma4_param		*param;
	
	//
	// Initialize Tx DMA channel
	//
	param = (dma4_param *)(ctx->dma_base + DMA4_CCR(ctx->dma_chnl));

	len = _omap_setup_pio(hdl, NULL, len, dir);

	if (len > 0) {
		// Clear all status bits
		param->csr  = 0x1FFE;
		param->cen  = len >> 2;
		param->cfn  = 1;			// Number of frames
		param->cse  = 1;
		param->cde  = 1;
		param->cicr = 0;			// We don't want any interrupts

		if (dir == MMC_DIR_IN) {
			// setup receive SDMA channel
			param->csdp = (2 <<  0)		// DATA_TYPE = 0x2:  32 bit element
						| (0 <<  2)		// RD_ADD_TRSLT = 0: Not used
						| (0 <<  6)		// SRC_PACKED = 0x0: Cannot pack source data
						| (0 <<  7)		// SRC_BURST_EN = 0x0: Cannot burst source
						| (0 <<  9)		// WR_ADD_TRSLT = 0: Undefined
						| (0 << 13)		// DST_PACKED = 0x0: No packing
						| (3 << 14)		// DST_BURST_EN = 0x3: Burst at 16x32 bits
						| (1 << 16)		// WRITE_MODE = 0x1: Write posted
						| (0 << 18)		// DST_ENDIAN_LOCK = 0x0: Endianness adapt
						| (0 << 19)		// DST_ENDIAN = 0x0: Little Endian type at destination
						| (0 << 20)		// SRC_ENDIAN_LOCK = 0x0: Endianness adapt
						| (0 << 21);	// SRC_ENDIAN = 0x0: Little endian type at source

			param->ccr  = DMA4_CCR_SYNCHRO_CONTROL(ctx->dma_rreq)	// Synchro control bits
						| (1 <<  5)		// FS = 1: Packet mode with BS = 0x1
						| (0 <<  6)		// READ_PRIORITY = 0x0: Low priority on read side
						| (0 <<  7)		// ENABLE = 0x0: The logical channel is disabled.
						| (0 <<  8)		// DMA4_CCRi[8] SUSPEND_SENSITIVE = 0
						| (0 << 12)		// DMA4_CCRi[13:12] SRC_AMODE = 0x0: Constant address mode
						| (1 << 14)		// DMA4_CCRi[15:14] DST_AMODE = 0x1: Post-incremented address mode
						| (1 << 18)		// DMA4_CCRi[18] BS = 0x1: Packet mode with FS = 0x1
						| (1 << 24)		// DMA4_CCRi[24] SEL_SRC_DST_SYNC = 0x1: Transfer is triggered by the source. The packet element number is specified in the DMA4_CSFI register.
						| (0 << 25);	// DMA4_CCRi[25] BUFFERING_DISABLE = 0x0

			param->cssa = ctx->mmc_pbase + OMAP3_MMCHS_DATA;
			param->cdsa = paddr;
			param->csf  = ctx->blksz >> 2;
		} else {
			// setup transmit SDMA channel
			param->csdp = (2 <<  0)		// DATA_TYPE = 0x2:  32 bit element
						| (0 <<  2)		// RD_ADD_TRSLT = 0: Not used
						| (0 <<  6)		// SRC_PACKED = 0x0: Cannot pack source data
						| (3 <<  7)		// SRC_BURST_EN = 0x3: Burst at 16x32 bits
						| (0 <<  9)		// WR_ADD_TRSLT = 0: Undefined
						| (0 << 13)		// DST_PACKED = 0x0: No packing
						| (0 << 14)		// DST_BURST_EN = 0x0: Cannot Burst
						| (0 << 16)		// WRITE_MODE = 0x0: Write not posted
						| (0 << 18)		// DST_ENDIAN_LOCK = 0x0: Endianness adapt
						| (0 << 19)		// DST_ENDIAN = 0x0: Little Endian type at destination
						| (0 << 20)		// SRC_ENDIAN_LOCK = 0x0: Endianness adapt
						| (0 << 21);	// SRC_ENDIAN = 0x0: Little endian type at source

			param->ccr  = DMA4_CCR_SYNCHRO_CONTROL(ctx->dma_treq)
						| (1 <<  5)		// FS = 1: Packet mode with BS = 0x1
						| (0 <<  6)		// READ_PRIORITY = 0x0: Low priority on read side
						| (0 <<  7)		// ENABLE = 0x0: The logical channel is disabled.
						| (0 <<  8)		// DMA4_CCRi[8] SUSPEND_SENSITIVE = 0
						| (1 << 12)		// DMA4_CCRi[13:12] SRC_AMODE = 0x1: Post-incremented address mode
						| (0 << 14)		// DMA4_CCRi[15:14] DST_AMODE = 0x0: Constant address mode
						| (1 << 18)		// DMA4_CCRi[18] BS = 0x1: Packet mode with FS = 0x1
						| (0 << 24)		// DMA4_CCRi[24] SEL_SRC_DST_SYNC = 0x0: Transfer is triggered by the source. The packet element number is specified in the DMA4_CSFI register.
						| (0 << 25);	// DMA4_CCRi[25] BUFFERING_DISABLE = 0x0

			param->cssa = paddr;
			param->cdsa = ctx->mmc_pbase + OMAP3_MMCHS_DATA;
			param->cdf  = ctx->blksz >> 2;
		}

		// Enable DMA event
		param->ccr |= 1 << 7;

		ctx->dcmd |= CMD_DE;
		ctx->dmask = INTR_TC;		// Use transfer complete interrupt
		ctx->dsize = len;
	}

	return (len);
}

/*
 * setup PIO transfer
 */
static int _omap_setup_pio(void *hdl, char *buf, int len, int dir)
{
	omap3_ext_t		*ctx;
	int				nblk;
	uintptr_t		base;
	
	ctx = (omap3_ext_t *)hdl;
	base  = ctx->mmc_base;

	nblk = len / ctx->blksz;
	len  = nblk * ctx->blksz;

	ctx->dcmd = nblk > 1 ? (CMD_MBS | CMD_BCE | CMD_ACEN | CMD_DP) : CMD_DP;
	if (dir == MMC_DIR_IN) {
		ctx->dcmd |= CMD_DDIR;
		ctx->dmask = INTR_BRR;
	} else
		ctx->dmask = INTR_BWR;

	_out32(ctx->mmc_base + OMAP3_MMCHS_BLK, (nblk << 16) | ctx->blksz);

	return (len);
}

static int _omap_dma_done(void *hdl, int dir)
{
	omap3_ext_t		*ctx;
	dma4_param		*param;
	int				ret = MMC_SUCCESS;

	ctx = (omap3_ext_t *)hdl;

	param = (dma4_param *)(ctx->dma_base + DMA4_CCR(ctx->dma_chnl));

#define	OMAP3_SDMA_ERROR	((1 << 11) | (1 << 10) | (1 << 9) | (1 << 8))
	while (1) {
		// transfer complete?
		if (param->ccen == (ctx->dsize >> 2))
			break;
		// Check DMA errors
		if (param->csr & OMAP3_SDMA_ERROR)
			break;
	}
	if (param->csr & OMAP3_SDMA_ERROR)
		ret = MMC_FAILURE;

	// Disable this DMA event
	param->ccr = 0;

	// Clear all status bits
	param->csr = 0x1FFE;

	return ret;
}

static int _omap_pio_done(void *hdl, char *buf, int len, int dir)
{
	omap3_ext_t		*ctx;
	uintptr_t		base;
	int				nbytes, cnt;
	uint32_t		*pbuf = (uint32_t *)buf;

	ctx = (omap3_ext_t *)hdl;
	base  = ctx->mmc_base;

	cnt = nbytes = len < MMCHS_FIFO_SIZE ? len : MMCHS_FIFO_SIZE;

	if (dir == MMC_DIR_IN) {
		if (!(_in32(base + OMAP3_MMCHS_PSTATE) & PSTATE_BRE)) {
			_out32(base + OMAP3_MMCHS_IE,  (_in32(base + OMAP3_MMCHS_IE) & INTR_CIRQ) | INTR_BRR );
			return 0;
		}

		for (; nbytes > 0; nbytes -= 4)
			*pbuf++ = _in32(base + OMAP3_MMCHS_DATA);
	} else {
		if (!(_in32(base + OMAP3_MMCHS_PSTATE) & PSTATE_BWE)) {
			_out32(base + OMAP3_MMCHS_IE,  (_in32(base + OMAP3_MMCHS_IE) & INTR_CIRQ) | INTR_BWR );
			return 0;
		}

		for (; nbytes > 0; nbytes -= 4)
			_out32(base + OMAP3_MMCHS_DATA, *pbuf++);

		if (len == cnt)
			_out32(base + OMAP3_MMCHS_IE,  (_in32(base + OMAP3_MMCHS_IE) & INTR_CIRQ) | INTR_TC );

	}

	return cnt;
}


static int _omap_command(void *hdl, sdio_cmd_t *cmd)
{
	omap3_ext_t		*ctx;
	uintptr_t		base;
	uint32_t		command;
	uint32_t		imask;
	TRACE;

	ctx = (omap3_ext_t *)hdl;
	base  = ctx->mmc_base;

	if (cmd->eflags & SDMMC_CMD_INIT) {
		uint32_t	tmp = _in32(base + OMAP3_MMCHS_CON);
		_out32(base + OMAP3_MMCHS_CON, tmp | CON_INIT);
		delay(10);
		_out32(base + OMAP3_MMCHS_CON, tmp);
	}

	/* Clear Status */
	_out32(base + OMAP3_MMCHS_STAT, 0x117F8033 & ~(INTR_CIRQ | INTR_ERRI));

	imask = 0x110f8000;

	command = (cmd->opcode) << 24;
	if (cmd->opcode == 12)
		command |= SDMMC_CAP_ACMD12;

	if (cmd->eflags & SDMMC_CMD_DATA) {
		command |= ctx->dcmd;
		imask |= INTR_DTO | INTR_DCRC | INTR_DEB;	// Enable all data error interrupts
		imask |= ctx->dmask;	// Data complete interrupt or data ready interrupt
	} else
		imask |= INTR_CC;		// Enable command complete interrupt

	if (cmd->rsptype & MMC_RSP_PRESENT) {
		if (cmd->rsptype & MMC_RSP_136)
			command |= CMD_RSP_TYPE_136;
		else if (cmd->rsptype & MMC_RSP_BUSY)	// Response with busy check
			command |= CMD_RSP_TYPE_48b;
		else
			command |= CMD_RSP_TYPE_48;

		if (cmd->rsptype & MMC_RSP_OPCODE)		// Index check
			command |= CMD_CICE;
		if (cmd->rsptype & MMC_RSP_CRC)		// CRC check
			command |= CMD_CCCE;
	}

	/* Setup the Argument Register and send CMD */
	_out32(base + OMAP3_MMCHS_IE,  _in32(base + OMAP3_MMCHS_IE)  | (imask & ~INTR_CIRQ));
	_out32(base + OMAP3_MMCHS_ISE, _in32(base + OMAP3_MMCHS_ISE) | (imask & ~INTR_CIRQ));
	_out32(base + OMAP3_MMCHS_ARG, cmd->argument);
	_out32(base + OMAP3_MMCHS_CMD, command);
	DEBUG_CMD(slogf(99,1,"_omap_command ise %x ie %x stat %x", _in32(base + OMAP3_MMCHS_ISE), _in32(base + OMAP3_MMCHS_IE), _in32(base + OMAP3_MMCHS_STAT));)
	
	return (MMC_SUCCESS);
}

static int _omap_bus_speed(void *hdl, int *clock)
{
	omap3_ext_t		*ctx = (omap3_ext_t *)hdl;
	uintptr_t		base  = ctx->mmc_base;
	uint32_t		sysctl;
	int				clkd;
	TRACE;

	clkd = ctx->mmc_clock / (*clock);
	if (ctx->mmc_clock != (*clock) * clkd)
		clkd++;
	*clock = ctx->mmc_clock / clkd;

	sysctl = _in32(base + OMAP3_MMCHS_SYSCTL);

	// Stop clock
	sysctl &= ~(SYSCTL_ICE | SYSCTL_CEN | SYSCTL_DTO_MASK);
	sysctl |= SYSCTL_DTO_MAX | SYSCTL_SRC | SYSCTL_SRD;
	_out32(base + OMAP3_MMCHS_SYSCTL, sysctl);

	// Enable internal clock
	sysctl &= ~(0x3FF << 6);
	sysctl |= (clkd << 6) | SYSCTL_ICE;
	_out32(base + OMAP3_MMCHS_SYSCTL, sysctl);

	// Wait for the clock to be stable
	while ((_in32(base + OMAP3_MMCHS_SYSCTL) & SYSCTL_ICS) == 0)
		;

	// Enable clock to the card
	_out32(base + OMAP3_MMCHS_SYSCTL, sysctl | SYSCTL_CEN | 1);

	return (MMC_SUCCESS);
}


static int _omap_bus_width(void *hdl, uint8_t width)
{
	omap3_ext_t		*ctx;
	uintptr_t		base;
	uint32_t		tmp;
	TRACE;

	ctx = (omap3_ext_t *)hdl;
	base  = ctx->mmc_base;

	tmp = _in32(base + OMAP3_MMCHS_HCTL);
	if (width == 4)
		tmp |= HCTL_DTW4;
	else
		tmp &= ~HCTL_DTW4;
	_out32(base + OMAP3_MMCHS_HCTL, tmp);

	return (MMC_SUCCESS);
}

static int _omap_block_size(void *hdl, int blksz)
{
	omap3_ext_t		*ctx;
	uintptr_t		base;
	TRACE;

	ctx = (omap3_ext_t *)hdl;
	base  = ctx->mmc_base;

	if (blksz > 1024)
		return (MMC_FAILURE);

	ctx->blksz = blksz;
	_out32(ctx->mmc_base + OMAP3_MMCHS_BLK, ctx->blksz);
	delay(10);
	return (MMC_SUCCESS);
}


/*
 * Reset host controller and card
 * The clock should be enabled and set to minimum (<400KHz)
 */
static int _omap_powerup(void *hdl)
{
	omap3_ext_t		*ctx;
	uintptr_t		base;
	int				clock;
	TRACE;

	ctx = (omap3_ext_t *)hdl;
	base  = ctx->mmc_base;

	// Disable All interrupts
	_out32(base + OMAP3_MMCHS_IE, 0);

	// Software reset
	_out32(base + OMAP3_MMCHS_SYSCONFIG, SYSCONFIG_SOFTRESET);
	while ((_in32(base + OMAP3_MMCHS_SYSSTATUS) & SYSSTATUS_RESETDONE) == 0)
		;

	_out32(base + OMAP3_MMCHS_SYSCTL, SYSCTL_SRA);
	while ((_in32(base + OMAP3_MMCHS_SYSCTL) & SYSCTL_SRA) != 0)
		;

	_out32(base + OMAP3_MMCHS_SYSCONFIG, SYSCONFIG_CLOCKACTIVITY | SYSCONFIG_SIDLEMODE);
	_out32(base + OMAP3_MMCHS_HCTL, HCTL_SDVS3V0);

	_out32(base + OMAP3_MMCHS_CAPA, _in32(base + OMAP3_MMCHS_CAPA) | OMAP_CAPA_VS3V3 | OMAP_CAPA_VS3V0 | OMAP_CAPA_VS1V8);

	_out32(base + OMAP3_MMCHS_CON, CON_CTPL | CON_DVAL| CON_CLKEXTFREE);

	clock = 400 * 1000;		// 400KHz clock
	_omap_bus_speed(hdl, &clock);

	_out32(base + OMAP3_MMCHS_HCTL, HCTL_DTW4 | HCTL_SDVS3V0 | HCTL_SDBP | HCTL_IWE | HCTL_IWE | HCTL_INS | HCTL_REM | HCTL_OBWE );

	_out32(base + OMAP3_MMCHS_ISE, MMCHS_MMCSD_ALL_INTERRUPTS);
	
	delay(10);

	return (MMC_SUCCESS);
}

static int _omap_powerdown(void *hdl)
{
	omap3_ext_t		*ctx;
	uintptr_t		base;
	TRACE;

	ctx = (omap3_ext_t *)hdl;
	base  = ctx->mmc_base;


	// Disable All interrupts
	_out32(base + OMAP3_MMCHS_IE, 0);


	// Software reset
	_out32(base + OMAP3_MMCHS_SYSCONFIG, SYSCONFIG_SOFTRESET);
	while ((_in32(base + OMAP3_MMCHS_SYSSTATUS) & SYSSTATUS_RESETDONE) == 0)
		;

	_out32(base + OMAP3_MMCHS_SYSCTL, SYSCTL_SRA);
	while ((_in32(base + OMAP3_MMCHS_SYSCTL) & SYSCTL_SRA) != 0)
		;


	return (MMC_SUCCESS);
}

static int _omap_shutdown(void *hdl)
{
	omap3_ext_t		*ctx;
	TRACE;

	ctx = (omap3_ext_t *)hdl;

	_omap_powerdown(hdl);

	munmap_device_io(ctx->mmc_base, OMAP3_MMCHS_SIZE);
	if (ctx->dma_base) {
		dma4_param	*param = (dma4_param *)(ctx->dma_base + DMA4_CCR(ctx->dma_chnl));

		// Disable this DMA event
		param->ccr = 0;

		// Clear all status bits
		param->csr = 0x1FFE;

		// Unmap the DMA registers
		munmap_device_io(ctx->dma_base, OMAP3_DMA4_SIZE);
	}

	free(ctx);

	return (MMC_SUCCESS);
}

 static int _omap_get_mask(void *hdl)
{
	omap3_ext_t		*ctx;	
	uintptr_t		base;
	TRACE;

	ctx = (omap3_ext_t *)hdl;
	base  = ctx->mmc_base;
	
	return (_in32(base + OMAP3_MMCHS_ISE) & ~INTR_CIRQ);
}


void *omap_init(void *sdio_hdl, void *sdio_cfg)
{
	struct Config_Info	local_cfg;
	struct Config_Info	*cfg = (struct Config_Info *)sdio_cfg;
	omap3_ext_t		    *ctx;
	sdio_ext_t			*sdio = (sdio_ext_t *)sdio_hdl;
	uintptr_t			 base;
	uint32_t			 reg;
	TRACE;


	if (sdio_cfg == NULL) {
		cfg = (struct Config_Info *)&local_cfg;

   	cfg->IOPort_Base[0]   = OMAP3_MMCHS1_BASE;
   	cfg->IOPort_Length[0] = OMAP3_MMCHS_SIZE;
   	cfg->IOPort_Base[1]   = OMAP3_DMA4_BASE;
   	cfg->IOPort_Length[1] = OMAP3_DMA4_SIZE;
   	cfg->NumIOPorts = 2;

   	cfg->IRQRegisters[0] = OMAP3_MMCHS1_IRQ;
   	cfg->NumIRQs = 1;

   	cfg->DMALst[0] = (DMA4_MMC1_TX >> 1) & 0xFF;
   	cfg->DMALst[1] = DMA4_MMC1_TX; // DMA request line for MMC1 TX
   	cfg->DMALst[2] = DMA4_MMC1_RX; // DMA request line for MMC1 RX
   	cfg->NumDMAs = 3;
	}

	twl4030_setup_sdio();
	
	if ((ctx = calloc(1, sizeof(omap3_ext_t))) == NULL)
		return (NULL);

	ctx->mmc_pbase = cfg->IOPort_Base[0];

	if ((ctx->mmc_base =
			mmap_device_io(OMAP3_MMCHS_SIZE, ctx->mmc_pbase))
				== (uintptr_t)MAP_FAILED) {
		goto fail0;
	}
	base = ctx->mmc_base;

	if (cfg->NumDMAs > 2 && cfg->NumIOPorts > 1) {
		ctx->dma_chnl = cfg->DMALst[0];
		ctx->dma_treq = cfg->DMALst[1];
		ctx->dma_rreq = cfg->DMALst[2];
		if ((ctx->dma_base = 
			mmap_device_io(OMAP3_DMA4_SIZE, cfg->IOPort_Base[1])) == (uintptr_t)MAP_FAILED) {
			goto fail1;
		}
		sdio->eflags   |= SDMMC_CAP_DMA;
	}

	ctx->mmc_clock = 96000000;

	sdio->eflags |= SDMMC_CAP_ACMD12 | SDMMC_CAP_BW1 | SDMMC_CAP_BW4 | SDMMC_CAP_PIO;

	reg = _in32(base + OMAP3_MMCHS_CAPA);

	if (reg & OMAP_CAPA_VS1V8)
		sdio->eflags |= SDMMC_CAP_18V;

	if (reg & OMAP_CAPA_VS3V0)
		sdio->eflags |= SDMMC_CAP_30V;

	if (reg & OMAP_CAPA_VS3V3)
		sdio->eflags |= SDMMC_CAP_33V;

	if (reg & OMAP_CAPA_DS)
		sdio->eflags |= SDMMC_CAP_DMA;

	if (reg & OMAP_CAPA_HSS)
		sdio->eflags |= SDMMC_CAP_HS;

	sdio->hclock     = ((_in32(base + OMAP3_MMCHS_CAPA) >> 8) & 0x3F) * 1000 * 1000;
	sdio->detect	 = _omap_detect;
	sdio->powerup	 = _omap_powerup;
	sdio->powerdown	 = _omap_powerdown;
	sdio->shutdown	 = _omap_shutdown;
	sdio->ienable	 = _omap_ienable;
	sdio->ivalidate	 = _omap_ivalidate;
	sdio->iprocess	 = _omap_iprocess;
	sdio->command	 = _omap_command;
	sdio->setup_dma	 = _omap_setup_dma;
	sdio->dma_done   = _omap_dma_done;
	sdio->setup_pio  = _omap_setup_pio;
	sdio->pio_done   = _omap_pio_done;
	sdio->block_size = _omap_block_size;
	sdio->bus_width  = _omap_bus_width;
	sdio->bus_speed  = _omap_bus_speed;
	sdio->get_mask = _omap_get_mask;
	
	if (!cfg->Description[0])
		strncpy(cfg->Description, "TI OMAP3 SDIO", sizeof(cfg->Description));
		
	return (ctx);

fail0:
	free(ctx);
fail1:
	munmap_device_io(ctx->mmc_base, OMAP3_MMCHS_SIZE);

	return (NULL);
}

__SRCVERSION( "$URL$ $Rev$" );
