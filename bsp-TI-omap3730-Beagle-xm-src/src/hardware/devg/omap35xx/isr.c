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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <atomic.h>

#include "omap.h"

/* Interrupt handler */
const struct sigevent * omap35xx_isr(void *arg, int id) {
	omap_ctx_t *o_ctx = arg;
	/* Bit 5  PROGRAMMEDLINENUMER ProgrammedLineNumber */
	if (( (*OMAP_DISPC_IRQSTATUS)&(1<<5) ) != 0) {
		atomic_add(&o_ctx->vsync_counter, 1);
		*OMAP_DISPC_IRQSTATUS = (1<<5);
		*OMAP_DISPC_IRQENABLE &= ~(1<<5);
		*OMAP_MIR_SET = (1<<25);
		return &o_ctx->event;
	}
	return NULL;
}

int omap_isr_setup(disp_adapter_t *adapter, omap_ctx_t *o_ctx) {
	int err;
	TRACE;

	if ((o_ctx->chid = ChannelCreate(_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK))
			== -1) {
		return -1;
	}

	if ((o_ctx->coid = ConnectAttach(0, 0, o_ctx->chid, _NTO_SIDE_CHANNEL, 0))
			== -1) {
		err = errno;
		goto fail;
	}

	SIGEV_PULSE_INIT(&o_ctx->event, o_ctx->coid, o_ctx->adapter->pulseprio,
			DISPC_INTERRUPT_PULSE, 0);

	o_ctx->iid = InterruptAttach(_NTO_INTR_CLASS_EXTERNAL | o_ctx->irq,
			omap35xx_isr, (void *)o_ctx, sizeof(*o_ctx), _NTO_INTR_FLAGS_END
					| _NTO_INTR_FLAGS_TRK_MSK | _NTO_INTR_FLAGS_PROCESS);

	if (o_ctx->iid == -1) {
		err = errno;
		goto fail2;
	}



	return 0;

	fail2: ConnectDetach(o_ctx->coid);

	fail: ChannelDestroy(o_ctx->chid);

	errno = err;

	return -1;
}

void omap_isr_cleanup(disp_adapter_t *adapter, omap_ctx_t *o_ctx) {
	TRACE;
	ConnectDetach(o_ctx->coid);
	ChannelDestroy(o_ctx->chid);
	InterruptDetach(o_ctx->iid);
}

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/devg/omap35xx/isr.c $ $Rev: 308279 $" );
