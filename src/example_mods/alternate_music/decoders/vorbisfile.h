// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderVorbisfile DecoderVorbisfile;

DecoderVorbisfile* Decoder_Vorbisfile_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, LinkedBackend *linked_backend);
void Decoder_Vorbisfile_Close(DecoderVorbisfile *this);
void Decoder_Vorbisfile_Rewind(DecoderVorbisfile *this);
unsigned long Decoder_Vorbisfile_GetSamples(DecoderVorbisfile *this, void *buffer, unsigned long bytes_to_do);

static const DecoderBackend DecoderBackend_Vorbisfile = {
	(void*)Decoder_Vorbisfile_Open, (void*)Decoder_Vorbisfile_Close, (void*)Decoder_Vorbisfile_Rewind, (void*)Decoder_Vorbisfile_GetSamples
};
