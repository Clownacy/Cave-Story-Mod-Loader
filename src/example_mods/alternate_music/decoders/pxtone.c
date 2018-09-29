// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "pxtone.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pxtone/shim.h"

#include "common.h"

typedef struct DecoderPxtone
{
	pxtnService *pxtn;
	bool loop;
} DecoderPxtone;

DecoderPxtone* Decoder_Pxtone_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, LinkedBackend *linked_backend)
{
	(void)linked_backend;
	(void)format;

	DecoderPxtone *decoder = NULL;

	pxtnService *pxtn = Pxtone_Open(file_path, loop);
	if (pxtn)
	{
		decoder = (DecoderPxtone*)malloc(sizeof(DecoderPxtone));
		decoder->pxtn = pxtn;
		decoder->loop = loop;

		info->sample_rate = 44100;
		info->channel_count = 2;
		info->format = DECODER_FORMAT_S16;
	}

	return decoder;
}

void Decoder_Pxtone_Close(DecoderPxtone *decoder)
{
	Pxtone_Close(decoder->pxtn);
	free(decoder);
}

void Decoder_Pxtone_Rewind(DecoderPxtone *decoder)
{
	Pxtone_Rewind(decoder->pxtn, decoder->loop);
}

unsigned long Decoder_Pxtone_GetSamples(DecoderPxtone *decoder, void *buffer, unsigned long bytes_to_do)
{
	memset(buffer, 0, bytes_to_do);
	Pxtone_GetSamples(decoder->pxtn, buffer, bytes_to_do);

	return bytes_to_do;
}
