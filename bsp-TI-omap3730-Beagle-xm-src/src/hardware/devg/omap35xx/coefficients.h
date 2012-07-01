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
#ifndef __OMAP35XX_COEFFICIENTS_H_INCLUDED
#define __OMAP35XX_COEFFICIENTS_H_INCLUDED

#define OMAP_CUSTOM_COLOR_COEFFICIENTS 4
void omap_coef_display(disp_adapter_t *adapter, int layer);
void omap_coef_vid_up_v3tap(disp_adapter_t * adapter, unsigned int layer);
void omap_coef_vid_up_v5tap(disp_adapter_t * adapter, unsigned int layer) ;
void omap_coef_vid_up_h5tap(disp_adapter_t * adapter, unsigned int layer);
void omap_coef_vid_down_v3tap(disp_adapter_t * adapter, unsigned int layer) ;
void omap_coef_vid_down_v5tap(disp_adapter_t * adapter, unsigned int layer);
void omap_coef_vid_down_h5tap(disp_adapter_t * adapter, unsigned int layer);
int set_yuv_to_rgb_conversion(disp_adapter_t *adapter, int layer, int entry);
void set_custom_color_coefficients(disp_adapter_t *adapter, int layer, short ry, 
                                   short rcr, short rcb, short gy, short gcr, 
								   short gcb, short by, short bcr, short bcb);

#endif
