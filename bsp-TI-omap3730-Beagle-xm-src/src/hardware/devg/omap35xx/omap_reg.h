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

#ifndef __OMAP35XX_REG_H_INCLUDED
#define __OMAP35XX_REG_H_INCLUDED

#define DISPC_INTERRUPT_PULSE	0x5b

#define dptr(x) 	(uint32_t volatile *)(((unsigned char volatile *)o_ctx->dispregptr) + x)
#define dsiptr(x) 	    (uint32_t volatile *)(((unsigned char volatile *)o_ctx->dsiptr) + x)
#define dsi_pll_ptr(x) 	 (uint32_t volatile *)(((unsigned char volatile *)o_ctx->dsi_pll_ptr) + x)
#define pcrmptr(x)	(uint32_t volatile *)(((unsigned char volatile *)o_ctx->pcrmregptr) + x)
#define gpio1ptr(x) (uint32_t volatile *)(((unsigned char volatile *)o_ctx->gpio1ptr) + x)
#define gpio5ptr(x) (uint32_t volatile *)(((unsigned char volatile *)o_ctx->gpio5ptr) + x)
#define gpio6ptr(x) (uint32_t volatile *)(((unsigned char volatile *)o_ctx->gpio6ptr) + x)
#define padptr(x) 	(uint32_t volatile *)(((unsigned char volatile *)o_ctx->padptr) + x)
#define intptr(x)	(uint32_t volatile *)(((unsigned char volatile *)o_ctx->intregptr) + x)

#define OMAP_BASEREGSIZE	0x1000
#define OMAP_DISPREGBASE	0x48050000
#define OMAP_DSIREGBASE	    0x4804FC00
#define OMAP_DSIPLLREGBASE	0x4804FF00
#define OMAP_PCRMREGBASE	0x48004000

#define OMAP_MMUREGBASE		0x5d000000
#define OMAP_PADCONFBASE	0x48002030
#define OMAP_PINTREGBASE	0x48200000

/* BITMASK */
#define OMAP_BURST_MASK 0x3
#define OMAP_THRESHOLD_MASK 0x3ff


#define OMAP_GPIO1_BASE		0x48310000
#define OMAP_GPIO1_SIZE		0x1000

#define OMAP_GPIO5_BASE		0x49056000
#define OMAP_GPIO5_SIZE		0x1000

#define OMAP_GPIO6_BASE		0x49058000
#define OMAP_GPIO6_SIZE		0x1000

/* LCD controller register offsets */
#define OMAP_DSS_SYSCONFIG			dptr(0x10)
#define OMAP_DSS_SYSCSTATUS			dptr(0x14)
#define OMAP_DSS_CONTROL			dptr(0x40)
#define OMAP_DSS_SDI_CONTROL        dptr(0x44)
#define OMAP_DSS_PLL_CONTROL        dptr(0x48)
#define OMAP_DSS_PSA_LCD_REG_1		dptr(0x50)
#define OMAP_DSS_PSA_LCD_REG_2		dptr(0x54)
#define OMAP_DSS_PSA_VIDEO_REG		dptr(0x58)
#define OMAP_DSS_STATUS				dptr(0x5C)


#define OMAP_DISPC_SYSCONFIG	    dptr(0x410)
#define OMAP_DISPC_SYSSTATUS		dptr(0x414)
#define OMAP_DISPC_IRQSTATUS		dptr(0x418)
#define OMAP_DISPC_IRQENABLE		dptr(0x41C)
#define OMAP_DISPC_CONTROL			dptr(0x440)
#define OMAP_DISPC_CONFIG			dptr(0x444)

/* 
 * LCDALPHABLENDERENABLED  (1<<18)  Selects the alpha blender overlay manager
 * FUNCGATED               (1<<9)   Functional clocks gated enabled (might not want this?)
 * LOADMODE                (0x2<<1) Frame data only loaded every frame.
*/
#define OMAP_DISPC_CONFIG_INIT_STATE  ((1<<18) | (1<<9) | (0x2<<1))


#define OMAP_DISPC_CAPABLE			dptr(0x448)
#define OMAP_DISPC_GLOBAL_ALPHA		dptr(0x474)
#define OMAP_DISPC_DEF_COLOR	    dptr(0x44C)
#define OMAP_DISPC_TRANS_COLOR		dptr(0x454)
#define OMAP_DISPC_LINE_STATUS		dptr(0x45C)
#define OMAP_DISPC_LINE_NUMBER		dptr(0x460)
#define OMAP_DISPC_TIMING_H			dptr(0x464)
#define OMAP_DISPC_TIMING_V			dptr(0x468)
#define OMAP_DISPC_POL_FREQ			dptr(0x46C)
#define OMAP_DISPC_DIVISOR			dptr(0x470)
#define OMAP_DISPC_SIZE_LCD			dptr(0x47C)
#define OMAP_DISPC_GFX_BA0			dptr(0x480)
#define OMAP_DISPC_GFX_BA1			dptr(0x484)
#define OMAP_DISPC_GFX_POSITION		dptr(0x488)
#define OMAP_DISPC_GFX_SIZE			dptr(0x48C)
#define OMAP_DISPC_GFX_ATTRIBUTES	dptr(0x4A0)
#define OMAP_DISPC_GFX_FIFO_THRESHOLD dptr(0x4A4)
#define OMAP_DISPC_GFX_FIFO_SIZE    dptr(0x4A8)
#define OMAP_DISPC_GFX_TABLE_BA		dptr(0x4B8)
#define OMAP_DISPC_GFX_ROW_INC		dptr(0x4AC)
#define OMAP_DISPC_GFX_PIXEL_INC	dptr(0x4B0)
#define OMAP_DISPC_SIZE_DIG			dptr(0x478)

/* Video buffer */
#define OMAP_DISPC_VID1_BA(i)  dptr(0x4BC +(i*0x04))
#define OMAP_DISPC_VID2_BA(i)  dptr(0x4BC+0x90+(i*0x04))
#define OMAP_DISPC_VID1_POSITION  dptr(0x4C4)
#define OMAP_DISPC_VID2_POSITION  dptr(0x4C4 +0x90)
#define OMAP_DISPC_VID1_SIZE  dptr(0x4C8)
#define OMAP_DISPC_VID2_SIZE  dptr(0x4C8+0x90)
#define OMAP_DISPC_VID1_ATTRIBUTES  dptr(0x4CC)
#define OMAP_DISPC_VID2_ATTRIBUTES  dptr(0x4CC+0x90)
#define OMAP_DISPC_VID1_FIFO_THRESHOLD  dptr(0x4D0)
#define OMAP_DISPC_VID2_FIFO_THRESHOLD  dptr(0x4D0+0x90)
#define OMAP_DISPC_VID1_FIFO_SIZE_STATUS  dptr(0x4D4)
#define OMAP_DISPC_VID2_FIFO_SIZE_STATUS  dptr(0x4D4+0x90)
#define OMAP_DISPC_VID1_ROW_INC  dptr(0x4D8)
#define OMAP_DISPC_VID2_ROW_INC  dptr(0x4D8+0x90)
#define OMAP_DISPC_VID1_PIXEL_INC  dptr(0x4DC)
#define OMAP_DISPC_VID2_PIXEL_INC  dptr(0x4DC+0x90)
#define OMAP_DISPC_VID1_FIR  dptr(0x4E0)
#define OMAP_DISPC_VID2_FIR  dptr(0x4E0+0x90)
#define OMAP_DISPC_VID1_PICTURE_SIZE  dptr(0x4E4)
#define OMAP_DISPC_VID2_PICTURE_SIZE  dptr(0x4E4+0x90)
#define OMAP_DISPC_VID1_ACCUi  dptr(0x4E8) 
#define OMAP_DISPC_VID2_ACCUi  dptr(0x4E8+0x90) 
#define OMAP_DISPC_VID1_FIR_COEF_H(i)  dptr(0x4F0+(i*0x08))
#define OMAP_DISPC_VID2_FIR_COEF_H(i)  dptr(0x4F0+0x90+(i*0x08))
#define OMAP_DISPC_VID1_FIR_COEF_HV(i)  dptr(0x4F4+(i*0x08))
#define OMAP_DISPC_VID2_FIR_COEF_HV(i)  dptr(0x4F4+0x90+(i*0x08))
#define OMAP_DISPC_VID1_CONV_COEF0  dptr(0x530)
#define OMAP_DISPC_VID2_CONV_COEF0  dptr(0x530+0x90)
#define OMAP_DISPC_VID1_CONV_COEF1  dptr(0x534)
#define OMAP_DISPC_VID2_CONV_COEF1  dptr(0x534+0x90)
#define OMAP_DISPC_VID1_CONV_COEF2  dptr(0x538)
#define OMAP_DISPC_VID2_CONV_COEF2  dptr(0x538+0x90)
#define OMAP_DISPC_VID1_CONV_COEF3  dptr(0x53C)
#define OMAP_DISPC_VID2_CONV_COEF3  dptr(0x53C+0x90)
#define OMAP_DISPC_VID1_CONV_COEF4  dptr(0x540)
#define OMAP_DISPC_VID2_CONV_COEF4  dptr(0x540+0x90)
#define OMAP_DISPC_DATA_CYCLE(i)  dptr(0x5D4+(i*0x04))
#define OMAP_DISPC_VID1_FIR_COEF_V(i)  dptr(0x5E0+(i*0x04))
#define OMAP_DISPC_VID2_FIR_COEF_V(i)  dptr(0x5E0+0x20+(i*0x04))
#define OMAP_DISPC_CPR_COEF_R  dptr(0x620)
#define OMAP_DISPC_CPR_COEF_G  dptr(0x624)
#define OMAP_DISPC_CPR_COEF_B  dptr(0x628)
#define OMAP_DISPC_GFX_PRELOAD  dptr(0x62C)
#define OMAP_DISPC_VID1_PRELOAD  dptr(0x630)
#define OMAP_DISPC_VID2_PRELOAD  dptr(0x630+0x04)

#define OMAP_DISPC_VID_FIR_COEF_H(n,i)    dptr(0x4F0+(n*0x90)+(i*0x08))
#define OMAP_DISPC_VID_FIR_COEF_HV(n,i)   dptr(0x4F4+(n*0x90)+(i*0x08))
#define OMAP_DISPC_VID_FIR_COEF_V(n,i)    dptr(0x5E0+(n*0x20)+(i*0x04))
#define OMAP_DISPC_VID_CONV_COEF(n,i)     dptr(0x530+(n*0x90)+(i*0x04))

/* RFBI */
#define OMAP_RFBI_SYSCFG		dptr(0x810)
#define OMAP_RFBI_SYSSTATUS		dptr(0x814)
#define OMAP_RFBI_CTRL			dptr(0x840)
#define OMAP_RFBI_PIXCNT		dptr(0x844)
#define OMAP_RFBI_LINENUM		dptr(0x848)
#define OMAP_RFBI_CONFIG(x)		dptr(0x860+x*0x18)
#define OMAP_RFBI_DATACYCLE0(x) 	dptr(0x868+x*0x18)
#define OMAP_RFBI_DATACYCLE1(x) 	dptr(0x86C+x*0x18)
#define OMAP_RFBI_DATACYCLE2(x) 	dptr(0x870+x*0x18)
#define OMAP_RFBI_DATACYCLE3(x) 	dptr(0x874+x*0x18)
#define OMAP_RFBI_VSYNC_WIDTH		dptr(0x890)
#define OMAP_RFBI_HSYNC_WIDTH		dptr(0x894)

#define OMAP_DISPC_IRQ_FRAMEDONE	0
#define OMAP_DISPC_IRQ_VSYNC	1
#define OMAP_DISPC_IRQ_PROGLINE	5
#define OMAP_DISPC_IRQ_SYNCLOST  14

/* PADCONF */
#define OMAP_CONTROL_PADCONF_DSS_PCLK     padptr(0xa4)
#define OMAP_CONTROL_PADCONF_DSS_VSYNC    padptr(0xa8)
#define OMAP_CONTROL_PADCONF_DSS_DATA0    padptr(0xac)
#define OMAP_CONTROL_PADCONF_DSS_DATA2    padptr(0xb0)
#define OMAP_CONTROL_PADCONF_DSS_DATA4    padptr(0xb4)
#define OMAP_CONTROL_PADCONF_DSS_DATA6    padptr(0xb8)
#define OMAP_CONTROL_PADCONF_DSS_DATA8    padptr(0xbc)
#define OMAP_CONTROL_PADCONF_DSS_DATA10   padptr(0xc0)
#define OMAP_CONTROL_PADCONF_DSS_DATA12   padptr(0xc4)
#define OMAP_CONTROL_PADCONF_DSS_DATA14   padptr(0xc8)
#define OMAP_CONTROL_PADCONF_DSS_DATA16   padptr(0xcc)
#define OMAP_CONTROL_PADCONF_DSS_DATA18   padptr(0xd0)
#define OMAP_CONTROL_PADCONF_DSS_DATA20   padptr(0xd4)
#define OMAP_CONTROL_PADCONF_DSS_DATA22   padptr(0xd8)
#define OMAP_CONTROL_PADCONF_I2C3_SDA     padptr(0x194)
#define OMAP_CONTROL_PADCONF_MCBSP4_CLKX  padptr(0x154)
#define OMAP_CONTROL_PADCONF_MCBSP4_DX    padptr(0x158)

#define OMAP_CONTROL_DEVCONF1			  padptr(0x2a8)
#define OMAP_CONTROL_PADCONF_SYS_NRESWARM padptr(0x9D8)
#define OMAP_CONTROL_PADCONF_SYS_BOOT1    padptr(0x9DC)
#define OMAP_CONTROL_PADCONF_SYS_BOOT3    padptr(0x9E0)
#define OMAP_CONTROL_PADCONF_SYS_BOOT5    padptr(0x9E4)

/* PRM CLK registers */

#define OMAP_CONTROL_PADCONF_MCSPI1_CS1  padptr(0x1A0)
#define OMAP_CONTROL_PADCONF_MCSPI1_CS3  padptr(0x1A4)

/* PRM CLK registers */

#define OMAP_CM_CLKSEL1_PLL_IVA2 		pcrmptr(0x040)
#define OMAP_CM_CLKEN_PLL 				pcrmptr(0xD00)
#define OMAP_CM_IDLEST_CKGEN			pcrmptr(0xD20)
#define OMAP_CM_AUTOIDLE_PLL 			pcrmptr(0xD30)
#define OMAP_CM_CLKSEL1_PLL				pcrmptr(0xD40)
#define OMAP_CM_CLKSEL2_PLL				pcrmptr(0xD44)

#define OMAP_CM_FCLKEN_DSS			pcrmptr(0xe00)
#define OMAP_CM_ICLKEN_DSS			pcrmptr(0xe10)
#define OMAP_CM_CLKSEL_DSS			pcrmptr(0xe40)
#define OMAP_CM_CLKSTST_DSS			pcrmptr(0xe4c)

#define OMAP_PCRM_FCLKEN1_CORE		pcrmptr(0xa00)
#define OMAP_PCRM_ICLKEN1_CORE		pcrmptr(0xa10)
#define OMAP_PCRM_CLKEN_PLL		pcrmptr(0xd00)

/* MPU Clock/Reset/Power Mode Control registers */

/* GPIO BANK 1*/
#define OMAP_GPIO1_SYSCONFIG	    gpio1ptr(0x10)
#define OMAP_GPIO1_SYSSTATUS	    gpio1ptr(0x14)
#define OMAP_GPIO1_CTRL			    gpio1ptr(0x30)
#define OMAP_GPIO1_OE			    gpio1ptr(0x34)
#define OMAP_GPIO1_DATAOUT		    gpio1ptr(0x3c)
#define OMAP_GPIO1_CLEARIRQENABL    gpio1ptr(0x70)
#define OMAP_GPIO1_CLEARWKUENA      gpio1ptr(0x80)
#define OMAP_GPIO1_CLEARDATAOUT		gpio1ptr(0x90)
#define OMAP_GPIO1_SETDATAOUT		gpio1ptr(0x94)


/* GPIO BANK 5 */
#define OMAP_GPIO5_SYSCONFIG	    gpio5ptr(0x10)
#define OMAP_GPIO5_SYSSTATUS	    gpio5ptr(0x14)
#define OMAP_GPIO5_CTRL			    gpio5ptr(0x30)
#define OMAP_GPIO5_OE			    gpio5ptr(0x34)
#define OMAP_GPIO5_DATAOUT		    gpio5ptr(0x3c)
#define OMAP_GPIO5_CLEARIRQENABL    gpio5ptr(0x70)
#define OMAP_GPIO5_CLEARWKUENA      gpio5ptr(0x80)
#define OMAP_GPIO5_CLEARDATAOUT		gpio5ptr(0x90)
#define OMAP_GPIO5_SETDATAOUT		gpio5ptr(0x94)

/* GPIO BANK 6 (for AM3517EVM) */
#define OMAP_GPIO6_SYSCONFIG      gpio6ptr(0x10)
#define OMAP_GPIO6_SYSSTATUS      gpio6ptr(0x14)
#define OMAP_GPIO6_CTRL           gpio6ptr(0x30)
#define OMAP_GPIO6_OE             gpio6ptr(0x34)
#define OMAP_GPIO6_DATAOUT        gpio6ptr(0x3c)
#define OMAP_GPIO6_CLEARIRQENABL  gpio6ptr(0x70)
#define OMAP_GPIO6_CLEARWKUENA    gpio6ptr(0x80)
#define OMAP_GPIO6_CLEARDATAOUT   gpio6ptr(0x90)
#define OMAP_GPIO6_SETDATAOUT     gpio6ptr(0x94)


/* Interrupt Registers */
#define OMAP_ITR			intptr(0x80)
#define OMAP_MIR			intptr(0x84)
#define OMAP_MIR_CLEAR			intptr(0x88)
#define OMAP_MIR_SET			intptr(0x8C)
#define OMAP_ITR_SET			intptr(0x90)
#define OMAP_ITR_CLEAR			intptr(0x94)
#define OMAP_IRQ_PEND			intptr(0x98)
#define OMAP_ILR_DSS			intptr(0x100 + 0x64)
#define OMAP_DSSIRQ			25

/* LCD DMA Control bits */
#define LCD_SOURCE_IMIF		0x40
#define LCD_SOURCE_EMIFF	0x00
#define BUS_ERROR_IT_COND	0x20
#define FRAME2_IT_COND		0x10
#define FRAME1_IT_COND		0x08
#define	BUS_ERROR_IT_IE		0x04
#define FRAME_IT_IE		0x02
#define	FRAME_MODE_DOUBLE	0x01 
#define FRAME_MODE_SINGLE	0x00

#define OMAP_VENC_STATUS    dptr(0x0C04)
#define OMAP_VENC_F_CONTROL    dptr(0x0C08)
#define OMAP_VENC_VIDOUT_CTRL    dptr(0x0C10)
#define OMAP_VENC_SYNC_CTRL    dptr(0x0C14)
#define OMAP_VENC_LLEN    dptr(0x0C1C)
#define OMAP_VENC_FLENS    dptr(0x0C20)
#define OMAP_VENC_HFLTR_CTRL    dptr(0x0C24)
#define OMAP_VENC_CC_CARR_WSS_CARR    dptr(0x0C28)
#define OMAP_VENC_C_PHASE    dptr(0x0C2C)
#define OMAP_VENC_GAIN_U    dptr(0x0C30)
#define OMAP_VENC_GAIN_V    dptr(0x0C34)
#define OMAP_VENC_GAIN_Y    dptr(0x0C38)
#define OMAP_VENC_BLACK_LEVEL    dptr(0x0C3C)
#define OMAP_VENC_BLANK_LEVEL    dptr(0x0C40)
#define OMAP_VENC_X_COLOR    dptr(0x0C44)
#define OMAP_VENC_M_CONTROL    dptr(0x0C48)
#define OMAP_VENC_BSTAMP_WSS_DATA    dptr(0x0C4C)
#define OMAP_VENC_S_CARR    dptr(0x0C50)
#define OMAP_VENC_LINE21    dptr(0x0C54)
#define OMAP_VENC_LN_SEL    dptr(0x0C58)
#define OMAP_VENC_L21_WC_CTL    dptr(0x0C5C)
#define OMAP_VENC_HTRIGGER_VTRIGGER    dptr(0x0C60)
#define OMAP_VENC_SAVID_EAVID    dptr(0x0C64)
#define OMAP_VENC_FLEN_FAL    dptr(0x0C68)
#define OMAP_VENC_LAL_PHASE_RESET    dptr(0x0C6C)
#define OMAP_VENC_HS_INT_START_STOP_X    dptr(0x0C70)
#define OMAP_VENC_HS_EXT_START_STOP_X    dptr(0x0C74)
#define OMAP_VENC_VS_INT_START_X    dptr(0x0C78)
#define OMAP_VENC_VS_INT_STOP_X_VS_INT_START_Y    dptr(0x0C7C)
#define OMAP_VENC_VS_INT_STOP_Y_VS_EXT_START_X    dptr(0x0C80)
#define OMAP_VENC_VS_EXT_STOP_X_VS_EXT_START_Y    dptr(0x0C84)
#define OMAP_VENC_VS_EXT_STOP_Y    dptr(0x0C88)
#define OMAP_VENC_AVID_START_STOP_X    dptr(0x0C90)
#define OMAP_VENC_AVID_START_STOP_Y    dptr(0x0C94)
#define OMAP_VENC_FID_INT_START_X_FID_INT_START_Y    dptr(0x0CA0)
#define OMAP_VENC_FID_INT_OFFSET_Y_FID_EXT_START_X    dptr(0x0CA4)
#define OMAP_VENC_FID_EXT_START_Y_FID_EXT_OFFSET_Y    dptr(0x0CA8)
#define OMAP_VENC_TVDETGP_INT_START_STOP_X    dptr(0x0CB0)
#define OMAP_VENC_TVDETGP_INT_START_STOP_Y    dptr(0x0CB4)
#define OMAP_VENC_GEN_CTRL    dptr(0x0CB8)
#define OMAP_VENC_OUTPUT_CONTROL    dptr(0x0CC4)
#define OMAP_VENC_OUTPUT_TEST    dptr(0x0CC8)


/* DSI */
#define OMAP_DSI_SYSCONFIG			dsiptr(0x0)
#define OMAP_DSI_SYSSTATUS			dsiptr(0x4)
#define OMAP_DSI_CLK_CTRL			dsiptr(0x54)


/* DSI PLL */
#define OMAP_DSI_PLL_CONTROL                          dsi_pll_ptr(0x0)
#define OMAP_DSI_PLL_STATUS	                        dsi_pll_ptr(0x4)
#define OMAP_DSI_PLL_GO	                              dsi_pll_ptr(0x8)
#define OMAP_DSI_PLL_CONFIGURATION1	                  dsi_pll_ptr(0xc)
#define OMAP_DSI_PLL_CONFIGURATION2	                  dsi_pll_ptr(0x10)


#endif 

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/devg/omap35xx/omap_reg.h $ $Rev: 308279 $" )
