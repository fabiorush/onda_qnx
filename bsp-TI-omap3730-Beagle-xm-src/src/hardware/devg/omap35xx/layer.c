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

#include <string.h>
#include <unistd.h>

#include <sys/neutrino.h>
#include <sys/trace.h>


#include "omap.h"

/*
 * VID1 Bottom
 * VID2 
 * GFX Top
 */

#define OMAP_GFX_NUMBER_FORMATS 3
static unsigned gfx_formats[OMAP_GFX_NUMBER_FORMATS] = {
		DISP_LAYER_FORMAT_RGB565, DISP_LAYER_FORMAT_RGB888,
		DISP_LAYER_FORMAT_ARGB8888 };

#define OMAP_VID1_NUMBER_FORMATS 6
static unsigned vid1_formats[OMAP_VID1_NUMBER_FORMATS] = {
		DISP_LAYER_FORMAT_RGB565, DISP_LAYER_FORMAT_RGB888,
		DISP_LAYER_FORMAT_YUY2, DISP_LAYER_FORMAT_UYVY,
		DISP_LAYER_FORMAT_YVYU, DISP_LAYER_FORMAT_V422 };

#define OMAP_VID2_NUMBER_FORMATS 7
static unsigned vid2_formats[OMAP_VID2_NUMBER_FORMATS] = {
		DISP_LAYER_FORMAT_RGB565, DISP_LAYER_FORMAT_RGB888,
		DISP_LAYER_FORMAT_ARGB8888,	DISP_LAYER_FORMAT_YUY2,
		DISP_LAYER_FORMAT_UYVY,	DISP_LAYER_FORMAT_YVYU,
		DISP_LAYER_FORMAT_V422 };

/* switch this code out to remove warning, want the functions to be static, don't need them right now but might later,  gives a warning so... */
#if 0
/*
 * Configures video up sampling for Video layer 1.
 * Configures 5 tap horizontal, 3 tap vertical
 * Vaules taken direclty from  SPRUFA4 February 2008
 */
static void omap_init_vid1_upscale_coefficients_v3andh5_tap(disp_adapter_t * adapter) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;
	*OMAP_DISPC_VID1_FIR_COEF_H(0)  = 0x00800000;
	*OMAP_DISPC_VID1_FIR_COEF_HV(0) = 0x00800000;
	*OMAP_DISPC_VID1_FIR_COEF_H(1)  = 0x0D7CF800;
	*OMAP_DISPC_VID1_FIR_COEF_HV(1) = 0x037B02FF;
	*OMAP_DISPC_VID1_FIR_COEF_H(2)  = 0x1E70F5FF;
	*OMAP_DISPC_VID1_FIR_COEF_HV(2) = 0x0C6F05FE;
	*OMAP_DISPC_VID1_FIR_COEF_H(3)  = 0x335FF5FE;
	*OMAP_DISPC_VID1_FIR_COEF_HV(3) = 0x205907FB;
	*OMAP_DISPC_VID1_FIR_COEF_H(4)  = 0xF74949F7;
	*OMAP_DISPC_VID1_FIR_COEF_HV(4) = 0x00404000;
	*OMAP_DISPC_VID1_FIR_COEF_H(5)  = 0xF55F33FB;
	*OMAP_DISPC_VID1_FIR_COEF_HV(5) = 0x075920FE;
	*OMAP_DISPC_VID1_FIR_COEF_H(6)  = 0xF5701EFE;
	*OMAP_DISPC_VID1_FIR_COEF_HV(6) = 0x056F0CFF;
	*OMAP_DISPC_VID1_FIR_COEF_H(7)  = 0xF87C0DFF;
	*OMAP_DISPC_VID1_FIR_COEF_HV(7) = 0x027B0300;
}




/*
 * Configures video up sampling for Video layer 2
 * Configures 5 tap horizontal, 3 tap vertical
 * Vaules taken direclty from  SPRUFA4 February 2008
 */
static void omap_init_vid2_upscale_coefficients_v3andh5_tap(disp_adapter_t * adapter) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;
	*OMAP_DISPC_VID2_FIR_COEF_H(0) = 0x00800000;
	*OMAP_DISPC_VID2_FIR_COEF_HV(0) = 0x00800000;
	*OMAP_DISPC_VID2_FIR_COEF_H(1) = 0x0D7CF800;
	*OMAP_DISPC_VID2_FIR_COEF_HV(1) = 0x037B02FF;
	*OMAP_DISPC_VID2_FIR_COEF_H(2) = 0x1E70F5FF;
	*OMAP_DISPC_VID2_FIR_COEF_HV(2) = 0x0C6F05FE;
	*OMAP_DISPC_VID2_FIR_COEF_H(3) = 0x335FF5FE;
	*OMAP_DISPC_VID2_FIR_COEF_HV(3) = 0x205907FB;
	*OMAP_DISPC_VID2_FIR_COEF_H(4) = 0xF74949F7;
	*OMAP_DISPC_VID2_FIR_COEF_HV(4) = 0x00404000;
	*OMAP_DISPC_VID2_FIR_COEF_H(5) = 0xF55F33FB;
	*OMAP_DISPC_VID2_FIR_COEF_HV(5) = 0x075920FE;
	*OMAP_DISPC_VID2_FIR_COEF_H(6) = 0xF5701EFE;
	*OMAP_DISPC_VID2_FIR_COEF_HV(6) = 0x056F0CFF;
	*OMAP_DISPC_VID2_FIR_COEF_H(7) = 0xF87C0DFF;
	*OMAP_DISPC_VID2_FIR_COEF_HV(7) = 0x027B0300;
}


/*
 * Configures video down sampling for Video layer 1
 * Configures 5 tap horizontal,3 tap vertical
 * Vaules taken direclty from  SPRUFA4 February 2008
 */
static void omap_init_vid1_downscale_coefficients_v3andh5_tap(disp_adapter_t * adapter) {
	omap_ctx_t *o_ctx= adapter->ms_ctx;
	TRACE;
	*OMAP_DISPC_VID1_FIR_COEF_H(0) = 0x24382400;
	*OMAP_DISPC_VID1_FIR_COEF_HV(0) = 0x24382400;
	*OMAP_DISPC_VID1_FIR_COEF_H(1) = 0x28371FFE;
	*OMAP_DISPC_VID1_FIR_COEF_HV(1) = 0x28391F04;
	*OMAP_DISPC_VID1_FIR_COEF_H(2) = 0x2C361BFB;
	*OMAP_DISPC_VID1_FIR_COEF_HV(2) = 0x2D381B08;
	*OMAP_DISPC_VID1_FIR_COEF_H(3) = 0x303516F9;
	*OMAP_DISPC_VID1_FIR_COEF_HV(3) = 0x3237170C;
	*OMAP_DISPC_VID1_FIR_COEF_H(4) = 0x11343311;
	*OMAP_DISPC_VID1_FIR_COEF_HV(4) = 0x123737F7;
	*OMAP_DISPC_VID1_FIR_COEF_H(5) = 0x1635300C;
	*OMAP_DISPC_VID1_FIR_COEF_HV(5) = 0x173732F9;
	*OMAP_DISPC_VID1_FIR_COEF_H(6) = 0x1B362C08;
	*OMAP_DISPC_VID1_FIR_COEF_HV(6) = 0x1B382DFB;
	*OMAP_DISPC_VID1_FIR_COEF_H(7) = 0x1F372804;
	*OMAP_DISPC_VID1_FIR_COEF_HV(7) = 0x1F3928FE;
}

/*
 * Configures video down sampling for Video layer 2
 * Configures 5 tap horizontal, 3 tap vertical
 * Vaules taken direclty from  SPRUFA4 February 2008
 */
static void omap_init_vid2_downscale_coefficients_v3andh5_tap(disp_adapter_t * adapter) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;
	*OMAP_DISPC_VID2_FIR_COEF_H(0) = 0x24382400;
	*OMAP_DISPC_VID2_FIR_COEF_HV(0) = 0x24382400;
	*OMAP_DISPC_VID2_FIR_COEF_H(1) = 0x28371FFE;
	*OMAP_DISPC_VID2_FIR_COEF_HV(1) = 0x28391F04;
	*OMAP_DISPC_VID2_FIR_COEF_H(2) = 0x2C361BFB;
	*OMAP_DISPC_VID2_FIR_COEF_HV(2) = 0x2D381B08;
	*OMAP_DISPC_VID2_FIR_COEF_H(3) = 0x303516F9;
	*OMAP_DISPC_VID2_FIR_COEF_HV(3) = 0x3237170C;
	*OMAP_DISPC_VID2_FIR_COEF_H(4) = 0x11343311;
	*OMAP_DISPC_VID2_FIR_COEF_HV(4) = 0x123737F7;
	*OMAP_DISPC_VID2_FIR_COEF_H(5) = 0x1635300C;
	*OMAP_DISPC_VID2_FIR_COEF_HV(5) = 0x173732F9;
	*OMAP_DISPC_VID2_FIR_COEF_H(6) = 0x1B362C08;
	*OMAP_DISPC_VID2_FIR_COEF_HV(6) = 0x1B382DFB;
	*OMAP_DISPC_VID2_FIR_COEF_H(7) = 0x1F372804;
	*OMAP_DISPC_VID2_FIR_COEF_HV(7) = 0x1F3928FE;
}


#endif
/*
 * Configures video up sampling for Video layer 1.
 * Configures 5 tap horizontal, 5 tap vertical
 * Vaules taken direclty from  SPRUFA4 February 2008
 */
static void omap_init_vid1_upscale_coefficients_v5andh5_tap(disp_adapter_t * adapter) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;
	*OMAP_DISPC_VID1_FIR_COEF_H(0) = 0x00800000;
	*OMAP_DISPC_VID1_FIR_COEF_HV(0) = 0x00800000;
	*OMAP_DISPC_VID1_FIR_COEF_V(0) = 0x00000000;
	*OMAP_DISPC_VID1_FIR_COEF_H(1) = 0x0D7CF800;
	*OMAP_DISPC_VID1_FIR_COEF_HV(1) = 0x0D7CF8FF;
	*OMAP_DISPC_VID1_FIR_COEF_V(1) = 0x0000FF00;
	*OMAP_DISPC_VID1_FIR_COEF_H(2) = 0x1E70F5FF;
	*OMAP_DISPC_VID1_FIR_COEF_HV(2) = 0x1E70F5FE;
	*OMAP_DISPC_VID1_FIR_COEF_V(2) = 0x0000FEFF;
	*OMAP_DISPC_VID1_FIR_COEF_H(3) = 0x335FF5FE;
	*OMAP_DISPC_VID1_FIR_COEF_HV(3) = 0x335FF5FB;
	*OMAP_DISPC_VID1_FIR_COEF_V(3) = 0x0000FBFE;
	*OMAP_DISPC_VID1_FIR_COEF_H(4) = 0xF74949F7;
	*OMAP_DISPC_VID1_FIR_COEF_HV(4) = 0xF7404000;
	*OMAP_DISPC_VID1_FIR_COEF_V(4) = 0x000000F7;
	*OMAP_DISPC_VID1_FIR_COEF_H(5) = 0xF55F33FB;
	*OMAP_DISPC_VID1_FIR_COEF_HV(5) = 0xF55F33FE;
	*OMAP_DISPC_VID1_FIR_COEF_V(5) = 0x0000FEFB;
	*OMAP_DISPC_VID1_FIR_COEF_H(6) = 0xF5701EFE;
	*OMAP_DISPC_VID1_FIR_COEF_HV(6) = 0xF5701EFF;
	*OMAP_DISPC_VID1_FIR_COEF_V(6) = 0x0000FFFE;
	*OMAP_DISPC_VID1_FIR_COEF_H(7) = 0xF87C0DFF;
	*OMAP_DISPC_VID1_FIR_COEF_HV(7) = 0xF87C0D00;
	*OMAP_DISPC_VID1_FIR_COEF_V(7) = 0x000000FF;
}

/*
 * Configures video up sampling for Video layer 2
 * Configures 5 tap horizontal, 5 tap vertical
 * Vaules taken direclty from  SPRUFA4 February 2008
 */
static void omap_init_vid2_upscale_coefficients_v5andh5_tap(disp_adapter_t * adapter) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;
	*OMAP_DISPC_VID2_FIR_COEF_H(0) = 0x00800000;
	*OMAP_DISPC_VID2_FIR_COEF_HV(0) = 0x00800000;
	*OMAP_DISPC_VID2_FIR_COEF_V(0) = 0x00000000;
	*OMAP_DISPC_VID2_FIR_COEF_H(1) = 0x0D7CF800;
	*OMAP_DISPC_VID2_FIR_COEF_HV(1) = 0x0D7CF8FF;
	*OMAP_DISPC_VID2_FIR_COEF_V(1) = 0x0000FF00;
	*OMAP_DISPC_VID2_FIR_COEF_H(2) = 0x1E70F5FF;
	*OMAP_DISPC_VID2_FIR_COEF_HV(2) = 0x1E70F5FE;
	*OMAP_DISPC_VID2_FIR_COEF_V(2) = 0x0000FEFF;
	*OMAP_DISPC_VID2_FIR_COEF_H(3) = 0x335FF5FE;
	*OMAP_DISPC_VID2_FIR_COEF_HV(3) = 0x335FF5FB;
	*OMAP_DISPC_VID2_FIR_COEF_V(3) = 0x0000FBFE;
	*OMAP_DISPC_VID2_FIR_COEF_H(4) = 0xF74949F7;
	*OMAP_DISPC_VID2_FIR_COEF_HV(4) = 0xF7404000;
	*OMAP_DISPC_VID2_FIR_COEF_V(4) = 0x000000F7;
	*OMAP_DISPC_VID2_FIR_COEF_H(5) = 0xF55F33FB;
	*OMAP_DISPC_VID2_FIR_COEF_HV(5) = 0xF55F33FE;
	*OMAP_DISPC_VID2_FIR_COEF_V(5) = 0x0000FEFB;
	*OMAP_DISPC_VID2_FIR_COEF_H(6) = 0xF5701EFE;
	*OMAP_DISPC_VID2_FIR_COEF_HV(6) = 0xF5701EFF;
	*OMAP_DISPC_VID2_FIR_COEF_V(6) = 0x0000FFFE;
	*OMAP_DISPC_VID2_FIR_COEF_H(7) = 0xF87C0DFF;
	*OMAP_DISPC_VID2_FIR_COEF_HV(7) = 0xF87C0D00;
	*OMAP_DISPC_VID2_FIR_COEF_V(7) = 0x000000FF;
}


/*
 * Configures video down sampling for Video layer 1
 * Configures 5 tap horizontal, 5 tap vertical
 * Vaules taken direclty from  SPRUFA4 February 2008
 */
static void omap_init_vid1_downscale_coefficients_v5andh5_tap(disp_adapter_t * adapter) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;
	*OMAP_DISPC_VID1_FIR_COEF_H(0) = 0x24382400;
	*OMAP_DISPC_VID1_FIR_COEF_HV(0) = 0x24382400;
	*OMAP_DISPC_VID1_FIR_COEF_V(0) = 0x00000000;
	*OMAP_DISPC_VID1_FIR_COEF_H(1) = 0x28371FFE;
	*OMAP_DISPC_VID1_FIR_COEF_HV(1) = 0x28371F04;
	*OMAP_DISPC_VID1_FIR_COEF_V(1) = 0x000004FE;
	*OMAP_DISPC_VID1_FIR_COEF_H(2) = 0x2C361BFB;
	*OMAP_DISPC_VID1_FIR_COEF_HV(2) = 0x2C361B08;
	*OMAP_DISPC_VID1_FIR_COEF_V(2) = 0x000008FB;
	*OMAP_DISPC_VID1_FIR_COEF_H(3) = 0x303516F9;
	*OMAP_DISPC_VID1_FIR_COEF_HV(3) = 0x3035160C;
	*OMAP_DISPC_VID1_FIR_COEF_V(3) = 0x00000CF9;
	*OMAP_DISPC_VID1_FIR_COEF_H(4) = 0x11343311;
	*OMAP_DISPC_VID1_FIR_COEF_HV(4) = 0x113433F7;
	*OMAP_DISPC_VID1_FIR_COEF_V(4) = 0x0000F711;
	*OMAP_DISPC_VID1_FIR_COEF_H(5) = 0x1635300C;
	*OMAP_DISPC_VID1_FIR_COEF_HV(5) = 0x163530F9;
	*OMAP_DISPC_VID1_FIR_COEF_V(5) = 0x0000F90C;
	*OMAP_DISPC_VID1_FIR_COEF_H(6) = 0x1B362C08;
	*OMAP_DISPC_VID1_FIR_COEF_HV(6) = 0x1B362CFB;
	*OMAP_DISPC_VID1_FIR_COEF_V(6) = 0x0000FB08;
	*OMAP_DISPC_VID1_FIR_COEF_H(7) = 0x1F372804;
	*OMAP_DISPC_VID1_FIR_COEF_HV(7) = 0x1F3728FE;
	*OMAP_DISPC_VID1_FIR_COEF_V(7) = 0x0000FE04;
}


/*
 * Configures video down sampling for Video layer 2
 * Configures 5 tap horizontal, 5 tap vertical
 * Vaules taken direclty from  SPRUFA4 February 2008
 */
static void omap_init_vid2_downscale_coefficients_v5andh5_tap(disp_adapter_t * adapter) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;
	*OMAP_DISPC_VID2_FIR_COEF_H(0) = 0x24382400;
	*OMAP_DISPC_VID2_FIR_COEF_HV(0) = 0x24382400;
	*OMAP_DISPC_VID2_FIR_COEF_V(0) = 0x00000000;
	*OMAP_DISPC_VID2_FIR_COEF_H(1) = 0x28371FFE;
	*OMAP_DISPC_VID2_FIR_COEF_HV(1) = 0x28371F04;
	*OMAP_DISPC_VID2_FIR_COEF_V(1) = 0x000004FE;
	*OMAP_DISPC_VID2_FIR_COEF_H(2) = 0x2C361BFB;
	*OMAP_DISPC_VID2_FIR_COEF_HV(2) = 0x2C361B08;
	*OMAP_DISPC_VID2_FIR_COEF_V(2) = 0x000008FB;
	*OMAP_DISPC_VID2_FIR_COEF_H(3) = 0x303516F9;
	*OMAP_DISPC_VID2_FIR_COEF_HV(3) = 0x3035160C;
	*OMAP_DISPC_VID2_FIR_COEF_V(3) = 0x00000CF9;
	*OMAP_DISPC_VID2_FIR_COEF_H(4) = 0x11343311;
	*OMAP_DISPC_VID2_FIR_COEF_HV(4) = 0x113433F7;
	*OMAP_DISPC_VID2_FIR_COEF_V(4) = 0x0000F711;
	*OMAP_DISPC_VID2_FIR_COEF_H(5) = 0x1635300C;
	*OMAP_DISPC_VID2_FIR_COEF_HV(5) = 0x163530F9;
	*OMAP_DISPC_VID2_FIR_COEF_V(5) = 0x0000F90C;
	*OMAP_DISPC_VID2_FIR_COEF_H(6) = 0x1B362C08;
	*OMAP_DISPC_VID2_FIR_COEF_HV(6) = 0x1B362CFB;
	*OMAP_DISPC_VID2_FIR_COEF_V(6) = 0x0000FB08;
	*OMAP_DISPC_VID2_FIR_COEF_H(7) = 0x1F372804;
	*OMAP_DISPC_VID2_FIR_COEF_HV(7) = 0x1F3728FE;
	*OMAP_DISPC_VID2_FIR_COEF_V(7) = 0x0000FE04;
}


int omap_layer_query(disp_adapter_t * adapter, int dispno, int layer_idx,
		int fmt_idx, disp_layer_query_t * info_t) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	disp_layer_query_t info;
	TRACE;
	DEBUG_CMD(disp_printf(adapter,"omap_layer_query Layer %d index %d", layer_idx, fmt_idx));

	/* Graphics layer GFX is used for hardware cursor */
	if (o_ctx->hcursor != 0) {
		if (layer_idx > (OMAP_NUMBER_LAYERS-1)) {
			DEBUG_MSG("Layer index of range, (Graphics layer GFX is used for hardware cursor)");
			return -1;
		}
	} else if (layer_idx > OMAP_NUMBER_LAYERS) {
		DEBUG_MSG("Layer index of range");
		return -1;
	}

	if (info_t->size > 0) {
		info.size = info_t->size;
	} else {
		info.size = sizeof(info);
	}

	info.src_max_height = 2048;
	info.src_max_width = 2048;
	info.dst_min_width = info.dst_min_height = 1;
	info.dst_max_width = info.src_max_viewport_width = o_ctx->width;
	info.dst_max_height = info.src_max_viewport_height = o_ctx->height;
	info.chromakey_caps = 0;
	info.alpha_valid_flags = 0;
	info.alpha_combinations = 0;
	info.max_scaledown_x = info.max_scaledown_y = 1;
	info.max_scaleup_x = info.max_scaleup_y = 1;

	switch (layer_idx) {
	case OMAP_LAYER_VID1:
		if (fmt_idx >= OMAP_VID1_NUMBER_FORMATS) {
			DEBUG_MSG("Video 1, layer format out of range (not an error, maybe end of format query)");
			return -1;
		}
		info.pixel_format = vid1_formats[fmt_idx];
		info.caps = DISP_LAYER_CAP_PAN_SOURCE | DISP_LAYER_CAP_DISABLE
				| DISP_LAYER_CAP_PAN_DEST | DISP_LAYER_CAP_MAIN_DISPLAY;
		info.alpha_valid_flags = 0;
		info.chromakey_caps = 0;
		info.order_mask = 0;
		break;
	case OMAP_LAYER_VID2:
		if (fmt_idx >= OMAP_VID2_NUMBER_FORMATS) {
			DEBUG_MSG("Video 2, layer format out of range (not an error, maybe end of format query)");
			return -1;
		}
		info.pixel_format = vid2_formats[fmt_idx];
		info.caps = DISP_LAYER_CAP_PAN_SOURCE | DISP_LAYER_CAP_DISABLE
				| DISP_LAYER_CAP_PAN_DEST;
		info.alpha_valid_flags = DISP_ALPHA_M1_GLOBAL | DISP_BLEND_SRC_M1_ALPHA;
		info.chromakey_caps = 0;
		info.order_mask = 0;
		break;
	case OMAP_LAYER_GFX:
		if (o_ctx->hcursor != 0) {
			return -1;
		}
		if (fmt_idx >= OMAP_GFX_NUMBER_FORMATS) {
			DEBUG_MSG("GFX layer format out of range (not an error, maybe end of format query)");
			return -1;
		}
		info.pixel_format = gfx_formats[fmt_idx];
		info.caps = DISP_LAYER_CAP_PAN_SOURCE | DISP_LAYER_CAP_DISABLE
				| DISP_LAYER_CAP_PAN_DEST;
		info.alpha_valid_flags = DISP_ALPHA_M1_GLOBAL | DISP_BLEND_SRC_M1_ALPHA;
		info.chromakey_caps =DISP_LAYER_CHROMAKEY_CAP_SRC_SINGLE
				| DISP_LAYER_CHROMAKEY_CAP_SHOWTHROUGH;
		info.order_mask = 0;
		break;
	default:
		DEBUG_MSG("Layer unknown, (no more layers, maybe end of layer query)");
		return -1;
	}
	memcpy(info_t, &info, info.size);

	return 0;
}

int omap_layer_enable(disp_adapter_t * adapter, int dispno, int layer_idx) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;

	if (layer_idx > OMAP_NUMBER_LAYERS) {
		SLOG_ERROR("Layer out of range");
		return -1;
	}
	o_ctx->layer[layer_idx].attribute |= 1;
	return 0;
}

int omap_layer_disable(disp_adapter_t * adapter, int dispno, int layer_idx) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;

	if (layer_idx > OMAP_NUMBER_LAYERS) {
		SLOG_ERROR("Layer out of range");
		return -1;
	}
	o_ctx->layer[layer_idx].attribute &= ~1;
	return 0;
}

int omap_layer_set_surface(disp_adapter_t * adapter, int dispno, int layer_idx,
		unsigned layer_format, disp_surface_t *surfaces[]) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	disp_surface_t *surf = surfaces[0];
	TRACE;

	if (layer_idx > OMAP_NUMBER_LAYERS) {
		SLOG_ERROR("Layer out of range");
		return -1;
	}

	o_ctx->layer[layer_idx].vidptr = surf->vidptr;
	o_ctx->layer[layer_idx].offset = surf->paddr;
	o_ctx->layer[layer_idx].stride = surf->stride;
	o_ctx->layer[layer_idx].height = surf->height;
	o_ctx->layer[layer_idx].surface_format = surf->pixel_format;

	/* Layer format types (SPRUFA4 February 2008)
	*   0x4: RGB 12 (16-bit container)
	*   0x5: ARGB 16
	*   0x6: RGB 16
	*   0x8: RGB 24 (un-packed in 32-bit container)
	*   0x9: RGB 24 (packed in 24-bit container)
	*   0xA: YUV2 4:2:2                                    => DISP_LAYER_FORMAT_YUY2
	*   0xA: YUV2 4:2:2  (Big Endian, bit 17)  => DISP_LAYER_FORMAT_V422
	*   0xB: UYVY 4:2:2                                     => DISP_LAYER_FORMAT_UYVY
	*   0xB: UYVY 4:2:2 (Big Endian, bit 17)   => DISP_LAYER_FORMAT_YVYU
	*   0xC: ARGB 32
	*   0xD: RGBA 32
	*   0xE: RGBx 32
	*/


	switch (layer_format) {
	case DISP_LAYER_FORMAT_RGB565:
		o_ctx->layer[layer_idx].bpp  = 2;
		o_ctx->layer[layer_idx].ppmp = 1;
		o_ctx->layer[layer_idx].omap_layer_format = (0x06<<1);
		break;
	case DISP_LAYER_FORMAT_RGB888:
		o_ctx->layer[layer_idx].bpp  = 3;
		o_ctx->layer[layer_idx].ppmp = 1;
		o_ctx->layer[layer_idx].omap_layer_format = (0x09<<1);
		break;
	case DISP_LAYER_FORMAT_ARGB8888:
		o_ctx->layer[layer_idx].bpp  = 4;
		o_ctx->layer[layer_idx].ppmp = 1;
		o_ctx->layer[layer_idx].omap_layer_format = (0x0C<<1);
		break;
	case DISP_LAYER_FORMAT_YUY2:
		o_ctx->layer[layer_idx].bpp  = 2;
		o_ctx->layer[layer_idx].ppmp = 2;
		o_ctx->layer[layer_idx].omap_layer_format = (0x0A<<1) | (1<<9);
		break;
	case DISP_LAYER_FORMAT_UYVY:
		o_ctx->layer[layer_idx].bpp  = 2;
		o_ctx->layer[layer_idx].ppmp = 2;
		o_ctx->layer[layer_idx].omap_layer_format = (0x0B<<1)  | (1<<9);
		break;
	case DISP_LAYER_FORMAT_YVYU:
		o_ctx->layer[layer_idx].bpp  = 2;
		o_ctx->layer[layer_idx].ppmp = 2;
		o_ctx->layer[layer_idx].omap_layer_format = (0x0B<<1) | (1<<17) | (1<<9);
		break;
	case DISP_LAYER_FORMAT_V422:
		o_ctx->layer[layer_idx].bpp  = 2;
		o_ctx->layer[layer_idx].ppmp = 2;
		o_ctx->layer[layer_idx].omap_layer_format = (0x0A<<1) | (1<<17) | (1<<9);
		break;

	default:
		SLOG_ERROR("format unknown");
		return -1;
	}

	return 0;
}


int omap_layer_set_source_viewport(disp_adapter_t * adapter, int dispno,
		int layer_idx, int x1, int y1, int x2, int y2) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;

	TRACE;

	if (layer_idx > OMAP_NUMBER_LAYERS) {
		SLOG_ERROR("Layer out of range");
		return -1;
	}

	o_ctx->layer[layer_idx].src1.x = x1;
	o_ctx->layer[layer_idx].src1.y = y1;
	o_ctx->layer[layer_idx].src2.x = x2;
	o_ctx->layer[layer_idx].src2.y = y2;
	return 0;
}

int omap_layer_set_dest_viewport(disp_adapter_t * adapter, int dispno,
		int layer_idx, int x1, int y1, int x2, int y2) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;

	if (layer_idx > OMAP_NUMBER_LAYERS) {
		SLOG_ERROR("Layer out of range");
		return -1;
	}

	DEBUG_CMD(disp_printf(adapter,"initial x1 %d, y1 %d, x2 %d, y2 %d", x1, y1, x2, y2));
	if (x1 < 0) {
		x1 = 0;
	}
	if (y1 < 0) {
		y1 = 0;
	}

	if (x2 >= o_ctx->width) {
		x2 = o_ctx->width-1;
	}

	if (y2 >= o_ctx->height) {
		y2 = o_ctx->height-1;
	}

	if (x2 == x1) {
		SLOG_ERROR("viewport width 0");
		return -1;
	}

	if (y2 == y1) {
		SLOG_ERROR("viewport height 0");
		return -1;
	}
	DEBUG_CMD(disp_printf(adapter,"final x1 %d, y1 %d, x2 %d, y2 %d",	x1, y1,	x2, y2));

	o_ctx->layer[layer_idx].dst1.x = x1 & 0xfff;
	o_ctx->layer[layer_idx].dst1.y = y1 & 0xfff;
	o_ctx->layer[layer_idx].dst2.x = x2 & 0xfff;
	o_ctx->layer[layer_idx].dst2.y = y2 & 0xfff;
	return 0;
}

int omap_layer_set_blending(disp_adapter_t *adapter, int dispno, int layer_idx,
		unsigned mode, int m1, int m2) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;

	if (layer_idx > OMAP_NUMBER_LAYERS) {
		SLOG_ERROR("Layer out of range");
		return -1;
	}

	if (mode == 0) {
		DEBUG_MSG("mode == 0 alpha blending off");
		o_ctx->layer[layer_idx].global_alpha_active = 0;
		o_ctx->layer[layer_idx].global_alpha = 0xff;
		return 0;
	} else if (DISP_ALPHA_M1_ORIGIN(mode) == DISP_ALPHA_M1_GLOBAL) {
		DEBUG_CMD(disp_printf(adapter,"Global alpha layer %d alpha %x", layer_idx, m1));
		o_ctx->layer[layer_idx].global_alpha_active = 1;
		o_ctx->layer[layer_idx].global_alpha = m1;
		return 0;
	} else if (DISP_ALPHA_M1_ORIGIN(mode) == DISP_ALPHA_M1_SRC_PIXEL_ALPHA) {
		DEBUG_MSG("Source pixel alpha blending on (global alph off)");
		o_ctx->layer[layer_idx].global_alpha_active = 0;
		o_ctx->layer[layer_idx].global_alpha = 0xff;
		return 0;
	}
	DEBUG_MSG("alpha blending off unknown type");
	o_ctx->layer[layer_idx].global_alpha_active = 0;
	o_ctx->layer[layer_idx].global_alpha = 0xff;
	return -1;
}

int omap_layer_set_chromakey(disp_adapter_t *adapter, int dispno,
		int layer_idx, unsigned chroma_mode, disp_color_t color0,
		disp_color_t color1, disp_color_t mask) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;

	//slogf(21,1, "color0 %x color1 %x mask %x chroma_mode %x layer %x", color0, color1, mask, chroma_mode, layer_idx);
	if (layer_idx != OMAP_LAYER_GFX) {
		SLOG_ERROR("Only GFX supports chroma");
		return -1;
	}

	/* DISPC_CONFIG (config)
	 * TCKDIGENABLE  bit 12  0 - disable transparence color key digital out
	 * TCKLCDANABLE  bit 10  1 - enable transparence color key lcd out
	 * TCKDIGSELECTION bit 13 - Digital (tv) 0 destination 1 - source
	 * TCKLCDSELECTION bit 11 - LCD 0 destination 1 - source
	 * Only  (0<<11) Graphics destination transparency color key selected works in alpha mode
	 * in Alpha mode.
	 */
	if (chroma_mode == 0) {
		DEBUG_MSG("Disable transparence ");
		/* disable transparence */
		o_ctx->layer[layer_idx].config &= ~((1<<10));
	}
	/* Only impelmented LCD,  digital (TV) not implemented */
	if (chroma_mode & DISP_CHROMA_OP_DST_MATCH) {
		SLOG_ERROR("DISP_CHROMA_OP_DST_MATCH not supported");
		return -1;
	}
	if (chroma_mode & DISP_CHROMA_OP_DRAW) {
		SLOG_ERROR("DISP_CHROMA_OP_DRAW not supported");
		return -1;
	}

	if (chroma_mode & DISP_CHROMA_OP_SRC_MATCH || chroma_mode
			& DISP_CHROMA_OP_NO_DRAW) {
		DEBUG_MSG("DISP_CHROMA_OP_SRC_MATCH");
		o_ctx->layer[layer_idx].config &= ~((1<<11));
		o_ctx->layer[layer_idx].config |= ((1<<10));
	}

	o_ctx->layer[layer_idx].transp = color0;

	return 0;
}

int omap_layer_set_flags(disp_adapter_t * adapter, int dispno, int layer_idx,
		unsigned flag_mask, unsigned flag_values) {
	return -1;
}

void omap_layer_update_begin(disp_adapter_t *adapter, int dispno,
		uint64_t layer_mask) {

}

int omap_layer_program(disp_adapter_t *adapter, omap_ctx_t *o_ctx, int layer_idx) {
	uint32_t display_offset;
	int dst_width = (o_ctx->layer[layer_idx].dst2.x
			- o_ctx->layer[layer_idx].dst1.x)+1;
	int dst_height = (o_ctx->layer[layer_idx].dst2.y
			- o_ctx->layer[layer_idx].dst1.y)+1;
	int src_width = (o_ctx->layer[layer_idx].src2.x
			- o_ctx->layer[layer_idx].src1.x)+1;
	int src_height = (o_ctx->layer[layer_idx].src2.y
			- o_ctx->layer[layer_idx].src1.y)+1;
	int resize_factor_vertical = 1;
	int resize_factor_horizontal = 1;
	int width_skip = 0;
	TRACE;

	if (layer_idx > OMAP_NUMBER_LAYERS) {
		SLOG_ERROR("Layer out of range");
		return -1;
	}

	display_offset = o_ctx->layer[layer_idx].offset
			+ o_ctx->layer[layer_idx].src1.y * o_ctx->layer[layer_idx].stride
			+ o_ctx->layer[layer_idx].src1.x * o_ctx->layer[layer_idx].bpp;

	DEBUG_CMD(disp_printf(adapter, "omap_layer_program layer %d", layer_idx));
	DEBUG_CMD(disp_printf(adapter,"src (%d, %d) (%d, %d) src_width %d src_height %d",
			o_ctx->layer[layer_idx].src1.x, o_ctx->layer[layer_idx].src1.y,
			o_ctx->layer[layer_idx].src2.x, o_ctx->layer[layer_idx].src2.y,
			src_width, src_height));
	DEBUG_CMD(disp_printf(adapter,"dst (%d, %d) (%d, %d) dst_width %d dst_height %d",
			o_ctx->layer[layer_idx].dst1.x, o_ctx->layer[layer_idx].dst1.y,
			o_ctx->layer[layer_idx].dst2.x, o_ctx->layer[layer_idx].dst2.y,
			dst_width, dst_height));

	if (layer_idx == OMAP_LAYER_GFX) {
		if (src_width > dst_width) {
			DEBUG_CMD(disp_printf(adapter,"GFX layer not scaling capable (down width)"));
		} else if (src_width < dst_width) {
			DEBUG_CMD(disp_printf(adapter,"GFX layer not scaling capable (up width)"));
			dst_width = src_width;
		}

		if (src_height > dst_height) {
			DEBUG_CMD(disp_printf(adapter,"GFX layer not scaling capable (down height)"));
		} else if (src_height < dst_height) {
			DEBUG_CMD(disp_printf(adapter,"GFX layer not scaling capable (up height)"));
			dst_height = src_height;
		}

	} else {
		/*
		  *  DISPC_VIDn_ATTRIBUTES
		  * (omap_layer_format)
		  *  Bit  22 VIDLINEBUFFERSPLIT Video vertical line buffer split RW
		  *     0x0: Vertical line buffers are not split.
		  *     0x1: Vertical line buffers are split into two.
		  *
		  *  Bit 21 VIDVERTICALTAPS Video vertical resize tap number RW 0
		  *       0x0: Three taps are used for the vertical filtering logic.
		  *       0x1: Five taps are used for the vertical filtering logic.
		  *
		  *  Bits 6:5 VIDRESIZEENABLE Video Resize Enable RW 0x0
		  *       0x0: Disable the resize processing
		  *       0x1: Enable the horizontal resize processing
		  *       0x2: Enable the vertical resize processing
		  *       0x3: Enable both horizontal and vertical resize processing
		  *
		  */
		if (src_width > dst_width) {
			resize_factor_horizontal = (1024*(src_width&0x7ff))/(dst_width&0x7ff);
			DEBUG_CMD(disp_printf(adapter,"Vid1 layer scale down, resize_factor_horizontal %d", resize_factor_horizontal));
			o_ctx->layer[layer_idx].omap_layer_format |=  (1<<22) | (1<<21) | (1<<7) | (1<<5);
			//omap_coef_vid_down_h5tap(adapter,layer_idx);
			if (layer_idx == OMAP_LAYER_VID1)
				omap_init_vid1_downscale_coefficients_v5andh5_tap(adapter);
			else
				omap_init_vid2_downscale_coefficients_v5andh5_tap(adapter);
		} else if (src_width < dst_width) {
			resize_factor_horizontal = (1024*src_width)/dst_width;
			DEBUG_CMD(disp_printf(adapter,"Vid1 layer scale up, resize_factor_horizontal %d", resize_factor_horizontal));
			o_ctx->layer[layer_idx].omap_layer_format |= (1<<22) | (1<<21) | (1<<5);
			//omap_coef_vid_up_h5tap(adapter,layer_idx);
			if (layer_idx == OMAP_LAYER_VID1)
				omap_init_vid1_upscale_coefficients_v5andh5_tap(adapter);
			else
				omap_init_vid2_upscale_coefficients_v5andh5_tap(adapter);
		} else {
			DEBUG_CMD(disp_printf(adapter,"Video layer no scaling width"));
		}

		if (src_height > dst_height) {
			resize_factor_vertical = (1024*(src_height&0x7ff))/(dst_height&0x7ff);
			DEBUG_CMD(disp_printf(adapter,"Vid1 layer scale down, resize_factor_vertical %d", resize_factor_vertical));
			o_ctx->layer[layer_idx].omap_layer_format |=  (1<<22) | (1<<21) | (1<<8) | (1<<6);
			//omap_coef_vid_down_v5tap(adapter,layer_idx);
			if (layer_idx == OMAP_LAYER_VID1)
				omap_init_vid1_downscale_coefficients_v5andh5_tap(adapter);
			else
				omap_init_vid2_downscale_coefficients_v5andh5_tap(adapter);

		} else if (src_height < dst_height) {
			resize_factor_vertical = (1024*src_height)/dst_height;
			DEBUG_CMD(disp_printf(adapter,"Vid1 layer scale up, resize_factor_vertical %d", resize_factor_vertical));
			o_ctx->layer[layer_idx].omap_layer_format |=  (1<<22) | (1<<21) | (1<<6);
			//omap_coef_vid_up_v5tap(adapter,layer_idx);
			if (layer_idx == OMAP_LAYER_VID1)
				omap_init_vid1_downscale_coefficients_v5andh5_tap(adapter);
			else
				omap_init_vid2_downscale_coefficients_v5andh5_tap(adapter);
		} else {
			DEBUG_CMD(disp_printf(adapter,"Video layer no scaling height"));
		}

	}


	/* might be called before or without omap_layer_set_surface (ppmp == 0)  */
	if (!(o_ctx->layer[layer_idx].ppmp == 0 || o_ctx->layer[layer_idx].bpp == 0)  ) {
		/* Adjust souce width for Macro pixel requirements, mainly for Yuv422 requires width divisable by 2 pixels */
		for ( ; (src_width%o_ctx->layer[layer_idx].ppmp); --src_width)
				;
		width_skip = (o_ctx->layer[layer_idx].stride - (o_ctx->layer[layer_idx].bpp*src_width));
	}


	DEBUG_CMD(disp_printf(adapter,
			"Ps(%d %d) sz(%d,%d) att(%x %x) sw(%d) st(%d) bpp(%d)",
			(o_ctx->layer[layer_idx].dst1.y ),
			(o_ctx->layer[layer_idx].dst1.x ), (dst_width-1), (dst_height-1),
			o_ctx->layer[layer_idx].attribute,
			o_ctx->layer[layer_idx].omap_layer_format, width_skip,
			o_ctx->layer[layer_idx].stride, o_ctx->layer[layer_idx].bpp));

	if(o_ctx->cached_mem != 0 && o_ctx->cache_flush) {
		//TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 666, "CacheFlush layer_program");

		CACHE_FLUSH(&o_ctx->cachectl, o_ctx->layer[layer_idx].vidptr,
			o_ctx->layer[layer_idx].offset,
			o_ctx->layer[layer_idx].stride*o_ctx->layer[layer_idx].height); /* this could be BPP * width * height to avoid over flush */
		//msync(o_ctx->layer[layer_idx].vidptr, o_ctx->layer[layer_idx].stride*o_ctx->layer[layer_idx].height, MS_SYNC|MS_CACHE_ONLY);
		DEBUG_CMD(disp_printf(adapter,"CACHE FLUSH update idx(%d) %x %x %x",
			layer_idx, o_ctx->layer[layer_idx].vidptr, o_ctx->layer[layer_idx].offset,
			o_ctx->layer[layer_idx].stride*o_ctx->layer[layer_idx].height));
	}

	/* OMAP_DISPC_CONFIG bits
	 *
	 * TCKLCDSELECTION (?<<11) Transparency color key selection (LCD output) RW 0
	 *	0x0: Graphics destination transparency color key selected
	 *	0x1: Video source transparency color key selected
	 * TCKLCDENABLE (?<<10) Transparency color key enabled (LCD output) RW 0
	 *  	0x0: Disable the transparency color key for the LCD
	 *	0x1: Enable the transparency color key for the LCD
	 *
	 * note: This register init value in mode.c and also updated in hcursor.c.
	 */
	*OMAP_DISPC_CONFIG =  OMAP_DISPC_CONFIG_INIT_STATE | o_ctx->layer[layer_idx].config;


	/*
	 * o_ctx->ccfull - set in config file
	 * Color conversion for yuv to rgb
	 * DISPC_VIDn_ATTRIBUTES  bit 11 VIDFULLRANGE VidFullRange
	 * 0x0: (0<<11) Limited range selected: 16 subtracted from Y before color space conversion
	 * 0x1:  (1<<11) Full range selected: Y is not modified before the color space conversion
	 *
	 */

	switch (layer_idx) {
	case OMAP_LAYER_GFX:
		DEBUG_MSG("gfx");
		*OMAP_DISPC_GFX_BA0 = display_offset;
		*OMAP_DISPC_GFX_POSITION = ((o_ctx->layer[layer_idx].dst1.y ) << 16)
				| (o_ctx->layer[layer_idx].dst1.x );
		*OMAP_DISPC_GFX_SIZE = ( ((dst_height-1)<<16) | (dst_width-1) );
		/* (1<<5) Enable replication logic to map 16bit coloro space to 24 display
		 * (burstsz<<6) Graphics DMA Burst Size 0 - 4x32bit,  1 - 8x32bit, 2 - 16x32bit
		 */
		*OMAP_DISPC_GFX_ATTRIBUTES = o_ctx->layer[layer_idx].attribute
				|o_ctx->layer[layer_idx].omap_layer_format | (1<<5) | ((o_ctx->gfx_burstsz & OMAP_BURST_MASK)<<6);
		*OMAP_DISPC_GFX_ROW_INC = width_skip+1;
		*OMAP_DISPC_GFX_PIXEL_INC = 1;
		*OMAP_DISPC_TRANS_COLOR = disp_color_translate(NULL,
				DISP_SURFACE_FORMAT_RGB888,
				o_ctx->layer[layer_idx].surface_format,
				o_ctx->layer[layer_idx].transp);
		/* Global alpha must be set in alpha mode or all layers transparent
		 * GFXGLOBALALPHA       0-transparent 0xff fully opaque;
		 */
		if (o_ctx->layer[layer_idx].global_alpha_active != 0) {
			*OMAP_DISPC_GLOBAL_ALPHA &= ~(0xff);
			*OMAP_DISPC_GLOBAL_ALPHA |= o_ctx->layer[layer_idx].global_alpha;
		} else {
			*OMAP_DISPC_GLOBAL_ALPHA |= 0xff;
		}
		break;
	case OMAP_LAYER_VID1:
		DEBUG_MSG("VIDEO 1");
		DEBUG_CMD(omap_coef_display(adapter, 0 ));
		*OMAP_DISPC_VID1_BA(0) = display_offset;
		*OMAP_DISPC_VID1_BA(1) = display_offset;
		*OMAP_DISPC_VID1_SIZE = ( ((dst_height-1)<<16) | (dst_width-1) );
		*OMAP_DISPC_VID1_PICTURE_SIZE = ( ((src_height-1)<<16) | (src_width-1) );
		*OMAP_DISPC_VID1_POSITION = ((o_ctx->layer[layer_idx].dst1.y ) << 16)
				| (o_ctx->layer[layer_idx].dst1.x );
		/*
		 * Bit 9   VIDCOLORCONVENABLE
		 * Bit 10 VIDREPLI-CATIONENABLE Enablereplication logic to map 16bit coloro space to 24 display
		 * Bit 11  VIDFULLRANGE
		 * (burstsz<<14) Video DMA Burst Size 0 - 4x32bit,  1 - 8x32bit, 2 - 16x32bit
		 */
		*OMAP_DISPC_VID1_ATTRIBUTES = o_ctx->layer[layer_idx].attribute
				| o_ctx->layer[layer_idx].omap_layer_format | (1<<10) | (o_ctx->ccfull<<11) | ((o_ctx->vid1_burstsz & OMAP_BURST_MASK)<<14);
		DEBUG_CMD(disp_printf(adapter, "OMAP_DISPC_VID1_ATTRIBUTES %x",*OMAP_DISPC_VID1_ATTRIBUTES ));
		*OMAP_DISPC_VID1_FIR = (((resize_factor_vertical-1)&0xfff)<<16) | ((resize_factor_horizontal-1)&0xfff);
		DEBUG_CMD(disp_printf(adapter, "OMAP_DISPC_VID1_FIR %x",*OMAP_DISPC_VID1_FIR ));
		*OMAP_DISPC_VID1_PIXEL_INC = 1;
		*OMAP_DISPC_VID1_ROW_INC = width_skip+1;
		break;
	case OMAP_LAYER_VID2:
		DEBUG_MSG("VIDEO 2");
		DEBUG_CMD(omap_coef_display(adapter, 1 ));
		*OMAP_DISPC_VID2_BA(0) = display_offset;
		*OMAP_DISPC_VID2_BA(1) = display_offset;
		*OMAP_DISPC_VID2_SIZE = ( ((dst_height-1)<<16) | (dst_width-1) );
		*OMAP_DISPC_VID2_PICTURE_SIZE = ( ((src_height-1)<<16) | (src_width-1) );
		*OMAP_DISPC_VID2_POSITION = ((o_ctx->layer[layer_idx].dst1.y ) << 16)
				| (o_ctx->layer[layer_idx].dst1.x );
		/*
		 * Bit 9   VIDCOLORCONVENABLE
		 * Bit 10  VIDREPLI-CATIONENABLE Enablereplication logic to map 16bit coloro space to 24 display
		 * Bit 11  VIDFULLRANGE
		 * (burstsz<<14) Video DMA Burst Size 0 - 4x32bit,  1 - 8x32bit, 2 - 16x32bit
		 */
		*OMAP_DISPC_VID2_ATTRIBUTES = o_ctx->layer[layer_idx].attribute
				| o_ctx->layer[layer_idx].omap_layer_format | (1<<10) | (o_ctx->ccfull<<11) | ((o_ctx->vid2_burstsz & OMAP_BURST_MASK)<<14);
		DEBUG_CMD(disp_printf(adapter, "OMAP_DISPC_VID2_ATTRIBUTES %x",*OMAP_DISPC_VID2_ATTRIBUTES ));
		*OMAP_DISPC_VID2_FIR = (((resize_factor_vertical-1)&0xfff)<<16) | ((resize_factor_horizontal-1)&0xfff);
		DEBUG_CMD(disp_printf(adapter, "OMAP_DISPC_VID2_FIR %x",*OMAP_DISPC_VID2_FIR ));
		*OMAP_DISPC_VID2_PIXEL_INC = 1;
		*OMAP_DISPC_VID2_ROW_INC = width_skip+1;
		/* Global alpha must be set in alpha mode or all layers transparent
		 * VID2GLOBALALPHA		0-transparent 0xff fully opaque */
		if (o_ctx->layer[layer_idx].global_alpha_active != 0) {
			*OMAP_DISPC_GLOBAL_ALPHA &= ~(0xff<<16);
			*OMAP_DISPC_GLOBAL_ALPHA |= o_ctx->layer[layer_idx].global_alpha
					<<16;
		} else {
			*OMAP_DISPC_GLOBAL_ALPHA |= 0xff<<16;
		}
		break;
	default:
		DEBUG_MSG("Layer type unknown");
		return -1;
	}
	return 0;
}

void omap_layer_update_end(disp_adapter_t *adapter, int dispno,
		uint64_t layer_mask, int wait_vsync) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	int i;
	TRACE;

	for (i = 0; i < OMAP_NUMBER_LAYERS; i++) {
		if (layer_mask & (1<<i)) {
			if (omap_layer_program(adapter, o_ctx, i) == -1)
				return;
		}
	}
	/* Hit GOBIT and enabled  LCD  */
	*OMAP_DISPC_CONTROL |= (1<<5) | 1;

	if (wait_vsync && adapter->callback) {
		adapter->callback(adapter->callback_handle,
    		DISP_CALLBACK_WAIT_VSYNC, &dispno);
	}
}

void omap_layer_reset(disp_adapter_t * adapter, int dispno, int layer_idx) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;
	/* Turn off chroma matching */
	o_ctx->layer[layer_idx].config &= ~((1<<10));

	o_ctx->layer[layer_idx].global_alpha_active = 0;
	o_ctx->layer[layer_idx].attribute = 0;
}

int omap_layer_set_alpha_map(disp_adapter_t *adapter, int dispno,
		int layer_idx, disp_surface_t *map) {
			omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;
	return 0;
}
int omap_layer_flushrect(disp_adapter_t *adapter, int dispno, int layer_idx, int x1, int y1, int x2, int y2) {
    omap_ctx_t *o_ctx = adapter->ms_ctx;
	int nlayer;
	// Commented to reduce debug in verbose mode, uncomment for cache flush debugging
	//TRACE;

	/* AM Note:
		Since the GF layer doesn't know which hw layer we are operating on, we must iterate
		through each one, see if it's enabled and has some paddr backing to it then flush
		each one for the x1/y1 -> x2->y2 area that needs to be flushed
	*/
	if(o_ctx->cached_mem != 0 && o_ctx->cache_flush>1) {
		for(nlayer=0 ; nlayer < OMAP_NUMBER_LAYERS ; nlayer++) {
			if ((o_ctx->layer[nlayer].attribute & 0x1) &&
				(o_ctx->layer[nlayer].offset)) {
				//TraceEvent(_NTO_TRACE_INSERTUSRSTREVENT, 666, "CacheFlush layer_flushrect");
				CACHE_FLUSH(&o_ctx->cachectl, o_ctx->layer[nlayer].vidptr,
					o_ctx->layer[nlayer].offset,
					o_ctx->layer[nlayer].stride*o_ctx->layer[nlayer].height); /* this could be BPP * width * height to avoid over flush */
    			//msync(o_ctx->layer[nlayer].vidptr, o_ctx->layer[nlayer].stride*o_ctx->layer[nlayer].height, MS_SYNC|MS_CACHE_ONLY);
				DEBUG_CMD(disp_printf(adapter,"CACHE FLUSH flushrect(%d) %x %x %x",
					nlayer, o_ctx->layer[nlayer].vidptr, o_ctx->layer[nlayer].offset,
					o_ctx->layer[nlayer].stride*o_ctx->layer[nlayer].height));
			}
		}
	}

	return 0; /* -1 on err */
}



__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/devg/omap35xx/layer.c $ $Rev: 308279 $" );
