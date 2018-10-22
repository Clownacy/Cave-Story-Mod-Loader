// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_XMPLite DecoderData_XMPLite;
typedef struct Decoder_XMPLite Decoder_XMPLite;

DecoderData_XMPLite* Decoder_XMPLite_LoadData(const char *file_path, bool loops, LinkedBackend *linked_backend);
void Decoder_XMPLite_UnloadData(DecoderData_XMPLite *data);
Decoder_XMPLite* Decoder_XMPLite_Create(DecoderData_XMPLite *data, DecoderInfo *info);
void Decoder_XMPLite_Destroy(Decoder_XMPLite *this);
void Decoder_XMPLite_Rewind(Decoder_XMPLite *this);
unsigned long Decoder_XMPLite_GetSamples(Decoder_XMPLite *this, void *buffer, unsigned long bytes_to_do);
