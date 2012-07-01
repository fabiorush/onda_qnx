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






/*
 * TI OMAP3530 specific timer support.
 * GPT1 is used.
 */

#include "startup.h"
#include <arm/omap3530.h>


extern struct callout_rtn   timer_load_omap3530;
extern struct callout_rtn   timer_value_omap3530;
extern struct callout_rtn   timer_reload_omap3530;

static uintptr_t	timer_base;

static const struct callout_slot	timer_callouts[] = {
	{ CALLOUT_SLOT(timer_load, _omap3530) },
	{ CALLOUT_SLOT(timer_value, _omap3530) },
	{ CALLOUT_SLOT(timer_reload, _omap3530) },
};

static unsigned
timer_start_omap3530()
{
	out32(timer_base + OMAP3530_GPT_TCLR, 
		in32(timer_base + OMAP3530_GPT_TCLR) | 1);

	return in32(timer_base + OMAP3530_GPT_TCRR);
}

static unsigned
timer_diff_omap3530(unsigned start)
{
	unsigned now = in32(timer_base + OMAP3530_GPT_TCRR);

	return (now - start);
}

void
init_qtime_omap3530()
{
	struct qtime_entry	*qtime = alloc_qtime();
	uint32_t			prcm_wkup_base = OMAP3530_PRCM_WKUP_CM;
	
	/*
	 * Enable GPT1 clock, select sys_clk as timer input clock
	 */
	out32(prcm_wkup_base + OMAP3530_CM_FCLKEN_WKUP,
		in32(prcm_wkup_base + OMAP3530_CM_FCLKEN_WKUP) | 1);
	out32(prcm_wkup_base + OMAP3530_CM_ICLKEN_WKUP,
		in32(prcm_wkup_base + OMAP3530_CM_ICLKEN_WKUP) | 1);
	out32(prcm_wkup_base + OMAP3530_CM_CLKSEL_WKUP,
		in32(prcm_wkup_base + OMAP3530_CM_CLKSEL_WKUP) | 1);

	while ((in32(prcm_wkup_base + OMAP3530_CM_IDLEST_WKUP) & 1));

	/*
	 * Map GPT1 registers
	 */
	timer_base = startup_io_map(OMAP3530_GPT_SIZE, OMAP3530_GPT1_BASE);

	/* Reset the timer */
	out32(timer_base + OMAP3530_GPT_TIOCP_CFG, 0x02);

	/* Wait for reset to complete */
	while (!(in32(timer_base + OMAP3530_GPT_TISTAT)) & 1)
		;

	/* Clear timer count and reload count */
	out32(timer_base + OMAP3530_GPT_TLDR, 0);
	out32(timer_base + OMAP3530_GPT_TCRR, 0);

	/*
	 * Setup GPT1
	 * Auto-reload enable
	 * Prescaler disable
	 * Stop timer, timer_load will enable it
	 */
	out32(timer_base + OMAP3530_GPT_TCLR, (1 << 1));

	timer_start = timer_start_omap3530;
	timer_diff  = timer_diff_omap3530;

	qtime->intr = 37;	/* GPT1 irq */

#define	OMAP_CLOCK_FREQ		13000000UL
#define	OMAP_CLOCK_RATE		76923077UL
#define	OMAP_CLOCK_SCALE	-15
	qtime->timer_rate = OMAP_CLOCK_RATE;
	qtime->timer_scale = OMAP_CLOCK_SCALE;
	qtime->cycles_per_sec = (uint64_t)OMAP_CLOCK_FREQ;

	add_callout_array(timer_callouts, sizeof(timer_callouts));
}


__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/startup/boards/omap3530/init_qtime_omap3530.c $ $Rev: 308279 $" );
