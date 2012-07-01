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
 * Definitions for Cavium Octeon CN5700 UART serial device
 *
 */

/* Offset definitions for registers */

#define OCTEON_REG_RX          0x00               /*R  Receive*/
#define OCTEON_REG_TX          0x08               /*W  Transmit*/
#define OCTEON_REG_DLL         0x10               /*W  Divisor Latch Low */
#define OCTEON_REG_DLH         0x11               /*W  Divisor Latch High*/
#define OCTEON_REG_IE          0x01               /*RW Interrupt Enable*/
#define OCTEON_REG_II          0x02               /*R  Interrupt Identification*/
#define OCTEON_REG_FC          0x0A               /*W  Fifo Control*/
#define OCTEON_REG_LC          0x03               /*RW Line Control*/
#define OCTEON_REG_MC          0x04               /*RW Modem Control*/
#define OCTEON_REG_LS          0x05               /*RW Line Status*/
#define OCTEON_REG_MS          0x06               /*RW Modem Status*/
	

/* Bit definitions for interrupt identification (REG_II) */
#define II_NOINTR       0x01
#define II_MS           0x00
#define II_TX           0x02
#define II_RX           0x04
#define II_LS           0x06
#define II_FIFO         0x80

/* Bit definitions for line control (REG_LC) */
#define LCR_BITS_MASK   0x03
#define LCR_STB2                0x04
#define LCR_PEN                 0x08
#define LCR_EPS                 0x10
#define LCR_SPS                 0x20
#define LCR_BREAK               0x40
#define LCR_DLAB                0x80

/* Bit definitions for modem control (REG_MC) */
#define MCR_DTR                 0x01
#define MCR_RTS                 0x02
#define MCR_OUT1                0x04
#define MCR_OUT2                0x08
#define MCR_LOOPBACK    0x10


/* Bit definitions for line status (REG_LS) */
#define LSR_RXRDY               0x01
#define LSR_OE                  0x02
#define LSR_PE                  0x04
#define LSR_FE                  0x08
#define LSR_BI                  0x10
#define LSR_TXRDY               0x20
#define LSR_TSRE                0x40
#define LSR_RCV_FIFO    	       0x80


/* Bit definitions for modem status (REG_MS) */
#define MSR_DCTS                0x01
#define MSR_DDSR                0x02
#define MSR_DRING               0x04
#define MSR_DDCD                0x08
#define MSR_CTS                 0x10
#define MSR_DSR                 0x20
#define MSR_RING                0x40
#define MSR_DCD                 0x80

/* Bit definitions for modem status (REG_FC) */
#define FCR_FIFO_ENABLE         0x01
#define FCR_RX_FIFO_RESET       0x02
#define FCR_TX_FIFO_RESET       0x04
