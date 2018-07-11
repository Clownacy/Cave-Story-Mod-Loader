#pragma once

#include <stdbool.h>

#include "decoder_common.h"

typedef enum
{
	DECODER_TYPE_OGG,
	DECODER_TYPE_FLAC
} DecoderType;

typedef struct Decoder Decoder;

Decoder* Decoder_Open(const char* file_path, DecoderType type, bool predecode, DecoderInfo *info);
void Decoder_Close(Decoder *this);
void Decoder_Rewind(Decoder *this);
void Decoder_Loop(Decoder *this);
unsigned long Decoder_GetSamples(Decoder *this, void *output_buffer, unsigned long bytes_to_do);
