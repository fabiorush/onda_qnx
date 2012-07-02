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

#ifndef __TWL_AUDIO_H
#define __TWL_AUDIO_H

/*
 *  Definitions for Audio registers for TWL4030
 */
#define CODEC_MODE			0x01
#define OPTION				0x02
#define MICBIAS_CTL			0x04
#define ANAMICL				0x05
#define ANAMICR				0x06
#define AVADC_CTL			0x07
#define ADCMICSEL			0x08
#define DIGMIXING			0x09
#define ATXL1PGA			0x0A
#define ATXR1PGA			0x0B
#define AVTXL2PGA			0x0C
#define AVTXR2PGA			0x0D
#define AUDIO_IF			0x0E
#define VOICE_IF			0x0F
#define ARXR1PGA			0x10
#define ARXL1PGA			0x11
#define ARXR2PGA			0x12
#define ARXL2PGA			0x13
#define VRXPGA				0x14
#define VSTPGA				0x15
#define VRX2ARXPGA			0x16
#define AVDAC_CTL			0x17
#define ARX2VTXPGA			0x18
#define ARXL1_APGA_CTL		0x19
#define ARXR1_APGA_CTL		0x1A
#define ARXL2_APGA_CTL		0x1B
#define ARXR2_APGA_CTL		0x1C
#define ATX2ARXPGA			0x1D
#define BT_IF				0x1E
#define BTPGA				0x1F
#define BTSTPGA				0x20
#define EAR_CTL				0x21
#define HS_SEL				0x22
#define HS_GAIN_SET			0x23
#define HS_POPN_SET			0x24
#define PREDL_CTL			0x25
#define PREDR_CTL			0x26
#define PRECKL_CTL			0x27
#define PRECKR_CTL			0x28
#define HFL_CTL				0x29
#define HFR_CTL				0x2A
#define ALC_CTL				0x2B
#define ALC_SET1			0x2C
#define ALC_SET2			0x2D
#define BOOST_CTL			0x2E
#define SOFTVOL_CTL			0x2F
#define DTMF_FREQSEL		0x30
#define DTMF_TONEXT1H		0x31
#define DTMF_TONEXT1L		0x32
#define DTMF_TONEXT2H		0x33
#define DTMF_TONEXT2L		0x34
#define DTMF_TONOFF			0x35
#define DTMF_WANONOFF		0x36
#define I2S_RX_SCRAMBLE_H	0x37
#define I2S_RX_SCRAMBLE_M	0x38
#define I2S_RX_SCRAMBLE_L	0x39
#define APLL_CTL			0x3A
#define DTMF_CTL			0x3B
#define DTMF_PGA_CTL2		0x3C
#define DTMF_PGA_CTL1		0x3D
#define MISC_SET_1			0x3E
#define PCMBTMUX			0x3F
#define RX_PATH_SEL			0x43
#define VDL_APGA_CTL		0x44
#define VIBRA_CTL			0x45
#define VIBRA_SET			0x46
#define ANAMIC_GAIN			0x48
#define MISC_SET_2			0x49

/* Vibrator Register set */
#define PMBR1               0x92	/* Pad Muxing Bank 1 (For BT function) */

/* Bit defination for register CODEC_MODE */
#define APLL_RATE_MASK		(0xf << 4)
#define APLL_RATE_8000		(0)
#define APLL_RATE_16000		(0x4 << 4)
#define APLL_RATE_44100		(0x9 << 4)
#define APLL_RATE_48000		(0xA << 4)
#define SEL_16K				(1<<3)
#define CODECPDZ			(1<<1)
#define OPT_MODE			(1<<0)

/* Bit defination for register OPTION */
#define ARXR2_EN			(1<<7)
#define ARXL2_EN			(1<<6)
#define ARXR1_EN 			(1<<5)
#define ARXL1_VRX_EN		(1<<4)
#define ARXL1_EN			(1<<4)				   /* In mode 1 ARXL1_VRX_EN == ARXL1 */
#define ATXR2_VTXR_EN		(1<<3)
#define ATXL2_VTXL_EN		(1<<2)
#define ATXR1_EN			(1<<1)
#define ATXL1_EN			(1<<0)

/* Bit defination for register RX_PATH_SEL */
#define RX_SEL_MSK				(0x3f)
#define RXL1_SEL_SDRL2_RXL1PGA	(2<<2)
#define RXR1_SEL_SDRR2_RXR1PGA	(2<<0)

/* Bit defination for register APLL_CTL */
#define APLL_EN				(1<<4)
#define APLL_INFREQ_MASK	(0xf << 0)
#define APLL_INFREQ_19_2	(0x5 << 0)
#define APLL_INFREQ_26		(0x6 << 0)
#define APLL_INFREQ_38_4	(0xf << 0)

/* Bit defination for register BOOST_CTL */
#define BOOST_CTL_EFFECT_1	(1<<1)
#define BOOST_CTL_EFFECT_0	(1<<0)

/* Bit defination for register AVDAC_CTL */
#define VDAC_EN				(1<<4)
#define ADACL2_EN 			(1<<3)
#define ADACR2_EN 			(1<<2)
#define ADACL1_EN 			(1<<1)
#define ADACR1_EN			(1<<0)

/* Bit defination for register ARXL1_APGA_CTL */
#define ARXL1_GAIN_SET_MASK	(0xf8)
#define ARXL1_GAIN_SET_0DB	(0x6 << 3)
#define ARXL1_FM_EN			(1<<2)
#define ARXL1_DA_EN			(1<<1)
#define ARXL1_PDZ			(1<<0)

/* Bit defination for register ARXR1_APGA_CTL */
#define ARXR1_GAIN_SET_MASK	(0xf8)
#define ARXR1_GAIN_SET_0DB	(0x6 << 3)
#define ARXR1_FM_EN			(1<<2)
#define ARXR1_DA_EN			(1<<1)
#define ARXR1_PDZ			(1<<0)

/* Bit defination for register SOFTVOL_CTL */
#define SOFTVOL_SET_MASK	(0xE0)
#define SOFTVOL_EN			(1<<0)

/* Bit defination for register AUDIO_IF */
#define AIF_SLAVE_EN		(1<<7)
#define DATA_WIDTH_MASK		(0x60)
#define DATA_WIDTH_16_16	(0)
#define DATA_WIDTH_32_16	(0x2 << 5)
#define DATA_WIDTH_32_32	(0x3 << 5)
#define AIF_FORMAT_MASK		(0x18)
#define AIF_FORMAT_CODEC	(0)
#define AIF_FORMAT_LJ		(0x1 << 3)
#define AIF_FORMAT_RJ		(0x2 << 3)
#define AIF_FORMAT_TDM		(0x3 << 3)
#define AIF_TRI_EN			(1<<2)
#define CLK256FS_EN			(1<<1)
#define AIF_EN				(1<<0)

/* Bit definition for register VOICE_IF */
#define VIF_SLAVE_EN        (1<<7)
#define VIF_DIN_EN          (1<<6)
#define VIF_DOUT_EN         (1<<5)
#define VIF_SWAP            (1<<4)
#define VIF_FORMAT          (1<<3)
#define VIF_SUB_EN          (1<<1)
#define VIF_EN              (1<<0)

/* Bit definition for register BT_IF */
#define BT_DIN_EN           (1<<6)
#define BT_DOUT_EN          (1<<5)
#define BT_SWAP             (1<<4)
#define BT_EN               (1<<0)

/* Bit definition for register PCMBTMUX */
#define MUXTX_PCM           (1<<7)
#define MUXRX_PCM           (1<<6)
#define MUXRX_BT            (1<<5)
#define TNTXACT_PCM         (1<<3)
#define TNRXACT_PCM         (1<<2)
#define TNTXACT_BT          (1<<1)
#define TNRXACT_BT          (1<<0)

/* Bit defination for register HS_SEL*/
#define HSR_INV_EN			(1<<7)
#define HS_OUTLOW_EN		(1<<6)
#define HSOR_AR2_EN			(1<<5)
#define HSOR_AR1_EN			(1<<4)
#define HSOR_VOICE_EN		(1<<3)
#define HSOL_AL2_EN			(1<<2)
#define HSOL_AL1_EN			(1<<1)
#define HSOL_VOICE_EN		(1<<0)
#define HSOL_MSK			(0xff)

/* Bit defination for register HS_GAIN_SET */
#define HS_GAIN_SET_MASK	(0xF)
#define HS_GAIN_SETL_0DB	(1<<3)
#define HS_GAIN_SETR_0DB	(1<<1)

/* Bit defination for register ANAMICL */
#define CNCL_OFFSET_START	(1<<7)
#define OFFSET_CNCL_SEL_1	(1<<6)
#define OFFSET_CNCL_SEL_0	(1<<5)
#define MICAMPL_EN			(1<<4)
#define AUXL_EN				(1<<2)
#define MAINMIC_EN			(1<<0)

#define SMOOTH_ANAVOL_EN	(1<<1)

/* Bit defination for register HS_POPN_SET */
#define VMID_EN				(1<<6)
#define RAMP_EN				(1<<1)
#define RAMP_DELAY_MASK		(0x1C)

/* Bit defination for register ANAMICR */
#define MICAMPR_EN 			(1<<4)
#define AUXR_EN 			(1<<2)
#define ANAMICR_SUBMIC_EN	(1<<0)

/* Bit defination for register AVADC_CTL */
#define ADCL_EN 			(1<<3)
#define ADCR_EN				(1<<1)

/* Bit defination for register ADCMICSEL */
#define ADCMICSEL_DIGMIC1_EN	(1<<3)
#define ADCMICSEL_TX2IN_SEL 	(1<<2)
#define ADCMICSEL_DIGIC0_EN 	(1<<1)
#define ADCMICSEL_TX1IN_SEL		(1<<0)

#endif

__SRCVERSION( "$URL: http://svn.ott.qnx.com/product/trunk/hardware/deva/ctrl/omap35xx/nto/arm/dll.le.twl4030/twl4030.h $ $Rev: 219612 $" )
