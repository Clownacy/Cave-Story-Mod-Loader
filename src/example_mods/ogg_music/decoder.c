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
	void (*loop)(void *this);
	long (*get_samples)(void *this, void *output_buffer, unsigned long bytes_to_do);

	void *backend;
} Decoder;

typedef struct DecoderPredecode
{
	MemoryFile *file;
} DecoderPredecode;

static DecoderPredecode* Decode_Predecode_Open(unsigned char *buffer, unsigned long size)
{
	DecoderPredecode *this = malloc(sizeof(DecoderPredecode));

	this->file = MemoryFile_fopen_from(buffer, size);

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


static void Decoder_Predecode_Loop(DecoderPredecode *this)
{
	Decoder_Predecode_Rewind(this);
}

static unsigned long Decode_Predecoded_GetSamples(DecoderPredecode *this, char *buffer, unsigned long bytes_to_do)
{
	return MemoryFile_fread(buffer, 1, bytes_to_do, this->file);
}

Decoder* Decoder_Open(const char* const file_path, DecoderType type, bool predecode, DecoderInfo *info)
{
	Decoder *this = NULL;

	void *backend;
	void (*close)(void*);
	void (*rewind)(void*);
	void (*loop)(void*);
	long (*get_samples)(void*, void*, unsigned long);

	DecoderInfo decoder_info;

#ifdef USE_SNDFILE
	if (type == DECODER_TYPE_OGG || type == DECODER_TYPE_FLAC)
	{
		close = (void (*)(void*))Decoder_Sndfile_Close;
		rewind = (void (*)(void*))Decoder_Sndfile_Rewind;
		loop = (void (*)(void*))Decoder_Sndfile_Loop;
		get_samples = (long (*)(void*, void*, unsigned long))Decoder_Sndfile_GetSamples;
		backend = Decoder_Sndfile_Open(file_path, &decoder_info);
	}
#else
	if (type == DECODER_TYPE_OGG)
	{
		close = (void (*)(void*))Decoder_Vorbisfile_Close;
		rewind = (void (*)(void*))Decoder_Vorbisfile_Rewind;
		loop = (void (*)(void*))Decoder_Vorbisfile_Loop;
		get_samples = (long (*)(void*, void*, unsigned long))Decoder_Vorbisfile_GetSamples;
		backend = Decoder_Vorbisfile_Open(file_path, &decoder_info);
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
			this->rewind = (void (*)(void*))Decoder_Predecode_Rewind;
			this->loop = (void (*)(void*))Decoder_Predecode_Loop;
			this->get_samples = (long (*)(void*, void*, unsigned long))Decode_Predecoded_GetSamples;
			this->backend = Decode_Predecode_Open(buffer, decoder_info.decoded_size);
		}
		else
		{
			this->close = close;
			this->rewind = rewind;
			this->loop = loop;
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

void Decoder_Loop(Decoder *this)
{
	if (this)
		this->loop(this->backend);
}

unsigned long Decoder_GetSamples(Decoder *this, void *output_buffer, unsigned long bytes_to_do)
{
	return this->get_samples(this->backend, output_buffer, bytes_to_do);
}
