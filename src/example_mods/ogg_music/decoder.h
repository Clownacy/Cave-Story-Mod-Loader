#pragma once

#include <stdbool.h>

typedef enum
{
	DECODER_OGG,
	DECODER_FLAC
} DecoderType;

typedef struct Decoder Decoder;

Decoder* Decoder_Open(const char* const file_path, DecoderType type, bool predecode);
void Decoder_Close(Decoder *this);
void Decoder_Rewind(Decoder *this);
unsigned long Decoder_GetSamples(Decoder *this, void *output_buffer, unsigned long bytes_to_do);
unsigned int Decoder_GetChannelCount(Decoder *decoder);
unsigned int Decoder_GetSampleRate(Decoder *decoder);
