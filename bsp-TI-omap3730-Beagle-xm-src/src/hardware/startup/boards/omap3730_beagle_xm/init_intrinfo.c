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
 * TI OMAP3530 interrupt controller support.
 */

#include "startup.h"
#include <arm/omap.h>
#include <arm/omap2420.h>
#include <arm/omap3530.h>

extern int mtp;
extern int teb;

extern struct callout_rtn	interrupt_id_omap3_sdma;
extern struct callout_rtn	interrupt_eoi_omap3_sdma;
extern struct callout_rtn	interrupt_mask_omap3_sdma;
extern struct callout_rtn	interrupt_unmask_omap3_sdma;

static paddr_t omap3530_intc_base = OMAP3530_INTC_BASE;
static paddr_t omap3530_gpio1_base = OMAP3530_GPIO1_BASE;
static paddr_t omap3530_gpio2_base = OMAP3530_GPIO2_BASE;
static paddr_t omap3530_gpio3_base = OMAP3530_GPIO3_BASE;
static paddr_t omap3530_gpio4_base = OMAP3530_GPIO4_BASE;
static paddr_t omap3530_gpio6_base = OMAP3530_GPIO6_BASE;
static paddr_t omap3530_sdma_base = 0x48056000;

const static struct startup_intrinfo	intrs[] = {
	/* primary interrupt controller */
	{	_NTO_INTR_CLASS_EXTERNAL, 	// vector base
		96,							// number of vectors
		_NTO_INTR_SPARE,			// cascade vector
		0,							// CPU vector base
		0,							// CPU vector stride
		0,							// flags
		{ INTR_GENFLAG_LOAD_SYSPAGE,	0, &interrupt_id_omap2420},
		{ INTR_GENFLAG_LOAD_SYSPAGE | INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_omap2420},
		&interrupt_mask_omap2420,	// mask   callout
		&interrupt_unmask_omap2420,	// unmask callout
		0,							// config callout
		&omap3530_intc_base
	},
	/* SDMA 3 interrupt */
	{	256,						// vector base
		32,							// number of vectors
		15,                         // cascade vector, SDMA_IRQ3
		0,							// CPU vector base
		0,							// CPU vector stride
		0,							// flags

		{ 0, 0, &interrupt_id_omap3_sdma },
		{ INTR_GENFLAG_LOAD_INTRMASK,	0, &interrupt_eoi_omap3_sdma },
		&interrupt_mask_omap3_sdma,	// mask   callout
		&interrupt_unmask_omap3_sdma,	// unmask callout
		0,							// config callout
		&omap3530_sdma_base,
	},
};
const static struct startup_intrinfo	gpio1intrs[] = {
	/* GPIO 1 interrupt */
	{	128,						// vector base
		32, 						// number of vectors
		29, 						// cascade vector
		0,							// CPU vector base
		0,							// CPU vector stride
		0,							// flags
	
		{ 0, 0, &interrupt_id_omap2420_gpio },
		{ INTR_GENFLAG_LOAD_INTRMASK,	0, &interrupt_eoi_omap2420_gpio },
		&interrupt_mask_omap2420_gpio,	// mask   callout
		&interrupt_unmask_omap2420_gpio,	// unmask callout
		0,							// config callout
		&omap3530_gpio1_base,
	},
};

const static struct startup_intrinfo	gpio2intrs[] = {
	/* GPIO 2 interrupt */
	{	160,						// vector base
		32, 						// number of vectors
		30, 						// cascade vector
		0,							// CPU vector base
		0,							// CPU vector stride
		0,							// flags
	
		{ 0, 0, &interrupt_id_omap2420_gpio },
		{ INTR_GENFLAG_LOAD_INTRMASK,	0, &interrupt_eoi_omap2420_gpio },
		&interrupt_mask_omap2420_gpio,	// mask   callout
		&interrupt_unmask_omap2420_gpio,	// unmask callout
		0,							// config callout
		&omap3530_gpio2_base,
	},
};

const static struct startup_intrinfo	gpio3intrs[] = {
	/* GPIO 3 interrupt */
	{	192,						// vector base
		32, 						// number of vectors
		31, 						// cascade vector
		0,							// CPU vector base
		0,							// CPU vector stride
		0,							// flags
	
		{ 0, 0, &interrupt_id_omap2420_gpio },
		{ INTR_GENFLAG_LOAD_INTRMASK,	0, &interrupt_eoi_omap2420_gpio },
		&interrupt_mask_omap2420_gpio,	// mask   callout
		&interrupt_unmask_omap2420_gpio,	// unmask callout
		0,							// config callout
		&omap3530_gpio3_base,
	},
};

const static struct startup_intrinfo	gpio4intrs[] = {
	/* GPIO 4 interrupt */
	{	224,						// vector base
		32, 						// number of vectors
		32, 						// cascade vector
		0,							// CPU vector base
		0,							// CPU vector stride
		0,							// flags
	
		{ 0, 0, &interrupt_id_omap2420_gpio },
		{ INTR_GENFLAG_LOAD_INTRMASK,	0, &interrupt_eoi_omap2420_gpio },
		&interrupt_mask_omap2420_gpio,	// mask   callout
		&interrupt_unmask_omap2420_gpio,	// unmask callout
		0,							// config callout
		&omap3530_gpio4_base,
	},
};
const static struct startup_intrinfo	gpio6intrs[] = {
	/* GPIO 6 interrupt */
	{	160,						// vector base
		32,							// number of vectors
		34,                         // cascade vector
		0,							// CPU vector base
		0,							// CPU vector stride
		0,							// flags

		{ 0, 0, &interrupt_id_omap2420_gpio },
		{ INTR_GENFLAG_LOAD_INTRMASK,	0, &interrupt_eoi_omap2420_gpio },
		&interrupt_mask_omap2420_gpio,	// mask   callout
		&interrupt_unmask_omap2420_gpio,	// unmask callout
		0,							// config callout
		&omap3530_gpio6_base,
	},
};

void
init_intrinfo()
{
	int	i;
	uint32_t reg = 0;

	reg |= GCR_ARBITRATION_RATE;
	reg |= DMA_DEFAULT_FIFO_DEPTH;
	out32(omap3530_sdma_base + OMAP3530_DMA4_GCR_REG, in32(omap3530_sdma_base + OMAP3530_DMA4_GCR_REG) | reg);

	/*
	 * Reset interrupt control module
	 */
	out32(omap3530_intc_base + OMAP2420_INTC_SYSCONFIG, 1 << 1);
	while (!(in32(omap3530_intc_base + OMAP2420_INTC_SYSSTATUS) & 1))
		;

	/*
	 * Mask off all the interrupts
	 */
	out32(omap3530_intc_base + OMAP2420_INTC_MIR_SET0, 0xFFFFFFFF);
	out32(omap3530_intc_base + OMAP2420_INTC_MIR_SET1, 0xFFFFFFFF);
	out32(omap3530_intc_base + OMAP2420_INTC_MIR_SET2, 0xFFFFFFFF);
	if(mtp){
		if(teb){
			out32(omap3530_gpio1_base + OMAP_GPIO_CLEAR_IRQENABLE1, 0xFFFFFFFF);
		}else{
			out32(omap3530_gpio2_base + OMAP_GPIO_CLEAR_IRQENABLE1, 0xFFFFFFFF);
			out32(omap3530_gpio3_base + OMAP_GPIO_CLEAR_IRQENABLE1, 0xFFFFFFFF);
		}
		out32(omap3530_gpio4_base + OMAP_GPIO_CLEAR_IRQENABLE1, 0xFFFFFFFF);
	}else{
		out32(omap3530_gpio6_base + OMAP_GPIO_CLEAR_IRQENABLE1, 0xFFFFFFFF);
	}
	
	/*
	 * Set the priority to the highest and route to IRQ
	 */
	for (i = OMAP2420_INTC_ILR(0); i <= OMAP2420_INTC_ILR(95); i += 4)
		out32(omap3530_intc_base + i, 0);

	if(mtp){
		out32(omap3530_gpio4_base + OMAP_GPIO_IRQSTATUS1, 0xFFFFFFFF);
		if(teb){
			out32(omap3530_gpio1_base + OMAP_GPIO_IRQSTATUS1, 0xFFFFFFFF);	
			//GPIO 15 V850 IPC_ACK falling edge -- vector 128+15 = 143
			out32(OMAP3530_GPIO1_BASE + OMAP2420_GPIO_FALLINGDETECT, (1 << 15));
			//GPIO 108 RDS tuner interrupt, falling edge -- vector 224+12 =236
			//GPIO 107 V850 IPC_REQ, falling edge -- vector 224+11 =235
			out32(OMAP3530_GPIO4_BASE + OMAP2420_GPIO_FALLINGDETECT, (0x3 << 11));
		}else{
			out32(omap3530_gpio2_base + OMAP_GPIO_IRQSTATUS1, 0xFFFFFFFF);	
			out32(omap3530_gpio3_base + OMAP_GPIO_IRQSTATUS1, 0xFFFFFFFF);
			//GPIO 42 Ethernet IRQ, Active level high -- vector 160+10 =170
			out32(OMAP3530_GPIO2_BASE + OMAP2420_GPIO_LEVELDETECT0, (1 << 10));
			//GPIO 95 ADV7180 IRQ, Active level high -- vector 192+31 =223
			out32(OMAP3530_GPIO3_BASE + OMAP2420_GPIO_LEVELDETECT1, (1 << 31));
			//GPIO 107 V850 IPC IRQ, Active level high -- vector 224+11 =235
			out32(OMAP3530_GPIO4_BASE + OMAP2420_GPIO_LEVELDETECT1, (1 << 11));
		}
	}else{
		out32(omap3530_gpio6_base + OMAP_GPIO_IRQSTATUS1, 0xFFFFFFFF);
		/*
		 * Ethernet IRQ, low level
		 */
		out32(OMAP3530_GPIO6_BASE + OMAP2420_GPIO_LEVELDETECT0, (1 << 16));
	}

	// Config SDMA interrupt
	out32(omap3530_sdma_base + 0x2C, 2);	// Software reset
	while (in32(omap3530_sdma_base + 0x2C) & 2)
		;
	for (i = 0x08; i <= 0x14; i += 4) {
		out32(omap3530_sdma_base + i + 0x10, 0);	// Mask
		out32(omap3530_sdma_base + i, 0xFFFFFFFF);	// Clear
	}

	omap3530_sdma_base += 0x14;		// We use SDMA IRQ3

	add_interrupt_array(intrs, sizeof(intrs));
	if(mtp){
		if(teb){
			add_interrupt_array(gpio1intrs, sizeof(gpio1intrs));
		}else{
			add_interrupt_array(gpio2intrs, sizeof(gpio2intrs));
			add_interrupt_array(gpio3intrs, sizeof(gpio3intrs));
		}
		add_interrupt_array(gpio4intrs, sizeof(gpio4intrs));
	}else{
		add_interrupt_array(gpio6intrs, sizeof(gpio6intrs));
	}
}


__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/startup/boards/omap3530/init_intrinfo.c $ $Rev: 308279 $" );
