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

#include "omap.h"

/* set context functions to use ffb library */
int devg_get_contextfuncs(disp_adapter_t * adapter,
		disp_draw_contextfuncs_t * funcs, int tabsize) {
	return ffb_get_contextfuncs(adapter, funcs, tabsize);
}

/* Set core functions, to use ffb library  */
int devg_get_corefuncs(disp_adapter_t *adapter, unsigned pixel_format,
		disp_draw_corefuncs_t *funcs, int tabsize) {
	return ffb_get_corefuncs(adapter, pixel_format, funcs, tabsize);
}

int omap_misc_wait_idle(disp_adapter_t *adapter) {
	return 0;
}

int omap_misc_init(disp_adapter_t *adapter, char *optstring) {
	return 0;
}

void omap_misc_fini(disp_adapter_t *adapter) {
}

int omap_attach_external(disp_adapter_t *adapter, disp_aperture_t aper[]) {
	omap_ctx_t *o_ctx = adapter->shmem;

	adapter->ms_ctx = o_ctx;
	adapter->caps |= DISP_CAP_2D_ACCEL | DISP_CAP_3D_ACCEL;

	return 0;
}

int omap_detach_external(disp_adapter_t *adapter) {
	return 0;
}

/*
 * Called when a client process terminated unexpectedly.  This could have
 * happened in the middle of adding commands to a draw list.  Discard
 * any partial draw list, so that an incomplete command is not sent
 * to the DSP.
 */
int omap_recover(disp_adapter_t *adapter) {
	return 0;
}

void omap_module_info(disp_adapter_t *adp, disp_module_info_t *info) {
	info->description = "TI OMAP 35xx display Controller";
	info->ddk_version_major = DDK_VERSION_MAJOR;
	info->ddk_version_minor = DDK_VERSION_MINOR;
	info->ddk_rev = DDK_REVISION;
	info->driver_rev = 0;
}

int omap_get_corefuncs_sw(disp_adapter_t *context, unsigned pixel_format,
		disp_draw_corefuncs_t *funcs, int tabsize) {
	if (ffb_get_corefuncs(context, pixel_format, funcs, tabsize) == -1) {
		return -1;
	}

	return 0;
}

int devg_get_miscfuncs(disp_adapter_t *adp, disp_draw_miscfuncs_t *funcs,
		int tabsize) {

	DISP_ADD_FUNC(disp_draw_miscfuncs_t, funcs, init, omap_misc_init, tabsize);
	DISP_ADD_FUNC(disp_draw_miscfuncs_t, funcs, fini, omap_misc_fini, tabsize);
	DISP_ADD_FUNC(disp_draw_miscfuncs_t, funcs, module_info, omap_module_info,
			tabsize);
	DISP_ADD_FUNC(disp_draw_miscfuncs_t, funcs, get_corefuncs_sw,
			ffb_get_corefuncs, tabsize);
	DISP_ADD_FUNC(disp_draw_miscfuncs_t, funcs, get_contextfuncs_sw,
			ffb_get_contextfuncs, tabsize);
	DISP_ADD_FUNC(disp_draw_miscfuncs_t, funcs, attach_external,
			omap_attach_external, tabsize);
	DISP_ADD_FUNC(disp_draw_miscfuncs_t, funcs, detach_external,
			omap_detach_external, tabsize);
	DISP_ADD_FUNC(disp_draw_miscfuncs_t, funcs, recover, omap_recover, tabsize);
	DISP_ADD_FUNC(disp_draw_miscfuncs_t, funcs, wait_idle, omap_misc_wait_idle,
			tabsize);

	return 0;
}

__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/devg/omap35xx/misc.c $ $Rev: 308279 $" );
