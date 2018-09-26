// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderSPC DecoderSPC;

DecoderSPC* Decoder_SPC_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, LinkedBackend *linked_backend);
void Decoder_SPC_Close(DecoderSPC *this);
void Decoder_SPC_Rewind(DecoderSPC *this);
unsigned long Decoder_SPC_GetSamples(DecoderSPC *this, void *buffer, unsigned long bytes_to_do);

static const DecoderBackend DecoderBackend_SPC = {(void*)Decoder_SPC_Open, (void*)Decoder_SPC_Close, (void*)Decoder_SPC_Rewind, (void*)Decoder_SPC_GetSamples};
