#pragma once

#include <stdbool.h>

#include "decoder_common.h"

typedef struct DecoderPredecode DecoderPredecode;

DecoderPredecode* Decoder_Predecode_Open(const char *file_path, bool loop, DecoderInfo *info, DecoderBackend *backend);
void Decoder_Predecode_Close(DecoderPredecode *this);
void Decoder_Predecode_Rewind(DecoderPredecode *this);
unsigned long Decoder_Predecode_GetSamples(DecoderPredecode *this, void *output_buffer, unsigned long bytes_to_do);
