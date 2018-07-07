#pragma once

typedef struct DecoderVorbisfile DecoderVorbisfile;

DecoderVorbisfile* Decoder_Vorbisfile_Load(const char* const file_path, unsigned int *channel_count, unsigned int *sample_rate);
void Decoder_Vorbisfile_Close(DecoderVorbisfile *this);
void Decoder_Vorbisfile_Rewind(DecoderVorbisfile *this);
unsigned long Decoder_Vorbisfile_GetSamples(DecoderVorbisfile *this, void *buffer, unsigned long bytes_to_do);
unsigned int Decoder_Vorbisfile_GetSize(DecoderVorbisfile *this);
