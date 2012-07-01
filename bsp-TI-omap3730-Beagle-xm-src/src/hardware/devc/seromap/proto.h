/*
 * $QNXLicenseC: 
 * Copyright 2008, QNX Software Systems.  
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







#ifndef PROTO_H
#define PROTO_H

DEV_OMAP *create_device(TTYINIT_OMAP *dip, unsigned unit, unsigned maxim_xcvr_kick);
void ser_stty(DEV_OMAP *dev);
void ser_ctrl(DEV_OMAP *dev, unsigned flags);
void set_port(unsigned port, unsigned mask, unsigned data);
void *query_default_device(TTYINIT_OMAP *dip, void *link);
const struct sigevent *ser_intr(void *area, int id);
unsigned options(int argc, char *argv[]);
void seromap_power_init(DEV_OMAP *dev, TTYINIT_OMAP *dip);

#endif

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/devc/seromap/proto.h $ $Rev: 308279 $" )
