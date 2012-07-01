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


#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "omap.h"

#define CONFIG_LINE_BUFFER_SZ 512

static char *omap_opts[] = {
#define CO_HSW		0
		"hsw",
#define CO_HBP		1
		"hbp",
#define CO_HFP		2
		"hfp",
#define CO_VSW		3
		"vsw",
#define CO_VFP		4
		"vfp",
#define CO_VBP		5
		"vbp",
#define CO_PHSVS	6
		"phsvs",
#define CO_PHSVSRF	7
		"phsvsrf",
#define CO_IVS		8
		"ivs",
#define CO_IHS		9
		"ihs",
#define CO_IPC		10
		"ipc",
#define CO_IEO		11
		"ieo",
#define CO_ACBI		12
		"acbi",
#define CO_ACB		13
		"acb",
#define CO_PCD		14
		"pcd",
#define CO_PPL		15
		"ppl",
#define CO_LPP		16
		"lpp",
#define CO_IRQ		17
		"irq",
#define CO_LCDFMT	18
		"lcdfmt",
#define CO_DISPTYPE	19
		"disptype",
#define CO_HCURSOR	20
		"hcursor",
#define CO_DVIPOWER_GPIO170	21
		"dpowerg70",
#define CO_VERBOSE	22
		"verbose",
#define CO_DRAW_LR	23
		"draw_lr",
#define CO_DRAW_TB	24
		"draw_tb",
#define CO_TW4030	25
		"tw4030",
#define CO_CCOEF	26
		"ccoef",
#define CO_CCRY	    27
		"ccry",
#define CO_CCRCR	28
		"ccrcr",
#define CO_CCRCB	29
		"ccrcb",
#define CO_CCGY	    30
		"ccgy",
#define CO_CCGCR	31
		"ccgcr",
#define CO_CCGCB	32
		"ccgcb",
#define CO_CCBY	    33
		"ccby",
#define CO_CCBCR	34
		"ccbcr",
#define CO_CCBCB 	35
		"ccbcb",
#define CO_CCFULL 	36
		"ccfull",
#define CO_CACHED_MEM 37
		"cached_mem",
#define CO_CACHE_FLUSH 38
		"cache_flush",
#define CO_GFX_BURSTSZ           39
		"gfx_burstsz",
#define CO_GFX_HIGHTHRESHOLD     40
		"gfx_highthreshold",
#define CO_GFX_LOWTHRESHOLD      41
		"gfx_lowthreshold",
#define CO_VID1_BURSTSZ          42
		"vid1_burstsz",
#define CO_VID1_HIGHTHRESHOLD    43
		"vid1_highthreshold",
#define CO_VID1_LOWTHRESHOLD     44
		"vid1_lowthreshold",
#define CO_VID2_BURSTSZ          45
		"vid2_burstsz",
#define CO_VID2_HIGHTHRESHOLD    46
		"vid2_highthreshold",
#define CO_VID2_LOWTHRESHOLD     47
		"vid2_lowthreshold",
#define CO_VBASE                 48
		"vbase",
#define CO_VSIZE                 49
		"vsize",
#define CO_USE_PLL                50
		"use_pll",
#define CO_PLL_USE_CUSTOMTIMING  51
		"pll_use_customtiming",
#define CO_PLL_FREQ              52
		"pll_freq",
		NULL
};

int get_config_data(omap_ctx_t *o_ctx, const char *filename) {
	FILE *fin= NULL;
	char buf[CONFIG_LINE_BUFFER_SZ], *c, *opt, *value;

	if (filename == NULL)
		disp_printf(o_ctx->adapter,
				"No config file specified, using default parameters\n");
	else if ((fin = fopen(filename, "r")) == NULL)
		disp_printf(o_ctx->adapter, "Could not open config file \"%s\": %s\n",
				filename, strerror(errno));

	/* Default values: OMAP3530 with SHARP LS037V7DW01  */
	o_ctx->hsw = 0x3;
	o_ctx->hfp = 0x2c;
	o_ctx->hbp = 0x26;

	o_ctx->vsw = 1;
	o_ctx->vfp = 1;
	o_ctx->vbp = 1;
	o_ctx->lpp = 320;
	o_ctx->ppl = 240;

	o_ctx->phsvs = 0;
	o_ctx->phsvs_rf = 0;
	o_ctx->ivs = 1;
	o_ctx->ihs = 1;
	o_ctx->ipc = 1;
	o_ctx->ieo = 0x4;
	o_ctx->acb = 0;
	o_ctx->acbi = 0;
	o_ctx->pcd = 0x9;

	o_ctx->irq = 25;
	o_ctx->disptype = 1;
	o_ctx->lcdfmt = 0x2;
	o_ctx->hcursor = 0;
	o_ctx->dpowerg70 = 0;

	o_ctx->verbose = 0;
	o_ctx->draw_lr = 1;
	o_ctx->draw_tb = 1;
	o_ctx->tw4030 = 0;
	o_ctx->ccoef = 0;
	o_ctx->ccry = 0;
	o_ctx->ccrcr = 0;
	o_ctx->ccrcb = 0;
	o_ctx->ccgy = 0;
	o_ctx->ccgcr = 0;
	o_ctx->ccgcb = 0;
	o_ctx->ccby = 0;
	o_ctx->ccbcr = 0;
	o_ctx->ccbcb = 0;
	o_ctx->ccfull = 0;
	memset(&o_ctx->cachectl, 0 , sizeof(o_ctx->cachectl));
	o_ctx->cached_mem = 0;
	o_ctx->cache_flush = 0;

	/* Layer DMA burst size and fifo threasholds
	 * Burst 0 - 4x32bits, 1 - 8x32bits 2 - 16x32bits
	 * default
	 * burst       2- 16-32bits
	 * high			0x3ff full
	 * low 			0x37f high - (2 x burst)
	 */
	o_ctx->gfx_burstsz = 2;
	o_ctx->gfx_highthreshold = 0x3ff;
	o_ctx->gfx_lowthreshold  = 0x37f;

	o_ctx->vid1_burstsz = 2;
	o_ctx->vid1_highthreshold = 0x3ff;
	o_ctx->vid1_lowthreshold  = 0x37f;

	o_ctx->vid2_burstsz = 2;
	o_ctx->vid2_highthreshold = 0x3ff;
	o_ctx->vid2_lowthreshold  = 0x37f;

	o_ctx->vidsize = 0;
	o_ctx->vidbase = 0;

	o_ctx->use_pll = 0;
	o_ctx->pll_use_customtiming = 0;
	o_ctx->pll_freq = 0;

	
	if (fin == NULL) {
		/* No config file, default values will be used */
		return 0;
	}

	while (fgets(buf, sizeof (buf), fin) != NULL) {
		c = buf;
		while (*c == ' ' || *c == '\t')
			c++;
		if (*c == '\015' || *c== '\032' || *c == '\0' || *c == '\n' || *c
				== '#')
			continue;
		opt = c;
		while (*c == '\015' || *c== '\032' || *c != '\0' && *c != '\n' && *c
				!= '#')
			c++;
		*c = '\0';
		break;
	}

	while (*opt != '\0') {
		c = opt;
		switch (getsubopt(&opt, omap_opts, &value)) {
		case CO_HSW:
			o_ctx->hsw = strtol(value, NULL, 0);
			break;
		case CO_HBP:
			o_ctx->hbp = strtol(value, NULL, 0);
			break;
		case CO_HFP:
			o_ctx->hfp = strtol(value, NULL, 0);
			break;
		case CO_VSW:
			o_ctx->vsw = strtol(value, NULL, 0);
			break;
		case CO_VFP:
			o_ctx->vfp = strtol(value, NULL, 0);
			break;
		case CO_VBP:
			o_ctx->vbp = strtol(value, NULL, 0);
			break;
		case CO_PHSVS:
			o_ctx->phsvs = strtol(value, NULL, 0);
			break;
		case CO_PHSVSRF:
			o_ctx->phsvs_rf = strtol(value, NULL, 0);
			break;
		case CO_IVS:
			o_ctx->ivs = strtol(value, NULL, 0);
			break;
		case CO_IHS:
			o_ctx->ihs = strtol(value, NULL, 0);
			break;
		case CO_IPC:
			o_ctx->ipc = strtol(value, NULL, 0);
			break;
		case CO_IEO:
			o_ctx->ieo = strtol(value, NULL, 0);
			break;
		case CO_ACBI:
			o_ctx->acbi = strtol(value, NULL, 0);
			break;
		case CO_ACB:
			o_ctx->acb = strtol(value, NULL, 0);
			break;
		case CO_PCD:
			o_ctx->pcd = strtol(value, NULL, 0);
			break;
		case CO_PPL:
			o_ctx->ppl = strtol(value, NULL, 0);
			break;
		case CO_LPP:
			o_ctx->lpp = strtol(value, NULL, 0);
			break;
		case CO_IRQ:
			o_ctx->irq = strtol(value, NULL, 0);
			break;
		case CO_DISPTYPE:
			o_ctx->disptype = strtol(value, NULL, 0);
			break;
		case CO_LCDFMT:
			o_ctx->lcdfmt = strtol(value, NULL, 0);
			break;
		case CO_HCURSOR:
			o_ctx->hcursor = strtol(value, NULL, 0);
			break;
		case CO_DVIPOWER_GPIO170:
			o_ctx->dpowerg70 = strtol(value, NULL, 0);
			break;
		case CO_VERBOSE:
			o_ctx->verbose = strtol(value, NULL, 0);
			break;
		case CO_DRAW_LR:
			o_ctx->draw_lr = strtol(value, NULL, 0);
			break;
		case CO_DRAW_TB:
			o_ctx->draw_tb = strtol(value, NULL, 0);
			break;
		case CO_TW4030:
			o_ctx->tw4030 = strtol(value, NULL, 0);
			break;
		case CO_CCOEF:
			o_ctx->ccoef = strtol(value, NULL, 0);
			break;
		case CO_CCRY:
			o_ctx->ccry = strtol(value, NULL, 0);
			break;
		case CO_CCRCR:
			o_ctx->ccrcr = strtol(value, NULL, 0);
			break;
		case CO_CCRCB:
			o_ctx->ccrcb = strtol(value, NULL, 0);
			break;
		case CO_CCGY:
			o_ctx->ccgy = strtol(value, NULL, 0);
			break;
		case CO_CCGCR:
			o_ctx->ccgcr = strtol(value, NULL, 0);
			break;
		case CO_CCGCB:
			o_ctx->ccgcb = strtol(value, NULL, 0);
			break;
		case CO_CCBY:
			o_ctx->ccby = strtol(value, NULL, 0);
			break;
		case CO_CCBCR:
			o_ctx->ccbcr = strtol(value, NULL, 0);
			break;
		case CO_CCBCB:
			o_ctx->ccbcb = strtol(value, NULL, 0);
			break;
		case CO_CCFULL:
			o_ctx->ccfull = strtol(value, NULL, 0);
			break;
		case CO_CACHED_MEM:
			o_ctx->cached_mem = strtol(value, NULL, 0);
			break;
		case CO_CACHE_FLUSH:
			o_ctx->cache_flush = strtol(value, NULL, 0);
			break;
		case CO_GFX_BURSTSZ:
			o_ctx->gfx_burstsz = strtol(value, NULL, 0);
			break;
		case CO_GFX_HIGHTHRESHOLD:
			o_ctx->gfx_highthreshold = strtol(value, NULL, 0);
			break;
		case CO_GFX_LOWTHRESHOLD:
			o_ctx->gfx_lowthreshold = strtol(value, NULL, 0);
			break;
		case CO_VID1_BURSTSZ:
			o_ctx->vid1_burstsz = strtol(value, NULL, 0);
			break;
		case CO_VID1_HIGHTHRESHOLD:
			o_ctx->vid1_highthreshold = strtol(value, NULL, 0);
			break;
		case CO_VID1_LOWTHRESHOLD:
			o_ctx->vid1_lowthreshold = strtol(value, NULL, 0);
			break;
		case CO_VID2_BURSTSZ:
			o_ctx->vid2_burstsz = strtol(value, NULL, 0);
			break;
		case CO_VID2_HIGHTHRESHOLD:
			o_ctx->vid2_highthreshold = strtol(value, NULL, 0);
			break;
		case CO_VID2_LOWTHRESHOLD:
			o_ctx->vid2_lowthreshold = strtol(value, NULL, 0);
			break;
		case CO_VBASE:
			o_ctx->vidbase = strtoul(value, NULL, 0);
			break;
		case CO_VSIZE:
			o_ctx->vidsize = strtoul(value, NULL, 0);
			break;
		case CO_USE_PLL:
			o_ctx->use_pll = strtol(value, NULL, 0);
			break;
		case CO_PLL_USE_CUSTOMTIMING:
			o_ctx->pll_use_customtiming = strtol(value, NULL, 0);
			break;
		case CO_PLL_FREQ:
			o_ctx->pll_freq = strtoul(value, NULL, 0);
			break;
		default:
			disp_printf(o_ctx->adapter, "Unknown option %s\n", c);
			break;
		}
	}

	fclose(fin);

	return 0;
}

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/devg/omap35xx/data.c $ $Rev: 308279 $" );
