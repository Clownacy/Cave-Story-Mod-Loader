#pragma once

typedef struct DecoderSndfile DecoderSndfile;

DecoderSndfile* Decoder_Sndfile_Load(const char* const file_path, unsigned int *channel_count, unsigned int *sample_rate);
void Decoder_Sndfile_Close(DecoderSndfile *this);
void Decoder_Sndfile_Rewind(DecoderSndfile *this);
unsigned long Decoder_Sndfile_GetSamples(DecoderSndfile *this, void *buffer, unsigned long bytes_to_do);
unsigned int Decoder_Sndfile_GetSize(DecoderSndfile *this);
