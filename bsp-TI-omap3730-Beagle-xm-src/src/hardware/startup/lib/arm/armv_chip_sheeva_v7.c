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



#include "startup.h"

const struct armv_chip armv_chip_sheeva_v7 = {
	0x5810,									// cpuid
	"Sheeva(v7)",							// name
	ARM_MMU_CR_I|ARM_MMU_CR_Z,				// mmu_cr
	0,										// mmu_cr_clr
	2,										// cycles	FIXME
	&armv_cache_sheeva_v7,					// cache
	&power_sheeva_v7,						// power
	&page_flush_sheeva_v7,					// flush
	&page_flush_deferred_sheeva_v7,			// deferred
	&armv_pte_v7wb,							// pte
	&armv_pte_v7wa,							// pte_wa
	&armv_pte_v7wb,							// pte_wb
	&armv_pte_v7wt,							// pte_wt
	armv_setup_sheeva_v7,					// setup
};


__SRCVERSION( "$URL: http://svn.ott.qnx.com/product/trunk/hardware/startup/lib/arm/armv_chip_sheeva_v7.c $ $Rev: 217585 $" );
