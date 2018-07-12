#pragma once

#include <stdbool.h>

#include "decoder_common.h"

typedef struct DecoderSndfile DecoderSndfile;

DecoderSndfile* Decoder_Sndfile_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, DecoderBackend *backend);
void Decoder_Sndfile_Close(DecoderSndfile *this);
void Decoder_Sndfile_Rewind(DecoderSndfile *this);
unsigned long Decoder_Sndfile_GetSamples(DecoderSndfile *this, void *buffer, unsigned long bytes_to_do);
