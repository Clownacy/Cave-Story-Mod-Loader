// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "decoder_spc.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "snes_spc/spc.h"

#include "decoder_common.h"

typedef struct DecoderSPC
{
	SNES_SPC *snes_spc;
//	SPC_Filter *filter;
	void *file_buffer;
	long file_size;
} DecoderSPC;

DecoderSPC* Decoder_SPC_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, LinkedBackend *linked_backend)
{
	(void)linked_backend;
	(void)loop;	// Unusable, sadly
	(void)format;

	DecoderSPC *this = NULL;

	SNES_SPC *snes_spc = spc_new();

	//SPC_Filter *filter = spc_filter_new();

	FILE *file = fopen(file_path, "rb");

	if (file)
	{
		fseek(file, 0, SEEK_END);
		const long file_size = ftell(file);
		rewind(file);
		void *file_buffer = malloc(file_size);
		fread(file_buffer, 1, file_size, file);
		fclose(file);

		if (!spc_load_spc(snes_spc, file_buffer, file_size))
		{
			spc_clear_echo(snes_spc);

		//	spc_filter_clear(filter);

			this = malloc(sizeof(DecoderSPC));
			this->snes_spc = snes_spc;
		//	this->filter = filter;
			this->file_buffer = file_buffer;
			this->file_size = file_size;

			info->sample_rate = spc_sample_rate;
			info->channel_count = 2;
			info->format = DECODER_FORMAT_S16;
		}
	}

	return this;
}

void Decoder_SPC_Close(DecoderSPC *this)
{
	spc_delete(this->snes_spc);
	free(this->file_buffer);
	free(this);
}

void Decoder_SPC_Rewind(DecoderSPC *this)
{
	spc_delete(this->snes_spc);
	spc_load_spc(this->snes_spc, this->file_buffer, this->file_size);
}

unsigned long Decoder_SPC_GetSamples(DecoderSPC *this, void *buffer, unsigned long bytes_to_do)
{
	spc_play(this->snes_spc, bytes_to_do / sizeof(short), buffer);

//	spc_filter_run(this->filter, buffer, bytes_to_do / sizeof(short));

	return bytes_to_do;
}
