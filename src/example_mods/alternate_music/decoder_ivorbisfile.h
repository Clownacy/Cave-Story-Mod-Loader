// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "decoder_common.h"

typedef struct DecoderIVorbisfile DecoderIVorbisfile;

DecoderIVorbisfile* Decoder_IVorbisfile_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, LinkedBackend *linked_backend);
void Decoder_IVorbisfile_Close(DecoderIVorbisfile *this);
void Decoder_IVorbisfile_Rewind(DecoderIVorbisfile *this);
unsigned long Decoder_IVorbisfile_GetSamples(DecoderIVorbisfile *this, void *buffer, unsigned long bytes_to_do);

static const DecoderBackend DecoderBackend_IVorbisfile = {
	(void*)Decoder_IVorbisfile_Open, (void*)Decoder_IVorbisfile_Close, (void*)Decoder_IVorbisfile_Rewind, (void*)Decoder_IVorbisfile_GetSamples
};
