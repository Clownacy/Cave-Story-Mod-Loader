// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_IVorbisfile DecoderData_IVorbisfile;
typedef struct Decoder_IVorbisfile Decoder_IVorbisfile;

DecoderData_IVorbisfile* Decoder_IVorbisfile_LoadData(const char *file_path, bool loops, LinkedBackend *linked_backend);
void Decoder_IVorbisfile_UnloadData(DecoderData_IVorbisfile *data);
Decoder_IVorbisfile* Decoder_IVorbisfile_Create(DecoderData_IVorbisfile *data, DecoderInfo *info);
void Decoder_IVorbisfile_Destroy(Decoder_IVorbisfile *this);
void Decoder_IVorbisfile_Rewind(Decoder_IVorbisfile *this);
unsigned long Decoder_IVorbisfile_GetSamples(Decoder_IVorbisfile *this, void *buffer, unsigned long bytes_to_do);
