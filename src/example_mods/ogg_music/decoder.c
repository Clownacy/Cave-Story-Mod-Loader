#include "decoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "decoder_ogg.h"
#include "memory_file.h"

typedef struct Decoder
{
	bool predecoded;
	unsigned int channel_count;
	unsigned int sample_rate;

	void (*close)(void *this);
	void (*rewind)(void *this);
	long (*get_samples)(void *this, void *output_buffer, unsigned long bytes_to_do);

	void *backend;
} Decoder;

static void Decode_Predecode_Close(MemoryFile *this)
{
	MemoryFile_fclose(this);
}

static void Decode_Predecoded_Rewind(MemoryFile *this)
{
	MemoryFile_fseek(this, 0, SEEK_SET);
}

static unsigned long Decode_Predecoded_ReadSamples(MemoryFile *this, char *buffer, unsigned long bytes_to_do)
{
	return MemoryFile_fread(buffer, 1, bytes_to_do, this);
}

Decoder* Decoder_Open(const char* const file_path, bool predecode)
{
	Decoder *this = NULL;

	unsigned int channel_count, sample_rate;
	void *decode_ogg = Decode_Ogg_Load(file_path, &channel_count, &sample_rate);

	if (decode_ogg)
	{
		this = malloc(sizeof(Decoder));

		this->predecoded = predecode;
		this->channel_count = channel_count;
		this->sample_rate = sample_rate;

		if (predecode)
		{
			const size_t size = Decode_Ogg_GetSize(decode_ogg);
			unsigned char *data = malloc(size);

			Decode_Ogg_ReadSamples(decode_ogg, data, size);

			Decode_Ogg_Close(decode_ogg);

			this->close = (void (*)(void*))Decode_Predecode_Close;
			this->rewind = (void (*)(void*))Decode_Predecoded_Rewind;
			this->get_samples = (long (*)(void*, void*, unsigned long))Decode_Predecoded_ReadSamples;
			this->backend = MemoryFile_fopen_from(data, size);
		}
		else
		{
			this->close = (void (*)(void*))Decode_Ogg_Close;
			this->rewind = (void (*)(void*))Decode_Ogg_Rewind;
			this->get_samples = (long (*)(void*, void*, unsigned long))Decode_Ogg_ReadSamples;
			this->backend = decode_ogg;
		}
	}

	return this;
}

void Decoder_Close(Decoder *this)
{
	if (this)
		this->close(this->backend);
}

void Decoder_Rewind(Decoder *this)
{
	if (this)
		this->rewind(this->backend);
}

unsigned long Decoder_GetSamples(Decoder *this, void *output_buffer, unsigned long bytes_to_do)
{
	return this->get_samples(this->backend, output_buffer, bytes_to_do);
}

unsigned int Decoder_GetChannelCount(Decoder *this)
{
	return this ? this->channel_count : 0;
}

unsigned int Decoder_GetSampleRate(Decoder *this)
{
	return this ? this->sample_rate : 0;
}
