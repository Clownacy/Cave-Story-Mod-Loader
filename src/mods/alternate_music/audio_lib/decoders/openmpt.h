// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_OpenMPT DecoderData_OpenMPT;
typedef struct Decoder_OpenMPT Decoder_OpenMPT;

DecoderData_OpenMPT* Decoder_OpenMPT_LoadData(const char *file_path, bool loops, LinkedBackend *linked_backend);
void Decoder_OpenMPT_UnloadData(DecoderData_OpenMPT *data);
Decoder_OpenMPT* Decoder_OpenMPT_Create(DecoderData_OpenMPT *data, DecoderInfo *info);
void Decoder_OpenMPT_Destroy(Decoder_OpenMPT *this);
void Decoder_OpenMPT_Rewind(Decoder_OpenMPT *this);
unsigned long Decoder_OpenMPT_GetSamples(Decoder_OpenMPT *this, void *buffer, unsigned long bytes_to_do);
