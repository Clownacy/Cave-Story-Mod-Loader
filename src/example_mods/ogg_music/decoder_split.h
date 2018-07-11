#pragma once

#include "decoder_common.h"

typedef struct DecoderSplit DecoderSplit;

DecoderSplit* Decoder_Split_Open(const char *file_path, DecoderInfo *info, DecoderBackend *backend);
void Decoder_Split_Close(DecoderSplit *this);
void Decoder_Split_Rewind(DecoderSplit *this);
void Decoder_Split_Loop(DecoderSplit *this);
unsigned long Decoder_Split_GetSamples(DecoderSplit *this, void *output_buffer, unsigned long samples_to_do);
