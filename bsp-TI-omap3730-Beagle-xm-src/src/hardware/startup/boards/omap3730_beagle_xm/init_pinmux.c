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

#define CONTROL_DEVCONF0 0x48002274
#define MMCSDIO1ADPCLKISEL  (1<<24)
#define MCBSP2_CLKS  (1<<6)
#define MCBSP2_CLKS  (1<<6)
#define MCBSP1_CLKS  (1<<2)
#define MCBSP1_FSR 	(1<<4)	/* Receive FS sourced from Transmit FS */
#define MCBSP1_CLKR (1<<3)	/* Receive CLK sourced from Transmit CLK */

#define CONTROL_DEVCONF1 0x480022D8
#define MCBSP3_CLKS (1<<0)

#define CONTROL_PBIAS_LITE  0x48002520
// Status indicating if USIM PBIAS is supplied by 1.8 V or 3.0 
#define PBIASLITESUPPLYHIGH1  (1<<15)
// Status indicating if the software programmed VMODE level 1 => Indicates VMODE_LEVEL 
#define PBIASLITEVMODEERROR1     (1<<11)
// Speed Control for MMC I/O, 1 => 52 MHz IO max speed
#define PBIASSPEEDCTRL1          (1<<10)  
// Input Signal Referenced to VDD. Software has to keep this bit, 0b1 => VDDS stable
#define PBIASLITEPWRDNZ1         (1<<9)  
// VDDS voltage level information control from software, 1 => VDDS = 3.0 V
#define PBIASLITEVMODE1          (1<<8)
// Status indicating if MMC/SD/SDIO1 PBIAS is supplied by 1.8,  1 => PBIAS is supplied by VDDS = 3.0 V
#define PBIASLITESUPPLYHIGH0     (1<<7)
// Status indicating if the software programmed VMODE level, 1 => Indicates VMODE_LEVEL not same as
#define PBIASLITEVMODEERROR0     (1<<3)
//      Speed Control for MMC I/O , 1 => 52 MHz IO max speed
#define PBIASSPEEDCTRL0          (1<<2)
// Input Signal Referenced to VDD. Software has to keep this bit,  1 => VDDS stable
#define PBIASLITEPWRDNZ0        (1<<1)     
//      VDDS voltage level information control from software, 1 => VDDS = 3.0 V
#define PBIASLITEVMODE0         (1<<0)

#define CONTROL_PADCONF_MMC1_CLK  0x48002144  /* MODE0 = MMC1_CLK  and MODE0 = MMC_CMD   */
#define CONTROL_PADCONF_MMC1_DAT0 0x48002148  /* MODE0 = MMC1_DAT0 and MODE0 = MMC1_DAT1 */
#define CONTROL_PADCONF_MMC1_DAT2 0x4800214C  /* MODE0 = MMC1_DAT2 and MODE0 = MMC1_DAT3 */
#define CONTROL_PADCONF_MMC1_DAT4 0x48002150  /* MODE0 = MMC1_DAT4 and MODE0 = MMC1_DAT5 */
#define CONTROL_PADCONF_MMC1_DAT6 0x48002154  /* MODE0 = MMC1_DAT6 and MODE0 = MMC1_DAT7 */

#define CONTROL_PADCONF_GPMC_WAIT0 0x480020CC

#define CONTROL_PADCONF_MCSPI4_CLK      0x4800218C
#define CONTROL_PADCONF_MCSPI4_SIMO_SOMI 0x48002190
#define CONTROL_PADCONF_MCSPI4_CS0      0x48002194

#define CONTROL_PADCONF_MCBSP2_FSX_CLKX	0x4800213C	/* MODE0 = MCBSP2 */
#define CONTROL_PADCONF_MCBSP2_DR_DX	0x48002140	/* MODE0 = MCBSP2 */
#define CONTROL_PADCONF_MCBSP3_DX_DR	0x4800216C	/* MODE0 = MCBSP3, MODE1 = UART2_CTS_RTS 	*/
#define CONTROL_PADCONF_MCBSP3_CLKX_FSX	0x48002170	/* MODE0 = MCBSP3, MODE1 = UART_TX_RX 		*/
#define CONTROL_PADCONF_UART2_CTS_RTS	0x48002174	/* MODE0 = UART2,  MODE1 = MCBSP3_DX_DR 	*/
#define CONTROL_PADCONF_UART2_TX_RX		0x48002178	/* MODE0 = UART2,  MODE1 = MCBSP3_CLKX_FSX 	*/
#define CONTROL_PADCONF_MCBSP1_DX_DR	0x48002190	/* MODE0 = MCBSP1, MODE2 = MCBSP3_DR_DX 	*/
#define CONTROL_PADCONF_MCBSP_CLKS		0x48002194	/* MODE0 = MCBSP1, MODE2 = MCBSP_FSX 		*/
#define CONTROL_PADCONF_MCBSP1_CLKX		0x48002198	/* MODE0 = MCBSP1, MODE2 = MCBSP_CLKX 		*/
#define CONTROL_PADCONF_HSUSB0_DATA7	0x480021B8	/* High byte for I2C1 */
#define CONTROL_PADCONF_I2C1_SDA_SCI	0x480021BC	/* Low byte for I2C1, High byte for I2C2 */
#define CONTROL_PADCONF_I2C2_SDA_SCI	0x480021C0	/* Low byte for I2C2 */

#define CONTROL_PADCONF_MCSPI1_CS3  	0x480021D4	
#define CONTROL_PADCONF_MCSPI2_SIMO  	0x480021D8	
#define CONTROL_PADCONF_MCSPI2_CS0	0x480021DC
#define CONTROL_PADCONF_ETK_D8          0x480025EC
#define CONTROL_PADCONF_ETK_D10         0x480025F0
#define CONTROL_PADCONF_ETK_D12  	0x480025F4
#define CONTROL_PADCONF_ETK_D14  	0x480025F8
#define CONTROL_PADCONF_MCBSP3_DX       0x4800216C

/* Bit definitions for the PADCONF registers */
#define MUXMODE0_MODE0  (0)
#define MUXMODE0_MODE1  (1)
#define MUXMODE0_MODE2  (2)
#define MUXMODE0_MODE3  (3)
#define MUXMODE0_MODE4  (4)
#define MUXMODE0_MODE7  (7) 					/* Safe Mode */
#define MUXMODE1_MODE0  (MUXMODE0_MODE0<<16)
#define MUXMODE1_MODE1  (MUXMODE0_MODE1<<16)
#define MUXMODE1_MODE2  (MUXMODE0_MODE2<<16)
#define MUXMODE1_MODE3  (MUXMODE0_MODE3<<16)
#define MUXMODE1_MODE4  (MUXMODE0_MODE4<<16)
#define MUXMODE1_MODE7  (MUXMODE0_MODE7<<16)    /* Safe Mode */
#define PULLUDENABLE0   (1<<3)
#define PULLTYPE0_UP    (1<<4)
#define INPUTENABLE0    (1<<8)
#define PULLUDENABLE1   (1<<19)
#define PULLTYPE1_UP    (1<<20)
#define INPUTENABLE1    (1<<24)

extern int pin_mux;
extern int is_beagle;
extern int is_mistral;
extern char board_rev[];

void init_pinmux(void)
{
    unsigned pad;

    /****************************************/
    /** Setup MMC Pins for MMC/SD function **/
    /****************************************/

    /* MMC1 CLK */
    pad = in32(CONTROL_PADCONF_MMC1_CLK) & 0x0000ffff;
    out32(CONTROL_PADCONF_MMC1_CLK, pad | INPUTENABLE1 | PULLTYPE1_UP | PULLUDENABLE1 | MUXMODE1_MODE0);
    /* MMC1 CMD */
    pad = in32(CONTROL_PADCONF_MMC1_CLK) & 0xffff0000;
    out32(CONTROL_PADCONF_MMC1_CLK, pad | INPUTENABLE0 | PULLTYPE0_UP | PULLUDENABLE0 | MUXMODE0_MODE0);

    /* MMC1 DAT0 */
    pad = in32(CONTROL_PADCONF_MMC1_DAT0) & 0x0000ffff;
    out32(CONTROL_PADCONF_MMC1_DAT0, pad | INPUTENABLE1 | PULLTYPE1_UP | PULLUDENABLE1 | MUXMODE1_MODE0);
    /* MMC1 DAT1 */
    pad = in32(CONTROL_PADCONF_MMC1_DAT0) & 0xffff0000;
    out32(CONTROL_PADCONF_MMC1_DAT0, pad | INPUTENABLE0 | PULLTYPE0_UP | PULLUDENABLE0 | MUXMODE0_MODE0);

    /* MMC1 DAT2 */
    pad = in32(CONTROL_PADCONF_MMC1_DAT2) & 0x0000ffff;
    out32(CONTROL_PADCONF_MMC1_DAT2, pad | INPUTENABLE1 | PULLTYPE1_UP | PULLUDENABLE1 | MUXMODE1_MODE0);
    /* MMC1 DAT3 */
    pad = in32(CONTROL_PADCONF_MMC1_DAT2) & 0xffff0000;
    out32(CONTROL_PADCONF_MMC1_DAT2, pad | INPUTENABLE0 | PULLTYPE0_UP | PULLUDENABLE0 | MUXMODE0_MODE0);

    /* MMC1 DAT4 */
    pad = in32(CONTROL_PADCONF_MMC1_DAT4) & 0x0000ffff;
    out32(CONTROL_PADCONF_MMC1_DAT4, pad | INPUTENABLE1 | PULLTYPE1_UP | PULLUDENABLE1 | MUXMODE1_MODE0);
    /* MMC1 DAT5 */
    pad = in32(CONTROL_PADCONF_MMC1_DAT4) & 0xffff0000;
    out32(CONTROL_PADCONF_MMC1_DAT4, pad | INPUTENABLE0 | PULLTYPE0_UP | PULLUDENABLE0 | MUXMODE0_MODE0);

    /* MMC1 DAT6 */
    pad = in32(CONTROL_PADCONF_MMC1_DAT6) & 0x0000ffff;
    out32(CONTROL_PADCONF_MMC1_DAT6, pad | INPUTENABLE1 | PULLTYPE1_UP | PULLUDENABLE1 | MUXMODE1_MODE0);
    /* MMC1 DAT7 */
    pad = in32(CONTROL_PADCONF_MMC1_DAT6) & 0xffff0000;
    out32(CONTROL_PADCONF_MMC1_DAT6, pad | INPUTENABLE0 | PULLTYPE0_UP | PULLUDENABLE0 | MUXMODE0_MODE0);
    if (is_beagle)
    {
        /* MMC1 WP */
        // check if we have beagle bx, since it uses a different gpio for MMC-WP
        if (is_beagle && board_rev[0] && (board_rev[0] <= 'b'))
        {
            //Rev bx, GPIO29
            pad = in32(CONTROL_PADCONF_ETK_D14) & 0x0000ffff;
            out32(CONTROL_PADCONF_ETK_D14, pad | INPUTENABLE1 | PULLTYPE1_UP | PULLUDENABLE1 | MUXMODE1_MODE4);
        }
        else
        {
            //Rev c or greater, GPIO23
            pad = in32(CONTROL_PADCONF_ETK_D8) & 0x0000ffff;
            out32(CONTROL_PADCONF_ETK_D8, pad | INPUTENABLE1 | PULLTYPE1_UP | PULLUDENABLE1 | MUXMODE1_MODE4);
        }
    }
    if (is_mistral)
    {
        /* MMC1 WP */
        pad = in32(CONTROL_PADCONF_GPMC_WAIT0) & 0x0000ffff;
        out32(CONTROL_PADCONF_GPMC_WAIT0, pad | INPUTENABLE1 | PULLTYPE1_UP | PULLUDENABLE1 | MUXMODE1_MODE4);
    }

    /***************************************/
    /** Setup I2C1 Pins for I2C1 function **/
    /***************************************/
    /* High byte is the SCI line for I2C1 */
    pad = in32(CONTROL_PADCONF_HSUSB0_DATA7) & 0x0000ffff;
    out32(CONTROL_PADCONF_HSUSB0_DATA7, pad | INPUTENABLE1 | PULLTYPE1_UP | PULLUDENABLE1 | MUXMODE1_MODE0);
    /* Low byte is the SDA line for I2C1 */
    pad = in32(CONTROL_PADCONF_I2C1_SDA_SCI) & 0xffff0000;
    out32(CONTROL_PADCONF_I2C1_SDA_SCI, pad | INPUTENABLE0 | PULLTYPE0_UP | PULLUDENABLE0 | MUXMODE0_MODE0);

    /***************************************/
    /** Setup I2C2 Pins for I2C2 function **/
    /***************************************/
    /* High byte is the SCI line for I2C2 */
    pad = in32(CONTROL_PADCONF_I2C1_SDA_SCI) & 0x0000ffff;
    out32(CONTROL_PADCONF_I2C1_SDA_SCI, pad | INPUTENABLE1 | PULLTYPE1_UP | PULLUDENABLE1 | MUXMODE1_MODE0);
    /* Low byte is the SDA line for I2C2 */
    pad = in32(CONTROL_PADCONF_I2C2_SDA_SCI) & 0xffff0000;
    out32(CONTROL_PADCONF_I2C2_SDA_SCI, pad | INPUTENABLE0 | PULLTYPE0_UP | PULLUDENABLE0 | MUXMODE0_MODE0);

    /***********************************/
    /** Setup UART2 Pins to SAFE Mode **/
    /***********************************/
    out32(CONTROL_PADCONF_UART2_CTS_RTS, MUXMODE1_MODE7 | MUXMODE0_MODE7);
    out32(CONTROL_PADCONF_UART2_TX_RX, MUXMODE1_MODE7 | MUXMODE0_MODE7);

    /**********************************************/
    /** Setup MCBSP3 Pins for UART2 (BT Control) **/
    /**********************************************/
    out32(CONTROL_PADCONF_MCBSP3_DX_DR, MUXMODE1_MODE1 | INPUTENABLE0 | PULLTYPE0_UP | PULLUDENABLE0 | MUXMODE0_MODE1);
    out32(CONTROL_PADCONF_MCBSP3_CLKX_FSX, INPUTENABLE1 | PULLUDENABLE1 | MUXMODE1_MODE1 | MUXMODE0_MODE1);

    /***************************************************/
    /** Setup MCBSP1 Pins for MCBSP3 Audio (TUNER/BT) **/
    /***************************************************/
    out32(CONTROL_PADCONF_MCBSP1_DX_DR, INPUTENABLE1 | MUXMODE1_MODE2 | MUXMODE0_MODE2);
    out32(CONTROL_PADCONF_MCBSP_CLKS, INPUTENABLE1 | MUXMODE1_MODE2 | INPUTENABLE0 | PULLTYPE0_UP);
    out32(CONTROL_PADCONF_MCBSP1_CLKX, MUXMODE1_MODE7 | INPUTENABLE0 | MUXMODE0_MODE2);

    /* Select CLKS input for MCBSP3 */
    pad = in32(CONTROL_DEVCONF1);
    out32(CONTROL_DEVCONF1, pad | MCBSP3_CLKS);

    /**************************************************************/
    /** Setup McSPI4 pins for SPI output (on expansion connector) */
    /**************************************************************/
    kprintf("Enabling SPI4 pins, this will affect McBSP1 pin configurations \n");
    pad = in32(CONTROL_PADCONF_MCSPI4_CLK);
    pad &= 0xffff0000;  //Clear bits 0:15 to ensure it's what we want.
    out32(CONTROL_PADCONF_MCSPI4_CLK, pad | INPUTENABLE0 | MUXMODE0_MODE1);
    out32(CONTROL_PADCONF_MCSPI4_SIMO_SOMI, INPUTENABLE1 | MUXMODE1_MODE1 | MUXMODE0_MODE1);
    pad = in32(CONTROL_PADCONF_MCSPI4_CS0);
    pad &= 0x0000ffff;  //Clear bits 31:16 to ensure it's what we want.
    out32(CONTROL_PADCONF_MCSPI4_CS0, pad | MUXMODE1_MODE1);
    
    /**************************************************/
    /** Setup MCBSP2 Pins for MCBSP2 Audio (TWL4030) **/
    /**************************************************/
    out32(CONTROL_PADCONF_MCBSP2_FSX_CLKX, INPUTENABLE1 | MUXMODE1_MODE0 | INPUTENABLE0 | MUXMODE0_MODE0);
    out32(CONTROL_PADCONF_MCBSP2_DR_DX, MUXMODE1_MODE0 | INPUTENABLE0 | MUXMODE0_MODE0);
    /* Select CLKS input for MCBSP2 */
    pad = in32(CONTROL_DEVCONF0);
    out32(CONTROL_DEVCONF0, pad | MCBSP2_CLKS);

    /* Select CLKS input for MMCSD1 */
    pad = in32(CONTROL_DEVCONF0);
    out32(CONTROL_DEVCONF0, pad | MMCSDIO1ADPCLKISEL);

    /* set pbais for mmc/sd */
    pad = in32(CONTROL_PBIAS_LITE);
    out32(CONTROL_PBIAS_LITE, pad | PBIASLITEVMODE1 | PBIASLITESUPPLYHIGH0 | PBIASSPEEDCTRL0 | PBIASLITEPWRDNZ0 | PBIASLITEVMODE0);

    if (!(is_beagle && board_rev[0] && (board_rev[0] <= 'b')) && !is_mistral)
    {
        /**************************************************/
        /** Setup USBHOST2 Pins for Beagle USB HOST2	 **/
        /**************************************************/
        out32(CONTROL_PADCONF_MCSPI1_CS3, INPUTENABLE1 | MUXMODE1_MODE3 | INPUTENABLE0 | MUXMODE0_MODE3); /*hsusb2_data2 and hsusb2_data7*/
        out32(CONTROL_PADCONF_MCSPI2_SIMO, INPUTENABLE1 | MUXMODE1_MODE3 | INPUTENABLE0 | MUXMODE0_MODE3); /*hsusb2_data4 and hsusb2_data5*/
        out32(CONTROL_PADCONF_MCSPI2_CS0, INPUTENABLE1 | MUXMODE1_MODE3 | INPUTENABLE0 | MUXMODE0_MODE3); /*hsusb2_data6 and hsusb2_data3*/
        out32(CONTROL_PADCONF_ETK_D10, MUXMODE1_MODE3 | MUXMODE0_MODE3); /*hsusb2_clk and hsusb2_stp*/
        out32(CONTROL_PADCONF_ETK_D12, INPUTENABLE1 | MUXMODE1_MODE3 | INPUTENABLE0 | MUXMODE0_MODE3); /*hsusb2_dir and hsusb2_nxt*/
        out32(CONTROL_PADCONF_ETK_D14, INPUTENABLE1 | MUXMODE1_MODE3 | INPUTENABLE0 | MUXMODE0_MODE3); /*hsusb2_data0 and hsusb2_data1*/
        out32(CONTROL_PADCONF_UART2_TX_RX, MUXMODE1_MODE4); /*GPIO_147: hsusb2_nRST*/

        /*********************************************************/
        /*  Now set the MSECURE pin to enable writing to the RTC */
        /*********************************************************/

        /* GPIO 140 == MSECURE output on Beagleboard xM */
        pad = in32(CONTROL_PADCONF_MCBSP3_DX) & 0xffff0000;
        out32(CONTROL_PADCONF_MCBSP3_DX, pad | PULLUDENABLE0 | PULLTYPE0_UP | MUXMODE0_MODE4);
    }
}

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/startup/boards/omap3530/init_pinmux.c $ $Rev: 308279 $" );
