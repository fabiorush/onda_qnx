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

#ifndef __CS42448_AUDIO_H
#define __CS42448_AUDIO_H

/*
 *  Definitions for Audio registers for CS42448
 */
#define PWR_CTL       		0x02
#define FN_MODE       		0x03
#define IFACE_FMT     		0x04
#define ADC_CTL      		0x05
#define TRANS_CTL    		0x06
#define DAC_CH_MUTE  		0x07
#define AOUT0_VOL_CTL		0x08
#define AOUT2_VOL_CTL		0x0A
#define AOUT4_VOL_CTL		0x0B
#define AOUT5_VOL_CTL		0x0E
#define AIN0_VOL_CTL		0x11
#define AIN1_VOL_CTL		0x12
#define AIN2_VOL_CTL		0x13
#define AIN3_VOL_CTL		0x14
#define AIN4_VOL_CTL		0x15

/* Bit definitions for Power Control Register */
#define PDN         		(1<<0)
#define PDN_DAC1    		(1<<1)
#define PDN_DAC2    		(1<<2)
#define PDN_DAC3    		(1<<3)
#define PDN_DAC4    		(1<<4)
#define PDN_ADC1    		(1<<5)
#define PDN_ADC2    		(1<<6)
#define PDN_ADC3    		(1<<7)

/* Bit definitions for Functional Mode Register */
#define DAC_FM_SLAVE		(3<<6)
#define ADC_FM_SLAVE		(3<<4)

/* Bit definitions for Interface Format Register */
#define DAC_DIF_I2S 		(1<<3)
#define ADC_DIF_I2S 		(1<<0)
#define DAC_DIF_TDM 		(3<<4)
#define ADC_DIF_TDM 		(3<<1)

/* Bit definitions for ADC Control Register */
#define ADC1_SINGLE 		(1<<4)
#define ADC2_SINGLE 		(1<<3)
#define ADC3_SINGLE 		(1<<2)

/* Bit definitions for Transition Control Register */
#define ADC_ZCROSS  		(1<<0)
#define ADC_SRAMP   		(1<<1)
#define ADC_MUTE_SP 		(1<<3)
#define DAC_ZCROSS  		(1<<5)
#define DAC_SRAMP   		(1<<6)

/* Bit definitions for DAC Channel Mute Register */
#define AOUT1_MUTE  		(1<<0)
#define AOUT2_MUTE  		(1<<1)
#define AOUT3_MUTE  		(1<<2)
#define AOUT4_MUTE  		(1<<3)
#define AOUT5_MUTE  		(1<<4)
#define AOUT6_MUTE  		(1<<5)
#define AOUT7_MUTE  		(1<<6)
#define AOUT8_MUTE  		(1<<7)

#endif
