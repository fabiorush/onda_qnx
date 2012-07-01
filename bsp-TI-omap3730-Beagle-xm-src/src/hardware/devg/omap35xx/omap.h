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

#ifndef __OMAP35XX_H_INCLUDED
#define __OMAP35XX_H_INCLUDED

#include <sys/slogcodes.h>
#include <graphics/display.h>
#include <graphics/disputil.h>
#include <graphics/ffb.h>
#include <sys/cache.h>

/* local header file includes bottom of file */

/* DEBUG & verbose control
 * undefine OMAP_DEBUG_ON to remove all verbose at compile time
 * Debug verbose controlled from configuration file omap35xx.h
 * OMAP_TRACE_DEBUG_LVL controls the verbose level to turn on function trace,.
 * Note no trace in kernal part of isr.
 */
#define OMAP_DEBUG_ON
#define OMAP_TRACE_DEBUG_LVL 2
#if defined(OMAP_DEBUG_ON)
#define TRACE if (o_ctx->verbose > OMAP_TRACE_DEBUG_LVL) disp_printf(adapter, "%s", __FUNCTION__)
#define SLOG_ERROR(x)  disp_perror(adapter, x)
#define DEBUG_MSG(x) if (o_ctx->verbose > 1) disp_printf(adapter, "%s %s", __FUNCTION__, x)
#define DEBUG_CMD(x) if (o_ctx->verbose > 5) x
#else
#define TRACE
#define SLOG_ERROR(x)
#define DEBUG_MSG(x)
#define DEBUG_CMD(x)
#endif

#include "omap_reg.h"

#define OMAP_LAYER_VID2 1
#define OMAP_LAYER_VID1 0
#define OMAP_LAYER_GFX 2
#define OMAP_NUMBER_LAYERS 3

typedef struct {
	void*	vidptr;
	uint32_t offset;
	int stride;
	int height;
	unsigned bpp;
	unsigned attribute;
	unsigned surface_format;
	unsigned omap_layer_format;
	uint32_t config;
	disp_point_t src1;
	disp_point_t src2;
	disp_point_t dst1;
	disp_point_t dst2;
	disp_color_t transp;
	int global_alpha_active;
	int global_alpha;
	unsigned ppmp; /* pixels per macro pixel used for yuv granularity 2 bpp but 4 byte granularity */
} layer_info;

/* OMAP35xx LCD controller private state */
typedef struct {
	disp_adapter_t *adapter;
	uint32_t *dispregptr;
	uint32_t *dsiptr;
	uint32_t *dsi_pll_ptr;
	uint32_t *mmuregptr;
	uint32_t *pcrmregptr;
	uint32_t *gpio1ptr;
	uint32_t *gpio5ptr;
	uint32_t *gpio6ptr;
	uint32_t *padptr;
	uint32_t *intregptr;
	int free_context;
	unsigned vsync_counter;
    struct cache_ctrl   cachectl;
	unsigned want_vsync_pulse;

	/* Layer control */
	layer_info layer[OMAP_NUMBER_LAYERS];

	/* Interrupt handling */
	int chid;
	int coid;
	int iid;
	unsigned irq;
	struct sigevent event;

	/* display */
	int width;
	int height;

	/* verbose  level */
	int verbose;

	/* hardware cursor */
	uint16_t *cursor_vptr;
	uint32_t cursor_size;
	uint32_t cursor_paddr;
	uint32_t cursor_width;
	uint32_t cursor_height;

	/* Configurable options */
	int hfp, hbp, hsw, ppl;
	int vfp, vbp, vsw, lpp;
	int phsvs, phsvs_rf;
	int ivs;
	int ihs;
	int ipc;
	int ieo;
	int acb;
	int acbi;
	int pcd;
	int type;
	int hcursor;
	int disptype;
	int lcdfmt;
	int dpowerg70;
	int draw_lr;
	int draw_tb;
	int tw4030;
	unsigned int ccoef;
	short ccry, ccrcr, ccrcb, ccgy, ccgcr, ccgcb, ccby, ccbcr, ccbcb;
	int ccfull;
	int cached_mem;
	int cache_flush;
	int gfx_burstsz;
	int gfx_highthreshold;
	int gfx_lowthreshold;
	int vid1_burstsz;
	int vid1_highthreshold;
	int vid1_lowthreshold;
	int vid2_burstsz;
	int vid2_highthreshold;
	int vid2_lowthreshold;
	void    *vidptr;
	unsigned vidbase;
	unsigned vidsize;
	int use_pll;
	int pll_use_customtiming;
	unsigned pll_freq;
} omap_ctx_t;

#include "omap35xx_proto.h"
#include "coefficients.h"

#endif /* __OMAP35XX_H_INCLUDED */

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/devg/omap35xx/omap.h $ $Rev: 308279 $" )
