#
# Copyright 2009, QNX Software Systems. 
# 
# Licensed under the Apache License, Version 2.0 (the "License"). You 
# may not reproduce, modify or distribute this software except in 
# compliance with the License. You may obtain a copy of the License 
# at: http://www.apache.org/licenses/LICENSE-2.0 
# 
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" basis, 
# WITHOUT WARRANTIES OF ANY KIND, either express or implied.
#
# This file may contain contributions from others, either as 
# contributors under the License or as licensors under other terms.  
# Please review this entire file for other proprietary rights or license 
# notices, as well as the QNX Development Suite License Guide at 
# http://licensing.qnx.com/license-guide/ for other information.
#
/*
 * cstart.S
 *
 *	Initial code to get Neutrino started
 *
 * This code runs as the first instructions after control has transferred
 * from any bootup monitor.  This module defines a minimal bootup stack,
 * and sets SP to this stack, and then starts running C code in _main().
 */

	.data
	.globl	stack
	.globl	boot_args
	.globl	vstart
	
boot_args:
	.ascii	"ddpvbskr"	/* signature for mkifs */
stack_end:
	.space	3*1024
stack:
	.text

	.extern	_main
	.globl	_start

_start:

	/*
	 * Turn off interrupts and make sure we are in SVC mode
	 */
	mrs		lr, cpsr
	bic		lr, lr, #0x1f
	orr		lr, lr, #0xd3
	msr		cpsr, lr

#if 1
	/*
	 * Need SMI to modify AUXCR because we are executing Non-Secure
	 * FIXME: clearing ASA bit seems to prevent random application crashes
	 *        so use this workaround until the problem is debugged
	 */
	mrc     p15, 0, r2, c1, c0, 1
	bic     r0, r2, #(1 << 4)			// FIXME: clear ASA bit
	orr		r0, r0, #(1 << 5)			// set L1NEON (errata #458693)
	orr		r0, r0, #(1 << 9)			// set PLDNOP (errata #458693)
	orr		r0, r0, #(1 << 1)			// set L2EN
#ifdef FIXME
	orr		r0, r0, #(1 << 6)			// set IBE (errata #430973, 454179)
#else
	/*
	 * Clearing IBE makes branch predictor operations behave as NOPs
	 * This significantly improves context switch performance.
	 *
	 * The errata workarounds requiring IBE are related to Thumb/Thumb2
	 * code execution which is not supported by QNX.
	 */
	bic		r0, r0, #(1 << 6)
#endif
	mov     r12, #0x3
	.word   0xe1600070
#endif

#if 0	// needed only on r2p0 silicon revisions
	/*
	 * Need SMI to disable write-allocate in L2 cache (errata #460075)
	 */
	mrc		p15, 1, r0, c9, c0, 2		// read L2 Aux control register
	orr		r0, r0, #(1<<22)			// disable write allocate
	mov		r12, #0x2
	.word	0xe1600070
#endif

	/*
	 * Turn off MMU and data cache if necessary.
	 * WARNING: assumes we are running with a 1-1 mapping if MMU is enabled.
	 */
	mrc		p15, 0, lr, c1, c0, 0
	bic		lr, lr, #0x0000000f			// WCAM bits
	bic		lr, lr, #0x00000300			// RS   bits
	mcr		p15, 0, lr, c1, c0, 0
	mov		r0, #0
	mov		r0, r0
	mov		r0, r0

#if 0
	/*
	 * Invalidate caches and TLBs.
	 * WARNING: assumes data caches are clean.
	 */
	mcr		p15, 0, r0, c7, c7, 0	// FIXME_v7: re-work for ARMv7
	mcr		p15, 0, r0, c8, c7, 0	
#endif

	mov		r0, #0
	mcr		p15, 0, r0, c2, c0, 2		// Always use TTBR0

	/*
	 * Set the translation table base
	 */
	ldr		r0, =_arm_board_ttb			// set start of Translation Table base
	orr		r0, r0, #((3 << 3) | 3)
	mcr		p15, 0, r0, c2, c0, 0
#	mcr		p15, 0, r0, c2, c0, 1		// set TTBR1 so armv_setup_v7 works

	/*
	 * Enable MMU domain 15
	 */
	mov		r0, #(1 << 30)				// Client mode
	mcr		p15, 0, r0, c3, c0, 0

	/*
	 * Enable the MMU, using read-modify-write to preserve reserved bits.
	 */
	mrc		p15, 0, ip, c1, c0, 0
	orr		ip, ip, #(1 << 12)			// enable I Cache
	orr		ip, ip, #(1 <<  2)			// enable D Cache
	orr		ip, ip, #(1	<<  0)			// enable MMU
	mcr		p15, 0, ip, c1, c0, 0
	mov		r0, r0
	mov		r0, r0

	ldr		sp, =stack
	bl		_main

oops:
	b		oops

/*
 * void	vstart(uintptr_t syspageptr, unsigned entry_point, unsigned cpunum)
 *
 * Enable the mmu and jump to the next program's entry point
 * The next program is responsible for tearing down the 1-1 section
 * mapping of the startup program set up by init_mmu().
 */ 
vstart:
	mov		r4, r1
	mov		r5, r2
	mov		r6, r0

	/*
	 * Fluch D-cache
	 */
	bl		_armv7_flush_whole_dcache

	/*
	 * Turn off MMU and data cache if necessary.
	 */
	mrc		p15, 0, ip, c1, c0, 0
	bic		ip, ip, #0x00002000			// clear bits 13 (--V-)
	bic		ip, ip, #0x00000007			// clear bits 2:0 (-CAM)
	mcr		p15, 0, ip, c1, c0, 0

	mov		ip, #0
	mov		ip, ip
	mov		ip, ip

	/*
	 * Invalidate caches and TLBs.
	 */
	mcr		p15, 0, ip, c8, c7, 0	// invalidate TLBs
	mcr		p15, 0, ip, c7, c5, 0	// invalidate icache

	/*
	 * Set TTBCR to use 8K boundary for TTBR registers
	 */
	mov		ip, #1
	mcr		p15, 0, ip, c2, c0, 2

	/*
	 * Set the translation table base
	 */
	ldr		ip, =L1_paddr
	ldr		ip, [ip]
	add		ip, ip, r5, lsl #14				// L1_paddr * (cpu * ARM_L1_SIZE)
	mcr		p15, 0, ip, c2, c0, 0

	/*
	 * Enable MMU domain 0
	 */
	mov		ip, #1
	mcr		p15, 0, ip, c3, c0, 0

	/*
	 * Enable the MMU, using read-modify-write to preserve reserved bits.
	 */
	ldr		r2, =mmu_cr_clr
	ldr		r3, =mmu_cr_set
	ldr		r2, [r2]
	ldr		r3, [r3]
	mrc		p15, 0, lr, c1, c0, 0
	bic		ip, lr, r2
	orr		ip, ip, r3
	mcr		p15, 0, ip, c1, c0, 0
	mov		ip, #0
	mov		ip, ip
	mov		ip, ip

	/*
	 * Invalidate the caches and TLBs
	 */
	.align 5
#if 0
	mcr		p15, 0, ip, c7, c7, 0	// FIXME_v7: re-work for ARMv7
#endif
	mcr		p15, 0, ip, c7, c5, 0	// invalidate icache
	mcr		p15, 0, ip, c8, c7, 0	// invalidate TLBs

	/*
	 * Call entry_point(_syspage_ptr, cpunum)
	 */
	mov		r0, r6
	mov		r1, r5
	mov		pc, r4


