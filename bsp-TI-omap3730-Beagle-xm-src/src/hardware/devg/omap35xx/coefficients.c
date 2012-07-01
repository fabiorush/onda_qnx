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
  * Initialize up/down sampling coefficients for Image scaling 
  */

#include <string.h>
#include <unistd.h>


#include "omap.h"  
  
/* Display coefficients  for debug */
void omap_coef_display(disp_adapter_t *adapter, int layer) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	int phase;
	TRACE;
	if (layer > 1) {
		DEBUG_MSG("layer out of range");
		return;
	}
	disp_printf(adapter, "Sampling coefficients Layer %d", layer);
	disp_printf(adapter, "OMAP_DISPC_VID_FIR_COEF_H");
	for (phase = 0; phase < 8; ++phase) {
		disp_printf(adapter, "OMAP_DISPC_VID_FIR_COEF_H(%d) value %x", phase, *OMAP_DISPC_VID_FIR_COEF_H(layer,phase));
	}
	disp_printf(adapter, "OMAP_DISPC_VID_FIR_COEF_HV");
	for (phase = 0; phase < 8; ++phase) {
		disp_printf(adapter, "OMAP_DISPC_VID_FIR_COEF_HV(%d) value %x", phase, *OMAP_DISPC_VID_FIR_COEF_HV(layer,phase));
	}
	disp_printf(adapter, "OMAP_DISPC_VID_FIR_COEF_V");
	for (phase = 0; phase < 8; ++phase) {
		disp_printf(adapter, "OMAP_DISPC_VID_FIR_COEF_V(%d) value %x", phase, *OMAP_DISPC_VID_FIR_COEF_V(layer,phase));
	}	
} 
  
/* 
 * Configures 3 tap vertical up sampling for Video layer.  
 * Values taken direclty from  SPRUFA4 February 2008 
 */ 
void omap_coef_vid_up_v3tap(disp_adapter_t * adapter, unsigned int layer) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;
	if (layer > 1) {
		DEBUG_MSG("layer out of range");
		return;
	}
	/* 
	 * OMAP_DISPC_VIDn_FIR_COEF_HVi
	 *   Bits 31:24 VIDFIRVC2   Signed coefficient C2 for the vertical up-/down-scaling with the phase RW 0x00
	 *   Bits 23:16 VIDFIRVC1 Unsigned coefficient C1 for the vertical up-/down-scaling with the       RW 0x00
	 *   Bits 15:8  VIDFIRVC0   Signed coefficient C0 for the vertical up-/down-scaling with the phase RW 0x00
	 * 
	 *   Not used for vertical (mask)
	 *   Bits  7:0  VIDFIRHC4   Signed coefficient C4 for the horizontal up-/down-scaling with the     RW 0x00
	 */
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,0) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,0) |= 0x00800000;	
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,1) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,1) |= 0x037B0200;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,2) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,2) |= 0x0C6F0500;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,3) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,3) |= 0x20590700;	
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,4) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,4) |= 0x00404000;	
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,5) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,5) |= 0x07592000;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,6) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,6) |= 0x056F0C00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,7) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,7) |= 0x027B0300;
}

/* 
 * Configures 5 tap vertical up sampling for Video layer.  
 * Values taken direclty from  SPRUFA4 February 2008 
 */ 
void omap_coef_vid_up_v5tap(disp_adapter_t * adapter, unsigned int layer) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;
	if (layer > 1) {
		DEBUG_MSG("layer out of range");
		return;
	}
	/* 
	 *
	 * OMAP_DISPC_VIDn_FIR_COEF_HVi
	 *   Bits 31:24 VIDFIRVC2 Signed   coefficient C2 for the vertical up-/down-scaling with the phase RW 0x00
	 *   Bits 23:16 VIDFIRVC1 Unsigned coefficient C1 for the vertical up-/down-scaling with the       RW 0x00
	 *   Bits 15:8  VIDFIRVC0 Signed   coefficient C0 for the vertical up-/down-scaling with the phase RW 0x00
	 *
	 * OMAP_DISPC_VIDn_FIR_COEF_Vi
	 *   Bits 15:8  VIDFIRVC22 Signed coefficient C22 for vertical up/down-scaling with phase n RW 0x00
	 *   Bits  7:0  VIDFIRVC00 Signed coefficient C00 for vertical up/down-scaling with phase n RW 0x00
	 *
	 */
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,0) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,0) |= 0x00800000;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,0)   = 0x00000000;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,1) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,1) |= 0x0D7CF800;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,1)   = 0x0000FF00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,2) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,2) |= 0x1E70F500;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,2)   = 0x0000FEFF;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,3) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,3) |= 0x335FF500;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,3)   = 0x0000FBFE;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,4) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,4) |= 0xF7404000;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,4)   = 0x000000F7;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,5) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,5) |= 0xF55F3300;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,5)   = 0x0000FEFB;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,6) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,6) |= 0xF5701E00;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,6)   = 0x0000FFFE;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,7) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,7) |= 0xF87C0D00;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,7)   = 0x000000FF;
}


/* 
 * Configures 5 tap horizontal up sampling for Video layer.  
 * Values from  SPRUFA4 February 2008 
 */ 
void omap_coef_vid_up_h5tap(disp_adapter_t * adapter, unsigned int layer) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;
	if (layer > 1) {
		DEBUG_MSG("layer out of range");
		return;
	}	
	/* 
	 *
	 * DISPC_VIDn_FIR_COEF_Hi
	 *   Bits 31:24 VIDFIRHC3   Signed coefficient C3 for the horizontal up-/down-scaling with the phase n    RW 0x00
	 *   Bits 23:16 VIDFIRHC2 Unsigned coefficient C2 for the horizontal up-/down-scaling with the phase n    RW 0x00
	 *   Bits 15:8  VIDFIRHC1   Signed coefficient C1 for the horizontal up-/down-scaling with the phase n    RW 0x00
	 *   Bits  7:0  VIDFIRHC0   Signed coefficient C0 for the horizontal up-/down-scaling with the phase n    RW 0x00
	 *
	 * OMAP_DISPC_VIDn_FIR_COEF_HVi
	 *   Bits  7:0 VIDFIRHC4    Signed coefficient C4 for the horizontal up-/down-scaling with the phase n    RW 0x00
	 *
	 */
	*OMAP_DISPC_VID_FIR_COEF_H(layer,0)   = 0x00800000;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,0) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,0) |= 0x00000000;
	*OMAP_DISPC_VID_FIR_COEF_H(layer,1)   = 0x0D7CF800;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,1) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,1) |= 0x000000FF;
	*OMAP_DISPC_VID_FIR_COEF_H(layer,2)   = 0x1E70F5FF;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,2) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,2) |= 0x000000FE;
	*OMAP_DISPC_VID_FIR_COEF_H(layer,3)   = 0x335FF5FE;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,3) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,3) |= 0x000000FB;
	*OMAP_DISPC_VID_FIR_COEF_H(layer,4)   = 0xF74949F7;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,4) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,4) |= 0x00000000;
	*OMAP_DISPC_VID_FIR_COEF_H(layer,5)   = 0xF55F33FB;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,5) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,5) |= 0x00000FE;
	*OMAP_DISPC_VID_FIR_COEF_H(layer,6)   = 0xF5701EFE;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,6) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,6) |= 0x000000FF;
	*OMAP_DISPC_VID_FIR_COEF_H(layer,7)   = 0xF87C0DFF;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,7) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,7) |= 0x00000000;
}

/* 
 * Configures 3 tap vertical down sampling for Video layer.  
 * Values taken direclty from  SPRUFA4 February 2008 
 */ 
void omap_coef_vid_down_v3tap(disp_adapter_t * adapter, unsigned int layer) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;
	if (layer > 1) {
		DEBUG_MSG("layer out of range");
		return;
	}
	/* 
	 * OMAP_DISPC_VIDn_FIR_COEF_HVi
	 *   Bits 31:24 VIDFIRVC2   Signed coefficient C2 for the vertical up-/down-scaling with the phase RW 0x00
	 *   Bits 23:16 VIDFIRVC1 Unsigned coefficient C1 for the vertical up-/down-scaling with the       RW 0x00
	 *   Bits 15:8  VIDFIRVC0   Signed coefficient C0 for the vertical up-/down-scaling with the phase RW 0x00
	 * 
	 *   Not used for vertical (mask)
	 *   Bits  7:0  VIDFIRHC4   Signed coefficient C4 for the horizontal up-/down-scaling with the     RW 0x00
	 */
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,0) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,0) |= 0x24382400;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,1) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,1) |= 0x28391F00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,2) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,2) |= 0x2D381B00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,3) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,3) |= 0x32371700;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,4) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,4) |= 0x12373700;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,5) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,5) |= 0x17373200;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,6) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,6) |= 0x1B382D00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,7) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,7) |= 0x1F392800;
}

/* 
 * Configures 5 tap vertical down sampling for Video layer.  
 * Values taken direclty from  SPRUFA4 February 2008 
 */ 
 void omap_coef_vid_down_v5tap(disp_adapter_t * adapter, unsigned int layer) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;
	if (layer > 1) {
		DEBUG_MSG("layer out of range");
		return;
	}
	/* 
	 *
	 * OMAP_DISPC_VIDn_FIR_COEF_HVi
	 *   Bits 31:24 VIDFIRVC2 Signed   coefficient C2 for the vertical up-/down-scaling with the phase RW 0x00
	 *   Bits 23:16 VIDFIRVC1 Unsigned coefficient C1 for the vertical up-/down-scaling with the       RW 0x00
	 *   Bits 15:8  VIDFIRVC0 Signed   coefficient C0 for the vertical up-/down-scaling with the phase RW 0x00
	 *
	 * OMAP_DISPC_VIDn_FIR_COEF_Vi
	 *   Bits 15:8  VIDFIRVC22 Signed coefficient C22 for vertical up/down-scaling with phase n RW 0x00
	 *   Bits  7:0  VIDFIRVC00 Signed coefficient C00 for vertical up/down-scaling with phase n RW 0x00
	 *
	 */
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,0) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,0) |= 0x24382400;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,0)   = 0x00000000;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,1) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,1) |= 0x28371F00;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,1)   = 0x000004FE;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,2) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,2) |= 0x2C361B00;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,2)   = 0x000008FB;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,3) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,3) |= 0x30351600;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,3)   = 0x00000CF9;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,4) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,4) |= 0x11343300;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,4)   = 0x0000F711;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,5) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,5) |= 0x16353000;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,5)   = 0x0000F90C;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,6) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,6) |= 0x1B362C00;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,6)   = 0x0000FB08;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,7) &= 0x000000ff;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,7) |= 0x1F372800;
	*OMAP_DISPC_VID_FIR_COEF_V(layer,7)   = 0x0000FE04;
}

/* 
 * Configures 5 tap horizontal down sampling for Video layer.  
 * Values taken direclty from  SPRUFA4 February 2008 
 */ 
void omap_coef_vid_down_h5tap(disp_adapter_t * adapter, unsigned int layer) {
	omap_ctx_t *o_ctx = adapter->ms_ctx;
	TRACE;
	if (layer > 1) {
		DEBUG_MSG("layer out of range");
		return;
	}
	/* 
	 *
	 * DISPC_VIDn_FIR_COEF_Hi
	 *   Bits 31:24 VIDFIRHC3   Signed coefficient C3 for the horizontal up-/down-scaling with the phase n    RW 0x00
	 *   Bits 23:16 VIDFIRHC2 Unsigned coefficient C2 for the horizontal up-/down-scaling with the phase n    RW 0x00
	 *   Bits 15:8  VIDFIRHC1   Signed coefficient C1 for the horizontal up-/down-scaling with the phase n    RW 0x00
	 *   Bits  7:0  VIDFIRHC0   Signed coefficient C0 for the horizontal up-/down-scaling with the phase n    RW 0x00
	 *
	 * OMAP_DISPC_VIDn_FIR_COEF_HVi
	 *   Bits  7:0 VIDFIRHC4    Signed coefficient C4 for the horizontal up-/down-scaling with the phase n    RW 0x00
	 *
	 */
	*OMAP_DISPC_VID_FIR_COEF_H(layer,0)   = 0x24382400;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,0) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,0) |= 0x00000000;
	*OMAP_DISPC_VID_FIR_COEF_H(layer,1)   = 0x28371FFE;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,1) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,1) |= 0x00000004;
	*OMAP_DISPC_VID_FIR_COEF_H(layer,2)   = 0x2C361BFB;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,2) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,2) |= 0x00000008;
	*OMAP_DISPC_VID_FIR_COEF_H(layer,3)   = 0x303516F9;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,3) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,3) |= 0x0000000C;
	*OMAP_DISPC_VID_FIR_COEF_H(layer,4)   = 0x11343311;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,4) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,4) |= 0x000000F7;
	*OMAP_DISPC_VID_FIR_COEF_H(layer,5)   = 0x1635300C;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,5) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,5) |= 0x000000F9;
	*OMAP_DISPC_VID_FIR_COEF_H(layer,6)   = 0x1B362C08;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,6) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,6) |= 0x000000FB;
	*OMAP_DISPC_VID_FIR_COEF_H(layer,7)   = 0x1F372804;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,7) &= 0xffffff00;
	*OMAP_DISPC_VID_FIR_COEF_HV(layer,7) |= 0x000000FE;
}



/* 
 *
 *  YUV is converted to RGB for display.
 *  This is done in the color space conversions block.  
 *  The coefficients require initialization to display YUV colors on the video layers.
 *
 */

/*
 * color convertion to table YUV to RGB 
 *
 * Coefficients 
 *		BT.601-5 		BT.601-5 		BT.709 		BT.709
 * RY 		298	 		256 			298 			256
 * RCr 	409 			351 			459 			394
 * RCb 	0 			0 			0 			0
 * GY 	298 			256 			298 			256
 * GCr 	-208			-179 			-137 			-118
 * GCb	 -11 			-86 			-55 			-47
 * BY 		298 			256 			298 			256
 * BCr 	0 			0 			0		 	0
 * BCb 	517 			443 			541 			465
 *
 */

static struct {
	const char *name;
	unsigned int range; /* 1 - full */
	short coefficients[9];
} color_coefficients_table[] = { 
/*	       name ,                       range,  {        Ry,      RCr,      RCb,       Gy,         GCr,     GCb,        By,      BCr,      BCb   }  */
	{ "BT601",      0,  { 298, 409,   0, 298, -208, -11 ,298,   0, 517 } }, 
	{ "BT601-5",    1,  { 256, 351,   0, 256, -179, -86, 298,   0, 443 } },
	{ "BT709",      0,  { 298, 459,   0, 256, -137, -55, 298,   0, 541 } },
	{ "BT709-full", 1,  { 256, 394,   0, 256, -118, -47, 298,   0, 465 } }
};


/* 
  * Private function to
  * Configure coefficients using  array.
  */
static void set_color_coefficients(disp_adapter_t *adapter, int layer, short coef[]) {
	omap_ctx_t *o_ctx= adapter->ms_ctx;

	TRACE;
	if (layer > 1) {
		DEBUG_MSG("layer out of range");
		return;
	}
	*OMAP_DISPC_VID_CONV_COEF(layer,0) = (coef[0]&0x7FF) | ((coef[1] &0x7FF) << 16);
	DEBUG_CMD(disp_printf(adapter, "OMAP_DISPC_VID_CONV_COEF0 layer %d  %x", layer, *OMAP_DISPC_VID_CONV_COEF(layer,0) ));
	*OMAP_DISPC_VID_CONV_COEF(layer,1) = (coef[2]&0x7FF) | ((coef[3] &0x7FF) << 16);
	DEBUG_CMD(disp_printf(adapter, "OMAP_DISPC_VID_CONV_COEF1 layer %d  %x", layer, *OMAP_DISPC_VID_CONV_COEF(layer,1) ));
	*OMAP_DISPC_VID_CONV_COEF(layer,2) = (coef[4]&0x7FF) | ((coef[5] &0x7FF) << 16);
	DEBUG_CMD(disp_printf(adapter, "OMAP_DISPC_VID_CONV_COEF2 layer %d  %x", layer, *OMAP_DISPC_VID_CONV_COEF(layer,2) ));
	*OMAP_DISPC_VID_CONV_COEF(layer,3) = (coef[6]&0x7FF) | ((coef[7] &0x7FF) << 16);
	DEBUG_CMD(disp_printf(adapter, "OMAP_DISPC_VID_CONV_COEF3 layer %d  %x", layer, *OMAP_DISPC_VID_CONV_COEF(layer,3) ));
	*OMAP_DISPC_VID_CONV_COEF(layer,4) = (coef[8]&0x7FF) ;
	DEBUG_CMD(disp_printf(adapter, "OMAP_DISPC_VID_CONV_COEF4 layer %d  %x", layer, *OMAP_DISPC_VID_CONV_COEF(layer,4) ));
}


/*
 * Set up color conversions coefficients table for YUV to RGB
 * Returns
 * 	0x0: Limited range selected: 16 subtracted from Y before color space conversion
 *	0x1: Full range selected: Y is not modified before the color space conversion
 * Note: Constistent with Bit 11 in DISPC_VIDn_ATTRIBUTES
 */
int set_yuv_to_rgb_conversion(disp_adapter_t *adapter, int layer, int entry) {
	omap_ctx_t *o_ctx= adapter->ms_ctx;
	TRACE;
	if (layer > 1) {
		DEBUG_MSG("layer out of range");
		return -1;
	}
	DEBUG_CMD(disp_printf(adapter, "Set YUV conversion coefficients table %s ", color_coefficients_table[entry].name));
	set_color_coefficients(adapter, layer, color_coefficients_table[entry].coefficients);
	return color_coefficients_table[entry].range;
}

/* 
 * Configure coefficients using   Ry,  RCr,  RCb,  Gy,  GCr,  GCb,  By,  BCr,  BCb
 * Normally used for custom yuv coefficients, maybe from configuration file
 */
void set_custom_color_coefficients(disp_adapter_t *adapter, int layer, short ry, short rcr, short rcb, short gy, short gcr, short gcb, short by, short bcr, short bcb) {
	omap_ctx_t *o_ctx= adapter->ms_ctx;

	TRACE;
	if (layer > 1) {
		DEBUG_MSG("layer out of range");
		return;
	}
	*OMAP_DISPC_VID_CONV_COEF(layer,0) = (ry&0x7FF) | ((rcr &0x7FF) << 16);
	DEBUG_CMD(disp_printf(adapter, "OMAP_DISPC_VID_CONV_COEF0 layer %d  %x", layer, *OMAP_DISPC_VID_CONV_COEF(layer,0) ));
	*OMAP_DISPC_VID_CONV_COEF(layer,1) = (rcb&0x7FF) | ((gy &0x7FF) << 16);
	DEBUG_CMD(disp_printf(adapter, "OMAP_DISPC_VID_CONV_COEF1 layer %d  %x", layer, *OMAP_DISPC_VID_CONV_COEF(layer,1) ));
	*OMAP_DISPC_VID_CONV_COEF(layer,2) = (gcr&0x7FF) | ((gcb &0x7FF) << 16);
	DEBUG_CMD(disp_printf(adapter, "OMAP_DISPC_VID_CONV_COEF2 layer %d  %x", layer, *OMAP_DISPC_VID_CONV_COEF(layer,2) ));
	*OMAP_DISPC_VID_CONV_COEF(layer,3) = (by&0x7FF) | ((bcr &0x7FF) << 16);
	DEBUG_CMD(disp_printf(adapter, "OMAP_DISPC_VID_CONV_COEF3 layer %d  %x", layer, *OMAP_DISPC_VID_CONV_COEF(layer,3) ));
	*OMAP_DISPC_VID_CONV_COEF(layer,3) = (bcb&0x7FF) ;
	DEBUG_CMD(disp_printf(adapter, "OMAP_DISPC_VID_CONV_COEF4 layer %d  %x", layer, *OMAP_DISPC_VID_CONV_COEF(layer,4) ));
}



