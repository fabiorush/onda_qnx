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
#include <sys/mman.h>

#include "omap.h"


#define OMAP_DISPC_MAX_FREQ    173000
#define OMAP_DSI_PLL_SYSCLK     13000
#define OMAP_DSI_PLL_FINT_MIN1    750
#define OMAP_DSI_PLL_FINT_MAX1   2100
#define OMAP_DSI_PLL_FINT_MIN2   7500
#define OMAP_DSI_PLL_FINT_MAX2   2100
#define OMAP_DSI_PLL_N_MAX       0x7f
#define OMAP_DSI_PLL_M_MAX      0x7ff
#define OMAP_DSI_PLL_M3_MAX       0xf
#define OMAP_DSI_PLL_WAIT_CNT    1000

#define OMAP_DISPC_MAX_CLK_KHz     72000
#define OMAP_DISPC_PLL_REAL_HIGH_INIT_DIFF 0xffffffff

int program_pll(disp_adapter_t *adapter, unsigned req_freq) {
	omap_ctx_t *o_ctx= adapter->ms_ctx;
	unsigned n = 1;
	unsigned m = 0;
	unsigned m3 = 0;
	unsigned fint = 0;
	unsigned pll_freq = 0;
	unsigned m3_out_freq = 0;
	unsigned m3_out_diff = 0;
	unsigned best_diff = OMAP_DISPC_PLL_REAL_HIGH_INIT_DIFF;
	unsigned best_fint = 0; 
	unsigned best_n = 0; 
	unsigned best_m = 0; 
	unsigned best_m3 = 0; 
	unsigned pll_program_wait_lock;
	TRACE;
	DEBUG_CMD(disp_printf(adapter,"req_freq %u", req_freq));

	if (req_freq > OMAP_DISPC_MAX_FREQ) { 
		DEBUG_MSG("PLL freq out of range (greater 173Mhz)");
		return -1;
	}

	for (n = 1; n < OMAP_DSI_PLL_N_MAX; n++) {
			fint = OMAP_DSI_PLL_SYSCLK / (n+1);
			if ( fint < OMAP_DSI_PLL_FINT_MIN1  || fint > OMAP_DSI_PLL_FINT_MAX2 
				  || (fint > OMAP_DSI_PLL_FINT_MAX1 && fint < OMAP_DSI_PLL_FINT_MIN2)) {
				continue;
			}
			for (m = 1; m < OMAP_DSI_PLL_M_MAX; ++m) {
				pll_freq = (2 * m * fint);
				//DEBUG_CMD(disp_printf(adapter,"fint %u n %u m %u pll_freq %u", fint ,n ,m, pll_freq));
				m3 = OMAP_DSI_PLL_M3_MAX;
				m3_out_freq = pll_freq / m3;
				if (m3_out_freq > OMAP_DISPC_MAX_FREQ) {
					break;
				}
				for (; m3 > 0; m3--) {
					m3_out_freq = pll_freq / m3;
					if (m3_out_freq > OMAP_DISPC_MAX_FREQ) {
						break;
					}
					m3_out_diff = abs(m3_out_freq - req_freq);
					if (m3_out_diff < best_diff) {
						best_diff = m3_out_diff;
						best_fint = fint;
						best_n = n;
						best_m = m;
						best_m3 = m3;
						//DEBUG_CMD(disp_printf(adapter,"** Found new best fit n %u m %u m3 %u diff %u freq %u", n, m, m3, m3_out_diff, m3_out_freq));
					}
				}
			}
	}
	if (best_diff == OMAP_DISPC_PLL_REAL_HIGH_INIT_DIFF) {
		DEBUG_MSG("DSI PLL requested frequency out of range");
		return -1;
	}

	DEBUG_CMD(disp_printf(adapter,"Program fint %u n %u m %u m3 %u diff %u", best_fint, best_n, best_m, best_m3, best_diff));
	if (((best_diff *100) / req_freq) > 5) {
		DEBUG_MSG("PLL frequency greater than +/- 5%");
	}
	/* Fint */
	if (best_fint < OMAP_DSI_PLL_FINT_MIN1) {
		DEBUG_MSG("PLL Fint out of range, below .75Mhz should not be here");
		return -1;
	} else if (best_fint < 1000) {
		fint = 0x3;
	} else if (best_fint < 1250) {
		fint = 0x4;
	} else if (best_fint < 1500) {
		fint = 0x5;
	} else if (best_fint < 1750) {
		fint = 0x6;
	} else if (best_fint < 2100) {
		fint = 0x7;
	} else if (best_fint < 7500) {
		DEBUG_MSG("PLL Fint out of range, between 2.1 and 7.5Mhz, should not be here");
		return -1;
	} else if (best_fint < 10000) {
		fint = 0xB;
	} else if (best_fint < 12500) {
		fint = 0xC;
	} else if (best_fint < 15000) {
		fint = 0xD;
	} else if (best_fint < 17500) {
		fint = 0xE;
	} else if (best_fint < 21000) {
		fint = 0xF;
	} else {
		DEBUG_MSG("PLL Fint out of range, greater than 21MHz, should not be here");
		return -1;
	}

	/* DSI PLL */
	*OMAP_DSI_PLL_CONTROL = 0x0;

	/* OMAP_DSI_PLL_CONFIGURATION1 
	 * 22:19 DSS_CLOCK_DIV      Divider value for DSS clock source M3REG
	 *  18:8 DSI_PLL_REGM       M Divider for PLL
	 *   7:1 DSI_PLL_REGN       N Divider for PLL (Reference)
	 *     0 DSI_PLL_STOPMODE   DSI PLL STOPMODE
	 */
	*OMAP_DSI_PLL_CONFIGURATION1 =  ((best_m3-1) << 19) | (best_m << 8) | (best_n << 1) | (1<<0); 
	DEBUG_CMD(disp_printf(adapter,"Program DSI PLL n %d m %d m3 %d", best_n, best_m, best_m3));

	/* OMAP_DSI_PLL_CONFIGURATION2
	 * 20  DSI_HSDIVBYPASS      Forces HSDIVIDER to bypass mode 0x1: bypass mode.
	 * 19  DSI_PROTO_CLOCK_PWDN Power down for DSI Protocol Engine clock source 0x1: clock divider is powered-down
	 * 18  DSI_PROTO_CLOCK_EN   Enable for DSI Protocol Engine clock source 0x1: clock divider is enabled
	 * 17  DSS_CLOCK_PWDN       Power down for DSS clock source 0x1: is powered-down
	 * 16  DSS_CLOCK_EN         Enable for DSS clock source  0x1: divider is enabled
	 * 15  DSI_BYPASSEN         Selects DSS functional clock as DSIPHY clock source   
	 *                           0x1: Force DSS functional clock to be used as DSIPHY clock source
	 * 14  DSIPHY_CLKINEN       DSIPHY clock control 0x1: is enabled
	 * 13  DSI_PLL_REFEN        PLL reference clock control 0x1: clock enabled
	 * 12  DSI_PLL_HIGHFREQ     Enables a division of pixel clock by 2 before input to the PLL 0x1: is divided by 2
	 * 11  DSI_PLL_CLKSEL       Reference clock selection 
	 *                           0x0: Selects DSS2_ALWON_FCLK as PLL reference clock
	 *                           0x1: Selects Pixel Clock (PCLKFREE) as PLL reference clock
	 * 10:9  DSI_PLL_LOCKSEL    Selects the lock criteria for the PLL
	 *                           0x1: Frequency Lock
	 *                           0x2: Spare
	 * 8  DSI_PLL_DRIFTGUARDEN  DSI PLL DRIFTGUARDEN 0x1: Temperature drift will initiate automatic recalibration.
	 * 7  DSI_PLL_TIGHTPHASELOCK DSI PLL Phase Lock criteria
	 *                           0x1: Tightenened phase lock criteria Phase error lower than 3.2 %
	 * 6  DSI_PLL_LOWCURRSTBY    PLL LOW CURRENT STANDBY 0x1: is selected
	 * 5  DSI_PLL_PLLLPMODE      Select the power / performance of the PLL 0x1: Reduced power, increased jitter
	 * 4:1   DSI_PLL_FREQSEL     PLL internal reference frequency range selection 
	 *                            0x3: 0.75MHz to 1.0MHz
	 *                            0x4: 1.0MHz to 1.25MHz
	 *                            0x5: 1.25MHz to 1.5MHz
	 *                            0x6: 1.5MHz to 1.75MHz
	 *                            0x7: 1.75MHz to 2.1MHz
	 *                            0xB: 7.5MHz to 10MHz
	 *                            0xC: 10MHz to 12.5MHz
	 *                            0xD: 12.5MHz to 15MHz
	 *                            0xE: 15MHz to 17.5MHz
	 *                            0xF: 17.5MHz to 21MHz
	 * 0  DSI_PLL_IDLE           DSI PLL IDLE: 0x1: IDLE is selected
	 */
	*OMAP_DSI_PLL_CONFIGURATION2 =  (1<<20) | (1<<13) | (fint<<1); 

	/*  DSI_PLL_GO Request (re-)locking sequence of the PLL.
	 *  0x1: Request PLL (re-)locking/locking pending
	 */
	*OMAP_DSI_PLL_GO = (1<<0);

	for (pll_program_wait_lock = OMAP_DSI_PLL_WAIT_CNT; pll_program_wait_lock--; ) {
		usleep(100);
		/* 1 DSI_PLL_LOCK DSI PLL Lock status
		 * See the programming guide for the use of this bit
		 * 0x0: PLL is not locked
		 * 0x1: PLL is locked
		 */
		if (*OMAP_DSI_PLL_STATUS & (1<<1)) 
			break;
	}
	if (pll_program_wait_lock == 0) {
		DEBUG_MSG("DSI PLL lock timed out");
		return -1;
	}
	*OMAP_DSI_PLL_CONFIGURATION2 =  (1<< 16) | (1<<14) | (1<<13) | (fint<<1); 

	return 1;
}

/* sets TWL4030 power control chip on Mistral for graphics (valid for beagle but not required)
*/
int twl4030_setup_graphics(disp_adapter_t *adapter);
int twl4030_enable_dvi(disp_adapter_t *adapter);

int omap_mode_init(disp_adapter_t *adapter, char *optstring) {
	omap_ctx_t *o_ctx;


	if (adapter->shmem == NULL) {
		if ((o_ctx = calloc(1, sizeof (*o_ctx))) == NULL) {
			return -1;
		}
		o_ctx->free_context = 1;
	} else {
		o_ctx = adapter->shmem;
		memset(o_ctx, 0, sizeof (*o_ctx));
	}

	adapter->ms_ctx = o_ctx;
	adapter->vsync_counter = &o_ctx->vsync_counter;
	adapter->caps |= DISP_CAP_2D_ACCEL | DISP_CAP_3D_ACCEL;
	o_ctx->adapter = adapter;

	/* Register with the display utility lib */
	if (disp_register_adapter(adapter) == -1) {
		if (o_ctx->free_context)
			free(o_ctx);
		return -1;
	}

	get_config_data(o_ctx, optstring);

	TRACE;
	o_ctx->width = o_ctx->ppl;
	o_ctx->height = o_ctx->lpp;

	if ((o_ctx->dispregptr = disp_mmap_device_memory(OMAP_DISPREGBASE,
			OMAP_BASEREGSIZE, DISP_PROT_READ | DISP_PROT_WRITE
					| DISP_PROT_NOCACHE, 0)) == 0) {
		goto fail;
	}
	if ((o_ctx->dsiptr = disp_mmap_device_memory(OMAP_DSIREGBASE,
			OMAP_BASEREGSIZE, DISP_PROT_READ | DISP_PROT_WRITE
					| DISP_PROT_NOCACHE, 0)) == 0) {
		goto fail;
	}
	if ((o_ctx->dsi_pll_ptr = disp_mmap_device_memory(OMAP_DSIPLLREGBASE,
			OMAP_BASEREGSIZE, DISP_PROT_READ | DISP_PROT_WRITE
					| DISP_PROT_NOCACHE, 0)) == 0) {
		goto fail;
	}
	if ((o_ctx->mmuregptr = disp_mmap_device_memory(OMAP_MMUREGBASE,
			OMAP_BASEREGSIZE, DISP_PROT_READ | DISP_PROT_WRITE
					| DISP_PROT_NOCACHE, 0)) == 0) {
		goto fail;
	}
	if ((o_ctx->pcrmregptr = disp_mmap_device_memory(OMAP_PCRMREGBASE,
			OMAP_BASEREGSIZE, DISP_PROT_READ | DISP_PROT_WRITE
					| DISP_PROT_NOCACHE, 0)) == 0) {
		free(o_ctx);
		goto fail;
	}
	if ((o_ctx->padptr = disp_mmap_device_memory(OMAP_PADCONFBASE,
			OMAP_BASEREGSIZE, DISP_PROT_READ | DISP_PROT_WRITE
					| DISP_PROT_NOCACHE, 0)) == 0) {
		goto fail;
	}
	if ((o_ctx->intregptr = disp_mmap_device_memory(OMAP_PINTREGBASE,
			OMAP_BASEREGSIZE, DISP_PROT_READ | DISP_PROT_WRITE
					| DISP_PROT_NOCACHE, 0)) == 0) {
		goto fail;
	}

	if ((o_ctx->gpio1ptr = disp_mmap_device_memory(OMAP_GPIO1_BASE,
			OMAP_GPIO1_SIZE, DISP_PROT_READ | DISP_PROT_WRITE
					| DISP_PROT_NOCACHE, 0)) == 0) {
		goto fail;
	}

	if ((o_ctx->gpio5ptr = disp_mmap_device_memory(OMAP_GPIO5_BASE,
			OMAP_GPIO5_SIZE, DISP_PROT_READ | DISP_PROT_WRITE
					| DISP_PROT_NOCACHE, 0)) == 0) {
		goto fail;
	}

	if ((o_ctx->gpio6ptr = disp_mmap_device_memory(OMAP_GPIO6_BASE,
			OMAP_GPIO6_SIZE, DISP_PROT_READ | DISP_PROT_WRITE
					| DISP_PROT_NOCACHE, 0)) == 0) {
		goto fail;
	}

	/* configure PADs for 12-24bit dss plus clocks (vsync/hsync/pclk)
	 */

	*OMAP_CONTROL_PADCONF_DSS_PCLK =0x00180018;
	*OMAP_CONTROL_PADCONF_DSS_VSYNC=0x00180018;
	*OMAP_CONTROL_PADCONF_DSS_DATA0=0x00180018;
	*OMAP_CONTROL_PADCONF_DSS_DATA2=0x00180018;
	*OMAP_CONTROL_PADCONF_DSS_DATA4=0x00180018;
	*OMAP_CONTROL_PADCONF_DSS_DATA6=0x00180018;
	*OMAP_CONTROL_PADCONF_DSS_DATA8=0x00180018;
	*OMAP_CONTROL_PADCONF_DSS_DATA10=0x00180018;
	if (o_ctx->lcdfmt > 0) {
		/* 16 bit LCD */
		*OMAP_CONTROL_PADCONF_DSS_DATA12=0x00180018;
		*OMAP_CONTROL_PADCONF_DSS_DATA14=0x00180018;
	}
	if (o_ctx->lcdfmt > 0x1) {
		/* 18 bit LCD */
		*OMAP_CONTROL_PADCONF_DSS_DATA16=0x00180018;
	}
	if (o_ctx->lcdfmt > 0x2) {
		/* 24 bit LCD */
		*OMAP_CONTROL_PADCONF_DSS_DATA18=0x00180018;
		*OMAP_CONTROL_PADCONF_DSS_DATA20=0x00180018;
		*OMAP_CONTROL_PADCONF_DSS_DATA22=0x00180018;
	}

	if (o_ctx->dpowerg70 == 1) {
		DEBUG_MSG("DVI");
		/* Power control for the DVI on Beagle board GPIO_170
		 * CONTROL_PADCONF_I2C3_SDA mode 4
		 */
		*OMAP_CONTROL_PADCONF_I2C3_SDA &= 0x0000ffff;
		*OMAP_CONTROL_PADCONF_I2C3_SDA |= (0x4<<16);
		*OMAP_GPIO6_OE &= 0xFFFFFDFF;
		*OMAP_GPIO6_CLEARWKUENA &= 0xFFFFFDFF;
		*OMAP_GPIO6_CLEARIRQENABL &= 0xFFFFFDFF;
		*OMAP_GPIO6_SETDATAOUT =0x00000400;
	}
	if (o_ctx->disptype == 1 || o_ctx->disptype == 2) {
		int temp;
		DEBUG_MSG("lcd Contorl Mistral 3530evm");
		/* Congifure padds for LCD control on Mistral 3530evm
		 * GPIO_152 LCD_INI
		 * GPIO_153 LCD_EnVDD
		 * GPIO_154 LCD_QVGA/VGA
		 * GPIO_155 LCD_RESB
		 *
		 */
		*OMAP_CONTROL_PADCONF_MCBSP4_CLKX &= 0x0;
		*OMAP_CONTROL_PADCONF_MCBSP4_CLKX |= 0x00040004;
		*OMAP_GPIO5_DATAOUT =0x0d000000;
		*OMAP_GPIO5_OE &= 0xF0FFFFFF;
		*OMAP_GPIO5_CLEARWKUENA &= 0xF0FFFFFF;
		*OMAP_GPIO5_CLEARIRQENABL &= 0xF0FFFFFF;
		*OMAP_GPIO5_SETDATAOUT = 0x08000000;

		/* put LR and UD in GPIO mode */
		temp = *OMAP_CONTROL_PADCONF_SYS_NRESWARM;
		temp &= 0x0000FFFF;
		temp |= 0x00040000;
		*OMAP_CONTROL_PADCONF_SYS_NRESWARM = temp;
		temp = *OMAP_CONTROL_PADCONF_SYS_BOOT1;
		temp &= 0xFFFF0000;
		temp |= 0x00000004;
		*OMAP_CONTROL_PADCONF_SYS_BOOT1 = temp;


		/*
		 * LCD draw direction control for Mistral OMAP3530evm.
		 * GPIO_2 LCD_LR
		 * GPIO_3 LCD_UD
		 */
		if (o_ctx->draw_lr == 1) {
			DEBUG_MSG("DRAW_LR GPIO2 on");
			*OMAP_GPIO1_OE &= 0xFFFFFFFB;
			*OMAP_GPIO1_SETDATAOUT =0x00000004;
		} else {
			*OMAP_GPIO1_CLEARDATAOUT =0x00000004;
			*OMAP_GPIO1_OE |= 0x00000004;
		}
		if (o_ctx->draw_tb == 1) {
			DEBUG_MSG("DRAW_TB GPIO3 on");
			*OMAP_GPIO1_OE &= 0xFFFFFFF7;
			*OMAP_GPIO1_SETDATAOUT =0x00000008;
		} else {
			*OMAP_GPIO1_CLEARDATAOUT =0x00000008;
			*OMAP_GPIO1_OE |= 0x00000008;
		}
	} else if (o_ctx->disptype == 4) {
		int temp;
		/* Display type for AM3517EVM
		 * AM3517EVM
		 *
		 * PAD SPI1 CS2 - GPIO176  - LCD POWER
		 * PAD SPI2 CS0 - GPIO181  - BACKLIGHT PWM
		 * PAD SPI2 CS1 - GPIO182  - BACKLIGHT
		 *
		 */

		temp = *OMAP_CONTROL_PADCONF_MCSPI1_CS1;
      	temp &= 0x0000FFFF;
      	temp |= 0x00040000;
		/* Configure pad SPI2 CS0 and CS1 in GPIO mode */
		*OMAP_CONTROL_PADCONF_MCSPI1_CS3 = 0x00040004;

		/* For LCD power configure GPIO176 as output and driver it */
		/* For LCD PWM configure GPIO181 as output and driver it */
		/* For LCD backlight configure GPIO176 as output and driver it */
		*OMAP_GPIO6_OE        &= 0xFF9EFFFF;
		*OMAP_GPIO6_SETDATAOUT = 0x00610000;
	} else if (o_ctx->disptype == 5) {
		// Using BeagleBoard-xM DVI adapter - enable it
		unsigned int temp;
		*OMAP_CONTROL_PADCONF_DSS_DATA0  = 0x00070007;
		*OMAP_CONTROL_PADCONF_DSS_DATA2  = 0x00070007;
		*OMAP_CONTROL_PADCONF_DSS_DATA4  = 0x00070007;
		*OMAP_CONTROL_PADCONF_DSS_DATA18 = 0x001B001B;
		*OMAP_CONTROL_PADCONF_DSS_DATA20 = 0x001B001B;
		*OMAP_CONTROL_PADCONF_DSS_DATA22 = 0x001B001B;

		temp = *OMAP_CONTROL_PADCONF_SYS_NRESWARM;
		temp &= 0x0000FFFF;
		temp |= 0x00030000;
		*OMAP_CONTROL_PADCONF_SYS_NRESWARM = temp;
		temp = *OMAP_CONTROL_PADCONF_SYS_BOOT1;
		temp &= 0xFFFF0000;
		temp |= 0x00000003;
		*OMAP_CONTROL_PADCONF_SYS_BOOT1 = temp;

		*OMAP_CONTROL_PADCONF_SYS_BOOT3 = 0x00030003;
		*OMAP_CONTROL_PADCONF_SYS_BOOT5 = 0x00030003;

		// Now switch the PMIC GPIO to enable the DVI
		twl4030_enable_dvi(adapter);
	}


	/* setup vpll2 and lcd backlight for graphics  (may not be needed for beagle as it should already be setup by u-boot ) */
	if (o_ctx->tw4030 == 1)
		twl4030_setup_graphics(adapter);

	/*
	* bit 9  GFXGammaTableCapable
	*      8  GFXLayerCapable
	*      7  GFXTransDstCapable
	*      6  STNditheringCapable
	*      5  TFTditheringCapable
	*      4  VidTransSrcCapable
	*      3  VidLayerCapable
	*      2  VidVertFIRCapable
	*      1  VidHorFIRCapable
	*     0  VidCapable
	*/
	*OMAP_DISPC_CAPABLE = (0<<9) | (1<<8) | (1<<7)| (0<<6) | (0<<5) | (1<<4) | (1<<3) | (0<<2) | (0<<1) | 1;

	/* setup clocks */
	*OMAP_CM_FCLKEN_DSS |= 0x7;
	*OMAP_CM_ICLKEN_DSS |= 1;

	/* reset display subsystems */
	*OMAP_DSS_SYSCONFIG |= 2;
	while ((*OMAP_DSS_SYSCSTATUS & 1) == 0) {

	}
	/* reset display controler */
	*OMAP_DISPC_SYSCONFIG = 1<<1;
	while ((*OMAP_DISPC_SYSSTATUS & 1) == 0) {

	}

	/*VENC reset  */
	*OMAP_VENC_F_CONTROL = 1<<8;
	while ((*OMAP_VENC_F_CONTROL & (1<<8)) != 0) {
	}

	/*	DISPC_CLK_SWITCH Selects the clock source for the DISPC functional clock RW 0
	 * 	0x0: DSS1_ALWON_FCLK clock is selected (from PRCM)
	 *	   0x1: DSI1_PLL_FCLK clock is selected (from DSI PLL)
	 */
	if (o_ctx->use_pll == 0) {
		DEBUG_MSG("DSS source clock DSS1_ALWON_FCLK");
		*OMAP_DSS_CONTROL &= 0xFFFFFFFE;
	} else {
		/*
		 * 31:30 PLL_PWR_CMD Command for power control of the DSI PLL Control module
       *           0x0: Command to change to OFF state
       *           0x1: Command to change to ON state for PLL only (HSDIVISER is OFF)
       *           0x2: Command to change to ON state for both PLL and HSDIVISER
       *           0x3: Command to change to ON state for both PLL and HSDIVISER (no clock output to the DSI complex I/O)
		 */
		*OMAP_DSI_CLK_CTRL |= (0x2<<30);
		if (o_ctx->pll_use_customtiming != 0) {
			DEBUG_MSG("Use custom timing");
			if (program_pll(adapter, (o_ctx->pll_freq*2)) == -1) {
				goto fail;
			}
		}
		DEBUG_MSG("DSS source clock DSS1_PLL_FCLK");
		*OMAP_DSS_CONTROL =  (*OMAP_DSS_CONTROL & 0xFFFFFFFE) | 0x3;
	}

	/* OMAP_DISPC_CONTROL bits
	 * LCDENABLESIGNALLCD (1<<27) Enable Signal: LCD interface enabled
	 * PCKFREEENABLE      (1<<26) Pixel clock free-running enabled
	 * GPOUT              (1<<15)   &
	 * GPOUT0             (1<<16)   Bypass RFBI
	 * TFTDATALINES       (0x3<<8)
	 * 	 	Number of lines of the LCD interface RW 0x0
	 * 			0x0: 12-bit output
	 * 			0x1: 16-bit output
	 * 			0x2: 18-bit output
	 * 			0x3: 24-bit output
	 * STNFT         (1<<3)    Active Matrix display
	 * LCDENABLE     (1)       LCD output enabled
	 */
	*OMAP_DISPC_CONTROL = (1<<27) | (1<<26) | (1<<16) | (1<<15)
			| (o_ctx->lcdfmt<<8) | (1<<3) | 1;

	/* Global alpha must be set in alpha mode or all layers transparent
	 *
	 * VID2GLOBALALPHA		0-transparent 0xff fully opaque
	 * GFXGLOBALALPHA       0-transparent 0xff fully opaque;
	 */
	*OMAP_DISPC_GLOBAL_ALPHA = 0xff<<16 | 0xff;

	/* clear IRQ STATUS */
	*OMAP_DISPC_IRQSTATUS = 0x1ffff;

	/* setup yuv to rgb conversions */
	if (o_ctx->ccoef >= OMAP_CUSTOM_COLOR_COEFFICIENTS) {
		set_custom_color_coefficients(adapter, 0, o_ctx->ccry, o_ctx->ccrcr, o_ctx->ccrcb,
		                                          o_ctx->ccgy, o_ctx->ccgcr, o_ctx->ccgcb,
												  o_ctx->ccby, o_ctx->ccbcr, o_ctx->ccbcb);
		set_custom_color_coefficients(adapter, 1, o_ctx->ccry, o_ctx->ccrcr, o_ctx->ccrcb,
		                                          o_ctx->ccgy, o_ctx->ccgcr, o_ctx->ccgcb,
												  o_ctx->ccby, o_ctx->ccbcr, o_ctx->ccbcb);

	} else {
		set_yuv_to_rgb_conversion(adapter, 0, o_ctx->ccoef);
		o_ctx->ccfull = set_yuv_to_rgb_conversion(adapter, 1, o_ctx->ccoef);
		if (o_ctx->ccfull == -1) {
			o_ctx->ccfull=0;
			DEBUG_MSG("set_yuv_to_rgb_conversion failed");
		}

	}

	/* Setup layer/ pipeline DMA threasholds */
    *OMAP_DISPC_GFX_FIFO_THRESHOLD  = ((o_ctx->gfx_highthreshold & OMAP_THRESHOLD_MASK) << 16)  | ((o_ctx->gfx_lowthreshold & OMAP_THRESHOLD_MASK) << 0);
    *OMAP_DISPC_VID1_FIFO_THRESHOLD = ((o_ctx->vid1_highthreshold & OMAP_THRESHOLD_MASK) << 16) | ((o_ctx->vid1_lowthreshold & OMAP_THRESHOLD_MASK) << 0);
    *OMAP_DISPC_VID2_FIFO_THRESHOLD = ((o_ctx->vid2_highthreshold & OMAP_THRESHOLD_MASK) << 16) | ((o_ctx->vid2_lowthreshold & OMAP_THRESHOLD_MASK) << 0);


	if (omap_isr_setup(adapter, o_ctx) == -1) {
		goto fail;
	}

	if (o_ctx->vidsize > 0) {
		slogf(21,0,"omap_35xx:  mapping allocator %d bytes at 0x%x.", o_ctx->vidsize, o_ctx->vidbase);
		o_ctx->vidptr = disp_mmap_device_memory(o_ctx->vidbase,
				o_ctx->vidsize, PROT_READ | PROT_WRITE | PROT_NOCACHE, 0);
		if (o_ctx->vidptr == MAP_FAILED) {
			omap_isr_cleanup(adapter, o_ctx);
			goto fail;
		}
	}
	return 1;

	fail: if (o_ctx->dispregptr)
		munmap_device_memory(o_ctx->dispregptr, OMAP_BASEREGSIZE);
	if (o_ctx->dsiptr)
		munmap_device_memory(o_ctx->dsiptr, OMAP_BASEREGSIZE);
	if (o_ctx->dsi_pll_ptr)
		munmap_device_memory(o_ctx->dsi_pll_ptr, OMAP_BASEREGSIZE);
	if (o_ctx->mmuregptr)
		munmap_device_memory(o_ctx->mmuregptr, OMAP_BASEREGSIZE);
	if (o_ctx->pcrmregptr)
		munmap_device_memory(o_ctx->pcrmregptr, OMAP_BASEREGSIZE);
	if (o_ctx->gpio1ptr)
		munmap_device_memory(o_ctx->gpio1ptr, OMAP_GPIO1_SIZE);
	if (o_ctx->gpio5ptr)
		munmap_device_memory(o_ctx->gpio5ptr, OMAP_GPIO5_SIZE);
	if (o_ctx->gpio6ptr)
		munmap_device_memory(o_ctx->gpio6ptr, OMAP_GPIO6_SIZE);
	if (o_ctx->padptr)
		munmap_device_memory(o_ctx->padptr, OMAP_BASEREGSIZE);
	if (o_ctx->intregptr)
		munmap_device_memory(o_ctx->intregptr, OMAP_BASEREGSIZE);
	disp_unregister_adapter(adapter);
	if (o_ctx->free_context) {
		free(o_ctx);
	}

	return -1;
}

void omap_mode_fini(disp_adapter_t *adapter) {
	omap_ctx_t *o_ctx= adapter->ms_ctx;
	TRACE;

	/* disable display */

	omap_isr_cleanup(adapter, o_ctx);

	if (o_ctx) {
		if (o_ctx->dispregptr)
			munmap_device_memory(o_ctx->dispregptr, OMAP_BASEREGSIZE);
		if (o_ctx->dsiptr)
			munmap_device_memory(o_ctx->dsiptr, OMAP_BASEREGSIZE);
		if (o_ctx->dsi_pll_ptr)
			munmap_device_memory(o_ctx->dsi_pll_ptr, OMAP_BASEREGSIZE);
		if (o_ctx->mmuregptr)
			munmap_device_memory(o_ctx->mmuregptr, OMAP_BASEREGSIZE);
		if (o_ctx->pcrmregptr)
			munmap_device_memory(o_ctx->pcrmregptr, OMAP_BASEREGSIZE);
		if (o_ctx->gpio1ptr)
			munmap_device_memory(o_ctx->gpio1ptr, OMAP_GPIO1_SIZE);
		if (o_ctx->gpio5ptr)
			munmap_device_memory(o_ctx->gpio5ptr, OMAP_GPIO5_SIZE);
		if (o_ctx->gpio6ptr)
			munmap_device_memory(o_ctx->gpio6ptr, OMAP_GPIO6_SIZE);
		if (o_ctx->padptr)
			munmap_device_memory(o_ctx->padptr, OMAP_BASEREGSIZE);
		if (o_ctx->intregptr)
			munmap_device_memory(o_ctx->intregptr, OMAP_BASEREGSIZE);
		if (o_ctx->cursor_vptr)
			munmap(o_ctx->cursor_vptr, o_ctx->cursor_size);
		if (o_ctx->free_context) {
			free(o_ctx);
		}
		disp_unregister_adapter(adapter);
	}
}

int omap_set_mode(disp_adapter_t *adapter, int dispno, disp_mode_t mode,
		disp_crtc_settings_t *settings, disp_surface_t *surf, unsigned flags) {
	omap_ctx_t *o_ctx= adapter->ms_ctx;
	int hfp, hbp, hsw;
	int vfp, vbp, vsw;
	int ivs = 0, ihs = 0;
	TRACE;

	if (o_ctx->disptype == 0) {
		/* DVI */
		DEBUG_MSG("DVI");
	} else if (o_ctx->disptype == 1) {
		DEBUG_MSG("QVGA mode");
		*OMAP_GPIO5_SETDATAOUT = 0x04000000;
	} else if (o_ctx->disptype == 2) {
		DEBUG_MSG("VGA mode");
		*OMAP_GPIO5_CLEARDATAOUT = 0x04000000;
	}



	*OMAP_DISPC_DEF_COLOR = 0x0;
	*OMAP_DISPC_TRANS_COLOR = 0x0;
	if (o_ctx->use_pll != 0) { 
		/* Use DSI PLL for display clock */
		if ( o_ctx->pll_use_customtiming == 0) {
			DEBUG_MSG("Use generic timing");
			o_ctx->width = settings->xres; 
			o_ctx->height = settings->yres;
			/* multiple pixel clock by 2, will be divided by 2 LCD divider (PCD) 
			 */
			if (program_pll(adapter, (settings->pixel_clock)*2) == -1) {
				return -1;
			}
			DEBUG_CMD(disp_printf(adapter,"h_blank_start %d h_blank_len %d h_sync_start %d h_sync_len %d", settings->h_blank_start, settings->h_blank_len, settings->h_sync_start, settings->h_sync_len ));
			DEBUG_CMD(disp_printf(adapter,"v_blank_start %d v_blank_len %d v_sync_start %d v_sync_len %d", settings->v_blank_start, settings->v_blank_len, settings->v_sync_start, settings->v_sync_len ));

			if (settings->sync_polarity | DISP_SYNC_POLARITY_V_POS) {
				ivs = 1;
			}
			if (settings->sync_polarity | DISP_SYNC_POLARITY_H_POS) {
				ihs = 1;
			}
			hfp = settings->h_sync_start - settings->h_blank_start;
			hbp = (settings->h_blank_start + settings->h_blank_len) - (settings->h_sync_start + settings->h_sync_len);
			hsw = settings->h_sync_len;
			/* sync with range check if sync is greater than 0x3f, 0x40 includeing -1,
			 * then add to hbp to maintain refresh */
			if (hsw > 0x40) {
				hbp += hsw-0x40;
				hsw = 0x40;
			}
	
			vfp = settings->v_sync_start - settings->v_blank_start;
			vbp = (settings->v_blank_start + settings->v_blank_len) - (settings->v_sync_start + settings->v_sync_len);
			vsw = settings->v_sync_len;
			
			DEBUG_CMD(disp_printf(adapter,"hfp %d hbp %d hsw %d vfp %d vbp %d vsw %d ihs %d ivs %d", hfp, hbp, hsw, vfp, vbp, vsw, ihs, ivs));
		
			if (vfp > 0xff || vbp > 0xff || hfp > 0x100 || hbp > 0x100) { 
				DEBUG_MSG("porch out of range greater than 0x100");
			}	
			if (vsw > 0x40 || hsw > 0x40) { 
				DEBUG_MSG("sync pulse width out of range greater than 0x40");
			}	
			*OMAP_DISPC_TIMING_H = ((hbp-1) << 20) | ((hfp) << 8) | ((hsw-1) & 0x3f);
			*OMAP_DISPC_TIMING_V = (vbp << 20) | (vfp << 8) | ((vsw-1) & 0x3f);
			*OMAP_DISPC_POL_FREQ = ((o_ctx->ieo&0x1) << 15) | ((o_ctx->ipc&0x1) << 14)
					| ((ivs&0x1) << 13) | ((ihs&0x1) << 12) | ((o_ctx->acbi&0xf) << 8)
					| ( o_ctx->acb&0xff);
			/* 2 is lowest PCD allowed */
			*OMAP_DISPC_DIVISOR = (1 << 16) | 0x2;
			*OMAP_DISPC_SIZE_LCD = (settings->yres-1) << 16 | (settings->xres-1);
		} else {
			DEBUG_MSG("PLL use custom timing");
			if (o_ctx->vfp > 0xff || o_ctx->vbp > 0xff || o_ctx->hfp > 0x100 || o_ctx->hbp > 0x100) { 
				DEBUG_MSG("porch out of range greater than 0x100");
			}	
			if (o_ctx->vsw > 0x40 || o_ctx->hsw > 0x40) { 
				DEBUG_MSG("sync pulse width out of range greater than 0x40");
			}	
			*OMAP_DISPC_TIMING_H = (((o_ctx->hbp-1) & 0xff) << 20) | (((o_ctx->hfp-1) & 0xff) << 8)
					| ((o_ctx->hsw-1) & 0x3f);
			*OMAP_DISPC_TIMING_V = ((o_ctx->vbp & 0xff) << 20) | ((o_ctx->vfp & 0xff) << 8) | ((o_ctx->vsw-1) & 0x3f);
			*OMAP_DISPC_POL_FREQ = ((o_ctx->phsvs & 0x1) << 17) | ((o_ctx->phsvs_rf & 0x1) << 16) 
								   | ((o_ctx->ieo & 0x1) << 15) | ((o_ctx->ipc & 0x1) << 14)
								   | ((o_ctx->ivs & 0x1) << 13) | ((o_ctx->ihs & 0x1) << 12) | ((o_ctx->acbi & 0xf) << 8)
								   | ( o_ctx->acb & 0xff);			
			*OMAP_DISPC_DIVISOR = (1 << 16) | 2;
			*OMAP_DISPC_SIZE_LCD = (o_ctx->lpp-1) << 16 | (o_ctx->ppl-1);
		}
	} else {
		DEBUG_MSG("Use custom timing");
		if (o_ctx->vfp > 0xff || o_ctx->vbp > 0xff || o_ctx->hfp > 0x100 || o_ctx->hbp > 0x100) { 
			DEBUG_MSG("porch out of range greater than 0x100");
		}	
		if (o_ctx->vsw > 0x40 || o_ctx->hsw > 0x40) { 
			DEBUG_MSG("sync pulse with out of range greater than 0x40");
		}	
		*OMAP_DISPC_TIMING_H = (((o_ctx->hbp-1)&0xff) << 20) | (((o_ctx->hfp-1)&0xff) << 8)
				| ((o_ctx->hsw-1) & 0x3f);
		*OMAP_DISPC_TIMING_V = ((o_ctx->vbp & 0xff) << 20) | ((o_ctx->vfp & 0xff) << 8) | ((o_ctx->vsw-1) & 0x3f);
		*OMAP_DISPC_POL_FREQ = ((o_ctx->ieo & 0x1) << 15) | ((o_ctx->ipc & 0x1) << 14)
				| ((o_ctx->ivs & 0x1) << 13) | ((o_ctx->ihs & 0x1) << 12) | ((o_ctx->acbi & 0xf) << 8)
				| ( o_ctx->acb & 0xff);
		*OMAP_DISPC_DIVISOR = (1 << 16) | o_ctx->pcd;
		*OMAP_DISPC_SIZE_LCD = (o_ctx->lpp-1) << 16 | (o_ctx->ppl-1);
	}

	/*
	 * Use OMAP_DISPC_LINE_NUMBER for vsync to avoid buffer timing/synchronization issue with video
	 * Set line number interrupt to vertical front portch (VFP) the point where shadow registers get programmed.
	 * The new buffer becomes active and old buffer becomes free.  This is to fix a problem with video where the
	 * video decoder believes a buffer is free but it is actually still being displayed. Vsync can occur before shadow regiesters hit.
	 *
	 * PROGRAMMEDLINENUMBER interrupt programmed  Vertical front portch.
	 */
	*OMAP_DISPC_LINE_NUMBER = ((o_ctx->lpp-1));// + (o_ctx->vbp)) & 0x7ff;

	*OMAP_DISPC_CONFIG = OMAP_DISPC_CONFIG_INIT_STATE;

	return 0;
}

int omap_get_modeinfo(disp_adapter_t *adapter, int dispno,
		disp_mode_t attributes, disp_mode_info_t *info) {
	omap_ctx_t *o_ctx= adapter->ms_ctx;
	TRACE;

	memset(info, 0, sizeof(*info));
	info->size = sizeof(*info);
	info->mode = attributes;

	switch (attributes) {
	case 16:
		info->pixel_format = DISP_SURFACE_FORMAT_RGB565;
		break;
	case 24:
		info->pixel_format = DISP_SURFACE_FORMAT_RGB888;
		break;
	default:
		return -1;
	}

	if (o_ctx->use_pll && !o_ctx->pll_use_customtiming) {
		DEBUG_MSG("Generic PLL programming");
		/* divided by 2, for minimum divider (PCD) value for LCD  
		 * switch to KHz divded 1000 */
		info->u.generic.min_pixel_clock = (OMAP_DSI_PLL_FINT_MIN1/2);
		info->u.generic.max_pixel_clock = OMAP_DISPC_MAX_CLK_KHz;
		info->flags |= DISP_MODE_GENERIC;
   	info->u.generic.h_granularity = 1; 
   	info->u.generic.v_granularity = 1; 
	} else {
		info->xres = o_ctx->width;
		info->yres = o_ctx->height;
		info->u.fixed.refresh[0] = 60;
		info->u.fixed.refresh[1] = 0;
	}
	return 0;
}

int omap_get_modelist(disp_adapter_t *adapter, int dispno,
		unsigned short *list, int index, int size) {
	omap_ctx_t *o_ctx= adapter->ms_ctx;
	int i=0;
	TRACE;

	switch (index) {
	case 0:
		list[i++] = 16;
	case 1:
		list[i++] = 24;
	}

	list[i] = DISP_MODE_LISTEND;

	return 0;
}

int omap_wait_vsync(disp_adapter_t *adapter, int dispno) {
	iov_t iov;
	struct _pulse pulse;
	uint64_t halfsecond = 500*1000*1000;
	omap_ctx_t *o_ctx= adapter->ms_ctx;
	TRACE;

	atomic_add(&o_ctx->want_vsync_pulse, 1);

	if (adapter->callback) {
		adapter->callback(adapter->callback_handle, DISP_CALLBACK_UNLOCK, NULL);
	}

	/* Bit 5  PROGRAMMEDLINENUMER ProgrammedLineNumber */
	*OMAP_DISPC_IRQSTATUS = (1<<5);
	*OMAP_DISPC_IRQENABLE = (1<<5);
	*OMAP_MIR_CLEAR = (1<<25);

	SETIOV(&iov, &pulse, sizeof (pulse));

	while (1) {
		TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, NULL, &halfsecond,
		NULL);
		if (MsgReceivev(o_ctx->chid, &iov, 1, NULL) == -1) {
			disp_perror(adapter, "MsgReceive");
			break;
		} else if (pulse.code == DISPC_INTERRUPT_PULSE) {
			break;
		}
	}
	if (adapter->callback) {
		adapter->callback(adapter->callback_handle, DISP_CALLBACK_LOCK, NULL);
	}

	return 0;
}

int devg_get_modefuncs(disp_adapter_t *adapter, disp_modefuncs_t *funcs,
		int tabsize) {
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, init, omap_mode_init, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, fini, omap_mode_fini, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, module_info, omap_module_info,
			tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, get_modeinfo, omap_get_modeinfo,
			tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, get_modelist, omap_get_modelist,
			tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, set_mode, omap_set_mode, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, wait_vsync, omap_wait_vsync, tabsize);

	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_query, omap_layer_query,
			tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_enable, omap_layer_enable,
			tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_disable, omap_layer_disable,
			tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_set_surface,
			omap_layer_set_surface, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_set_source_viewport,
			omap_layer_set_source_viewport, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_set_dest_viewport,
			omap_layer_set_dest_viewport, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_set_blending,
			omap_layer_set_blending, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_set_chromakey,
			omap_layer_set_chromakey, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_update_begin,
			omap_layer_update_begin, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_update_end,
			omap_layer_update_end, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_reset, omap_layer_reset,
			tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, layer_set_alpha_map,
			omap_layer_set_alpha_map, tabsize);

	DISP_ADD_FUNC(disp_modefuncs_t, funcs, set_hw_cursor, omap_set_hw_cursor,
			tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, enable_hw_cursor,
			omap_enable_hw_cursor, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, disable_hw_cursor,
			omap_disable_hw_cursor, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs, set_hw_cursor_pos,
			omap_set_hw_cursor_pos, tabsize);
	DISP_ADD_FUNC(disp_modefuncs_t, funcs,
        layer_flushrect, omap_layer_flushrect, tabsize);

	return 0;
}

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/devg/omap35xx/mode.c $ $Rev: 308279 $" );
