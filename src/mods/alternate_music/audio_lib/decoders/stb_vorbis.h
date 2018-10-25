// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_STBVorbis DecoderData_STBVorbis;
typedef struct Decoder_STBVorbis Decoder_STBVorbis;

DecoderData_STBVorbis* Decoder_STBVorbis_LoadData(const char *file_path, bool loops, LinkedBackend *linked_backend);
void Decoder_STBVorbis_UnloadData(DecoderData_STBVorbis *data);
Decoder_STBVorbis* Decoder_STBVorbis_Create(DecoderData_STBVorbis *data, DecoderInfo *info);
void Decoder_STBVorbis_Destroy(Decoder_STBVorbis *this);
void Decoder_STBVorbis_Rewind(Decoder_STBVorbis *this);
unsigned long Decoder_STBVorbis_GetSamples(Decoder_STBVorbis *this, void *buffer_void, unsigned long frames_to_do);
