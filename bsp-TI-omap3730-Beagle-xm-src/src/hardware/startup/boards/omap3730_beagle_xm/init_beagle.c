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

#include "startup.h"
#include <arm/omap2420.h>
#include <arm/omap3530.h>

/* PADCONF Regs for Beagle board revision GPIO */
#define CONTROL_PADCONF_MCSPI1_CLK      0x480021c8
#define CONTROL_PADCONF_MCSPI1_SOMI     0x480021cc

/* Bit definitions for the PADCONF registers */
#define MUXMODE0_MODE4  (4)
#define MUXMODE1_MODE4  (MUXMODE0_MODE4<<16)
#define PULLUDENABLE0   (1<<3)
#define PULLTYPE0_UP    (1<<4)
#define INPUTENABLE0    (1<<8)
#define PULLUDENABLE1   (1<<19)
#define PULLTYPE1_UP    (1<<20)
#define INPUTENABLE1    (1<<24)

// controls GPIOs 160-191
#define OMAP35XX_GPIO6_OE               0x49058034
#define OMAP35XX_GPIO6_DATAIN           0x49058038

// GPIOs 171-173 contain revision info
#define BEAGLE_REVISION_GPIO6_MASK      0x00003800
#define BEAGLE_REVISION_GPIO6_SHIFT     11

// Defines to determine revision from GPIO
#define BEAGLE_REVISION_A_B             7
#define BEAGLE_REVISION_C1_C3           6
#define BEAGLE_REVISION_C4              5
#define BEAGLE_REVISION_xM_B            0
#define BEAGLE_REVISION_xM_C            2

// BEAGLE machine name must be in the format below.
// resource_seed relies on OMAP3530 being at the front and
// pmic_tw4030_cfg relies on the entire string format.
#define BEAGLE_MACHINE_NAME(_rev) \
  "OMAP3730_BEAGLE_Rev_%s", _rev

extern int pin_mux;
extern int ddr_size;
extern char board_rev[];
extern char machine_name[];
extern int is_beagle;


/************************************************************/
void init_beagle(void)
{
    unsigned long raw_rev;
    is_beagle = 1;
    pin_mux = 1;

    /*
     * Configure pins for GPIO 171-173 for beagle board revision.
     * Enable pull-ups since earlier revs are unconnected.
     * GPIO 174 comes along for the ride, which is fine.
     */
    out32( CONTROL_PADCONF_MCSPI1_CLK, PULLUDENABLE1 | PULLTYPE1_UP | INPUTENABLE1 | MUXMODE1_MODE4
            | PULLUDENABLE0 | PULLTYPE0_UP | INPUTENABLE0 | MUXMODE0_MODE4);
    out32( CONTROL_PADCONF_MCSPI1_SOMI, PULLUDENABLE1 | PULLTYPE1_UP | INPUTENABLE1
            | MUXMODE1_MODE4 | PULLUDENABLE0 | PULLTYPE0_UP | INPUTENABLE0 | MUXMODE0_MODE4);

    // read GPIOs 171-173 to determine board rev

    out32( OMAP35XX_GPIO6_OE, in32( OMAP35XX_GPIO6_OE ) | BEAGLE_REVISION_GPIO6_MASK);
    raw_rev = (in32( OMAP35XX_GPIO6_DATAIN ) & BEAGLE_REVISION_GPIO6_MASK)
            >> BEAGLE_REVISION_GPIO6_SHIFT;

    switch (raw_rev) {
        case BEAGLE_REVISION_A_B:
            ddr_size = 128; // fall back to smaller DDR for older boards
            board_rev[0] = 'b';
            board_rev[1] = 'x';
            board_rev[2] = 0;
            break;

        case BEAGLE_REVISION_C1_C3:
            board_rev[0] = 'c';
            board_rev[1] = '3';
            board_rev[2] = 0;
            break;

        case BEAGLE_REVISION_C4:
            board_rev[0] = 'c';
            board_rev[1] = '4';
            board_rev[2] = 0;
            break;

        case BEAGLE_REVISION_xM_B:
            ddr_size = 512;
            board_rev[0] = 'x';
            board_rev[1] = 'm';
            board_rev[2] = '_';
            board_rev[3] = 'b';
            board_rev[4] = 0;
            break;
        case BEAGLE_REVISION_xM_C:
            ddr_size = 512;
            board_rev[0] = 'x';
            board_rev[1] = 'm';
            board_rev[2] = '_';
            board_rev[3] = 'c';
            board_rev[4] = 0;
            break;

            // Mark unknown revisions as 'xx'
        default:
            board_rev[0] = 'x';
            board_rev[1] = 'x';
            board_rev[2] = 0;
            break;
    }

    kprintf( "Beagle Board Revision: %s\n", board_rev );

    // add comment, use macro
    ksprintf( machine_name, BEAGLE_MACHINE_NAME(board_rev));

}
