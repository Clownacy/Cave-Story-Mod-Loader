#include "decoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "decoder_common.h"
#include "decoder_predecode.h"
#include "decoder_split.h"
#include "memory_file.h"

#ifdef USE_VORBISFILE
#include "decoder_vorbisfile.h"
#endif
#ifdef USE_FLAC
#include "decoder_flac.h"
#endif
#ifdef USE_SNDFILE
#include "decoder_sndfile.h"
#endif
#ifdef USE_OPENMPT
#include "decoder_openmpt.h"
#endif
#ifdef USE_SPC
#include "decoder_spc.h"
#endif

typedef struct Decoder
{
	LinkedBackend *linked_backend;
	void *backend_object;
} Decoder;

void* TryOpen(const DecoderBackend *backend, LinkedBackend **out_linked_backend, const char *file_path, bool loop, DecoderInfo *info, bool predecode)
{
	LinkedBackend *last_backend;
	LinkedBackend *linked_backend = malloc(sizeof(LinkedBackend));
	linked_backend->next = NULL;
	linked_backend->backend = backend;

	if (predecode)
	{
		last_backend = linked_backend;
		linked_backend = malloc(sizeof(LinkedBackend));
		linked_backend->next = last_backend;
		linked_backend->backend = &DecoderBackend_Predecode;
	}

	last_backend = linked_backend;
	linked_backend = malloc(sizeof(LinkedBackend));
	linked_backend->next = last_backend;
	linked_backend->backend = &DecoderBackend_Split;

	void *backend_object = linked_backend->backend->Open(file_path, loop, DECODER_FORMAT_F32, info, linked_backend->next);

	if (backend_object == NULL)
	{
		for (LinkedBackend *current_backend = linked_backend, *next_backend; current_backend != NULL; current_backend = next_backend)
		{
			next_backend = current_backend->next;
			free(current_backend);
		}
	}
	else
	{
		*out_linked_backend = linked_backend;
	}

	return backend_object;
}

static const struct
{
	const DecoderBackend *decoder;
	bool can_be_predecoded;
} backends[] = {
#ifdef USE_VORBISFILE
	{&DecoderBackend_Vorbisfile, true},
#endif
#ifdef USE_FLAC
	{&DecoderBackend_FLAC, true},
#endif
#ifdef USE_SNDFILE
	{&DecoderBackend_Sndfile, true},
#endif
#ifdef USE_OPENMPT
	{&DecoderBackend_OpenMPT, false},
#endif
#ifdef USE_SPC
	{&DecoderBackend_SPC, false},
#endif
};

Decoder* Decoder_Open(const char *file_path, bool loop, DecoderInfo *info, bool predecode)
{
	void *backend_object;
	LinkedBackend *linked_backend;
	for (unsigned int i = 0; i < sizeof(backends) / sizeof(backends[0]); ++i)
	{
		backend_object = TryOpen(backends[i].decoder, &linked_backend, file_path, loop, info, predecode && backends[i].can_be_predecoded);

		if (backend_object)
			break;
	}

	Decoder *this = NULL;

	if (backend_object)
	{
		this = malloc(sizeof(Decoder));
		this->backend_object = backend_object;
		this->linked_backend = linked_backend;
	}

	return this;
}

void Decoder_Close(Decoder *this)
{
	this->linked_backend->backend->Close(this->backend_object);

	for (LinkedBackend *current_backend = this->linked_backend, *next_backend; current_backend != NULL; current_backend = next_backend)
	{
		next_backend = current_backend->next;
		free(current_backend);
	}

	free(this);
}

void Decoder_Rewind(Decoder *this)
{
	this->linked_backend->backend->Rewind(this->backend_object);
}

unsigned long Decoder_GetSamples(Decoder *this, void *output_buffer, unsigned long bytes_to_do)
{
	return this->linked_backend->backend->GetSamples(this->backend_object, output_buffer, bytes_to_do);
}
