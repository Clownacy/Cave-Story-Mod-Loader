#pragma once

#include <stdbool.h>

#include "decoder_common.h"

typedef struct DecoderOpenMPT DecoderOpenMPT;

DecoderOpenMPT* Decoder_OpenMPT_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, DecoderBackend *backend);
void Decoder_OpenMPT_Close(DecoderOpenMPT *this);
void Decoder_OpenMPT_Rewind(DecoderOpenMPT *this);
unsigned long Decoder_OpenMPT_GetSamples(DecoderOpenMPT *this, void *buffer, unsigned long bytes_to_do);
