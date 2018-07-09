#pragma once

#include "decoder_common.h"

typedef struct DecoderSndfile DecoderSndfile;

DecoderSndfile* Decoder_Sndfile_Open(const char* const file_path, DecoderInfo *info);
void Decoder_Sndfile_Close(DecoderSndfile *this);
void Decoder_Sndfile_Rewind(DecoderSndfile *this);
void Decoder_Sndfile_Loop(DecoderSndfile *this);
unsigned long Decoder_Sndfile_GetSamples(DecoderSndfile *this, void *buffer, unsigned long bytes_to_do);
