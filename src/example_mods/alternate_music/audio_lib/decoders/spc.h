// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_SPC DecoderData_SPC;
typedef struct Decoder_SPC Decoder_SPC;

DecoderData_SPC* Decoder_SPC_LoadData(const char *file_path, bool loops, LinkedBackend *linked_backend);
void Decoder_SPC_UnloadData(DecoderData_SPC *data);
Decoder_SPC* Decoder_SPC_Create(DecoderData_SPC *data, DecoderInfo *info);
void Decoder_SPC_Destroy(Decoder_SPC *this);
void Decoder_SPC_Rewind(Decoder_SPC *this);
unsigned long Decoder_SPC_GetSamples(Decoder_SPC *this, void *buffer, unsigned long bytes_to_do);
