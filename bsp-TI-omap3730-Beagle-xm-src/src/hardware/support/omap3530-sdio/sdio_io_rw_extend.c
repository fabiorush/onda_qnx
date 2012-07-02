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


int
sdio_read_iomem(void *hdl, uint8_t fn, uint32_t address, 
				uint8_t opcode, uint32_t blkcnt, uint32_t blksz, uint8_t *buffer, off64_t paddr)
{
	sdio_ext_t	*sdio = (sdio_ext_t *)hdl;
	int			nbytes = blkcnt * blksz;
	sdio_cmd_t	cmd;

	if (sdio->blksz[fn] != blksz) {
		if (sdio_set_blksz(hdl, fn, blksz) != SDIO_SUCCESS)
			return (SDIO_FAILURE);
	}

	cmd.opcode   = 53;
	cmd.rsptype  = SDIO_RSP_R5;
	cmd.eflags   = SDMMC_CMD_DATA_IN | SDMMC_CMD_DATA | (blkcnt > 1 ? SDMMC_CMD_DATA_MULTI : 0);
	cmd.argument = MAKE_SDIO_OFFSET(address) |
				    MAKE_SDIO_OP_CODE(opcode) |
				    MAKE_SDIO_BLOCK_MODE(1) |
				    MAKE_SDIO_FUNCTION(fn) |
				    MAKE_SDIO_DIR(0) | (blkcnt & 0x01FF);
	cmd.blkcnt   = blkcnt;			// block count;
	cmd.size     = blksz;			// block size 

	if (sdio->eflags & SDMMC_CAP_DMA) {
		CACHE_INVAL(&sdio->cachectl, buffer, paddr, nbytes);

		if (sdio->setup_dma(sdio->hchdl, paddr, nbytes, MMC_DIR_IN) != nbytes)
			return (SDIO_FAILURE);

		cmd.eflags |= SDMMC_CMD_DATA_DMA;
		if (sdio_send_command(sdio, &cmd) != MMC_SUCCESS)
			return (SDIO_FAILURE);

		if (sdio->dma_done(sdio->hchdl, MMC_DIR_IN) != MMC_SUCCESS)
			return (SDIO_FAILURE);
	} else {
		if (sdio->setup_pio(sdio->hchdl, (char *)buffer, nbytes, MMC_DIR_IN) != nbytes)
			return (SDIO_FAILURE);

		cmd.data = buffer;			// buffer pointer

		if (sdio_send_command(sdio, &cmd) != MMC_SUCCESS)
			return (SDIO_FAILURE);

		if (sdio->pio_done(sdio->hchdl, (char *)buffer, nbytes, MMC_DIR_IN) != nbytes)
			return (SDIO_FAILURE);
	}

	return (SDIO_SUCCESS);
}

int
sdio_write_iomem(void *hdl, uint8_t fn, uint32_t address, 
				uint8_t opcode, uint32_t blkcnt, uint32_t blksz, uint8_t *buffer, off64_t paddr)
{
	sdio_ext_t	*sdio = (sdio_ext_t *)hdl;
	int			nbytes = blkcnt * blksz;
	sdio_cmd_t	cmd;

	if (sdio->blksz[fn] != blksz) {
		if (sdio_set_blksz(hdl, fn, blksz) != SDIO_SUCCESS)
			return (SDIO_FAILURE);
	}

	cmd.opcode   = 53;
	cmd.rsptype  = SDIO_RSP_R5;
	cmd.eflags   = SDMMC_CMD_DATA | (blkcnt > 1 ? SDMMC_CMD_DATA_MULTI : 0);
	cmd.argument = MAKE_SDIO_OFFSET(address) |
				    MAKE_SDIO_OP_CODE(opcode) |
				    MAKE_SDIO_BLOCK_MODE(1) |
				    MAKE_SDIO_FUNCTION(fn) |
				    MAKE_SDIO_DIR(1) | (blkcnt & 0x01FF);
	cmd.blkcnt   = blkcnt;			// block count;
	cmd.size     = blksz;			// block size 

	if (sdio->eflags & SDMMC_CAP_DMA) {
		CACHE_FLUSH(&sdio->cachectl, buffer, paddr, blksz * blkcnt);

		if (sdio->setup_dma(sdio->hchdl, paddr, nbytes, MMC_DIR_OUT) != nbytes)
			return (SDIO_FAILURE);

		cmd.eflags |= SDMMC_CMD_DATA_DMA;
		if (sdio_send_command(sdio, &cmd) != MMC_SUCCESS)
			return (SDIO_FAILURE);

		if (sdio->dma_done(sdio->hchdl, 0) != MMC_SUCCESS)
			return (SDIO_FAILURE);
	} else {
		if (sdio->setup_pio(sdio->hchdl, (char *)buffer, nbytes, MMC_DIR_OUT) != nbytes)
			return (SDIO_FAILURE);

		cmd.data = buffer;			// buffer pointer

		if (sdio_send_command(sdio, &cmd) != MMC_SUCCESS)
			return (SDIO_FAILURE);

		if (sdio->pio_done(sdio->hchdl, (char *)buffer, nbytes, MMC_DIR_OUT) != nbytes)
			return (SDIO_FAILURE);
	}

	return (SDIO_SUCCESS);
}

__SRCVERSION( "$URL$ $Rev$" );
