/*
 * $QNXLicenseC:
 * Copyright 2010, QNX Software Systems. 
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

// Module Description:  board specific interface

#include <sim_mmc.h>
#include <sim_omap3.h>
#include <arm/omap3530.h>
#include <fcntl.h>
#include <devctl.h>
#include <hw/i2c.h>
#include <confname.h>

/****************************************************************************
 BEAGLE MMC notes

 On the beagle board, MMC_CD is connected to GPIO0 of the TWL4030 PMIC chip.
 So in order to read the state of MMC_CD, we poll the GPIO register on the
 TWL4030.  Ideally we would use the interrupt output instead of polling, but
 we would still require I2C accesses in order to service that interrupt.
 Debouncing of the GPIO is enabled in the TWL4030.

 MMC_WP is connected to GPIO23, so that pad needs to be made a GPIO and configured
 as an input (done in startup), and is polled in the detect function.

 *****************************************************************************/

#define MACHINE_NAME_MAX_LEN 256

static int twl4030_i2c_read(int fd, uint8_t addr, uint8_t reg, uint8_t * val)
{
    struct send_recv
    {
        i2c_sendrecv_t hdr;
        uint8_t buf[2];
    } twl4030_rd_data;

    /*Read the Registers Current Value */
    twl4030_rd_data.buf[0] = reg;
    twl4030_rd_data.hdr.send_len = 1;
    twl4030_rd_data.hdr.recv_len = 1;

    twl4030_rd_data.hdr.slave.addr = addr;
    twl4030_rd_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
    twl4030_rd_data.hdr.stop = 1;

    if (devctl(fd, DCMD_I2C_SENDRECV, &twl4030_rd_data, sizeof(twl4030_rd_data), NULL))
    {
        slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "BEAGLE MMCSD: twl4030_i2c_read fail");
        return -1;
    }

    *val = twl4030_rd_data.buf[0];

    return 0;
}

static int twl4030_i2c_write(int fd, uint8_t addr, uint8_t reg, uint8_t val)
{
    iov_t siov[3];
    i2c_send_t hdr;

    hdr.slave.addr = addr;
    hdr.slave.fmt = I2C_ADDRFMT_7BIT;
    hdr.len = 2;
    hdr.stop = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &reg, sizeof(reg));
    SETIOV(&siov[2], &val, 1);

    if (devctlv(fd, DCMD_I2C_SEND, 3, 0, siov, NULL, NULL) != EOK)
    {
        slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "BEAGLE MMCSD: twl4030_i2c_write fail");
        return -1;
    }

    return 0;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
static int beagle_detect(SIM_HBA *hba)
{
    SIM_MMC_EXT *ext;
    omap3_ext_t *omap;
    beagle_ext_t *beagle;
    uint8_t pmic_gpio_status;
    ext = (SIM_MMC_EXT *) hba->ext;
    omap = (omap3_ext_t *) ext->handle;
    beagle = (beagle_ext_t *) omap->bshdl;

    // read MMC1_CD via PMIC GPIO (i2c access) on GPIODATAIN1 register
    if (twl4030_i2c_read(beagle->fd, 0x49, 0x98, &pmic_gpio_status))
        return MMC_FAILURE; // bail if the access fails

    // MMC1_CD is bit 0 and is high when card removed and low when card present
    if (!(pmic_gpio_status & 1))
    {
        // Beagleboard xM has no write protect pin on the microSD slot
        // as such, return not write protected.
        ext->eflags &= ~MMC_EFLAG_WP;

        return MMC_SUCCESS;
    }

    return MMC_FAILURE;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int bs_init(SIM_HBA *hba)
{
    SIM_MMC_EXT *ext;
    omap3_ext_t *omap;
    CONFIG_INFO *cfg;
    beagle_ext_t *beagle;
    unsigned gpiobase;
    char machine_name[MACHINE_NAME_MAX_LEN];

    if ((beagle = calloc(1, sizeof(beagle_ext_t))) == NULL)
        return MMC_FAILURE;

    cfg = (CONFIG_INFO *) &hba->cfg;

    if (!cfg->NumIOPorts)
    {
        cfg->IOPort_Base[0] = OMAP3_MMCHS1_BASE;
        cfg->IOPort_Length[0] = OMAP3_MMCHS_SIZE;
        cfg->IOPort_Base[1] = OMAP3_DMA4_BASE;
        cfg->IOPort_Length[1] = OMAP3_DMA4_SIZE;
        cfg->NumIOPorts = 2;
    }
    else if (cfg->NumIOPorts < 2)
    {
        slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "OMAP3 MMCSD: DMA4 base address must be specified");
        return MMC_FAILURE;
    }

    if (!cfg->NumIRQs)
    {
        cfg->IRQRegisters[0] = OMAP3_MMCHS1_IRQ;
        cfg->NumIRQs = 1;
    }

    if (!cfg->NumDMAs)
    {
        cfg->DMALst[0] = (DMA4_MMC1_TX >> 1) & 0xFF;
        cfg->DMALst[1] = DMA4_MMC1_TX; // DMA request line for MMC1 TX
        cfg->DMALst[2] = DMA4_MMC1_RX; // DMA request line for MMC1 RX
        cfg->NumDMAs = 3;
    }
    else if (cfg->NumDMAs == 1)
    {
        cfg->DMALst[1] = DMA4_MMC1_TX; // DMA request line for MMC1 TX
        cfg->DMALst[2] = DMA4_MMC1_RX; // DMA request line for MMC1 RX
        cfg->NumDMAs = 3;
    }
    else if (cfg->NumDMAs < 3)
    {
        slogf(_SLOGC_SIM_MMC, _SLOG_ERROR,
                "OMAP3 MMCSD: DMA channel and Tx/Rx request line must be specified");
        return MMC_FAILURE;
    }

    // check if we have beagle bx, since it uses a different gpio for MMC-WP
    confstr(_CS_MACHINE, machine_name, MACHINE_NAME_MAX_LEN);
    if (!strcmp(machine_name, "OMAP3530_BEAGLE_Rev_bx"))
    {
        beagle->gpio_ppin = 29;
    }
    else
    {
        beagle->gpio_ppin = 23;
    }

    gpiobase = OMAP3530_GPIO1_BASE;

    if ((beagle->gpio_base = mmap_device_io(0x100, gpiobase)) == (uintptr_t) MAP_FAILED)
    {
        slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "BEAGLE MMCSD: mmap_device_io failed");
        return MMC_FAILURE;
    }

    beagle->fd = open("/dev/i2c0", O_RDWR);
    if (beagle->fd == NULL)
    {
        slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "BEAGLE MMCSD: unable to open /dev/i2c0");
        return MMC_FAILURE;
    }

    // use GPIO_DEBEN1 reg to enable debouncing on GPIO0 for the beagle
    if (twl4030_i2c_write(beagle->fd, 0x49, 0xa7, 0x01))
    {
        slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "BEAGLE MMCSD: unable to enable debouncing");
        return MMC_FAILURE;
    }

    if (omap3_attach(hba) != MMC_SUCCESS)
        return MMC_FAILURE;

    ext = (SIM_MMC_EXT *) hba->ext;
    omap = (omap3_ext_t *) ext->handle;
    omap->bshdl = beagle;
    ext->detect = beagle_detect;

    omap->mmc_clock = 96000000;

    return MMC_SUCCESS;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
int bs_dinit(SIM_HBA *hba)
{
    SIM_MMC_EXT *ext;
    omap3_ext_t *omap;
    beagle_ext_t *beagle;

    ext = (SIM_MMC_EXT *) hba->ext;
    omap = (omap3_ext_t *) ext->handle;
    if (omap)
    {
        beagle = omap->bshdl;
        if (beagle)
        {
            if (beagle->fd)
            {
                close(beagle->fd);
                beagle->fd = NULL;
            }
            free(omap->bshdl);
        }
    }

    return (CAM_SUCCESS);
}

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/devb/mmcsd/arm/beagle.le/sim_bs.c $ $Rev: 308279 $" );
