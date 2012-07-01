/*
 * $QNXLicenseC:
 * Copyright 2008, QNX Software Systems. 
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





#include "startup.h"

void	vstart(uintptr_t syspageptr, unsigned entry_point, unsigned cpu);

void
cpu_startnext(uintptr_t eip, unsigned cpu)
{
	if (debug_flag) {
		kprintf("cpu_startnext: cpu%d -> %x\n", cpu, eip);
	}

	/*
	 * Call the next program, passing syspage pointer in r0
	 */
	if (shdr->flags1 & STARTUP_HDR_FLAGS1_VIRTUAL) {
		vstart((uintptr_t)lsp.system_private.p->kern_syspageptr, eip, cpu);
	}
	((void (*)(void *, unsigned))eip)(lsp.system_private.p->kern_syspageptr, cpu);
}


__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/startup/lib/arm/cpu_startnext.c $ $Rev: 308279 $" );
