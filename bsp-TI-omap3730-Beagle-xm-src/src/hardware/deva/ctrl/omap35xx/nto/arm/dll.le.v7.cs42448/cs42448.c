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

struct cs42448_context;

#define	 MIXER_CONTEXT_T struct cs42448_context

#include <omap35xx.h>
#include <hw/i2c.h>
#include "cs42448.h"

typedef struct cs42448_context
{
	ado_mixer_t *mixer;
	HW_CONTEXT_T *omap35xx;
	int     fd;
	int     i2c_addr;
	uint32_t prev_ainx_mute;
} cs42448_context_t;

static int32_t pcm_devices[1] = {
	0
};

static snd_mixer_voice_t default_voices[3] = {
	{SND_MIXER_VOICE_LEFT, 0},
	{SND_MIXER_VOICE_RIGHT, 0},
	{SND_MIXER_VOICE_CENTER, 0},
};

static snd_mixer_voice_t quad_voices[4] = {
	{SND_MIXER_VOICE_LEFT, 0},
	{SND_MIXER_VOICE_RIGHT, 0},
	{SND_MIXER_VOICE_REAR_LEFT, 0},
	{SND_MIXER_VOICE_REAR_RIGHT, 0}
};

static struct snd_mixer_element_volume1_range output_range[4] = {
	{0, 255, -12750, 0},
	{0, 255, -12750, 0},
	{0, 255, -12750, 0},
	{0, 255, -12750, 0}
};

static struct snd_mixer_element_volume1_range input_range[2] = {
	{0, 176, -6400, 2400},
};

static int
disable_power_amp (MIXER_CONTEXT_T * cs42448)
{
#define TDA8595_PWR_AMP_SLAVE_ADDR 0x6F
	struct send
	{
		i2c_send_t hdr;
		uint8_t buf[3];
	} cs42448_wr_data;


	cs42448_wr_data.buf[0] = 0x00;						   /* Enable power amp    */
	cs42448_wr_data.buf[1] = 0x01;						   /* Fast mute all channels */
	cs42448_wr_data.buf[2] = 0x0f;						   /* Disable channels 1-4 */

	cs42448_wr_data.hdr.len = 3;
	cs42448_wr_data.hdr.slave.addr = TDA8595_PWR_AMP_SLAVE_ADDR;
	cs42448_wr_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	cs42448_wr_data.hdr.stop = 1;

	if (devctl (cs42448->fd, DCMD_I2C_SEND, &cs42448_wr_data, sizeof (cs42448_wr_data), NULL))
	{
		ado_error ("Failed to write to power amp: %s\n", strerror (errno));
		return -1;
	}
	return 0;
}


static int
enable_power_amp (MIXER_CONTEXT_T * cs42448)
{
#define TDA8595_PWR_AMP_SLAVE_ADDR 0x6F
	struct send
	{
		i2c_send_t hdr;
		uint8_t buf[3];
	} cs42448_wr_data;

	cs42448_wr_data.buf[0] = 0x01;						   /* Enable power amp    */
	cs42448_wr_data.buf[1] = 0x01;						   /* Fast Mute all channels */
	cs42448_wr_data.buf[2] = 0x00;						   /* Enable channels 1-4 */

	cs42448_wr_data.hdr.len = 3;
	cs42448_wr_data.hdr.slave.addr = TDA8595_PWR_AMP_SLAVE_ADDR;
	cs42448_wr_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	cs42448_wr_data.hdr.stop = 1;

	if (devctl (cs42448->fd, DCMD_I2C_SEND, &cs42448_wr_data, sizeof (cs42448_wr_data), NULL))
	{
		ado_error ("Failed to write to power amp: %s\n", strerror (errno));
		return -1;
	}

	delay (2200);										   /* Wait for capacitor to charge (must wait to avoid POP) */
	cs42448_wr_data.buf[0] = 0x01;						   /* Enable power amp     */
	cs42448_wr_data.buf[1] = 0x00;						   /* Un-Mute channels 1-4 */
	cs42448_wr_data.buf[2] = 0x00;						   /* Enable channels 1-4  */

	if (devctl (cs42448->fd, DCMD_I2C_SEND, &cs42448_wr_data, sizeof (cs42448_wr_data), NULL))
	{
		ado_error ("Failed to write to power amp: %s\n", strerror (errno));
		return -1;
	}

	return 0;
}

static int
cs42448_write (MIXER_CONTEXT_T * cs42448, uint8_t register_address, uint8_t val, uint8_t bitmask)
{
	struct send
	{
		i2c_send_t hdr;
		uint8_t buf[2];
	} cs42448_wr_data;

	struct send_recv
	{
		i2c_sendrecv_t hdr;
		uint8_t buf[2];
	} cs42448_rd_data;

	uint8_t data_tmp;

	cs42448_rd_data.buf[0] = register_address;
	cs42448_rd_data.hdr.send_len = 1;
	cs42448_rd_data.hdr.recv_len = 1;
	cs42448_rd_data.hdr.slave.addr = cs42448->i2c_addr;
	cs42448_rd_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	cs42448_rd_data.hdr.stop = 0;

	if (devctl (cs42448->fd, DCMD_I2C_SENDRECV, &cs42448_rd_data, sizeof (cs42448_rd_data), NULL))
	{
		ado_error ("Failed to read codec reg values: %s\n", strerror (errno));
		return -1;
	}

	data_tmp = cs42448_rd_data.buf[0];

	/*Now that the register value is read, Set and clear the required bits only */
	cs42448_wr_data.buf[0] = register_address;
	cs42448_wr_data.buf[1] = (data_tmp & ~bitmask) | (val & bitmask);

	cs42448_wr_data.hdr.len = 2;
	cs42448_wr_data.hdr.slave.addr = cs42448->i2c_addr;
	cs42448_wr_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	cs42448_wr_data.hdr.stop = 1;

	if (devctl (cs42448->fd, DCMD_I2C_SEND, &cs42448_wr_data, sizeof (cs42448_wr_data), NULL))
	{
		ado_error ("Failed to write to codec: %s\n", strerror (errno));
		return -1;
	}

	return 0;
}

static  uint8_t
cs42448_read (MIXER_CONTEXT_T * cs42448, uint8_t register_address)
{
	struct send_recv
	{
		i2c_sendrecv_t hdr;
		uint8_t buf[2];
	} cs42448_rd_data;

	/*Read the Registers Current Value */
	cs42448_rd_data.buf[0] = register_address;
	cs42448_rd_data.hdr.send_len = 1;
	cs42448_rd_data.hdr.recv_len = 1;

	cs42448_rd_data.hdr.slave.addr = cs42448->i2c_addr;
	cs42448_rd_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	cs42448_rd_data.hdr.stop = 1;

	if (devctl (cs42448->fd, DCMD_I2C_SENDRECV, &cs42448_rd_data, sizeof (cs42448_rd_data), NULL))
		ado_error ("Failed to write to codec: %s\n", strerror (errno));

	return cs42448_rd_data.buf[0];
}

static  int32_t
cs42448_ainx_mute_control (MIXER_CONTEXT_T * cs42448, ado_mixer_delement_t * element, uint8_t set,
	uint32_t * val, void *instance_data)
{
	int32_t altered = 0;

	if (set)
	{
		altered = cs42448->prev_ainx_mute != val[0] ? 1 : 0;
		if (altered)
		{
			if (val[0] > cs42448->prev_ainx_mute)
			{
				cs42448_write (cs42448, TRANS_CTL, ADC_MUTE_SP, ADC_MUTE_SP);
				cs42448->prev_ainx_mute = cs42448->omap35xx->cap_strm.voices == 1 ? 1 : 0x3;
			}
			else
			{
				cs42448_write (cs42448, TRANS_CTL, 0, ADC_MUTE_SP);
				cs42448->prev_ainx_mute = 0x0;
			}
		}
	}
	else
	{
		if (cs42448_read (cs42448, TRANS_CTL) & ADC_MUTE_SP)
			val[0] = cs42448->omap35xx->cap_strm.voices == 1 ? 1 : 0x3;
		else
			val[0] = 0;
	}

	return altered;
}

static  int32_t
cs42448_ainx_vol_control (MIXER_CONTEXT_T * cs42448, ado_mixer_delement_t * element, uint8_t set,
	uint32_t * vol, void *instance_data)
{
	int32_t altered = 0;
	uint8_t data[2] = { 0, 0 };
	uint32_t ainx_vol_ctl = (uint32_t) instance_data;

	data[0] = cs42448_read (cs42448, ainx_vol_ctl + 0);
	if (cs42448->omap35xx->cap_strm.voices == 2)
		data[1] = cs42448_read (cs42448, ainx_vol_ctl + 1);

	if (set)
	{
		vol[0] = vol[0] >= 128 ? vol[0] - 128 : vol[0] + 128;
		if (cs42448->omap35xx->cap_strm.voices == 2)
			vol[1] = vol[1] >= 128 ? vol[1] - 128 : vol[1] + 128;

		altered = vol[0] != data[0] || (cs42448->omap35xx->cap_strm.voices == 2 &&
			vol[1] != data[1]);

		cs42448_write (cs42448, ainx_vol_ctl + 0, vol[0], 0xff);
		if (cs42448->omap35xx->cap_strm.voices == 2)
			cs42448_write (cs42448, ainx_vol_ctl + 1, vol[1], 0xff);
	}
	else
	{
		vol[0] = data[0] >= 128 ? data[0] - 128 : data[0] + 128;
		if (cs42448->omap35xx->cap_strm.voices == 2)
			vol[1] = data[1] >= 128 ? data[1] - 128 : data[1] + 128;
	}
	return altered;
}

static  int32_t
cs42448_aoutx_mute_control (MIXER_CONTEXT_T * cs42448, ado_mixer_delement_t * element, uint8_t set,
	uint32_t * val, void *instance_data)
{
	int32_t altered = 0;
	uint32_t mute_mask = (uint32_t) instance_data;
	int     aoutx_mute = cs42448_read (cs42448, DAC_CH_MUTE) & mute_mask;
	uint32_t data;


	if (set)
	{

		if (cs42448->omap35xx->play_strm.voices == 4)
		{
			val[0] &=
				(SND_MIXER_CHN_MASK_FRONT_LEFT | SND_MIXER_CHN_MASK_FRONT_RIGHT |
				SND_MIXER_CHN_MASK_REAR_RIGHT | SND_MIXER_CHN_MASK_REAR_LEFT);
			/* Re-format val[0] to match the mute bitmap of the codec.
			 * (drop the center spk position and shift the rears over one bit) 
			 */
			data = val[0] & (SND_MIXER_CHN_MASK_FRONT_LEFT | SND_MIXER_CHN_MASK_FRONT_RIGHT);
			data |=
				((val[0] & (SND_MIXER_CHN_MASK_REAR_RIGHT | SND_MIXER_CHN_MASK_REAR_LEFT)) >> 1);
		}
		else
			data = val[0];

		altered = data != aoutx_mute;
		if (altered)
			cs42448_write (cs42448, DAC_CH_MUTE, data, mute_mask);
	}
	else
	{
		if (cs42448->omap35xx->play_strm.voices == 4)
		{
			/* Re-format codec mute bitmap to conform to audio's mute bitmap.
			 * (shift the rears over one bit to allow for center spk bit position) 
			 */
			data = ((aoutx_mute & (AOUT3_MUTE | AOUT4_MUTE)) << 1);
			data |= (aoutx_mute & (AOUT1_MUTE | AOUT2_MUTE));
		}
		else
			data = aoutx_mute;

		val[0] = data;
	}

	return altered;
}

static  int32_t
cs42448_aoutx_vol_control (MIXER_CONTEXT_T * cs42448, ado_mixer_delement_t * element, uint8_t set,
	uint32_t * vol, void *instance_data)
{
	int32_t altered = 0;
	uint8_t data[4] = { 0 };
	uint32_t aoutx_vol_ctl = (uint32_t) instance_data;


	data[0] = 255 - cs42448_read (cs42448, aoutx_vol_ctl + 0);
	if (cs42448->omap35xx->play_strm.voices > 1)
		data[1] = 255 - cs42448_read (cs42448, aoutx_vol_ctl + 1);
	if (cs42448->omap35xx->play_strm.voices > 2)
		data[2] = 255 - cs42448_read (cs42448, aoutx_vol_ctl + 2);
	if (cs42448->omap35xx->play_strm.voices > 3)
		data[3] = 255 - cs42448_read (cs42448, aoutx_vol_ctl + 3);

	switch (cs42448->omap35xx->play_strm.voices)
	{
	case 1:
		if (set)
		{
			altered = vol[0] != data[0];
			cs42448_write (cs42448, aoutx_vol_ctl + 0, 255 - vol[0], 0xff);
		}
		else
		{
			vol[0] = data[0];
		}
		break;
	case 2:
		if (set)
		{
			altered = vol[0] != data[0] || vol[1] != data[1];
			cs42448_write (cs42448, aoutx_vol_ctl + 0, 255 - vol[0], 0xff);
			cs42448_write (cs42448, aoutx_vol_ctl + 1, 255 - vol[1], 0xff);
		}
		else
		{
			vol[0] = data[0];
			vol[1] = data[1];
		}
		break;
	case 3:
		if (set)
		{
			altered = vol[0] != data[0] || vol[1] != data[1] || vol[2] != data[2];
			cs42448_write (cs42448, aoutx_vol_ctl + 0, 255 - vol[0], 0xff);
			cs42448_write (cs42448, aoutx_vol_ctl + 1, 255 - vol[1], 0xff);
			cs42448_write (cs42448, aoutx_vol_ctl + 2, 255 - vol[2], 0xff);
		}
		else
		{
			vol[0] = data[0];
			vol[1] = data[1];
			vol[2] = data[2];
		}
		break;
	case 4:
		/* vol[2] is for the center channel. In 4chn mode we skip the center.
		 * 0 = Left, 1 = Right, 3 = Rear Left, 4 = Rear Right
		 */
		if (set)
		{
			altered = vol[0] != data[0] || vol[1] != data[1] || vol[3] != data[2] ||
				vol[4] != data[3];
			cs42448_write (cs42448, aoutx_vol_ctl + 0, 255 - vol[0], 0xff);
			cs42448_write (cs42448, aoutx_vol_ctl + 1, 255 - vol[1], 0xff);
			cs42448_write (cs42448, aoutx_vol_ctl + 2, 255 - vol[3], 0xff);
			cs42448_write (cs42448, aoutx_vol_ctl + 3, 255 - vol[4], 0xff);
		}
		else
		{
			vol[0] = data[0];
			vol[1] = data[1];
			vol[3] = data[2];
			vol[4] = data[3];
		}
	default:
		break;
	}
	return altered;
}

static  int32_t
cs42448_mixer_build (MIXER_CONTEXT_T * cs42448, ado_mixer_t * mixer)
{
	int     val;
	uint32_t chn_mask = 0;
	snd_mixer_voice_t *voices;
	ado_mixer_delement_t *play_vol;
	ado_mixer_delement_t *play_mute;
	ado_mixer_delement_t *micin_vol;
	ado_mixer_delement_t *micin_mute;

	ado_mixer_dgroup_t *play_grp;
	ado_mixer_dgroup_t *micin_grp;

	int     error = 0;
	ado_mixer_delement_t *pre_elem = NULL, *elem = NULL;

	/*#################### */
	/*## Playback Group ## */
	/*#################### */
	if (cs42448->omap35xx->play_strm.voices)
	{
		voices = default_voices;
		val = AOUT1_MUTE;
		chn_mask = SND_MIXER_CHN_MASK_MONO;
		if (cs42448->omap35xx->play_strm.voices == 2)
		{
			val |= AOUT2_MUTE;
			chn_mask = SND_MIXER_CHN_MASK_STEREO;
		}
		else if (cs42448->omap35xx->play_strm.voices == 3)
		{
			val |= AOUT2_MUTE | AOUT3_MUTE;
			chn_mask = SND_MIXER_CHN_MASK_STEREO | SND_MIXER_CHN_MASK_FRONT_CENTER;
		}
		else if (cs42448->omap35xx->play_strm.voices == 4)
		{
			val |= AOUT2_MUTE | AOUT3_MUTE | AOUT4_MUTE;
			chn_mask = SND_MIXER_CHN_MASK_4;
			voices = quad_voices;
		}

		if (!error && (pre_elem = ado_mixer_element_pcm1 (mixer, SND_MIXER_ELEMENT_PLAYBACK,
					SND_MIXER_ETYPE_PLAYBACK1, 1, &pcm_devices[0])) == NULL)
			error++;

		if (!error && (elem = ado_mixer_element_volume1 (mixer, "PCM Volume",
					cs42448->omap35xx->play_strm.voices, output_range, cs42448_aoutx_vol_control,
					(void *) AOUT0_VOL_CTL, NULL)) == NULL)
			error++;

		if (!error && ado_mixer_element_route_add (mixer, pre_elem, elem) != 0)
			error++;

		play_vol = elem;
		pre_elem = elem;

		if (!error &&
			(elem =
				ado_mixer_element_sw1 (mixer, "PCM Mute", cs42448->omap35xx->play_strm.voices,
					cs42448_aoutx_mute_control, (void *) val, NULL)) == NULL)
			error++;

		if (!error && ado_mixer_element_route_add (mixer, pre_elem, elem) != 0)
			error++;

		play_mute = elem;
		pre_elem = elem;

		if (!error && (elem = ado_mixer_element_io (mixer, "PCM_OUT",
					SND_MIXER_ETYPE_OUTPUT, 0, cs42448->omap35xx->play_strm.voices, voices)) == NULL)
			error++;

		if (!error && ado_mixer_element_route_add (mixer, pre_elem, elem) != 0)
			error++;

		if (!error && (play_grp = ado_mixer_playback_group_create (mixer, SND_MIXER_PCM_OUT,
					chn_mask, play_vol, play_mute)) == NULL)
			error++;
	}

	/*################## */
	/*## Mic In Group ## */
	/*################## */
	if (cs42448->omap35xx->cap_strm.voices)
	{
		if (cs42448->omap35xx->cap_strm.voices == 1)
			chn_mask = SND_MIXER_CHN_MASK_MONO;
		else
			chn_mask = SND_MIXER_CHN_MASK_STEREO;

		if (!error && (pre_elem = ado_mixer_element_pcm1 (mixer, SND_MIXER_ELEMENT_CAPTURE,
					SND_MIXER_ETYPE_CAPTURE1, 1, &pcm_devices[0])) == NULL)
			error++;

		if (!error && (elem = ado_mixer_element_volume1 (mixer, "Mic Volume",
					cs42448->omap35xx->cap_strm.voices, input_range, cs42448_ainx_vol_control,
					(void *) AIN0_VOL_CTL, NULL)) == NULL)
			error++;

		if (!error && ado_mixer_element_route_add (mixer, pre_elem, elem) != 0)
			error++;

		micin_vol = elem;
		pre_elem = elem;

		if (!error &&
			(elem =
				ado_mixer_element_sw2 (mixer, "Mic Mute", cs42448_ainx_mute_control, NULL,
					NULL)) == NULL)
			error++;

		if (!error && ado_mixer_element_route_add (mixer, pre_elem, elem) != 0)
			error++;

		micin_mute = elem;
		pre_elem = elem;

		if (!error && (elem = ado_mixer_element_io (mixer, "MicIn",
					SND_MIXER_ETYPE_INPUT, 0, cs42448->omap35xx->cap_strm.voices,
					default_voices)) == NULL)
			error++;

		if (!error && ado_mixer_element_route_add (mixer, pre_elem, elem) != 0)
			error++;

		if (!error && (micin_grp = ado_mixer_capture_group_create (mixer, "Mic In",
					chn_mask, micin_vol, micin_mute, NULL, NULL)) == NULL)
			error++;
	}

	return (!error ? 0 : -1);
}


static ado_mixer_reset_t cs42448_reset;
static  int32_t
cs42448_reset (MIXER_CONTEXT_T * cs42448)
{
	int     cnt;
	uint8_t mute_mask = 0;

	/* Power down */
	cs42448_write (cs42448, PWR_CTL, PDN, PDN);			   /* Power down Codec */
	cs42448_write (cs42448, PWR_CTL, 0xFE, 0xFE);		   /* Power down all DACs and ADCs */

	/* Configure Function Mode (MCLK 12.288 Mhz, Slave mode) */
	cs42448_write (cs42448, FN_MODE, DAC_FM_SLAVE | ADC_FM_SLAVE, 0xFF);

	/* Configure Interface Formats */
#if defined (CODEC_FORMAT_TDM)
	cs42448_write (cs42448, IFACE_FMT, DAC_DIF_TDM | ADC_DIF_TDM, 0xFF);
#else
	cs42448_write (cs42448, IFACE_FMT, DAC_DIF_I2S | ADC_DIF_I2S, 0xFF);
#endif
	/* Configure ADC Control Register */
	cs42448_write (cs42448, ADC_CTL, ADC1_SINGLE | ADC2_SINGLE | ADC3_SINGLE, 0xFF);

	/* Configure Transition Control (Zero Cross & Soft Ramp ) */
	cs42448_write (cs42448, TRANS_CTL,
		DAC_ZCROSS | DAC_SRAMP | ADC_MUTE_SP | ADC_ZCROSS | ADC_SRAMP, 0xFF);

	/* Mute DACs */
	cs42448_write (cs42448, DAC_CH_MUTE, 0xFF, 0xFF);

	/* Initialize DAC Volume to -4.5dB (max volume w/o clipping) */
	for (cnt = 0; cnt < cs42448->omap35xx->play_strm.voices; cnt++)
		cs42448_write (cs42448, AOUT0_VOL_CTL + cnt, 0x09, 0xFF);

	/* Initialize ADC Volume to 0dB */
	for (cnt = 0; cnt < cs42448->omap35xx->cap_strm.voices; cnt++)
		cs42448_write (cs42448, AIN0_VOL_CTL + cnt, 0x00, 0xFF);

	/* Power up */
	cs42448_write (cs42448, PWR_CTL, 0x0, PDN_DAC1 | PDN_DAC2 | PDN_ADC1);	/* Power up DAC pairs 1 & 2, ADC pair 1 */
	cs42448_write (cs42448, PWR_CTL, 0x0, PDN);			   /* Power up Codec */

	delay (50);											   /* Wait ~2090 FSYNC cycles for codec power up */

	/* Clear mutes */
	mute_mask = AOUT1_MUTE;
	if (cs42448->omap35xx->play_strm.voices == 2)
		mute_mask |= AOUT2_MUTE;
	else if (cs42448->omap35xx->play_strm.voices == 3)
		mute_mask |= AOUT2_MUTE | AOUT3_MUTE;
	else if (cs42448->omap35xx->play_strm.voices == 4)
		mute_mask |= AOUT2_MUTE | AOUT3_MUTE | AOUT4_MUTE;

	cs42448_write (cs42448, DAC_CH_MUTE, 0x0, mute_mask);
	cs42448_write (cs42448, TRANS_CTL, 0x0, ADC_MUTE_SP);

	return 0;
}

static ado_mixer_destroy_t cs42448_destroy;
static  int32_t
cs42448_destroy (MIXER_CONTEXT_T * cs42448)
{
	ado_debug (DB_LVL_MIXER, "Destroying CS42448 Codec");

	/* Power down Amp */
	if (cs42448->omap35xx->en_power_amp)
		disable_power_amp (cs42448);

	cs42448_write (cs42448, DAC_CH_MUTE, 0xFF, 0xFF);	   /* Mute DACs */
	cs42448_write (cs42448, PWR_CTL, PDN, PDN);			   /* Power down Codec */
	cs42448_write (cs42448, PWR_CTL, 0xFE, 0xFE);		   /* Power down all DACs and ADCs */

	ado_free (cs42448);
	return 0;
}

int
codec_mixer (ado_card_t * card, HW_CONTEXT_T * omap35xx)
{
	cs42448_context_t *cs42448;
	int32_t status;

	ado_debug (DB_LVL_MIXER, "Initializing CS42448 Codec");

	if ((cs42448 = (cs42448_context_t *) ado_calloc (1, sizeof (cs42448_context_t))) == NULL)
	{
		ado_error ("cs42448: no memory (%s)", strerror (errno));
		return -1;
	}
	if ((status = ado_mixer_create (card, "cs42448", &omap35xx->mixer, cs42448)) != EOK)
	{
		ado_free (cs42448);
		return status;
	}
	cs42448->mixer = omap35xx->mixer;
	cs42448->omap35xx = omap35xx;
	cs42448->i2c_addr = omap35xx->codec_i2c_addr;

	if ((cs42448->fd = open ("/dev/i2c0", O_RDWR)) < 0)
	{
		ado_error ("cs42448: could not open i2c device %s", strerror (errno));
		ado_free (cs42448);
		return (-1);
	}

	if (cs42448_mixer_build (cs42448, cs42448->mixer))
	{
		close (cs42448->fd);
		ado_free (cs42448);
		return -1;
	}

	if (cs42448_reset (cs42448))
	{
		close (cs42448->fd);
		ado_free (cs42448);
		return -1;
	}

	ado_mixer_set_reset_func (cs42448->mixer, cs42448_reset);
	ado_mixer_set_destroy_func (cs42448->mixer, cs42448_destroy);

	/* Enable TDA8595 Power Amp */
	if (cs42448->omap35xx->en_power_amp)
		enable_power_amp (cs42448);

	if (omap35xx->play_strm.voices)
	{
		ado_pcm_chn_mixer (omap35xx->pcm1, ADO_PCM_CHANNEL_PLAYBACK, omap35xx->mixer,
			ado_mixer_find_element (omap35xx->mixer, SND_MIXER_ETYPE_PLAYBACK1,
				SND_MIXER_ELEMENT_PLAYBACK, 0), ado_mixer_find_group (omap35xx->mixer,
				SND_MIXER_PCM_OUT, 0));
	}

	if (omap35xx->cap_strm.voices)
	{
		ado_pcm_chn_mixer (omap35xx->pcm1, ADO_PCM_CHANNEL_CAPTURE, omap35xx->mixer,
			ado_mixer_find_element (omap35xx->mixer, SND_MIXER_ETYPE_CAPTURE1,
				SND_MIXER_ELEMENT_CAPTURE, 0), ado_mixer_find_group (omap35xx->mixer, "Mic In", 0));
	}

	if (omap35xx->play_strm.voices)
	{
		if (ado_pcm_sw_mix (card, omap35xx->pcm1, omap35xx->mixer))
		{
			close (cs42448->fd);
			ado_free (cs42448);
			return (-1);
		}
	}
	return 0;
}
