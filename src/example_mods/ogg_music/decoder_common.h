#pragma once

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
} DecoderInfo;

typedef struct DecoderBackend
{
	struct DecoderBackend *backend;

	void* (*Open)(const char *file_path, DecoderInfo *info, struct DecoderBackend *backend);
	void (*Close)(void *this);
	void (*Rewind)(void *this);
	void (*Loop)(void *this);
	long (*GetSamples)(void *this, void *output_buffer, unsigned long bytes_to_do);
} DecoderBackend;
