// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderPxtone DecoderPxtone;

DecoderPxtone* Decoder_Pxtone_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, LinkedBackend *linked_backend);
void Decoder_Pxtone_Close(DecoderPxtone *decoder);
void Decoder_Pxtone_Rewind(DecoderPxtone *decoder);
unsigned long Decoder_Pxtone_GetSamples(DecoderPxtone *decoder, void *buffer, unsigned long bytes_to_do);

static const DecoderBackend DecoderBackend_Pxtone = {(void*)Decoder_Pxtone_Open, (void*)Decoder_Pxtone_Close, (void*)Decoder_Pxtone_Rewind, (void*)Decoder_Pxtone_GetSamples};
