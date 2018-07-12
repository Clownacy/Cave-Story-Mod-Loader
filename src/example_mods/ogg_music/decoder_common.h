#pragma once

#include <stdbool.h>

#define DECODER_GET_FORMAT_SIZE(a) (a == DECODER_FORMAT_F32 ? sizeof(float) : sizeof(short))

typedef enum
{
	DECODER_FORMAT_S16,
	DECODER_FORMAT_F32
} DecoderFormat;

typedef struct
{
	unsigned int sample_rate;
	unsigned int channel_count;
	unsigned long decoded_size;
	DecoderFormat format;
} DecoderInfo;

typedef struct DecoderBackend
{
	struct DecoderBackend *backend;

	void* (*Open)(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, struct DecoderBackend *backend);
	void (*Close)(void *this);
	void (*Rewind)(void *this);
	long (*GetSamples)(void *this, void *output_buffer, unsigned long bytes_to_do);
} DecoderBackend;
