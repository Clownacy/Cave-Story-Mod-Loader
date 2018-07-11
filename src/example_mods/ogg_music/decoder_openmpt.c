#include "decoder_openmpt.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <libopenmpt/libopenmpt.h>

#include "decoder_common.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

typedef struct DecoderOpenMPT
{
	openmpt_module *module;
} DecoderOpenMPT;

DecoderOpenMPT* Decoder_OpenMPT_Open(const char *file_path, DecoderInfo *info, DecoderBackend *backend)
{
	(void)backend;

	DecoderOpenMPT *this = malloc(sizeof(DecoderOpenMPT));

	FILE *file = fopen(file_path, "rb");
	fseek(file, 0, SEEK_END);
	const long file_size = ftell(file);
	rewind(file);

	unsigned char file_buffer[file_size];
	fread(file_buffer, 1, file_size, file);
	fclose(file);

	this->module = openmpt_module_create_from_memory2(file_buffer, file_size, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	info->sample_rate = SAMPLE_RATE;
	info->channel_count = CHANNEL_COUNT;
	info->decoded_size = openmpt_module_get_duration_seconds(this->module) * SAMPLE_RATE * CHANNEL_COUNT * sizeof(short);

	openmpt_module_set_repeat_count(this->module, -1);

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

void Decoder_OpenMPT_Loop(DecoderOpenMPT *this)
{
	Decoder_OpenMPT_Rewind(this);
}

unsigned long Decoder_OpenMPT_GetSamples(DecoderOpenMPT *this, void *buffer, unsigned long bytes_to_do)
{
	const unsigned int bytes_per_frame = CHANNEL_COUNT * sizeof(short);

	unsigned long bytes_done_total = 0;

	for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
	{
		bytes_done = openmpt_module_read_interleaved_stereo(this->module, SAMPLE_RATE, (bytes_to_do - bytes_done_total) / bytes_per_frame, buffer + bytes_done_total) * bytes_per_frame;

		if (bytes_done == 0)
			break;
	}

	return bytes_done_total;
}
