// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "openmpt.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <libopenmpt/libopenmpt.h>

#include "common.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

typedef struct DecoderOpenMPT
{
	openmpt_module *module;
	unsigned int bytes_per_frame;
	DecoderFormat format;
} DecoderOpenMPT;

DecoderOpenMPT* Decoder_OpenMPT_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, LinkedBackend *linked_backend)
{
	(void)linked_backend;

	DecoderOpenMPT *this = NULL;

	FILE *file = fopen(file_path, "rb");

	if (file)
	{
		fseek(file, 0, SEEK_END);
		const long file_size = ftell(file);
		rewind(file);

		unsigned char file_buffer[file_size];
		fread(file_buffer, 1, file_size, file);
		fclose(file);

		openmpt_module *module = openmpt_module_create_from_memory2(file_buffer, file_size, openmpt_log_func_silent, NULL, openmpt_error_func_ignore, NULL, NULL, NULL, NULL);

		if (module)
		{
			this = malloc(sizeof(DecoderOpenMPT));

			this->module = module;
			this->format = format;

			this->bytes_per_frame = CHANNEL_COUNT * DECODER_GET_FORMAT_SIZE(format);

			info->sample_rate = SAMPLE_RATE;
			info->channel_count = CHANNEL_COUNT;
			info->format = format;

			if (loop)
				openmpt_module_set_repeat_count(this->module, -1);
		}
	}

	return this;
}

void Decoder_OpenMPT_Close(DecoderOpenMPT *this)
{
	openmpt_module_destroy(this->module);
	free(this);
}

void Decoder_OpenMPT_Rewind(DecoderOpenMPT *this)
{
	openmpt_module_set_position_seconds(this->module, 0);
}

unsigned long Decoder_OpenMPT_GetSamples(DecoderOpenMPT *this, void *buffer, unsigned long bytes_to_do)
{
	unsigned long bytes_done_total = 0;

	if (this->format == DECODER_FORMAT_F32)
	{
		for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
		{
			bytes_done = openmpt_module_read_interleaved_float_stereo(this->module, SAMPLE_RATE, (bytes_to_do - bytes_done_total) / this->bytes_per_frame, buffer + bytes_done_total) * this->bytes_per_frame;

			if (bytes_done == 0)
				break;
		}
	}
	else if (this->format == DECODER_FORMAT_S16)
	{
		for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
		{
			bytes_done = openmpt_module_read_interleaved_stereo(this->module, SAMPLE_RATE, (bytes_to_do - bytes_done_total) / (CHANNEL_COUNT * sizeof(short)), buffer + bytes_done_total) * CHANNEL_COUNT * sizeof(short);

			if (bytes_done == 0)
				break;
		}
	}

	return bytes_done_total;
}
