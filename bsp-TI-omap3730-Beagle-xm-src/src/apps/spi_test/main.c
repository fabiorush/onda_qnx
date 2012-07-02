//-----------------------------------------------------------------
//
// Copyright (c) MPC Data Limited 2011.  All rights reserved.
//
// main.c : Main SPI test function
//
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Includes
//-----------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <hw/spi-master.h>
#include <errno.h>
#include <sys/types.h>


//-----------------------------------------------------------------
// tspi_getdrvinfo
//-----------------------------------------------------------------
int tspi_getdrvinfo()
{
    spi_drvinfo_t info;
    int i, fd;

    fprintf( stdout, "\n########### spi_getdrvinfo() test ###########\n" );

    /*
     * Open
     */
    if ((fd = spi_open( "/dev/spi0" )) < 0)
    {
        fprintf( stdout, "SPI open failed, fd = %d errno = %d\n", fd, errno);
        return (-1);
    }

    i = spi_getdrvinfo( fd, &info );
    if (i == EOK)
    {
        fprintf( stdout, "version     = %x\n", info.version );
        fprintf( stdout, "driver name = %s\n", info.name );
        fprintf( stdout, "feature     = %x\n", info.feature );
    }
    else
        fprintf( stdout, "get driver info failed\n" );

    spi_close( fd );

    return 0;
}


//-----------------------------------------------------------------
// Main
//-----------------------------------------------------------------
int main(int argc, char *argv[])
{
    int i, r;
    int n;
    spi_cfg_t spicfg;
    unsigned char data[512];
    unsigned char retbuf[512];

    //tspi_getdrvinfo();

    printf( "Opening SPI driver\n" );

    i = spi_open( "/dev/spi0" );
    if (i < 0)
    {
        printf( "spi_open() call failed\n" );
        return 0;
    }

    spicfg.mode = ((8 & SPI_MODE_CHAR_LEN_MASK) | SPI_MODE_BODER_MSB | SPI_MODE_CKPOL_HIGH);
    spicfg.clock_rate = 500000;

    r = spi_setcfg( i, (0 | SPI_DEV_DEFAULT), // Device
            &spicfg );

    if (r != EOK)
    {
        printf( "spi0_setcfg() call failed! (err=0x%x)\n", r );
        goto cleanup;
    }

    // fill data buffer with test data
    for (n = 0; n < 64; ++n)
    {
        data[n] = n;
    }

    printf( "Writing some data...\n" );
    for (n = 0; n < 100000; ++n)
    {
        // reset variables for each pass
        r = EOK;
        errno = EOK;
        memset( retbuf, 0, sizeof(retbuf) );

        r = spi_xchange( i, 0, data, retbuf, 64 );
        r = errno;

        if (r != EOK)
        {
            printf( "spi_xchange failed (err=0x%x)\n", r );
            printf( "error means %s \n", strerror( r ) );
            goto cleanup;
        }

        for (r = 0; r < 32; r++)
        {
            // Print out some of the return buffer to see it's valid data
            printf( "[r] = %x ", retbuf[r] );
        }
        printf( "\n" );
    }

    cleanup: spi_close( i );

    return EXIT_SUCCESS;
}
