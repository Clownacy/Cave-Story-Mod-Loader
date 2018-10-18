// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_FLAC DecoderData_FLAC;
typedef struct Decoder_FLAC Decoder_FLAC;

DecoderData_FLAC* Decoder_FLAC_LoadData(const char *file_path, bool loops, LinkedBackend *linked_backend);
void Decoder_FLAC_UnloadData(DecoderData_FLAC *data);
Decoder_FLAC* Decoder_FLAC_Create(DecoderData_FLAC *data, DecoderInfo *info);
void Decoder_FLAC_Destroy(Decoder_FLAC *this);
void Decoder_FLAC_Rewind(Decoder_FLAC *this);
unsigned long Decoder_FLAC_GetSamples(Decoder_FLAC *this, void *buffer, unsigned long bytes_to_do);
