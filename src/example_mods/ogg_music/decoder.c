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

Decoder* Decoder_Open(const char* const file_path, bool loop, DecoderInfo *info, DecoderType type, bool predecode)
{
	Decoder *this = malloc(sizeof(Decoder));

	DecoderBackend *backend = malloc(sizeof(DecoderBackend));

#ifdef USE_SNDFILE
	if (type == DECODER_TYPE_OGG || type == DECODER_TYPE_FLAC)
	{
		backend->Open = (void*)Decoder_Sndfile_Open;
		backend->Close = (void*)Decoder_Sndfile_Close;
		backend->Rewind = (void*)Decoder_Sndfile_Rewind;
		backend->GetSamples = (void*)Decoder_Sndfile_GetSamples;
	}
#else
	if (type == DECODER_TYPE_OGG)
	{
		backend->Open = (void*)Decoder_Vorbisfile_Open;
		backend->Close = (void*)Decoder_Vorbisfile_Close;
		backend->Rewind = (void*)Decoder_Vorbisfile_Rewind;
		backend->GetSamples = (void*)Decoder_Vorbisfile_GetSamples;
	}
	else if (type == DECODER_TYPE_FLAC)
	{
		backend->Open = (void*)Decoder_FLAC_Open;
		backend->Close = (void*)Decoder_FLAC_Close;
		backend->Rewind = (void*)Decoder_FLAC_Rewind;
		backend->GetSamples = (void*)Decoder_FLAC_GetSamples;
	}
#endif

	if (predecode)
	{
		DecoderBackend *last_backend = backend;
		backend = malloc(sizeof(DecoderBackend));
		backend->backend = last_backend;

		backend->Open = (void*)Decoder_Predecode_Open;
		backend->Close = (void*)Decoder_Predecode_Close;
		backend->Rewind = (void*)Decoder_Predecode_Rewind;
		backend->GetSamples = (void*)Decoder_Predecode_GetSamples;
	}

	DecoderBackend *last_backend = backend;
	backend = malloc(sizeof(DecoderBackend));
	backend->backend = last_backend;

	backend->Open = (void*)Decoder_Split_Open;
	backend->Close = (void*)Decoder_Split_Close;
	backend->Rewind = (void*)Decoder_Split_Rewind;
	backend->GetSamples = (void*)Decoder_Split_GetSamples;

	this->backend = backend;

	this->backend_object = this->backend->Open(file_path, loop, info, this->backend->backend);

	return this;
}

void Decoder_Close(Decoder *this)
{
	this->backend->Close(this->backend_object);
	free(this->backend);
	free(this);
}

void Decoder_Rewind(Decoder *this)
{
	this->backend->Rewind(this->backend_object);
}

unsigned long Decoder_GetSamples(Decoder *this, void *output_buffer, unsigned long bytes_to_do)
{
	return this->backend->GetSamples(this->backend_object, output_buffer, bytes_to_do);
}
