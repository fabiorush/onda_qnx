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
 
#ifndef __OMAP35XX_PROT_H_INCLUDED
#define __OMAP35XX_PROT_H_INCLUDED

/* mode.c */

int omap_mode_init(disp_adapter_t *adapter, char *optstring);

void omap_mode_fini(disp_adapter_t *adapter);

int omap_set_mode(disp_adapter_t *adapter, int dispno, disp_mode_t mode,
		disp_crtc_settings_t *settings, disp_surface_t *surf, unsigned flags);
		
int omap_get_modeinfo(disp_adapter_t *adapter, int dispno,
		disp_mode_t attributes, disp_mode_info_t *info) ;
		
int omap_get_modelist(disp_adapter_t *adapter, int dispno,
		unsigned short *list, int index, int size) ;

int omap_wait_vsync(disp_adapter_t *adapter, int dispno) ;

int omap_layer_flushrect(disp_adapter_t *adapter, int dispno, int layer_idx, int x1, int y1, int x2, int y2);

int devg_get_modefuncs(disp_adapter_t *adapter, disp_modefuncs_t *funcs,
		int tabsize);
		

/* layer.c */
int omap_layer_query(disp_adapter_t * adapter, int dispno, int layer_idx,
		int fmt_idx, disp_layer_query_t * info_t) ;

int omap_layer_enable(disp_adapter_t * adapter, int dispno, int layer_idx);

int omap_layer_disable(disp_adapter_t * adapter, int dispno, int layer_idx);

int omap_layer_set_surface(disp_adapter_t * adapter, int dispno, int layer_idx,
		unsigned layer_format, disp_surface_t *surfaces[]) ;

int omap_layer_set_source_viewport(disp_adapter_t * adapter, int dispno,
		int layer_idx, int x1, int y1, int x2, int y2);

int omap_layer_set_dest_viewport(disp_adapter_t * adapter, int dispno,
		int layer_idx, int x1, int y1, int x2, int y2);

int omap_layer_set_blending(disp_adapter_t *adapter, int dispno, int layer_idx,
		unsigned mode, int m1, int m2);


int omap_layer_set_chromakey(disp_adapter_t *adapter, int dispno,
		int layer_idx, unsigned chroma_mode, disp_color_t color0,
		disp_color_t color1, disp_color_t mask) ;
		
int omap_layer_set_flags(disp_adapter_t * adapter, int dispno, int layer_idx,
		unsigned flag_mask, unsigned flag_values) ;

void omap_layer_update_begin(disp_adapter_t *adapter, int dispno,
		uint64_t layer_mask) ;


void omap_layer_update_end(disp_adapter_t *adapter, int dispno,
		uint64_t layer_mask, int wait_vsync) ;

void omap_layer_reset(disp_adapter_t * adapter, int dispno, int layer_idx) ;

int omap_layer_set_alpha_map(disp_adapter_t *adapter, int dispno,
		int layer_idx, disp_surface_t *map);



/* hcursor.c */
int omap_set_hw_cursor(disp_adapter_t *adapter, int dispno, 
	uint8_t * bmp0, uint8_t * bmp1, unsigned color0, unsigned color1,
	int hotspot_x, int hotspot_y, int size_x, int size_y, int bmp_stride);
	
void omap_enable_hw_cursor(disp_adapter_t *adapter, int dispno);

void omap_disable_hw_cursor(disp_adapter_t *adapter, int dispno);

void omap_set_hw_cursor_pos(disp_adapter_t *adapter, int dispno, int x, int y);

/* data.c */
int get_config_data(omap_ctx_t *o_ctx, const char *filename);

/* irs.c */
int omap_isr_setup(disp_adapter_t *adapter, omap_ctx_t *o_ctx) ;
void omap_isr_cleanup(disp_adapter_t *adapter, omap_ctx_t *o_ctx);
		

/* misc.c */
int devg_get_contextfuncs(disp_adapter_t * adapter,
		disp_draw_contextfuncs_t * funcs, int tabsize);

int devg_get_corefuncs(disp_adapter_t *adapter, unsigned pixel_format,
		disp_draw_corefuncs_t *funcs, int tabsize);

int omap_misc_wait_idle(disp_adapter_t *adapter);

int omap_misc_init(disp_adapter_t *adapter, char *optstring);

void omap_misc_fini(disp_adapter_t *adapter);

int omap_attach_external(disp_adapter_t *adapter, disp_aperture_t aper[]);

int omap_detach_external(disp_adapter_t *adapter);

int omap_recover(disp_adapter_t *adapter);

void omap_module_info(disp_adapter_t *adp, disp_module_info_t *info);

int omap_get_corefuncs_sw(disp_adapter_t *context, unsigned pixel_format,
		disp_draw_corefuncs_t *funcs, int tabsize);

int devg_get_miscfuncs(disp_adapter_t *adp, disp_draw_miscfuncs_t *funcs,
		int tabsize);




/* mem.h */
int omap_mem_init(disp_adapter_t *adapter, char *optstring);

void omap_mem_fini(disp_adapter_t *adapter);

int omap_mem_reset(disp_adapter_t *adapter, disp_surface_t *surf);

disp_surface_t * omap_alloc_surface(disp_adapter_t *adapter, int width,
		int height, unsigned format, unsigned flags, unsigned user_flags);

int omap_free_surface(disp_adapter_t *adapter, disp_surface_t *surf);

unsigned long omap_mem_avail(disp_adapter_t *adapter, unsigned flags);

int omap_query_apertures(disp_adapter_t *adapter, disp_aperture_t *ap);

int omap_query_surface(disp_adapter_t *adapter, disp_surface_t *surf,
		disp_surface_info_t *info);

int omap_get_alloc_info(disp_adapter_t *adapter, int width, int height,
		unsigned format, unsigned flags, unsigned user_flags,
		disp_alloc_info_t *info);
		
int omap_get_alloc_layer_info(disp_adapter_t *adapter, int dispno[],
		int layer_idx[], int nlayers, unsigned format, int surface_index,
		int width, int height, unsigned sflags, unsigned hint_flags,
		disp_alloc_info_t *info);

int devg_get_memfuncs(disp_adapter_t *adapter, disp_memfuncs_t *funcs,
		int tabsize);

#endif

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/devg/omap35xx/omap35xx_proto.h $ $Rev: 308279 $" )
