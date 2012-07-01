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

int sdio_set_blksz(void *hdl, int func, int blksz)
{
	sdio_ext_t	*sdio = (sdio_ext_t *)hdl;
	uint8_t		reg;

	// Config block size
	if (sdio_cmd52_write(sdio, SDIO_FN_BLOCK_SIZE_0_REG(func), 0, blksz & 0xFF) != SDIO_SUCCESS) 
		return (SDIO_FAILURE);
	if (sdio_cmd52_read(sdio, SDIO_FN_BLOCK_SIZE_0_REG(func), 0, &reg) != SDIO_SUCCESS)
		return (SDIO_FAILURE);
	if (reg != (blksz & 0xFF))
		return (SDIO_FAILURE);
	if (sdio_cmd52_write(sdio, SDIO_FN_BLOCK_SIZE_1_REG(func), 0, blksz >> 8) != SDIO_SUCCESS) 
		return (SDIO_FAILURE);
	if (sdio_cmd52_read(sdio, SDIO_FN_BLOCK_SIZE_1_REG(func), 0, &reg) != SDIO_SUCCESS)
		return (SDIO_FAILURE);
	if (reg != (blksz >> 8))
		return (SDIO_FAILURE);

	sdio->blksz[func] = blksz;

	return SDIO_SUCCESS;
}

/*
 */
int sdio_enable_func(void *hdl, sdio_dev_t *dev, uint16_t blksz)
{
	sdio_ext_t	*sdio = (sdio_ext_t *)hdl;
	uint8_t		i;

	if (dev->vid != 0xFFFF && dev->vid != sdio->dev_vid) {
		return (SDIO_FAILURE);
	}

	if (dev->did != 0xFFFF && dev->did != sdio->dev_did) {
		return (SDIO_FAILURE);
	}

	if (dev->fun == -1) {
		if (dev->ccd == 0xFF) {
			return (SDIO_FAILURE);
		}

		for (i = 0; i <= sdio->nfunc; i++) {
			if (sdio->func[i].ccd == dev->ccd) {
				dev->fun = i;
				break;
			}
		}
		if (i > sdio->nfunc) {
			return (SDIO_FAILURE);
		}
	} else {
		if (dev->fun > sdio->nfunc) {
			return (SDIO_FAILURE);
		}
		else {
			dev->ccd = sdio->func[dev->fun].ccd;
		}
	}

	if (sdio_set_blksz(hdl, dev->fun, blksz) != SDIO_SUCCESS) {
		return (SDIO_FAILURE);
	}
	
	if (sdio_func_intr(hdl, dev->fun,1) != SDIO_SUCCESS) {
		return (SDIO_FAILURE);
	}
	
	sdio->block_size(sdio->hchdl, blksz);

	return SDIO_SUCCESS;
}


__SRCVERSION( "$URL: http://community.qnx.com/svn/repos/internal_core_networking/branches/6.4.1/trunk/sys/dev_nda/mv8688uap/lib/sdio/sdio_enable_func.c $ $Rev: 252 $" );
