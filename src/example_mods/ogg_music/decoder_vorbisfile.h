#pragma once

#include <stdbool.h>

#include "decoder_common.h"

typedef struct DecoderVorbisfile DecoderVorbisfile;

DecoderVorbisfile* Decoder_Vorbisfile_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, DecoderBackend *backend);
void Decoder_Vorbisfile_Close(DecoderVorbisfile *this);
void Decoder_Vorbisfile_Rewind(DecoderVorbisfile *this);
unsigned long Decoder_Vorbisfile_GetSamples(DecoderVorbisfile *this, void *buffer, unsigned long bytes_to_do);
