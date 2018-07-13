#include "decoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "decoder_common.h"
#include "decoder_predecode.h"
#include "decoder_split.h"
#include "memory_file.h"

#ifdef USE_SNDFILE
#include "decoder_sndfile.h"
#else
#include "decoder_flac.h"
#include "decoder_vorbisfile.h"
#endif

#ifdef USE_OPENMPT
#include "decoder_openmpt.h"
#endif

typedef struct Decoder
{
	DecoderBackend *backend;
	void *backend_object;
} Decoder;

Decoder* TryOpen(DecoderBackend *backend, DecoderBackend **out_backend, const char *file_path, bool loop, DecoderInfo *info, bool predecode)
{
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

	Decoder *backend_object = backend->Open(file_path, loop, DECODER_FORMAT_F32, info, backend->backend);

	if (backend_object == NULL)
	{
		for (DecoderBackend *current_backend = backend, *next_backend; current_backend->backend != NULL; current_backend = next_backend)
		{
			next_backend = current_backend->backend;
			free(current_backend);
		}
	}
	else
	{
		*out_backend = backend;
	}

	return backend_object;
}

static DecoderBackend backends[] = {
#ifdef USE_SNDFILE
	{
		.Open = (void*)Decoder_Sndfile_Open,
		.Close = (void*)Decoder_Sndfile_Close,
		.Rewind = (void*)Decoder_Sndfile_Rewind,
		.GetSamples = (void*)Decoder_Sndfile_GetSamples
	},
#else
	{
		.Open = (void*)Decoder_Vorbisfile_Open,
		.Close = (void*)Decoder_Vorbisfile_Close,
		.Rewind = (void*)Decoder_Vorbisfile_Rewind,
		.GetSamples = (void*)Decoder_Vorbisfile_GetSamples
	},
	{
		.Open = (void*)Decoder_FLAC_Open,
		.Close = (void*)Decoder_FLAC_Close,
		.Rewind = (void*)Decoder_FLAC_Rewind,
		.GetSamples = (void*)Decoder_FLAC_GetSamples
	},
#endif
#ifdef USE_OPENMPT
	{
		.Open = (void*)Decoder_OpenMPT_Open,
		.Close = (void*)Decoder_OpenMPT_Close,
		.Rewind = (void*)Decoder_OpenMPT_Rewind,
		.GetSamples = (void*)Decoder_OpenMPT_GetSamples
	}
#endif
};

Decoder* Decoder_Open(const char *file_path, bool loop, DecoderInfo *info, bool predecode)
{
	Decoder *backend_object;
	DecoderBackend *backend;
	for (unsigned int i = 0; i < sizeof(backends) / sizeof(backends[0]); ++i)
	{
		backend_object = TryOpen(&backends[i], &backend, file_path, loop, info, predecode);

		if (backend_object)
			break;
	}

	Decoder *this = NULL;

	if (backend_object)
	{
		this = malloc(sizeof(Decoder));
		this->backend_object = backend_object;
		this->backend = backend;
	}

	return this;
}

void Decoder_Close(Decoder *this)
{
	this->backend->Close(this->backend_object);

	for (DecoderBackend *backend = this->backend, *next_backend; backend->backend != NULL; backend = next_backend)
	{
		next_backend = backend->backend;
		free(backend);
	}

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
