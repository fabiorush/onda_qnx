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

#include <sys/slog.h>

#include "proto.h"

static void sdio_interrupt(sdio_ext_t *sdio)
{
	sdio->istatus = sdio->iprocess(sdio->hchdl, sdio->cmd);

	if (sdio->istatus) {
		if (sdio->istatus & ~SDMMC_INT_SERVICE) {
			if (sdio->wait_cmd == 1) {
				pthread_sleepon_lock();
				sdio->wait_cmd++;
				pthread_sleepon_signal(&sdio->wait_cmd);
				pthread_sleepon_unlock();
			}
		}
		if (sdio->istatus & SDMMC_INT_SERVICE) {
			// FIXME We need to queue this event if we are not the 
			// owner of this event, otherwise we could lose interrupt
			if (sdio->wait_srv == 1) {
				pthread_sleepon_lock();
				sdio->wait_srv++;
				pthread_sleepon_signal(&sdio->wait_srv);
				pthread_sleepon_unlock();
			} else {
				atomic_add(&sdio->pend_srv, 1);
			}
		}
	}
}

int sdio_event_get(void *hdl, int wait)
{
	sdio_ext_t	*sdio = (sdio_ext_t *)hdl;
	uint64_t	tmo;
	int			ret = SDIO_SUCCESS;

	if (sdio->pend_srv) {
		atomic_sub(&sdio->pend_srv, 1);
		return (SDIO_SUCCESS);
	}

	// TODO!!!
	// Review the timing to enable interrupt
	if (!wait)
		return (SDIO_FAILURE);

	sdio->wait_srv = 1;
	tmo = (uint64_t)wait * 1000 * 1000 * 1000;

	/* No pending event, wait */
	sdio->ienable(sdio->hchdl, 1);
	pthread_sleepon_lock();
	if (sdio->wait_srv == 1) {
		if (pthread_sleepon_timedwait(&sdio->wait_srv, tmo) != EOK) {

			ret = SDIO_FAILURE;
		}
	}
	pthread_sleepon_unlock();

	if (ret != SDIO_FAILURE)
		sdio->ienable(sdio->hchdl, 0);

	sdio->wait_srv = 0;

	return (ret);
}

void *sdio_event_handler(void *data)
{
	struct _pulse	pulse;
	iov_t			iov;
	int				rcvid;
	sdio_ext_t		*sdio = (sdio_ext_t *)data;

	// In case there is something has to be initialized in the event handler
	if (sdio->hdl_init)
		sdio->hdl_init(sdio->hchdl);

	SETIOV(&iov, &pulse, sizeof(pulse));

	while (1) {
		if ((rcvid = MsgReceivev(sdio->chid, &iov, 1, NULL)) == -1)
			continue;

		switch (pulse.code) {
			case SDIO_PULSE:
				sdio_interrupt(sdio);
				break;
			default:
				if (rcvid)
					MsgReplyv(rcvid, ENOTSUP, &iov, 1);
				break;
		}
	}

	return (NULL);
}

__SRCVERSION( "$URL$ $Rev$" );
