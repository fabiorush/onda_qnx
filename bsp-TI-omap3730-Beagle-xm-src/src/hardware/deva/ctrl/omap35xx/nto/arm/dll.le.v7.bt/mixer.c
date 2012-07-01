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

struct omap35xx_card;

#define MIXER_CONTEXT_T struct omap35xx_card
#include <omap35xx.h>

static int32_t pcm_devices[1] = {
	0
};

static snd_mixer_voice_t mono_voice[1] = {
	{SND_MIXER_VOICE_MONO, 0}
};

static int
build_bt_mixer (MIXER_CONTEXT_T * bt, ado_mixer_t * mixer)
{
	int     error = 0;
	ado_mixer_delement_t *pre_elem, *elem = NULL;


	/* ################ */
	/* the OUTPUT GROUP */
	/* ################ */
	if (bt->play_strm.voices)
	{
		if (!error && (elem = ado_mixer_element_pcm1 (bt->mixer, SND_MIXER_ELEMENT_PLAYBACK,
					SND_MIXER_ETYPE_PLAYBACK1, 1, &pcm_devices[0])) == NULL)
			error++;
		pre_elem = elem;

		if (!error &&
			(elem =
				ado_mixer_element_io (mixer, "Bluetooth PCM Out", SND_MIXER_ETYPE_OUTPUT, 0, 1,
					mono_voice)) == NULL)
			error++;
		if (!error && ado_mixer_element_route_add (mixer, pre_elem, elem) != 0)
			error++;

		if (!error && ado_mixer_playback_group_create (mixer, "Bluetooth PCM Out",
				SND_MIXER_CHN_MASK_MONO, NULL, NULL) == NULL)
			error++;
	}

	/* ############### */
	/* the INPUT GROUP */
	/* ############### */
	if (bt->cap_strm.voices)
	{
		if (!error && (elem = ado_mixer_element_io (bt->mixer, "Bluetooth PCM In",
					SND_MIXER_ETYPE_INPUT, 0, 1, mono_voice)) == NULL)
			error++;
		pre_elem = elem;

		if (!error && (elem = ado_mixer_element_pcm1 (mixer, SND_MIXER_ELEMENT_CAPTURE,
					SND_MIXER_ETYPE_CAPTURE1, 1, &pcm_devices[0])) == NULL)
			error++;
		if (!error && ado_mixer_element_route_add (mixer, pre_elem, elem) != 0)
			error++;

		if (!error && ado_mixer_capture_group_create (mixer, "Bluetooth PCM In",
				SND_MIXER_CHN_MASK_MONO, NULL, NULL, NULL, NULL) == NULL)
			error++;
	}
	return (0);
}

static ado_mixer_reset_t reset_bt_mixer;
static  int32_t
reset_bt_mixer (MIXER_CONTEXT_T * bt)
{
	return (0);
}

int
codec_mixer (ado_card_t * card, HW_CONTEXT_T * omap35xx)
{
	int32_t status;

	ado_debug (DB_LVL_MIXER, "Initializing Bluetooth Mixer");

	if ((status = ado_mixer_create (card, "BT", &omap35xx->mixer, omap35xx)) != EOK)
		return (status);

	if (build_bt_mixer (omap35xx, omap35xx->mixer))
		return (-1);

	if (reset_bt_mixer (omap35xx))
		return (-1);

	ado_mixer_set_reset_func (omap35xx->mixer, reset_bt_mixer);

	if (omap35xx->play_strm.voices)
	{
		ado_pcm_chn_mixer (omap35xx->pcm1, ADO_PCM_CHANNEL_PLAYBACK, omap35xx->mixer,
			ado_mixer_find_element (omap35xx->mixer, SND_MIXER_ETYPE_PLAYBACK1, SND_MIXER_PCM_OUT, 0),
			ado_mixer_find_group (omap35xx->mixer, "Bluetooth PCM Out", 0));
	}

	if (omap35xx->cap_strm.voices)
	{
		ado_pcm_chn_mixer (omap35xx->pcm1, ADO_PCM_CHANNEL_CAPTURE, omap35xx->mixer,
			ado_mixer_find_element (omap35xx->mixer, SND_MIXER_ETYPE_CAPTURE1,
				SND_MIXER_ELEMENT_CAPTURE, 0), ado_mixer_find_group (omap35xx->mixer,
				"Bluetooth PCM In", 0));
	}
	return (0);
}

__SRCVERSION( "$URL: http://svn.ott.qnx.com/product/trunk/hardware/deva/ctrl/omap35xx/nto/arm/dll.le.bt/mixer.c $ $Rev: 236582 $" );
