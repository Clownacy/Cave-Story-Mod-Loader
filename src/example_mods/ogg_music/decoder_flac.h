#pragma once

#include <stdbool.h>

#include "decoder_common.h"

typedef struct DecoderFLAC DecoderFLAC;

DecoderFLAC* Decoder_FLAC_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, DecoderBackend *backend);
void Decoder_FLAC_Close(DecoderFLAC *this);
void Decoder_FLAC_Rewind(DecoderFLAC *this);
unsigned long Decoder_FLAC_GetSamples(DecoderFLAC *this, void *buffer, unsigned long bytes_to_do);
