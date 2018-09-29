// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>
#include <stddef.h>

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

struct LinkedBackend;

typedef struct DecoderBackend
{
	void* (*Open)(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, struct LinkedBackend *linked_backend);
	void (*Close)(void *this);
	void (*Rewind)(void *this);
	unsigned long (*GetSamples)(void *this, void *output_buffer, unsigned long bytes_to_do);
} DecoderBackend;

typedef struct LinkedBackend
{
	struct LinkedBackend *next;
	const DecoderBackend *backend;
} LinkedBackend;
