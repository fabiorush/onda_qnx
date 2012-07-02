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
#include <arm/omap2420.h>
#include <arm/omap3530.h>

/*
 * Initialize hwinfo structure in the system page.
 * This code is hardware dependant and may have to be changed
 * changed by end users.
 */
#define HDRC_ULPIREGDATA 		0x480ab074
#define HDRC_ULPIREGADDR 		0x480ab075
#define HDRC_ULPIREGCONTROL 	0x480ab076
#define HDRC_ULPIVBUSCONTROL 	0x480ab070
#define EHCI_ULPI_REG			0x480648a4
#define EHCI_CONFIG_REG			0x48064850
#define CONFIG_REG_CF			0x1

#define UHH_SYSCONFIG			0x48064010
#define UHH_HOSTCONFIG			0x48064040
#define OMAP_USBTLL_SYSCONFIG	0x48062010

#define LDELAY      	            12000000
#define PLL_STOP                    1	/* PER & IVA */
#define PLL_LOCK                    7	/* MPU, IVA, CORE & PER */
#define OMAP35XX_CM_CLKEN2_PLL		0x48004D04
#define OMAP35XX_CM_IDLEST2_CKGEN	0x48004D24
#define OMAP35XX_CM_CLKSEL4_PLL		0x48004D4C
#define OMAP35XX_CM_CLKSEL5_PLL		0x48004D50
#define OMAP35XX_CM_FCLKEN_USBHOST  0x48005400
#define EN_USBHOST1			        1<<0
#define EN_USBHOST2   		        1<<1
#define OMAP35XX_CM_ICLKEN_USBHOST  0x48005410
#define EN_USBHOST   		        0x1

#define RSTDELAY      	            1000
#define OMAP35XX_GPIO5_OE		    0x49056034
#define OMAP35XX_GPIO5_DATAOUT		0x4905603C
#define OMAP35XX_GPIO5		        128
#define USB332X_INTERFACECTL        0x7
#define USB332X_OTGCTL              0xa
#define USB332X_STP_PULLUP_DISABLE  0x90
#define USB332X_EXTERNALVBUS_ENABLE 0x86
#define TEB_USB_PHY_RESET_PIN		14

/* I2C1 related registers */
#define I2C_IE 				0x48070004
#define I2C_STAT			0x48070008
#define I2C_WE				0x4807000C
#define I2C_SYSS			0x48070010
#define I2C_BUF				0x48070014
#define I2C_CNT				0x48070018
#define I2C_DATA			0x4807001C
#define I2C_SYSC			0x48070020
#define I2C_CON				0x48070024
#define I2C_OA0				0x48070028
#define I2C_SA				0x4807002C
#define I2C_PSC				0x48070030
#define I2C_SCLL			0x48070034
#define I2C_SCLH			0x48070038
#define I2C_SYSTEST			0x4807003C
#define I2C_BUFSTAT			0x48070040
#define I2C_OA1				0x48070044
#define I2C_OA2				0x48070048
#define I2C_OA3				0x4807004C
#define I2C_ACTOA			0x48070050
#define I2C_SBLOCK			0x48070054

#define BEAGLE_BX_MMC1_WP_PIN   29
#define BEAGLE_MMC1_WP_PIN      23
#define MISTRAL_MMC1_WP_PIN     (63-32)

/* 
 * Definition for DVI_PUP. OMAP3730 Rev B.
 */

// Taken from wl4030_reg.h.
#define TWL4030_ADDR_GRP4	        0x49

#define TWL4030_GPIO_DATA_DIR1		0x9B
#define TWL4030_GPIO_SET_DATA_OUT1	0xA4

#define TWL4030_GPIO2_OUT		    0x04
#define TWL4030_GPIO2_HIGH		    0x04
/* End definitions. */

extern int teb;
extern int mtp;
extern int pin_mux;
extern int omap3730teb;
extern int is_beagle;
extern int is_mistral;
extern char board_rev[];


/************************************************************/
void i2c_delay(int t)
{
    volatile uint32_t i = t * 1000;
    while (i--)
        ;
}

/************************************************************/
uint8_t mentor_ulpi_read(uint8_t addr)
{
    int timeout = 10000000;
    uint8_t val = 0;

    out8( HDRC_ULPIREGADDR, addr );
    out8( HDRC_ULPIREGCONTROL, 0x5 );
    while (!(in8( HDRC_ULPIREGCONTROL ) & 0x2) && timeout--)
        ;
    if (timeout <= 0)
    {
        kprintf( "ULPI read timeout at addr %x\n", addr );
    }
    else
    {
        val = in8( HDRC_ULPIREGDATA );
    }

    return val;
}

/************************************************************/
int mentor_ulpi_write(uint8_t addr, uint8_t val)
{
    int timeout = 10000000;

    out8( HDRC_ULPIREGADDR, addr );
    out8( HDRC_ULPIREGDATA, val );
    out8( HDRC_ULPIREGCONTROL, 0x1 );
    while (!(in8( HDRC_ULPIREGCONTROL ) & 0x2) && timeout--)
        ;
    if (timeout <= 0)
    {
        kprintf( "ULPI write timeout at addr %x\n", addr );
        return -1;
    }
    return 0;
}


/************************************************************/
uint8_t EHCI_ViewPort_read(int port, uint8_t reg)
{

    uint8_t val = 0;
    int i = 100000;
    uint32_t regval = 0x80000000 | (reg << 16) | (port << 24) | (0x3 << 22);
    out32( EHCI_ULPI_REG, regval );
    while ((in32( EHCI_ULPI_REG ) & 0x80000000) && --i)
        ;
    if (i <= 0)
        kprintf( " EHCI_ViewPort_read timeout %x\n", in32( EHCI_ULPI_REG ) );
    val = in32( EHCI_ULPI_REG ) & 0xFF;
    return val;

}

/************************************************************/
void EHCI_ViewPort_write(int port, uint8_t reg, uint8_t val)
{
    int i = 100000;
    uint32_t regval = 0x80000000 | (reg << 16) | val | (port << 24) | (0x2 << 22);
    out32( EHCI_ULPI_REG, regval );
    while ((in32( EHCI_ULPI_REG ) & 0x80000000) && --i)
        ;
    if (i <= 0)
        kprintf( " EHCI_ViewPort_write timeout %x\n", in32( EHCI_ULPI_REG ) );
}

/************************************************************/
void init_hwinfo()
{
    int count = LDELAY;
    int i;

    if (mtp)
    {
        if (teb)
        {
            //teb board, we need to reset the EHCI PHY using GPIO14
            out32( OMAP3530_GPIO1_BASE + OMAP2420_GPIO_OE, in32( OMAP3530_GPIO1_BASE
                    + OMAP2420_GPIO_OE ) & ~(1 << (TEB_USB_PHY_RESET_PIN)) );
            out32( OMAP3530_GPIO1_BASE + OMAP2420_GPIO_DATAOUT, in32( OMAP3530_GPIO1_BASE
                    + OMAP2420_GPIO_DATAOUT ) & ~(1 << (TEB_USB_PHY_RESET_PIN)) );
            /* Hold the PHY in RESET for enough time till DIR is high */
            i = RSTDELAY;
            while (i--)
                in32( OMAP3530_GPIO1_BASE + OMAP2420_GPIO_DATAOUT );
        }
        if (!omap3730teb)
        {
            /*Init HS USB -EHCI, OHCI clocks */
            out32( 0x48005410, 0x1 ); /*ICLK*/
            out32( 0x48005400, 0x3 ); /*FCLK*/
        }

        /* Init OTG USB */
        out8( HDRC_ULPIVBUSCONTROL, 0x3 );
        mentor_ulpi_write( 0x7, 0x40 );
        mentor_ulpi_write( 0xa, 0x86 );

        if (!omap3730teb)
        {
            /* perform TLL soft reset, and wait until reset is complete */
            /* (1<<3) = no idle mode only for initial debugging */
            out32( OMAP_USBTLL_SYSCONFIG, 0x1a ); /*OMAP_USBTLL_SYSCONFIG*/
            /* Wait for TLL reset to complete */
            while (!(in32( OMAP_USBTLL_SYSCONFIG + 0x4 ) & 1))
                ;

            /* Put UHH in NoIdle/NoStandby mode */
            out32( UHH_SYSCONFIG, 0x1108 ); /*UHH_SYSCONFIG */

            /* Bypass the TLL module for PHY mode operation */
            out32( UHH_HOSTCONFIG, 0x21C ); /*UHH_HOSTCONFIG -- bypass mode*/
            /* Ensure that BYPASS is set */
            while (in32( UHH_HOSTCONFIG ) & 1)
                ;

            if (teb)
            {
                /* reset PHY: USB3320 */
                out32( OMAP3530_GPIO1_BASE + OMAP2420_GPIO_DATAOUT, in32( OMAP3530_GPIO1_BASE
                        + OMAP2420_GPIO_DATAOUT ) | (1 << (TEB_USB_PHY_RESET_PIN)) );
                /* Hold the PHY in RESET for enough time till PHY is settled and ready */
                i = RSTDELAY;
                while (i--)
                    in32( OMAP3530_GPIO1_BASE + OMAP2420_GPIO_DATAOUT );
            }

            /*EHCI  Take the ownership */
            out32( EHCI_CONFIG_REG, 0x1 );

            EHCI_ViewPort_write( 2, 0x7, 0x40 );
            EHCI_ViewPort_write( 2, 0xa, 0x86 );

            /*EHCI  release the ownership */
            out32( EHCI_CONFIG_REG, 0x0 );
        }
    }
    else if (pin_mux)
    {
        if (is_beagle)
        {
            /*set GOIP_147 direction as output */
            out32( OMAP35XX_GPIO5_OE, in32( OMAP35XX_GPIO5_OE ) & ~(1 << (147 - OMAP35XX_GPIO5)) );
            out32( OMAP35XX_GPIO5_DATAOUT, in32( OMAP35XX_GPIO5_DATAOUT ) & ~(1 << (147
                    - OMAP35XX_GPIO5)) );
            /* Hold the PHY in RESET for enough time till DIR is high */
            i = RSTDELAY;
            while (i--)
                in32( OMAP35XX_GPIO5_DATAOUT );

            /*for beagle board revc*/
            /* set clock*/
            out32( OMAP35XX_CM_CLKEN_PLL, (in32( OMAP35XX_CM_CLKEN_PLL ) & ~(0xf << 20)) | (0x3
                    << 20) ); /* FREQSEL */
            out32( OMAP35XX_CM_CLKEN_PLL, (in32( OMAP35XX_CM_CLKEN_PLL ) & ~(0x7 << 16))
                    | (PLL_LOCK << 16) );/* lock mode */
            count = LDELAY;
            while ((!(in32( OMAP35XX_CM_IDLEST_CKGEN ) & 2)) && count--)
                ;

            /* PER2 DPLL values 120M*/
            out32( OMAP35XX_CM_CLKEN2_PLL, (in32( OMAP35XX_CM_CLKEN2_PLL ) & ~(0x7)) | (PLL_STOP) );
            count = LDELAY;
            while ((in32( OMAP35XX_CM_IDLEST2_CKGEN ) & 1) && count--)
                ;
            out32( OMAP35XX_CM_CLKSEL4_PLL, (120 << 8) | (12) );/* set M2 */
            out32( OMAP35XX_CM_CLKSEL5_PLL, 1 );/* set M2 */
            out32( OMAP35XX_CM_CLKEN2_PLL, (in32( OMAP35XX_CM_CLKEN2_PLL ) & ~(0xf << 4))
                    | (7 << 4) ); /* FREQSEL */
            out32( OMAP35XX_CM_CLKEN2_PLL, (in32( OMAP35XX_CM_CLKEN2_PLL ) & ~(0x7)) | (PLL_LOCK) );/* lock mode */
            count = LDELAY;
            while ((!(in32( OMAP35XX_CM_IDLEST2_CKGEN ) & 1)) && count--)
                ;

            /*Init HS USB -EHCI, OHCI clocks */
            out32( OMAP35XX_CM_ICLKEN_USBHOST, EN_USBHOST); /*ICLK*/
            out32( OMAP35XX_CM_FCLKEN_USBHOST, EN_USBHOST1 | EN_USBHOST2); /*FCLK*/

            /* perform TLL soft reset, and wait until reset is complete */
            /* (1<<3) = no idle mode only for initial debugging */
            out32( OMAP_USBTLL_SYSCONFIG, 0x1a ); /*OMAP_USBTLL_SYSCONFIG*/
            /* Wait for TLL reset to complete */
            while (!(in32( OMAP_USBTLL_SYSCONFIG + 0x4 ) & 1))
                ;

            /* Put UHH in NoIdle/NoStandby mode */
            out32( UHH_SYSCONFIG, 0x1108 ); /*UHH_SYSCONFIG */

            /* Bypass the TLL module for PHY mode operation */
            out32( UHH_HOSTCONFIG, 0x21C ); /*UHH_HOSTCONFIG -- bypass mode*/
            /* Ensure that BYPASS is set */
            while (in32( UHH_HOSTCONFIG ) & 1)
                ;

            /* reset PHY: USB3326 */
            out32( OMAP35XX_GPIO5_DATAOUT, in32( OMAP35XX_GPIO5_DATAOUT ) | (1 << (147
                    - OMAP35XX_GPIO5)) );
            /* Hold the PHY in RESET for enough time till PHY is settled and ready */
            i = RSTDELAY;
            while (i--)
                in32( OMAP35XX_GPIO5_DATAOUT );

            // skip EHCI for beagle rev b
            if (!(is_beagle && board_rev[0] && (board_rev[0] <= 'b')))
            {
                /*EHCI  Take the ownership */
                out32( EHCI_CONFIG_REG, CONFIG_REG_CF);

                EHCI_ViewPort_write( 2, USB332X_INTERFACECTL, USB332X_STP_PULLUP_DISABLE); /*configured to disable the integrated STP pull-up resistor */
                EHCI_ViewPort_write( 2, USB332X_OTGCTL, USB332X_EXTERNALVBUS_ENABLE); /*enable external VBUS*/

                /*EHCI  release the ownership */
                out32( EHCI_CONFIG_REG, ~CONFIG_REG_CF);
            }

            // make GPIO23 an input for MMC_WP
            // check if we have beagle bx, since it uses a different gpio for MMC-WP
            if (is_beagle && board_rev[0] && (board_rev[0] <= 'b'))
            {
                out32( OMAP3530_GPIO1_BASE + OMAP2420_GPIO_OE, in32( OMAP3530_GPIO1_BASE
                        + OMAP2420_GPIO_OE ) | (1 << (BEAGLE_BX_MMC1_WP_PIN)) );
            }
            else
            {
                out32( OMAP3530_GPIO1_BASE + OMAP2420_GPIO_OE, in32( OMAP3530_GPIO1_BASE
                        + OMAP2420_GPIO_OE ) | (1 << (BEAGLE_MMC1_WP_PIN)) );
            }

        }

        if (is_mistral)
        {
            //make GPIO63 an input for MMC_WP
            out32( OMAP3530_GPIO2_BASE + OMAP2420_GPIO_OE, in32( OMAP3530_GPIO2_BASE
                    + OMAP2420_GPIO_OE ) | (1 << (MISTRAL_MMC1_WP_PIN)) );
        }

    }
    set_syspage_section( &lsp.cpu.arm_boxinfo, sizeof(*lsp.cpu.arm_boxinfo.p) );
}

/************************************************************/
int i2c_reset()
{
    unsigned scll;
    unsigned long iclk;
    uint16_t s;
    int fifo_size;
    int timeout = 2000;

    /* Disable and place module in reset */
    out16( I2C_IE, 0 );
    out16( I2C_BUF, 0 );
    out16( I2C_SYSTEST, 0 );
    if (in16( I2C_CON ) & 0x8000)
        out16( I2C_CON, 0 );

    out16( I2C_SYSC, 0x2 );
    out16( I2C_CON, 0x8000 );
    // wait for reste to complete
    while (0 == (in16( I2C_SYSS ) & 0x0001) && timeout--)
    {
        i2c_delay( 1 );
    }
    if (timeout <= 0)
    {
        return -1;
    }

    out16( I2C_SYSC, 0x215 ); //Smart idle mode, Fclock, Enable Wakeup, autoidle
    out16( I2C_WE, 0x636f ); // enable all wakeup sources

    out16( I2C_CON, 0 );

    // assuming functional and interface clocks are enabled
    iclk = 4000000UL;
    scll = iclk / (100000 << 1) - 7;
    // Set clock prescaler, in order to set internal clock for the I2C module to 4MHz
    out16( I2C_PSC, 23 );

    // Set clock for "speed" bps
    out16( I2C_SCLL, scll );
    out16( I2C_SCLH, scll + 2 );

    // Setup FIFO size
    s = (in16( I2C_BUFSTAT ) >> 14) & 0X3;
    fifo_size = 0x8 << s;
    fifo_size >>= 1;

    out16( I2C_BUF, (fifo_size - 1) << 8 | /* RTRSH*/
    (1 << 14) | (fifo_size - 1) | /* XTRSH*/
    (1 << 6) );

    // Set own address
    out16( I2C_OA0, 1 );

    // Take module out of reset
    out16( I2C_CON, 0x8000 );

    if (in16( I2C_STAT ) & (0x1000))
    {
        out16( I2C_CON, (0x8000 | 0x0002) );
        i2c_delay( 10 );
    }

    return 0;
}

/************************************************************/
int i2c_wait()
{
    unsigned tries = 1000000;

    while (in16( I2C_STAT ) & 0x1000)
    {
        if (tries-- == 0)
        {
            /* reset the controller to see if it's able to recover*/
            i2c_reset();
            //try again to see if it's OK now after reset
            if (in16( I2C_STAT ) & 0x1000)
            {
                i2c_delay( 1 );
                return -1;
            }
            else
                break;
        }
    }
    return 0;
}

/************************************************************/
int i2c_write(unsigned slave_addr, uint8_t *buf, unsigned int len)
{
    uint32_t timeout;
    int numbytes = len;

    if (-1 == i2c_wait())
    {
        kprintf( "i2c_wait failed\n" );
        return -1;
    }

    /* set slave address */
    out16( I2C_SA, slave_addr );

    /* set data count */
    out16( I2C_CNT, len );

    /* Clear FIFO Buffer */
    out16( I2C_BUF, in16( I2C_BUF ) | (1 << 14) | (1 << 6) );

    /* set start condition */
    out16( I2C_CON, 0x8000 | /* module enabled */
    0x0400 | /* master mode */
    0x0200 | /* transmitter mode */
    0x0001 | /* start condition queried */
    0x0002 ); /* stop condition queried*/
    i2c_delay( 1 );
    // poll for xrdy
    timeout = 100000;
    //kprintf("BUFSTAT=0x%x\n",in16(I2C_BUFSTAT));
    //kprintf("STAT=0x%x\n", in16(I2C_STAT));
    while ((0 == (in16( I2C_STAT ) & 0x4010)) && timeout--)
    {
        i2c_delay( 1 );
    }

    if (timeout <= 0)
    {
        kprintf( "i2c_write: timeout!\n" );
        kprintf( "I2C_STAT=0x%x\n", in16( I2C_STAT ) );
        return -1;
    }

    while (numbytes)
    {
        timeout = 100000;
        numbytes--;
        out8( I2C_DATA, *buf++ );
        while ((0 == (in16( I2C_STAT ) & 0x4010)) && timeout--)
        {
            i2c_delay( 1 );
        }
    }

    return 0;
}

/************************************************************/
void init_onboardhub()
{
	uint8_t i;
    uint8_t buf[5][2] = {
    						{ 0xee, 0x00 },
    						{ 0xef, 0x00 },
    						{ 0xf0, 0x00 },
    						{ 0xf1, 0x00 },
    						{ 0xf2, 0x00 }
    };

    if (is_beagle)
    {
        if ((board_rev[0] == 'x') && (board_rev[1] == 'm'))
        {
        	kprintf( "Enabling USB hub\n" );

        	if (board_rev[3] == 'c')
        	{
        		// Rev C
        		buf[0][1] = 0x33;
        	}
        	else
        	{
        		// Rev B or earlier
        		buf[0][1] = 0x32;
        	}

            // Hit TPS65950 on Beagle xM Rev A2
            // The hub power enable is attached to LEDA on the TPS65950.
            i2c_reset();
            for (i = 0; i < 5; i++)
            {
            	if (0 != i2c_write( 0x4a, buf[i], 2 ))
            	{
            		kprintf( "i2c write failed\n" );
            		break;
            	}
            }
        }
    }
}

/************************************************************/
void init_dvi_pup()
{
    uint8_t dir[2] = { TWL4030_GPIO_DATA_DIR1, TWL4030_GPIO2_OUT };
    uint8_t hi[2] = { TWL4030_GPIO_SET_DATA_OUT1, TWL4030_GPIO2_HIGH };

    /* There may be a i2c request unfinished. Be patient and wait. */
    i2c_delay( 1000000 );

    /* Assume i2c has not been used so far. */
    i2c_reset();
    if (0 != i2c_write( TWL4030_ADDR_GRP4, dir, sizeof(dir) ))
        kprintf( "i2c write failed\n" );

    /* Wait for the previous request to finish. */
    i2c_delay( 1000000 );
    if (0 != i2c_write( TWL4030_ADDR_GRP4, hi, sizeof(hi) ))
        kprintf( "i2c write failed\n" );

}

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/startup/boards/omap3530/init_hwinfo.c $ $Rev: 308279 $" );
