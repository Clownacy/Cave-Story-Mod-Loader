#pragma once

#include <stdbool.h>

#include "decoder_common.h"

typedef struct DecoderSplit DecoderSplit;

DecoderSplit* Decoder_Split_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, LinkedBackend *linked_backend);
void Decoder_Split_Close(DecoderSplit *this);
void Decoder_Split_Rewind(DecoderSplit *this);
unsigned long Decoder_Split_GetSamples(DecoderSplit *this, void *output_buffer, unsigned long samples_to_do);

static const DecoderBackend DecoderBackend_Split = {(void*)Decoder_Split_Open, (void*)Decoder_Split_Close, (void*)Decoder_Split_Rewind, (void*)Decoder_Split_GetSamples};
