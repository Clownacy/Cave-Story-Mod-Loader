#pragma once

typedef enum
{
	DECODER_TYPE_OGG,
	DECODER_TYPE_FLAC
} DecoderType;

typedef enum
{
	DECODER_FORMAT_S16,
	DECODER_FORMAT_F32
} DecoderFormat;

typedef struct DecoderInfo
{
	unsigned int sample_rate;
	unsigned int channel_count;
	unsigned long decoded_size;
} DecoderInfo;
