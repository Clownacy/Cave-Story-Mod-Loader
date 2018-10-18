// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_Sndfile DecoderData_Sndfile;
typedef struct Decoder_Sndfile Decoder_Sndfile;

DecoderData_Sndfile* Decoder_Sndfile_LoadData(const char *file_path, bool loops, LinkedBackend *linked_backend);
void Decoder_Sndfile_UnloadData(DecoderData_Sndfile *data);
Decoder_Sndfile* Decoder_Sndfile_Create(DecoderData_Sndfile *data, DecoderInfo *info);
void Decoder_Sndfile_Destroy(Decoder_Sndfile *this);
void Decoder_Sndfile_Rewind(Decoder_Sndfile *this);
unsigned long Decoder_Sndfile_GetSamples(Decoder_Sndfile *this, void *buffer, unsigned long bytes_to_do);
