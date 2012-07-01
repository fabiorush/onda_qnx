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

#include "omap3530spi.h"

static char *omap3530_opts[] = {
        "base",     /* Base address for this CSPI controller */
        "bitrate",  /* defines the bitrate to to assigned to the spi */
        "clock",    /* defined the value of the clock source of the SPI */
        "channel",  /* defines the channel  connected */
        "irq",      /* IRQ for this CSPI interface */
        "force",    /* Force The default CS level */
        "num_cs",   /* number of devices supported */
        "sdma",     /* Disable/enable SDMA for SPI */
        "cs_delay", /* Disable/enable 1 1/2 clock CS to data delay */
#ifdef CPU_OMAP4430
        "hdtx",     /* half duplex transmit only mode ,
                     * usage hdtx=<chipselects which must be configured as
                     * tx only mode each separated by ^ >
                     */
        "hdrx",     /* half duplex reveive only mode ,
                     * usage hdrx=<chipselects which must be configured as
                     * tx only mode each separated by ^ >
                     */
        "fifo_len", /* length of the fifo */
        "fifo_ael", /* almost empty level for fifo */
        "fifo_afl", /* almost full level for fifo */
#endif
        NULL
};

spi_funcs_t spi_drv_entry = {
        sizeof(spi_funcs_t),
        omap3530_init,      /* init() */
        omap3530_dinit,     /* fini() */
        omap3530_drvinfo,   /* drvinfo() */
        omap3530_devinfo,   /* devinfo() */
        omap3530_setcfg,    /* setcfg() */
        omap3530_xfer,      /* xfer() */
        omap3530_dmaxfer    /* dma_xfer() */
};

/*
 * Note:
 * The devices listed are just examples, users should change
 * this according to their own hardware spec.
 */
static spi_devinfo_t devlist[4] = {
    {
        0x00,           // Device ID, for SS0
        "TSC-2046",     // Description
        {
            (8 | SPI_MODE_CKPOL_HIGH), // data length 8bit, MSB
            125000             // TSC-2046 will function at 125000
        },
    },
    {
        0x01,           // Device ID, for SS0
        "TSC-2046",     // Description
        {
            (8 | SPI_MODE_CKPOL_HIGH), // data length 8bit, MSB
            125000             // TSC-2046 will function at 125000
        },
    },
    {
        0x02,           // Device ID, for SS0
        "TSC-2046",     // Description
        {
            (8 | SPI_MODE_CKPOL_HIGH), // data length 8bit, MSB
            125000             // TSC-2046 will function at 125000
        },
    },
    {
        0x03,           // Device ID, for SS0
        "TSC-2046",     // Description
        {
            (8 | SPI_MODE_CKPOL_HIGH), // data length 8bit, MSB
            125000             // TSC-2046 will function at 125000
        },
    }
};

static uint32_t devctrl[NUM_OF_SPI_DEVS];

#ifdef CPU_OMAP4430
static pthread_mutex_t xfer_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif


/***********************************************************/
void set_port(unsigned port, unsigned mask, unsigned data)
{
    uint32_t c;

    c = read_omap(port);
    write_omap(port, (c & ~mask) | (data & mask));
}


/***********************************************************/
static int omap3530_options(omap3530_spi_t *spi, char *optstring)
{
    int opt, rc = 0, err = EOK;
    char *options, *freeptr, *c, *value;

    if (optstring == NULL)
        return 0;

    freeptr = options = strdup( optstring );

    while (options && *options != '\0')
    {
        c = options;
        if ((opt = getsubopt( &options, omap3530_opts, &value )) == -1)
            goto error;

        switch (opt) {
            case 0:
                spi->pbase = strtoull( value, 0, 0 );
                continue;
            case 1:
                spi->bitrate = strtoul( value, NULL, 0 );
                continue;
            case 2:
                spi->clock = strtoul( value, NULL, 0 );
                continue;
            case 3:
                spi->channel_num = strtoul( value, NULL, 0 );
                continue;
            case 4:
                spi->irq_spi = strtoul( value, 0, 0 );
                continue;
            case 5:
                spi->force = strtoul( value, 0, 0 );
                continue;
            case 6:
                spi->num_cs = strtoul( value, 0, 0 );
                continue;
            case 7:
                spi->sdma = strtoul( value, 0, 0 );
                continue;
            case 8:
                spi->cs_delay = strtoul( value, 0, 0 );
                continue;
#ifdef CPU_OMAP4430
            case 9:
            case 10:
                for(;value && *value; value++)
                {
                    unsigned long channel;
                    if(*value == '^')
                    {
                        continue;
                    }
                    channel = *value - '0'; /* expect channel number to be single digit number */
                    if(channel > NUM_OF_SPI_DEVS || channel < 0)
                    {
                        break;
                    }
                    if(opt == 9)
                    {
                        spi->hdtx |= 1 << channel;
                    }
                    else if(opt == 10)
                    {
                        spi->hdrx |= 1 << channel;
                    }
                }
                continue;
            case 11:
                spi->fifo_len = strtoul(value, 0 , 0);
                continue;
            case 12:
                spi->fifo_ael = strtoul(value, 0 , 0);
                continue;
            case 13:
                spi->fifo_afl = strtoul(value, 0 , 0);
                continue;
#endif
        }

error:  
        fprintf( stderr, "omap3530-spi: unknown option %s", c );
        err = EINVAL;
        rc = -1;
    }

    free( freeptr );

    return rc;

}


/***********************************************************/
void *omap3530_init(void *hdl, char *options)
{
    omap3530_spi_t *dev;
    uintptr_t base;
    int i;
    uint32_t reg;

    dev = calloc( 1, sizeof(omap3530_spi_t) );
    if (dev == NULL)
        return NULL;

    //Set defaults
    dev->pbase = OMAP3530_SPI1_BASE;
    dev->spi_int = OMAP3530_SPI_MASTER_TWO;
    dev->irq_spi = OMAP3530_SPI_SPI1_INTR;
    dev->clock = OMAP3530_SPI_INPUT_CLOCK;
    dev->channel_num = 1;
    dev->force = 0;
    dev->sdmapbase = DMA4_BASE_ADDR;
    dev->irq_sdma = SDMA_BASE_IRQ; /* We use interrupt of receive channel   */
    dev->sdma = 0;
    dev->num_cs = 1;
    dev->cs_delay = 0;

#ifdef CPU_OMAP4430
    dev->fifo_len = 16; /* taking fifo as 32 bytes & half of available fifo is used here */
    dev->fifo_ael = 10;
    dev->fifo_afl = 15;
    dev->hdtx = 0;
    dev->hdrx = 0;
#endif

    if (omap3530_options( dev, options ))
        goto fail0;

    /*
     * Map in SPI registers
     */
    if ((base = mmap_device_io( OMAP3530_SPI_REGLEN, dev->pbase )) == (uintptr_t) MAP_FAILED)
        goto fail0;
    dev->vbase = base;

    /* Reset the SPI interface
     * and wait for the reset to complete
     */
    set_port( base + OMAP3530_MCSPI_SYS_CONFIG, OMAP3530_MCSPI_CONFIG_SOFT_RESET,
            OMAP3530_MCSPI_CONFIG_SOFT_RESET);
    while (!(in32( base + OMAP3530_MCSPI_SYS_CONFIG ) & 1))
    {
        nanospin_ns( 20 );
    }

    /*Set Master mode -- single channel mode*/
    out32( (base + OMAP3530_MCSPI_MODCTRL_OFFSET), (in32( base + OMAP3530_MCSPI_MODCTRL_OFFSET )
            & OMAP3530_MCSPI_MODCTRL_MASTER_SEL) | OMAP3530_MCSPI_MODCTRL_MULTI_CH_SEL);

    /*
     * Calculate all device configuration here
     */
    for (i = 0; i < dev->num_cs; i++)
    {
        //we have just one device defined in the driver till now....
        devctrl[i] = omap3530_cfg( dev, &devlist[i].cfg );
        if (dev->force)
        {
            uint32_t trm = SPI_COMM_TX_RX;
#ifdef CPU_OMAP4430
            if(dev->hdtx & 1 << i)
            trm = SPI_COMM_TX_ONLY;
            if(dev->hdrx & 1 << i)
            trm = SPI_COMM_RX_ONLY;
#endif
            /* if we need to set the default CSx level to other than default low, we need to kick it*/
            out32( (base + OMAP3530_MCSPI_CH1_CONFIG_OFFSET + 0x14 * i), devctrl[i] | trm << 12 );
            set_port( base + OMAP3530_MCSPI_CH1_CTRL_OFFSET + 0x14 * i,
                    OMAP3530_MCSPI_CHANNEL_ENABLE, OMAP3530_MCSPI_CHANNEL_ENABLE);
            /* force CS */
            set_port( (base + OMAP3530_MCSPI_CH1_CONFIG_OFFSET + 0x14 * i),
                    OMAP3530_MCSPI_FORCE_MODE_ONE, OMAP3530_MCSPI_FORCE_MODE_ONE);
            delay( 1 );
            /*un force CS */
            set_port( (base + OMAP3530_MCSPI_CH1_CONFIG_OFFSET + 0x14 * i),
                    OMAP3530_MCSPI_FORCE_MODE_ONE, 0 );
            set_port( base + OMAP3530_MCSPI_CH1_CTRL_OFFSET + 0x14 * i,
                    OMAP3530_MCSPI_CHANNEL_ENABLE, 0 );
        }
    }

    /*
     * Attach SPI interrupt
     */
    if (omap3530_attach_intr( dev ))
        goto fail1;

    dev->spi.hdl = hdl;

    /* Clear the appropriate Interrupts if any*/
    reg = in32( base + OMAP3530_MCSPI_IRQ_STATUS_OFFSET );
    out32( (base + OMAP3530_MCSPI_IRQ_STATUS_OFFSET), reg );

    if (dev->sdma)
    {
        if (omap3530_init_sdma( dev ))
            dev->sdma = 0;

        /*
         * Attach SDMA interrupt
         */
        if (omap3530_sdma_attach_intr( dev ))
            goto fail2;
    }

    return dev;

fail2:
    munmap_device_memory( dev->dma4, sizeof(dma4_t) );

fail1:
    munmap_device_io( dev->vbase, OMAP3530_SPI_REGLEN);

fail0:
    free( dev );

    return NULL;
}


/***********************************************************/
void omap3530_setup(omap3530_spi_t *dev, uint32_t device)
{
    uintptr_t base = dev->vbase;
    uint32_t id;

    id = device & SPI_DEV_ID_MASK;
    uint8_t trm = SPI_COMM_TX_RX;
#ifdef CPU_OMAP4430
    if(dev->hdtx & 1 << id)
    trm = SPI_COMM_TX_ONLY;
    if(dev->hdrx & 1 << id)
    trm = SPI_COMM_RX_ONLY;
#endif
    out32( (base + OMAP3530_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id), (trm << 12) | devctrl[id] );

    if (dev->sdma)
    {
        switch (dev->channel_num) {
            case 1:
                switch (id) {
                    case 0:
                        dev->spi_tx_dma_req = S_DMA_34;
                        dev->spi_rx_dma_req = S_DMA_35;
                        break;
                    case 1:
                        dev->spi_tx_dma_req = S_DMA_36;
                        dev->spi_rx_dma_req = S_DMA_37;
                        break;
                    case 2:
                        dev->spi_tx_dma_req = S_DMA_38;
                        dev->spi_rx_dma_req = S_DMA_39;
                        break;
                    case 3:
                        dev->spi_tx_dma_req = S_DMA_40;
                        dev->spi_rx_dma_req = S_DMA_41;
                        break;
                    default:
                        break;
                }
                break;
            case 2:
                switch (id) {
                    case 0:
                        dev->spi_tx_dma_req = S_DMA_42;
                        dev->spi_rx_dma_req = S_DMA_43;
                        break;
                    case 1:
                        dev->spi_tx_dma_req = S_DMA_44;
                        dev->spi_rx_dma_req = S_DMA_45;
                        break;
                    default:
                        break;
                }
                break;
            case 3:
                switch (id) {
                    case 0:
                        dev->spi_tx_dma_req = S_DMA_14;
                        dev->spi_rx_dma_req = S_DMA_15;
                        break;
                    case 1:
                        dev->spi_tx_dma_req = S_DMA_22;
                        dev->spi_rx_dma_req = S_DMA_23;
                        break;
                    default:
                        break;
                }
                break;
            case 4:
                dev->spi_tx_dma_req = S_DMA_69;
                dev->spi_rx_dma_req = S_DMA_70;
                break;
            default:
                break;
        }
    }
}


/***********************************************************/
void omap3530_dinit(void *hdl)
{
    omap3530_spi_t *dev = hdl;

    /*
     * unmap the register, detach the interrupt
     */
    InterruptDetach( dev->iid_spi );
    munmap_device_io( dev->vbase, OMAP3530_SPI_REGLEN);

    /*
     * Disable SDMA
     */
    if (dev->sdma)
    {
        omap3530_sdma_disablespi( dev );
        InterruptDetach( dev->iid_sdma );
        omap3530_sdma_detach( dev );
        munmap_device_memory( dev->dma4, sizeof(dma4_t) );
        munmap( dev->dmabuf, OMAP3530_SPI_MAXDMALEN);
        ConnectDetach( dev->sdma_coid );
    }
    ConnectDetach( dev->coid );
    ChannelDestroy( dev->chid );

    free( hdl );
}

/***********************************************************/
int omap3530_drvinfo(void *hdl, spi_drvinfo_t *info)
{
    info->version = (SPI_VERSION_MAJOR << SPI_VERMAJOR_SHIFT) | (SPI_VERSION_MINOR
            << SPI_VERMINOR_SHIFT) | (SPI_REVISION << SPI_VERREV_SHIFT);
    strcpy( info->name, "OMAP3530 SPI" );
    info->feature = SPI_FEATURE_DMA; /* DMA supported, buffer address alignemnt is 1 */

    return (EOK);
}

/***********************************************************/
int omap3530_setcfg(void *hdl, uint16_t device, spi_cfg_t *cfg)
{
    uint32_t control;
    omap3530_spi_t *dev = hdl;

    if (device >= dev->num_cs)
        return (EINVAL);

    memcpy( &devlist[device].cfg, cfg, sizeof(spi_cfg_t) );

    control = omap3530_cfg( hdl, &devlist[device].cfg );
    if (control == 0)
        return (EINVAL);

    devctrl[device] = control;

    return (EOK);
}


/***********************************************************/
int omap3530_devinfo(void *hdl, uint32_t device, spi_devinfo_t *info)
{
    omap3530_spi_t *dev = hdl;
    int id = device & SPI_DEV_ID_MASK;

    if (device & SPI_DEV_DEFAULT)
    {
        /*
         * Info of this device
         */
        if (id >= 0 && id < dev->num_cs)
            memcpy( info, &devlist[id], sizeof(spi_devinfo_t) );
        else
            return (EINVAL);
    }
    else
    {
        /*
         * Info of next device
         */
        if (id == SPI_DEV_ID_NONE)
            id = -1;
        if (id < (dev->num_cs - 1))
            memcpy( info, &devlist[id + 1], sizeof(spi_devinfo_t) );
        else
            return (EINVAL);
    }

    return (EOK);
}


/***********************************************************/
void *omap3530_xfer(void *hdl, uint32_t device, uint8_t *buf, int *len)
{
#ifdef CPU_OMAP4430
    /* locking mutex here to make smp safe */
    if(pthread_mutex_lock(&xfer_mutex))
    {
        *len = -1;
        return buf;
    }
#endif

    omap3530_spi_t *dev = hdl;
    uintptr_t base = dev->vbase;
    uint32_t id;
    int i;
    int timeout, expected;
    uint32_t reg_value = 0;

    id = device & SPI_DEV_ID_MASK;
    if (id >= dev->num_cs)
    {
        *len = -1;
        return buf;
    }

    dev->xlen = *len;
    dev->rlen = 0;
#ifndef CPU_OMAP4430
    dev->tlen = min(OMAP3530_SPI_FIFOLEN, dev->xlen);
#else
    dev->tlen = min(dev->fifo_len, dev->xlen);
#endif
    dev->pbuf = buf;
    dev->dlen = ((devlist[id].cfg.mode & SPI_MODE_CHAR_LEN_MASK) + 7) >> 3;

    // Cannot set more than 64KB of data at one time
    if (dev->xlen > (64 * 1024))
    {
        *len = -1;
        return buf;
    }

    // Estimate transfer time in us... The calculated dtime is only used for
    // the timeout, so it doesn't have to be that accurate.  At higher clock
    // rates, a calcuated dtime of 0 would mess-up the timeout calculation, so
    // round up to 1 us
    dev->dtime = dev->dlen * 1000 * 1000 / devlist[id].cfg.clock_rate;
    if (dev->dtime == 0)
        dev->dtime = 1;

    omap3530_setup( dev, device );

    /* force CS */
    set_port( (base + OMAP3530_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id), OMAP3530_MCSPI_FORCE_MODE_ONE,
            OMAP3530_MCSPI_FORCE_MODE_ONE);

    /*set FIFO */
    set_port( (base + OMAP3530_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id), OMAP3530_MCSPI_FFER
            | OMAP3530_MCSPI_FFEW, OMAP3530_MCSPI_FFER | OMAP3530_MCSPI_FFEW );

#ifndef CPU_OMAP4430
    out32( base + OMAP3530_MCSPI_XFERLEVEL_OFFSET, (dev->xlen << 16) | 0xF << 8 | 0xF ); /* set transfer levels :MCBSP_FIFO_THRESHOLD= 16-1*/

#else
    out32(base + OMAP3530_MCSPI_XFERLEVEL_OFFSET , (dev->xlen<<16) | (dev->fifo_afl - 1) << 8 | (dev->fifo_ael - 1)); /* set transfer levels :MCBSP_FIFO_THRESHOLD= 16-1*/
#endif

    /* Configue the SPI control register to enable the corresponding channel of the SPI */
    set_port( base + OMAP3530_MCSPI_CH1_CTRL_OFFSET + 0x14 * id, OMAP3530_MCSPI_CHANNEL_ENABLE,
            OMAP3530_MCSPI_CHANNEL_ENABLE);

    // Clear any pending interrupts
    out32( base + OMAP3530_MCSPI_IRQ_STATUS_OFFSET, 0xF );

    /* start the data transmit.....this happens by writing data to
     * the corresponding transmit register. This module has been
     * designed for Transmit/Recieve Mode. This part will change
     * according to the design.
     */
    for (i = 0; i < dev->tlen; i++)
    {
        out32( base + OMAP3530_MCSPI_CH1_TX_BUFFER_OFFSET + 0x14 * id, dev->pbuf[i] );
    }

    uint8_t enint_txrx = INTR_TYPE_RX0_FULL;

#ifdef CPU_OMAP4430
    if(dev->hdtx)
    enint_txrx = INTR_TYPE_TX0_EMPTY;
#endif

    /* Enable Interrupts */
    out32( base + OMAP3530_MCSPI_IRQ_ENABLE_OFFSET, INTR_TYPE_EOWKE | (enint_txrx << (id
            * OMAP3530_INTERRUPT_BITS_PER_SPI_CHANNEL)) );

    /*
     * Wait for exchange to finish
     */
    if (omap3530_wait( dev, dev->xlen * 10 ))
    {
        fprintf( stderr, "OMAP3530 SPI: XFER Timeout!!!" );
        dev->rlen = -1;
    }

    // Read the last spi words
    if (dev->rlen < dev->xlen && dev->rlen != -1)
    {
        reg_value = in32( base + OMAP3530_MCSPI_CH1_STATUS_OFFSET + (OMAP3530_SPI_DEVICE_OFFSET
                * id) );
        timeout = 1000;
        while (timeout-- && ((reg_value & OMAP3530_MCSPI_CH_RX_REG_FULL) == 0))
        {
            nanospin_ns( 100 );
            reg_value = in32( base + OMAP3530_MCSPI_CH1_STATUS_OFFSET + (OMAP3530_SPI_DEVICE_OFFSET
                    * id) );
        }

        if (timeout <= 0)
        {
            dev->rlen = -1;
        }
        else
        {
            // last words to read from buffer
            expected = dev->tlen - dev->rlen;
            for (i = 0; i < expected; i++)
            {
                dev->pbuf[dev->rlen++] = in32( base + OMAP3530_MCSPI_CH1_RX_BUFFER_OFFSET
                        + (OMAP3530_SPI_DEVICE_OFFSET * id) );
            }
        }
    }

    //disable interrupts
    out32( base + OMAP3530_MCSPI_IRQ_ENABLE_OFFSET, 0 );

#ifdef CPU_OMAP4430
    if(!(devlist[id].cfg.mode & SPI_MODE_CSHOLD_HIGH))
#endif

    /*un-force CS */
    set_port( (base + OMAP3530_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id), OMAP3530_MCSPI_FORCE_MODE_ONE, 0 );

    set_port( base + OMAP3530_MCSPI_CH1_CTRL_OFFSET + 0x14 * id, OMAP3530_MCSPI_CHANNEL_ENABLE, 0 );
    set_port( base + OMAP3530_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id, OMAP3530_MCSPI_FFER
            |OMAP3530_MCSPI_FFEW, 0 );
    out32( base + OMAP3530_MCSPI_XFERLEVEL_OFFSET, 0 );

    *len = dev->rlen;

#ifdef CPU_OMAP4430
    /* unlocking mutex */
    pthread_mutex_unlock(&xfer_mutex);
#endif

    return buf;
}


/***********************************************************/
int omap3530_dmaxfer(void *hdl, uint32_t device, spi_dma_paddr_t *paddr, int len)
{
#ifdef CPU_OMAP4430
    /* locking mutex here to make smp safe */
    if(pthread_mutex_lock(&xfer_mutex))
    {
        return -1;
    }
#endif
    omap3530_spi_t *dev = hdl;
    int id = device & SPI_DEV_ID_MASK;
    uintptr_t base = dev->vbase;

    /* Is the SDMA disabled? */
    if (dev->sdma == 0 || id >= dev->num_cs)
        return -1;

    // Estimate transfer time in us... The calculated dtime is only used for
    // the timeout, so it doesn't have to be that accurate.  At higher clock
    // rates, a calculated dtime of 0 would mess-up the timeout calculation, so
    // round up to 1 us
    dev->dtime = dev->dlen * 1000 * 1000 / devlist[id].cfg.clock_rate;
    if (dev->dtime == 0)
        dev->dtime = 1;

    omap3530_setup( dev, device );

    if (omap3530_setup_sdma( hdl, id, paddr, len ))
    {
        return -1;
    }

    /* Enable sdma request   */
    set_port( base + OMAP3530_MCSPI_CH1_CONFIG_OFFSET + (0x14 * id), OMAP3530_MCSPI_DMAR
            |OMAP3530_MCSPI_DMAW, OMAP3530_MCSPI_DMAR | OMAP3530_MCSPI_DMAW);

    /* Configue the SPI control register to enable the corresponding channel of the SPI */
    set_port( base + OMAP3530_MCSPI_CH1_CTRL_OFFSET + 0x14 * id, OMAP3530_MCSPI_CHANNEL_ENABLE,
            OMAP3530_MCSPI_CHANNEL_ENABLE);

    /* force CS */
    set_port( (base + OMAP3530_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id), OMAP3530_MCSPI_FORCE_MODE_ONE,
            OMAP3530_MCSPI_FORCE_MODE_ONE);

    if (omap3530_wait( dev, len * 10 ))
    {
        fprintf( stderr, "OMAP3530 SPI: DMA XFER Timeout!!!" );
        len = -1;
    }

#ifdef CPU_OMAP4430
    if(!(devlist[id].cfg.mode & SPI_MODE_CSHOLD_HIGH))
#endif

    /*un-force CS */
    set_port( (base + OMAP3530_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id), OMAP3530_MCSPI_FORCE_MODE_ONE, 0 );

    /*
     * Disable SDMA request and SPI function
     */
    set_port( base + OMAP3530_MCSPI_CH1_CTRL_OFFSET + 0x14 * id, OMAP3530_MCSPI_CHANNEL_ENABLE, 0 );
    set_port( base + OMAP3530_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id, OMAP3530_MCSPI_DMAR
            |OMAP3530_MCSPI_DMAW, 0 );

    omap3530_sdma_disablespi( dev );

#ifdef CPU_OMAP4430
    /* unlocking mutex */
    pthread_mutex_unlock(&xfer_mutex);
#endif

    return len;
}

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/spi/omap3530/omap3530spi.c $ $Rev: 308279 $" );
