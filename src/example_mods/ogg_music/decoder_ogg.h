#pragma once

typedef struct DecoderOgg DecoderOgg;

DecoderOgg* Decode_Ogg_Load(const char* const file_path, unsigned int *channel_count, unsigned int *sample_rate);
void Decode_Ogg_Close(DecoderOgg *this);
void Decode_Ogg_Rewind(DecoderOgg *this);
unsigned long Decode_Ogg_GetSamples(DecoderOgg *this, void *buffer, unsigned long bytes_to_do);
unsigned int Decode_Ogg_GetSize(DecoderOgg *this);
