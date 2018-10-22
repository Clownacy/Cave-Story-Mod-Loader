// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "openmpt.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <libopenmpt/libopenmpt.h>

#include "common.h"
#include "memory_file.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

typedef struct DecoderData_OpenMPT
{
	unsigned char *file_buffer;
	size_t file_size;
	bool loops;
} DecoderData_OpenMPT;

typedef struct Decoder_OpenMPT
{
	openmpt_module *module;
} Decoder_OpenMPT;

DecoderData_OpenMPT* Decoder_OpenMPT_LoadData(const char *file_path, bool loops, LinkedBackend *linked_backend)
{
	(void)linked_backend;

	DecoderData_OpenMPT *data = NULL;

	size_t file_size;
	unsigned char *file_buffer = MemoryFile_fopen_to(file_path, &file_size);

	if (file_buffer)
	{
		data = malloc(sizeof(DecoderData_OpenMPT));
		data->file_buffer = file_buffer;
		data->file_size = file_size;
		data->loops = loops;
	}

	return data;
}

void Decoder_OpenMPT_UnloadData(DecoderData_OpenMPT *data)
{
	if (data)
	{
		free(data->file_buffer);
		free(data);
	}
}

Decoder_OpenMPT* Decoder_OpenMPT_Create(DecoderData_OpenMPT *data, DecoderInfo *info)
{
	Decoder_OpenMPT *decoder = NULL;

	if (data)
	{
		openmpt_module *module = openmpt_module_create_from_memory2(data->file_buffer, data->file_size, openmpt_log_func_silent, NULL, openmpt_error_func_ignore, NULL, NULL, NULL, NULL);

		if (module)
		{
			decoder = malloc(sizeof(Decoder_OpenMPT));

			decoder->module = module;

			info->sample_rate = SAMPLE_RATE;
			info->channel_count = CHANNEL_COUNT;
			info->format = DECODER_FORMAT_F32;

			if (data->loops)
				openmpt_module_set_repeat_count(decoder->module, -1);
		}
	}

	return decoder;
}

void Decoder_OpenMPT_Destroy(Decoder_OpenMPT *decoder)
{
	if (decoder)
	{
		openmpt_module_destroy(decoder->module);
		free(decoder);
	}
}

void Decoder_OpenMPT_Rewind(Decoder_OpenMPT *decoder)
{
	openmpt_module_set_position_seconds(decoder->module, 0);
}

unsigned long Decoder_OpenMPT_GetSamples(Decoder_OpenMPT *decoder, void *buffer_void, unsigned long frames_to_do)
{
	float *buffer = buffer_void;

	unsigned long frames_done_total = 0;

	for (unsigned long frames_done; frames_done_total != frames_to_do; frames_done_total += frames_done)
	{
		frames_done = openmpt_module_read_interleaved_float_stereo(decoder->module, SAMPLE_RATE, frames_to_do - frames_done_total, buffer + (frames_done_total * CHANNEL_COUNT));

		if (frames_done == 0)
			break;
	}

	return frames_done_total;
}
