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
 * init_raminfo.c
 *	Tell syspage about our RAM configuration
 */
#include "startup.h"

extern int mtp, pin_mux, ddr_size;

void init_raminfo() 
{
  if (!ddr_size)
  {
    if (mtp || pin_mux)
    {
        kprintf("Adding 512 MB, MTP or PIN_MUX set \n");
      add_ram(0x80000000, MEG(512));

      //add_ram(0x80000000, MEG(256));
    }
    else
      add_ram(0x80000000, MEG(128));
  }
  else
  {
    switch (ddr_size)
    {
      case 512: add_ram(0x80000000, MEG(512));  break;
      case 256: add_ram(0x80000000, MEG(256));  break;
      case 128: add_ram(0x80000000, MEG(128));  break;
      default:  
        kprintf("ERROR: failed to init RAM -- invalid size specified.\n");
        while (1);
        break;
    }
  }
}

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/startup/boards/omap3530/init_raminfo.c $ $Rev: 308279 $" );
