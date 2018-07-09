#pragma once

#include <stdbool.h>

#include "decoder_common.h"

typedef struct DualDecoder DualDecoder;

DualDecoder* DualDecoder_Open(const char* const path, DecoderType type, bool loops, bool predecode, DecoderInfo *info);
void DualDecoder_Close(DualDecoder *this);
void DualDecoder_Reset(DualDecoder *this);
void DualDecoder_Rewind(DualDecoder *this);
unsigned long DualDecoder_GetSamples(DualDecoder *this, void *output_buffer, unsigned long samples_to_do);
