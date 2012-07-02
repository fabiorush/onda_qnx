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

#include "omap.h"

/* 64 bytes granularity for DC */
#define DC_WIDTH_GRANULARITY 16

/* 8 pixels granularity for SGX */
#define SGX_WIDTH_GRANULARITY 8

/* 1 pixels granularity for offscreen CPU accesible */
#define CPU_WIDTH_GRANULARITY 1

#define ALIGNMENT 4

int omap_mem_init(disp_adapter_t *adapter, char *optstring) {
	omap_ctx_t	*o_ctx = adapter->ms_ctx;
	TRACE;
	o_ctx->cachectl.fd = NOFD;
	if(cache_init(0, &o_ctx->cachectl, NULL) == -1) {
		slogf(21,0,"cache_init failed");
	}

	return 0;
}

void omap_mem_fini(disp_adapter_t *adapter) {
	omap_ctx_t	*o_ctx = adapter->ms_ctx;
	TRACE;
	cache_fini(&o_ctx->cachectl);
	if (adapter->mm_ctx != NULL) {
		disp_vm_destroy_pool(adapter, adapter->mm_ctx);
	}
	adapter->mm_ctx = NULL;
}

int omap_mem_reset(disp_adapter_t *adapter, disp_surface_t *surf) {
	omap_ctx_t	*o_ctx = adapter->ms_ctx;
	disp_surface_t          vram;
	TRACE;

	if (adapter->mm_ctx != NULL) {
		disp_vm_destroy_pool(adapter, adapter->mm_ctx);
	}

	if (o_ctx->vidsize > 0) {
		vram.stride = vram.width = o_ctx->vidsize;
		vram.height = 1;
		vram.pixel_format = DISP_SURFACE_FORMAT_BYTES;
		vram.vidptr = o_ctx->vidptr;
		vram.paddr = vram.offset = o_ctx->vidbase;
		vram.flags = DISP_SURFACE_DISPLAYABLE |
			DISP_SURFACE_CPU_LINEAR_READABLE | DISP_SURFACE_ALPHA_MAP |
			DISP_SURFACE_CPU_LINEAR_WRITEABLE | DISP_SURFACE_PHYS_CONTIG | DISP_SURFACE_NON_CACHEABLE;
		adapter->mm_ctx = disp_vm_create_pool(adapter, &vram, 32);
	} else {
		adapter->mm_ctx = NULL;
	}

	return 0;
}

disp_surface_t * omap_alloc_surface(disp_adapter_t *adapter, int width,
		int height, unsigned format, unsigned flags, unsigned user_flags) {
	omap_ctx_t	*o_ctx = adapter->ms_ctx;
	int                     stride;
	unsigned                reject_flags = 0;
	disp_surface_t		*surf;
	TRACE;

	if (o_ctx->vidsize == 0 || !(flags & DISP_SURFACE_DISPLAYABLE)) {
		/* No graphics aperture present or not a displayable surface*/
		return NULL;
	}

	/* We only want to use our aperature for video. */
	switch (format) {
		case DISP_SURFACE_FORMAT_PACKEDYUV_UYVY:
			/* Displayable only */
			reject_flags =
				DISP_SURFACE_2D_TARGETABLE |
				DISP_SURFACE_3D_TARGETABLE |
				DISP_SURFACE_2D_READABLE |
				DISP_SURFACE_3D_READABLE |
				DISP_SURFACE_VG_TARGETABLE |
				DISP_SURFACE_VG_READABLE;
				break;
		default:
			/* No HW use possible */
			return NULL;
	}

	if (flags & reject_flags) {
		return NULL;
	}

	stride = (width * DISP_BYTES_PER_PIXEL(format) + DC_WIDTH_GRANULARITY) & ~DC_WIDTH_GRANULARITY;

	surf = disp_vm_alloc_surface(adapter->mm_ctx,
			width, height, stride, format, flags);

	if (surf != NULL) {
		surf->flags &= ~reject_flags;
	}

	return surf;
}

int omap_free_surface(disp_adapter_t *adapter, disp_surface_t *surf) {
	omap_ctx_t	*o_ctx = adapter->ms_ctx;
	TRACE;
	if (adapter->mm_ctx == NULL) {
		return 0;
	}
	return disp_vm_free_surface(adapter, surf);
}

unsigned long omap_mem_avail(disp_adapter_t *adapter, unsigned flags) {
	omap_ctx_t	*o_ctx = adapter->ms_ctx;
	TRACE;
	return 0;
}

int omap_query_apertures(disp_adapter_t *adapter, disp_aperture_t *ap) {
	omap_ctx_t	*o_ctx = adapter->ms_ctx;
	TRACE;
    if (o_ctx->vidsize !=0) {
            ap->base = o_ctx->vidbase;
            ap->size = o_ctx->vidsize;
            ap->flags = DISP_APER_NOCACHE;
           	return 1;
    } else {
        return 0;
    }
}

int omap_query_surface(disp_adapter_t *adapter, disp_surface_t *surf,
		disp_surface_info_t *info) {
			omap_ctx_t	*o_ctx = adapter->ms_ctx;
	TRACE;

    if (o_ctx->vidsize > 0 && surf->paddr >= o_ctx->vidbase &&
            surf->paddr < o_ctx->vidbase + o_ctx->vidsize) {
            /* Surface resides within the pre-allocated aperture */
            info->aperture_index = 0;
            info->offset = surf->offset - o_ctx->vidbase;
    }

	return 0;
}

/*
 * If a client of the driver wants to allocate memory itself,
 * it must allocate it in accordance with the parameters returned by
 * this function.  Since this memory will not be coming from
 * video memory, we must check the flags accordingly.
 */
int omap_get_alloc_info(disp_adapter_t *adapter, int width, int height,
		unsigned format, unsigned flags, unsigned user_flags,
		disp_alloc_info_t *info) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	int			bpp = DISP_BYTES_PER_PIXEL(format);
	int			granularity;
	TRACE;

	if (flags & (DISP_SURFACE_2D_READABLE | DISP_SURFACE_2D_TARGETABLE |
			DISP_SURFACE_3D_READABLE | DISP_SURFACE_3D_TARGETABLE)) {
		granularity = SGX_WIDTH_GRANULARITY;
	} else {
		granularity = CPU_WIDTH_GRANULARITY;
	}

	info->start_align = ALIGNMENT;
	info->end_align = ALIGNMENT;
	info->min_stride = ((width + granularity-1) & ~(granularity-1)) * bpp;
	info->max_stride = ~0;
	info->stride_gran = granularity * bpp;
	info->map_flags = DISP_MAP_PHYS;
	info->prot_flags = DISP_PROT_READ | DISP_PROT_WRITE | (o_ctx->cached_mem ? 0 : DISP_PROT_NOCACHE) ;
	info->surface_flags = (DISP_SURFACE_CPU_LINEAR_READABLE
			| DISP_SURFACE_CPU_LINEAR_WRITEABLE | DISP_SURFACE_PHYS_CONTIG
			| DISP_SURFACE_DISPLAYABLE | DISP_SURFACE_2D_READABLE
			| DISP_SURFACE_2D_TARGETABLE | DISP_SURFACE_3D_READABLE
			| DISP_SURFACE_3D_TARGETABLE);

	return 0;
}

int omap_get_alloc_layer_info(disp_adapter_t *adapter, int dispno[],
		int layer_idx[], int nlayers, unsigned format, int surface_index,
		int width, int height, unsigned sflags, unsigned hint_flags,
		disp_alloc_info_t *info) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	unsigned alloc_format;
	int bpp, i;
	int granularity;

	if (surface_index > 0) {
		DEBUG_MSG("surface_index > 0");
		return -1;
	}

	switch (format) {
	case DISP_LAYER_FORMAT_RGB565:
		alloc_format = DISP_SURFACE_FORMAT_RGB565;
		bpp = 2;
		break;
	case DISP_LAYER_FORMAT_RGB888:
		alloc_format = DISP_SURFACE_FORMAT_RGB888;
		bpp=3;
		break;
	case DISP_LAYER_FORMAT_ARGB8888:
		alloc_format = DISP_SURFACE_FORMAT_ARGB8888;
		bpp=4;
		break;
	case DISP_LAYER_FORMAT_YUY2:
		alloc_format = DISP_SURFACE_FORMAT_PACKEDYUV_YUY2;
		bpp=2;
		break;
	case DISP_LAYER_FORMAT_UYVY:
		alloc_format = DISP_SURFACE_FORMAT_PACKEDYUV_UYVY;
		bpp=2;
		break;
	case DISP_LAYER_FORMAT_V422:
		alloc_format = DISP_SURFACE_FORMAT_PACKEDYUV_V422;
		bpp=2;
		break;
	case DISP_LAYER_FORMAT_YVYU:
		alloc_format = DISP_SURFACE_FORMAT_PACKEDYUV_YVYU;
		bpp=2;
		break;
	default:
		SLOG_ERROR("format unknown");
		return -1;
	}


	for (i = 0; i < nlayers; i++) {
		switch (layer_idx[i]) {
		case OMAP_LAYER_GFX:
			if (format == DISP_LAYER_FORMAT_YUY2 || format
					== DISP_LAYER_FORMAT_UYVY) {
				SLOG_ERROR("Format not supported for layer");
				return -1;
			}
			break;
		case OMAP_LAYER_VID1:
			if (format == DISP_LAYER_FORMAT_ARGB8888) {
				SLOG_ERROR("Format not supported for layer");
				return -1;
			}
			break;
		case OMAP_LAYER_VID2:
			break;
		default:
			SLOG_ERROR("format unknown");
			return -1;
		}
	}

	if (sflags & (DISP_SURFACE_2D_READABLE | DISP_SURFACE_2D_TARGETABLE |
			DISP_SURFACE_3D_READABLE | DISP_SURFACE_3D_TARGETABLE)) {
		granularity = SGX_WIDTH_GRANULARITY;
	} else {
		granularity = DC_WIDTH_GRANULARITY;
	}

	info->start_align = ALIGNMENT;
	info->end_align = ALIGNMENT;
	info->min_stride = ((width + granularity-1) & ~(granularity-1)) * bpp;
	info->max_stride = ~0;
	info->stride_gran = granularity * bpp;
	info->map_flags = DISP_MAP_PHYS;
	info->prot_flags = DISP_PROT_READ | DISP_PROT_WRITE | (o_ctx->cached_mem ? 0 : DISP_PROT_NOCACHE) ;
	info->surface_flags = (DISP_SURFACE_CPU_LINEAR_READABLE
			| DISP_SURFACE_CPU_LINEAR_WRITEABLE | DISP_SURFACE_PHYS_CONTIG
			| DISP_SURFACE_DISPLAYABLE | DISP_SURFACE_2D_READABLE
			| DISP_SURFACE_2D_TARGETABLE | DISP_SURFACE_3D_READABLE
			| DISP_SURFACE_3D_TARGETABLE);

	return 0;
}


int devg_get_memfuncs(disp_adapter_t *adapter, disp_memfuncs_t *funcs,
		int tabsize) {
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, init, omap_mem_init, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, fini, omap_mem_fini, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, module_info, omap_module_info,
			tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, reset, omap_mem_reset, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, alloc_surface, omap_alloc_surface,
			tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, free_surface, omap_free_surface,
			tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, mem_avail, omap_mem_avail, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, query_apertures,
			omap_query_apertures, tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, query_surface, omap_query_surface,
			tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, get_alloc_info, omap_get_alloc_info,
			tabsize);
	DISP_ADD_FUNC(disp_memfuncs_t, funcs, get_alloc_layer_info,
			omap_get_alloc_layer_info, tabsize);

	return 0;
}


__SRCVERSION( "$URL: http://svn/product/tags/public/bsp/nto641/ti-omap3530-beagle/latest/src/hardware/devg/omap35xx/mem.c $ $Rev: 308279 $" );
