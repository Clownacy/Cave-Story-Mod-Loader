#pragma once

#include "decoder_common.h"

typedef struct DecoderVorbisfile DecoderVorbisfile;

DecoderVorbisfile* Decoder_Vorbisfile_Open(const char *file_path, DecoderInfo *info, DecoderBackend *backend);
void Decoder_Vorbisfile_Close(DecoderVorbisfile *this);
void Decoder_Vorbisfile_Rewind(DecoderVorbisfile *this);
void Decoder_Vorbisfile_Loop(DecoderVorbisfile *this);
unsigned long Decoder_Vorbisfile_GetSamples(DecoderVorbisfile *this, void *buffer, unsigned long bytes_to_do);
