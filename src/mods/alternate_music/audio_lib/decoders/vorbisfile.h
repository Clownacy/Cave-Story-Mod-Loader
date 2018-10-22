// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_Vorbisfile DecoderData_Vorbisfile;
typedef struct Decoder_Vorbisfile Decoder_Vorbisfile;

DecoderData_Vorbisfile* Decoder_Vorbisfile_LoadData(const char *file_path, bool loops, LinkedBackend *linked_backend);
void Decoder_Vorbisfile_UnloadData(DecoderData_Vorbisfile *data);
Decoder_Vorbisfile* Decoder_Vorbisfile_Create(DecoderData_Vorbisfile *data, DecoderInfo *info);
void Decoder_Vorbisfile_Destroy(Decoder_Vorbisfile *this);
void Decoder_Vorbisfile_Rewind(Decoder_Vorbisfile *this);
unsigned long Decoder_Vorbisfile_GetSamples(Decoder_Vorbisfile *this, void *buffer_void, unsigned long frames_to_do);
