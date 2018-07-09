#include "decoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "decoder_common.h"
#ifdef USE_SNDFILE
#include "decoder_sndfile.h"
#else
#include "decoder_vorbisfile.h"
#endif
#include "memory_file.h"

typedef struct Decoder
{
	bool predecoded;

	void (*close)(void *this);
	void (*rewind)(void *this);
	long (*get_samples)(void *this, void *output_buffer, unsigned long bytes_to_do);

	void *backend;
} Decoder;

typedef struct DecoderPredecode
{
	MemoryFile *file;
	bool loop;
} DecoderPredecode;

static DecoderPredecode* Decode_Predecode_Open(unsigned char *buffer, unsigned long size, bool loop)
{
	DecoderPredecode *this = malloc(sizeof(DecoderPredecode));

	this->file = MemoryFile_fopen_from(buffer, size);
	this->loop = loop;

	return this;
}

static void Decode_Predecode_Close(DecoderPredecode *this)
{
	MemoryFile_fclose(this->file);
	free(this);
}

static void Decoder_Predecode_Rewind(DecoderPredecode *this)
{
	MemoryFile_fseek(this->file, 0, SEEK_SET);
}

static unsigned long Decode_Predecoded_GetSamples(DecoderPredecode *this, char *buffer, unsigned long bytes_to_do)
{
	unsigned long bytes_done_total = 0;

	for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
	{
		bytes_done = MemoryFile_fread(buffer + bytes_done_total, 1, bytes_to_do - bytes_done_total, this->file);

		if (bytes_done < bytes_to_do - bytes_done_total)
		{
			if (this->loop)
				Decoder_Predecode_Rewind(this);
			else
				break;
		}
	}

	return bytes_done_total;
}

Decoder* Decoder_Open(const char* const file_path, DecoderType type, bool loop, bool predecode, DecoderInfo *info)
{
	Decoder *this = NULL;

	void *backend;
	void (*close)(void*);
	long (*get_samples)(void*, void*, unsigned long);

	DecoderInfo decoder_info;

#ifdef USE_SNDFILE
	if (type == DECODER_TYPE_OGG || type == DECODER_TYPE_FLAC)
	{
		close = (void (*)(void*))Decoder_Sndfile_Close;
		get_samples = (long (*)(void*, void*, unsigned long))Decoder_Sndfile_GetSamples;
		backend = Decoder_Sndfile_Open(file_path, loop, &decoder_info);
	}
#else
	if (type == DECODER_TYPE_OGG)
	{
		close = (void (*)(void*))Decoder_Vorbisfile_Close;
		get_samples = (long (*)(void*, void*, unsigned long))Decoder_Vorbisfile_GetSamples;
		backend = Decoder_Vorbisfile_Open(file_path, loop, &decoder_info);
	}
#endif
	else
	{
		backend = NULL;
	}

	if (backend)
	{
		this = malloc(sizeof(Decoder));

		this->predecoded = predecode;

		if (predecode)
		{
			unsigned char *buffer = malloc(decoder_info.decoded_size);

			get_samples(backend, buffer, decoder_info.decoded_size);

			close(backend);

			this->close = (void (*)(void*))Decode_Predecode_Close;
			this->get_samples = (long (*)(void*, void*, unsigned long))Decode_Predecoded_GetSamples;
			this->backend = Decode_Predecode_Open(buffer, decoder_info.decoded_size, loop);
		}
		else
		{
			this->close = close;
			this->get_samples = get_samples;
			this->backend = backend;
		}

		if (info)
			*info = decoder_info;
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
