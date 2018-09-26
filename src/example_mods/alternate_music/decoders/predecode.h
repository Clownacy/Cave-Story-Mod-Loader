// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderPredecode DecoderPredecode;

DecoderPredecode* Decoder_Predecode_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, LinkedBackend *linked_backend);
void Decoder_Predecode_Close(DecoderPredecode *this);
void Decoder_Predecode_Rewind(DecoderPredecode *this);
unsigned long Decoder_Predecode_GetSamples(DecoderPredecode *this, void *output_buffer, unsigned long bytes_to_do);

static const DecoderBackend DecoderBackend_Predecode = {(void*)Decoder_Predecode_Open, (void*)Decoder_Predecode_Close, (void*)Decoder_Predecode_Rewind, (void*)Decoder_Predecode_GetSamples};
