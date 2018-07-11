#include "decoder.h"

#include <stdbool.h>
#include <stdlib.h>

#include "decoder_common.h"
#include "decoder_predecode.h"
#include "decoder_split.h"
#ifdef USE_SNDFILE
#include "decoder_sndfile.h"
#else
#include "decoder_flac.h"
#include "decoder_vorbisfile.h"
#endif
#include "memory_file.h"

typedef struct Decoder
{
	DecoderBackend *backend;
	void *backend_object;
} Decoder;

Decoder* Decoder_Open(const char* const file_path, DecoderType type, bool predecode, DecoderInfo *info)
{
	Decoder *this = malloc(sizeof(Decoder));

	DecoderBackend *backend = malloc(sizeof(DecoderBackend));

#ifdef USE_SNDFILE
	if (type == DECODER_TYPE_OGG || type == DECODER_TYPE_FLAC)
	{
		backend->Open = (void* (*)(const char*, DecoderInfo*, DecoderBackend*))Decoder_Sndfile_Open;
		backend->Close = (void (*)(void*))Decoder_Sndfile_Close;
		backend->Rewind = (void (*)(void*))Decoder_Sndfile_Rewind;
		backend->Loop = (void (*)(void*))Decoder_Sndfile_Loop;
		backend->GetSamples = (long (*)(void*, void*, unsigned long))Decoder_Sndfile_GetSamples;
	}
#else
	if (type == DECODER_TYPE_OGG)
	{
		backend->Open = (void* (*)(const char*, DecoderInfo*, DecoderBackend*))Decoder_Vorbisfile_Open;
		backend->Close = (void (*)(void*))Decoder_Vorbisfile_Close;
		backend->Rewind = (void (*)(void*))Decoder_Vorbisfile_Rewind;
		backend->Loop = (void (*)(void*))Decoder_Vorbisfile_Loop;
		backend->GetSamples = (long (*)(void*, void*, unsigned long))Decoder_Vorbisfile_GetSamples;
	}
	else if (type == DECODER_TYPE_FLAC)
	{
		backend->Open = (void* (*)(const char*, DecoderInfo*, DecoderBackend*))Decoder_FLAC_Open;
		backend->Close = (void (*)(void*))Decoder_FLAC_Close;
		backend->Rewind = (void (*)(void*))Decoder_FLAC_Rewind;
		backend->Loop = (void (*)(void*))Decoder_FLAC_Loop;
		backend->GetSamples = (long (*)(void*, void*, unsigned long))Decoder_FLAC_GetSamples;
	}
#endif

	if (predecode)
	{
		DecoderBackend *last_backend = backend;
		backend = malloc(sizeof(DecoderBackend));
		backend->backend = last_backend;

		backend->Open = (void* (*)(const char*, DecoderInfo*, DecoderBackend*))Decoder_Predecode_Open;
		backend->Close = (void (*)(void*))Decoder_Predecode_Close;
		backend->Rewind = (void (*)(void*))Decoder_Predecode_Rewind;
		backend->Loop = (void (*)(void*))Decoder_Predecode_Loop;
		backend->GetSamples = (long (*)(void*, void*, unsigned long))Decoder_Predecode_GetSamples;
	}

	DecoderBackend *last_backend = backend;
	backend = malloc(sizeof(DecoderBackend));
	backend->backend = last_backend;

	backend->Open = (void* (*)(const char*, DecoderInfo*, DecoderBackend*))Decoder_Split_Open;
	backend->Close = (void (*)(void*))Decoder_Split_Close;
	backend->Rewind = (void (*)(void*))Decoder_Split_Rewind;
	backend->Loop = (void (*)(void*))Decoder_Split_Loop;
	backend->GetSamples = (long (*)(void*, void*, unsigned long))Decoder_Split_GetSamples;

	this->backend = backend;

	this->backend_object = this->backend->Open(file_path, info, this->backend->backend);

	return this;
}

void Decoder_Close(Decoder *this)
{
	if (this)
	{
		this->backend->Close(this->backend_object);
		free(this->backend);
		free(this);
	}
}

void Decoder_Rewind(Decoder *this)
{
	if (this)
		this->backend->Rewind(this->backend_object);
}

void Decoder_Loop(Decoder *this)
{
	if (this)
		this->backend->Loop(this->backend_object);
}

unsigned long Decoder_GetSamples(Decoder *this, void *output_buffer, unsigned long bytes_to_do)
{
	return this->backend->GetSamples(this->backend_object, output_buffer, bytes_to_do);
}
