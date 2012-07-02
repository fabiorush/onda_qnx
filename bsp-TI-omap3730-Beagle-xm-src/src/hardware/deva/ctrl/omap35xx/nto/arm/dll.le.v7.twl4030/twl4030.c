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

struct twl4030_context;

#define	 MIXER_CONTEXT_T struct twl4030_context

#include <omap35xx.h>
#include <hw/i2c.h>
#include "twl4030.h"

typedef struct twl4030_context
{
	ado_mixer_t *mixer;
	int     fd;
	int     i2c_addr;
} twl4030_context_t;

static int32_t pcm_devices[1] = {
	0
};

static snd_mixer_voice_t stereo_voices[2] = {
	{SND_MIXER_VOICE_LEFT, 0},
	{SND_MIXER_VOICE_RIGHT, 0}
};

static struct snd_mixer_element_volume1_range output_range[1] = {
	{1, 63, -6200, 0},
};

static struct snd_mixer_element_volume1_range input_range[1] = {
	{0, 31, 0, 3100},
};

static int
twl4030_write (MIXER_CONTEXT_T * twl4030, uint8_t register_address, uint8_t val, uint8_t bitmask)
{
	struct send
	{
		i2c_send_t hdr;
		uint8_t buf[2];
	} twl4030_wr_data;

	struct send_recv
	{
		i2c_sendrecv_t hdr;
		uint8_t buf[2];
	} twl4030_rd_data;

	uint8_t data_tmp;

	twl4030_rd_data.buf[0] = register_address;
	twl4030_rd_data.hdr.send_len = 1;
	twl4030_rd_data.hdr.recv_len = 1;
	twl4030_rd_data.hdr.slave.addr = twl4030->i2c_addr;
	twl4030_rd_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	twl4030_rd_data.hdr.stop = 0;

	if (devctl (twl4030->fd, DCMD_I2C_SENDRECV, &twl4030_rd_data, sizeof (twl4030_rd_data), NULL))
	{
		ado_error ("Failed to read codec reg values: %s\n", strerror (errno));
		return -1;
	}

	data_tmp = twl4030_rd_data.buf[0];

	/*Now that the register value is read, Set and clear the required bits only */
	twl4030_wr_data.buf[0] = register_address;
	twl4030_wr_data.buf[1] = (data_tmp & ~bitmask) | (val & bitmask);

	twl4030_wr_data.hdr.len = 2;
	twl4030_wr_data.hdr.slave.addr = twl4030->i2c_addr;
	twl4030_wr_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	twl4030_wr_data.hdr.stop = 1;

	if (devctl (twl4030->fd, DCMD_I2C_SEND, &twl4030_wr_data, sizeof (twl4030_wr_data), NULL))
	{
		ado_error ("Failed to write to codec: %s\n", strerror (errno));
		return -1;
	}

	return 0;
}

static  uint8_t
twl4030_read (MIXER_CONTEXT_T * twl4030, uint8_t register_address)
{
	struct send_recv
	{
		i2c_sendrecv_t hdr;
		uint8_t buf[2];
	} twl4030_rd_data;

	/*Read the Registers Current Value */
	twl4030_rd_data.buf[0] = register_address;
	twl4030_rd_data.hdr.send_len = 1;
	twl4030_rd_data.hdr.recv_len = 1;

	twl4030_rd_data.hdr.slave.addr = twl4030->i2c_addr;
	twl4030_rd_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	twl4030_rd_data.hdr.stop = 1;

	if (devctl (twl4030->fd, DCMD_I2C_SENDRECV, &twl4030_rd_data, sizeof (twl4030_rd_data), NULL))
	{
		ado_error ("Failed to write to codec: %s\n", strerror (errno));
	}

	return twl4030_rd_data.buf[0];
}

static  int32_t
twl4030_auxin_mute_control (MIXER_CONTEXT_T * twl4030, ado_mixer_delement_t * element, uint8_t set,
	uint32_t * val, void *instance_data)
{
	int32_t altered = 0;

	if (set)
	{
		altered = 1;
		if (val[0])
		{
			twl4030_write (twl4030, ANAMICL, 0, AUXL_EN);
			twl4030_write (twl4030, ANAMICR, 0, AUXR_EN);
		}
		else
		{
			twl4030_write (twl4030, ANAMICL, AUXL_EN, AUXL_EN);
			twl4030_write (twl4030, ANAMICR, AUXR_EN, AUXR_EN);
		}
	}
	else
	{
		altered = 0;

		if (twl4030_read (twl4030, ANAMICL) & AUXL_EN)
		{
			if (twl4030_read (twl4030, ANAMICR) & AUXR_EN)
			{
				val[0] = 0;
				return altered;
			}
		}
		val[0] = 1;
	}

	return altered;
}

static  int32_t
twl4030_auxin_vol_control (MIXER_CONTEXT_T * twl4030, ado_mixer_delement_t * element, uint8_t set,
	uint32_t * vol, void *instance_data)
{
	int32_t altered = 0;
	uint8_t data[2];

	data[0] = twl4030_read (twl4030, ATXL1PGA);
	data[1] = twl4030_read (twl4030, ATXR1PGA);
	if (set)
	{
		altered = vol[0] != data[0] || vol[1] != data[1];
		twl4030_write (twl4030, ATXL1PGA, vol[0], 0x1f);
		twl4030_write (twl4030, ATXR1PGA, vol[1], 0x1f);
	}
	else
	{
		vol[0] = data[0];
		vol[1] = data[1];
	}
	return altered;
}

static  int32_t
twl4030_hs_mute_control (MIXER_CONTEXT_T * twl4030, ado_mixer_delement_t * element, uint8_t set,
	uint32_t * val, void *instance_data)
{
	int32_t altered = 0;

	if (set)
	{
		altered = 1;
		if (val[0])
		{
			twl4030_write (twl4030, HS_SEL, 0x0, 0xff);
		}
		else
		{
			twl4030_write (twl4030, HS_SEL, HSOL_AL1_EN | HSOR_AR1_EN, HSOL_AL1_EN | HSOR_AR1_EN);
		}
	}
	else
	{
		altered = 0;
		if (twl4030_read (twl4030, HS_SEL) & (HSOL_AL1_EN | HSOR_AR1_EN))
			val[0] = 0;
		else
			val[0] = 1;
	}
	return altered;
}


static  int32_t
twl4030_hs_vol_control (MIXER_CONTEXT_T * twl4030, ado_mixer_delement_t * element, uint8_t set,
	uint32_t * vol, void *instance_data)
{
	int32_t altered = 0;
	uint8_t data[2];

	data[0] = twl4030_read (twl4030, ARXL1PGA);
	data[1] = twl4030_read (twl4030, ARXR1PGA);
	if (set)
	{
		altered = vol[0] != data[0] || vol[1] != data[1];
		twl4030_write (twl4030, ARXL1PGA, vol[0], 0x3f);
		twl4030_write (twl4030, ARXR1PGA, vol[1], 0x3f);
	}
	else
	{
		vol[0] = data[0];
		vol[1] = data[1];
	}
	return altered;
}


int32_t
twl4030_sw_read_reg (MIXER_CONTEXT_T * twl4030, ado_dswitch_t * dswitch, snd_switch_t * cswitch,
	void *instance_data)
{
	cswitch->type = SND_SW_TYPE_BOOLEAN;
	if (twl4030_read (twl4030, ATX2ARXPGA) & 0x3f)
		cswitch->value.enable = 1;

	return (0);
}

int32_t
twl4030_sw_write_reg (MIXER_CONTEXT_T * twl4030, ado_dswitch_t * dswitch, snd_switch_t * cswitch,
	void *instance_data)
{
	int32_t altered = 0;

	altered = cswitch->value.enable != (twl4030_read (twl4030, ATX2ARXPGA) ? 1 : 0);

	if (cswitch->value.enable)
		twl4030_write (twl4030, ATX2ARXPGA, 0x3f, 0x3f);   /* Set Digital Loopback Gain to 0dB */
	else
		twl4030_write (twl4030, ATX2ARXPGA, 0x00, 0x3f);   /* Mute Digital Loopback */

	return (altered);
}


static  int32_t
twl4030_mixer_build (MIXER_CONTEXT_T * twl4030, ado_mixer_t * mixer)
{

	ado_mixer_delement_t *hs_vol;
	ado_mixer_delement_t *hs_mute;
	ado_mixer_delement_t *auxin_vol;
	ado_mixer_delement_t *auxin_mute;

	ado_mixer_dgroup_t *hs_grp;
	ado_mixer_dgroup_t *auxin_grp;

	int     error = 0;
	ado_mixer_delement_t *pre_elem = NULL, *elem = NULL;


	/*######################## */
	/* ## HeadSet Output Group */
	/*######################## */

	/* Create PCM element */
	if (!error && (pre_elem = ado_mixer_element_pcm1 (mixer, SND_MIXER_PCM_OUT,
				SND_MIXER_ETYPE_PLAYBACK1, 1, &pcm_devices[0])) == NULL)
		error++;

	if (!error && (elem = ado_mixer_element_volume1 (mixer, "HS Volume",
				2, output_range, twl4030_hs_vol_control, NULL, NULL)) == NULL)
		error++;

	if (!error && ado_mixer_element_route_add (mixer, pre_elem, elem) != 0)
		error++;

	hs_vol = elem;
	pre_elem = elem;

	if (!error && (elem = ado_mixer_element_sw1 (mixer, "HS Mute", 1,
				twl4030_hs_mute_control, NULL, NULL)) == NULL)
		error++;

	if (!error && ado_mixer_element_route_add (mixer, pre_elem, elem) != 0)
		error++;

	hs_mute = elem;
	pre_elem = elem;

	if (!error && (elem = ado_mixer_element_io (mixer, "HSOUT",
				SND_MIXER_ETYPE_OUTPUT, 0, 2, stereo_voices)) == NULL)
		error++;

	if (!error && ado_mixer_element_route_add (mixer, pre_elem, elem) != 0)
		error++;

	if (!error && (hs_grp = ado_mixer_playback_group_create (mixer, SND_MIXER_HEADPHONE_OUT,
				SND_MIXER_CHN_MASK_STEREO, hs_vol, hs_mute)) == NULL)
		error++;


	/*################# */
	/*## AuxIn Group ## */
	/*################# */
	if (!error && (pre_elem = ado_mixer_element_pcm1 (mixer, SND_MIXER_ELEMENT_CAPTURE,
				SND_MIXER_ETYPE_CAPTURE1, 1, &pcm_devices[0])) == NULL)
		error++;

	if (!error && (elem = ado_mixer_element_volume1 (mixer, "AuxIn Volume",
				2, input_range, twl4030_auxin_vol_control, NULL, NULL)) == NULL)
		error++;

	if (!error && ado_mixer_element_route_add (mixer, pre_elem, elem) != 0)
		error++;

	auxin_vol = elem;
	pre_elem = elem;

	if (!error && (elem = ado_mixer_element_sw1 (mixer, "AuxIn Mute", 1,
				twl4030_auxin_mute_control, NULL, NULL)) == NULL)
		error++;

	if (!error && ado_mixer_element_route_add (mixer, pre_elem, elem) != 0)
		error++;

	auxin_mute = elem;
	pre_elem = elem;

	if (!error && (elem = ado_mixer_element_io (mixer, "AUXIN",
				SND_MIXER_ETYPE_INPUT, 0, 2, stereo_voices)) == NULL)
		error++;

	if (!error && ado_mixer_element_route_add (mixer, pre_elem, elem) != 0)
		error++;

	if (!error && (auxin_grp = ado_mixer_capture_group_create (mixer, SND_MIXER_AUX_IN,
				SND_MIXER_CHN_MASK_STEREO, auxin_vol, auxin_mute, NULL, NULL)) == NULL)
		error++;

	/* ############ */
	/* the Switches */
	/* ############ */
	if (!error &&
		ado_mixer_switch_new (mixer, SND_MIXER_GRP_DIGITAL_LOOPBACK, SND_SW_TYPE_BOOLEAN, 0,
			twl4030_sw_read_reg, twl4030_sw_write_reg, NULL, NULL) == NULL)
		error++;

	return (!error ? 0 : -1);
}


static ado_mixer_reset_t twl4030_reset;
static  int32_t
twl4030_reset (MIXER_CONTEXT_T * twl4030)
{
	/* Ensure Codec is powered down */
	if (twl4030_read (twl4030, CODEC_MODE) & CODECPDZ)
	{
		twl4030_write (twl4030, HS_POPN_SET, 0, RAMP_EN);
		twl4030_write (twl4030, HS_GAIN_SET, 0, 0xF);
		twl4030_write (twl4030, HS_POPN_SET, 0, VMID_EN);
		twl4030_write (twl4030, CODEC_MODE, 0, CODECPDZ);
	}

	/* Set FS rate */
	twl4030_write (twl4030, CODEC_MODE, OPT_MODE, OPT_MODE);	/*  Mode 1 */
	twl4030_write (twl4030, CODEC_MODE, 0x0, SEL_16K);  		/*  Voice mode: 8Khz (for BT) */
	twl4030_write (twl4030, CODEC_MODE, APLL_RATE_48000, APLL_RATE_MASK);

	/* Enable RX left/right and TX left/right */
	twl4030_write (twl4030, OPTION, 0x0, 0xff);
	twl4030_write (twl4030, OPTION, ARXR2_EN | ARXL2_EN | ATXR1_EN | ATXL1_EN,
		ARXR2_EN | ARXL2_EN | ATXR1_EN | ATXL1_EN);

	/* Route to RXL1 and RXR1 PGA */
	twl4030_write (twl4030, RX_PATH_SEL, RXL1_SEL_SDRL2_RXL1PGA | RXR1_SEL_SDRR2_RXR1PGA,
		RX_SEL_MSK);

	/* Config input Clock Frequency */
	twl4030_write (twl4030, APLL_CTL, APLL_INFREQ_26, APLL_INFREQ_MASK);
	twl4030_write (twl4030, APLL_CTL, APLL_EN, APLL_EN);

	twl4030_write (twl4030, BOOST_CTL, BOOST_CTL_EFFECT_1, BOOST_CTL_EFFECT_1 | BOOST_CTL_EFFECT_0);

	/* Enable Audio DACs */
	twl4030_write (twl4030, AVDAC_CTL, ADACL1_EN | ADACR1_EN, ADACL1_EN | ADACR1_EN);

	/* Config Analog RXL1 PGA control */
	twl4030_write (twl4030, ARXL1_APGA_CTL, ARXL1_DA_EN | ARXL1_PDZ | ARXL1_GAIN_SET_0DB,
		ARXL1_DA_EN | ARXL1_PDZ | ARXL1_GAIN_SET_MASK);

	/* Config Analog RXR1 PGA control */
	twl4030_write (twl4030, ARXR1_APGA_CTL, ARXR1_DA_EN | ARXR1_PDZ | ARXR1_GAIN_SET_0DB,
		ARXR1_DA_EN | ARXR1_PDZ | ARXR1_GAIN_SET_MASK);

	/* Enable softvol sweep time of 1024 * 0.8/FS */
	twl4030_write (twl4030, SOFTVOL_CTL, 0, SOFTVOL_SET_MASK);
	twl4030_write (twl4030, SOFTVOL_CTL, SOFTVOL_EN, SOFTVOL_EN);

	/* Slave mode, 16-bit words, */
	twl4030_write (twl4030, AUDIO_IF, AIF_SLAVE_EN | CLK256FS_EN,
		AIF_SLAVE_EN | CLK256FS_EN | DATA_WIDTH_MASK | AIF_FORMAT_MASK);
	twl4030_write (twl4030, AUDIO_IF, AIF_EN, AIF_EN);

	/* Select BT function in Pin Muxing Bank 1 */
	twl4030_write ( twl4030, PMBR1, (1<<6), (1<<6));

	/* Board wiring is backwards, so we must use the VIF_SWAP bit.
	 * Next rev of the hardware will resolve this and we will need to remove the VIF_SWAP bit.
	 */
	twl4030_write (twl4030, VOICE_IF, (VIF_SLAVE_EN | VIF_DIN_EN | VIF_DOUT_EN | VIF_FORMAT | VIF_SWAP | VIF_EN), 0xFF);
	twl4030_write (twl4030, BT_IF, (BT_DIN_EN | BT_DOUT_EN | BT_EN), 0xFF);

	/* Set BT Digital Gain to 0db */
	twl4030_write (twl4030, BTPGA, (0x5<<4) | 0xA, 0xFF);
	twl4030_write (twl4030, BTSTPGA, 0x0, 0xFF);	/* Mute BT sidetone */

	/* Route Voice interface to BT interface */
	twl4030_write (twl4030, PCMBTMUX, (MUXTX_PCM | MUXRX_BT), 0xFF);

	/* Enabel Headset Audio L/R1 */
	twl4030_write (twl4030, HS_SEL, HSOL_AL1_EN | HSOR_AR1_EN, HSOL_MSK);

	/* Set the Digital PGA HeadSet volume to 0db */
	twl4030_write (twl4030, ARXL1PGA, 0x3f, 0xff);
	twl4030_write (twl4030, ARXR1PGA, 0x3f, 0xff);

	twl4030_write (twl4030, HS_GAIN_SET, HS_GAIN_SETL_0DB | HS_GAIN_SETR_0DB, 0xF);

	/* Enable Pop reduction on analog gain changes (delay until zero cross) */
	twl4030_write (twl4030, MISC_SET_1, SMOOTH_ANAVOL_EN, SMOOTH_ANAVOL_EN);

	/* Select Analog inputs */
	twl4030_write (twl4030, ADCMICSEL, 0, 0);

	/* Enable Aux inputs left/right */
	twl4030_write (twl4030, ANAMICL, AUXL_EN | MICAMPL_EN,
		AUXL_EN | MICAMPL_EN | OFFSET_CNCL_SEL_0 | OFFSET_CNCL_SEL_1);
	twl4030_write (twl4030, ANAMICR, AUXR_EN | MICAMPR_EN, AUXR_EN | MICAMPR_EN);

	/* Enable Audio ADC */
	twl4030_write (twl4030, AVADC_CTL, ADCL_EN | ADCR_EN, ADCL_EN | ADCR_EN);

	/* Set the initial AuxIn Volume */
	twl4030_write (twl4030, ATXL1PGA, 0x09, 0x1f);
	twl4030_write (twl4030, ATXR1PGA, 0x09, 0x1f);

	/* Enable ADCs */
	twl4030_write (twl4030, AVADC_CTL, ADCL_EN | ADCR_EN, 0xFF);

	/* Mute Digital Loopback */
	twl4030_write (twl4030, ATX2ARXPGA, 0x0, 0x3f);

	/* As per Codec doc's toggle the Power to apply reg setting */
	twl4030_write (twl4030, CODEC_MODE, 0, CODECPDZ);	   /* Power down Codec */
	twl4030_write (twl4030, CODEC_MODE, CODECPDZ, CODECPDZ);	/* Power Up Codec */

	/* To prevent clipping of the negative portion of the audio output */
	twl4030_write (twl4030, HS_POPN_SET, VMID_EN | (1 << 2), VMID_EN | RAMP_DELAY_MASK);
	twl4030_write (twl4030, HS_POPN_SET, RAMP_EN, RAMP_EN);

	return 0;
}

static ado_mixer_destroy_t twl4030_destroy;
static  int32_t
twl4030_destroy (MIXER_CONTEXT_T * twl4030)
{
	ado_debug (DB_LVL_MIXER, "Destroying TWL4030 Codec");
	ado_free (twl4030);
	return 0;
}

int
codec_mixer (ado_card_t * card, HW_CONTEXT_T * omap35xx)
{
	twl4030_context_t *twl4030;
	int32_t status;
	unsigned int speed;

	ado_debug (DB_LVL_MIXER, "Initializing TWL4030 Codec");

	if ((twl4030 = (twl4030_context_t *) ado_calloc (1, sizeof (twl4030_context_t))) == NULL)
	{
		ado_error ("twl4030: no memory (%s)", strerror (errno));
		return -1;
	}
	if ((status = ado_mixer_create (card, "twl4030", &omap35xx->mixer, twl4030)) != EOK)
	{
		ado_free (twl4030);
		return status;
	}
	twl4030->mixer = omap35xx->mixer;
	twl4030->i2c_addr = omap35xx->codec_i2c_addr;

	if ((twl4030->fd = open ("/dev/i2c0", O_RDWR)) < 0)
	{
		ado_error ("twl4030: could not open i2c device %s", strerror (errno));
		return (-1);
	}

	/* Set the bus speed to 100K */
	speed = 100000;
	if (devctl (twl4030->fd, DCMD_I2C_SET_BUS_SPEED, &speed, sizeof (speed), NULL))
	{
		ado_error ("twl4030: Failed to set I2C bus speed\n");
		close (twl4030->fd);
		return -1;
	}

	if (twl4030_mixer_build (twl4030, twl4030->mixer))
	{
		close (twl4030->fd);
		ado_free (twl4030);
		return -1;
	}

	if (twl4030_reset (twl4030))
	{
		close (twl4030->fd);
		ado_free (twl4030);
		return -1;
	}

	ado_mixer_set_reset_func (twl4030->mixer, twl4030_reset);
	ado_mixer_set_destroy_func (twl4030->mixer, twl4030_destroy);

	ado_pcm_chn_mixer (omap35xx->pcm1, ADO_PCM_CHANNEL_PLAYBACK, omap35xx->mixer,
		ado_mixer_find_element (omap35xx->mixer, SND_MIXER_ETYPE_PLAYBACK1, SND_MIXER_PCM_OUT, 0),
		ado_mixer_find_group (omap35xx->mixer, SND_MIXER_HEADPHONE_OUT, 0));

	ado_pcm_chn_mixer (omap35xx->pcm1, ADO_PCM_CHANNEL_CAPTURE, omap35xx->mixer,
		ado_mixer_find_element (omap35xx->mixer, SND_MIXER_ETYPE_CAPTURE1,
			SND_MIXER_ELEMENT_CAPTURE, 0), ado_mixer_find_group (omap35xx->mixer, SND_MIXER_AUX_IN,
			0));

	if (ado_pcm_sw_mix (card, omap35xx->pcm1, omap35xx->mixer))
	{
		close (twl4030->fd);
		ado_free (twl4030);
		return (-1);
	}

	return 0;
}

__SRCVERSION( "$URL: http://svn.ott.qnx.com/product/trunk/hardware/deva/ctrl/omap35xx/nto/arm/dll.le.twl4030/twl4030.c $ $Rev: 217582 $" );
