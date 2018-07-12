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

static void SplitFileExtension(const char *path, char **path_no_extension, char **extension)
{
	// Get filename
	const char *slash1 = strrchr(path, '/');
	const char *slash2 = strrchr(path, '\\');

	if (!slash1)
		slash1 = path - 1;
	if (!slash2)
		slash2 = path - 1;

	const char* const filename = (slash1 > slash2 ? slash1 : slash2) + 1;

	// Get address of extension
	const char *dot = strrchr(filename, '.');

	if (!dot || dot == filename)
		dot = strchr(filename, '\0');

	// Output them
	if (path_no_extension)
	{
		const unsigned int size = dot - path;
		*path_no_extension = malloc(size + 1);
		memcpy(*path_no_extension, path, size);
		(*path_no_extension)[size] = '\0';
	}

	if (extension)
		*extension = strdup(dot);
}

Decoder* Decoder_Open(const char *file_path, bool loop, DecoderInfo *info, bool predecode)
{
	char *extension;
	SplitFileExtension(file_path, NULL, &extension);

	DecoderType type;
	if (!strcmp(extension, ".ogg"))
		type = DECODER_TYPE_OGG;
	else if (!strcmp(extension, ".flac"))
		type = DECODER_TYPE_FLAC;
#ifdef USE_OPENMPT
	else if (!strcmp(extension, ".xm"))
		type = DECODER_TYPE_MODULE;
	else if (!strcmp(extension, ".it"))
		type = DECODER_TYPE_MODULE;
	else if (!strcmp(extension, ".mod"))
		type = DECODER_TYPE_MODULE;
	else if (!strcmp(extension, ".s3m"))
		type = DECODER_TYPE_MODULE;
	else if (!strcmp(extension, ".mptm"))
		type = DECODER_TYPE_MODULE;
#endif
	else
		return NULL;

	Decoder *this = malloc(sizeof(Decoder));

	DecoderBackend *backend = malloc(sizeof(DecoderBackend));
	backend->backend = NULL;

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
#ifdef USE_OPENMPT
	else if (type == DECODER_TYPE_MODULE)
	{
		backend->Open = (void*)Decoder_OpenMPT_Open;
		backend->Close = (void*)Decoder_OpenMPT_Close;
		backend->Rewind = (void*)Decoder_OpenMPT_Rewind;
		backend->GetSamples = (void*)Decoder_OpenMPT_GetSamples;
	}
#endif
	else
	{
		return NULL;
	}

	if (predecode && type != DECODER_TYPE_MODULE)
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

	this->backend_object = this->backend->Open(file_path, loop, DECODER_FORMAT_F32, info, this->backend->backend);

	if (this->backend_object == NULL)
	{
		for (DecoderBackend *backend = this->backend, *next_backend; next_backend != NULL; backend = next_backend)
		{
			next_backend = backend->backend;
			free(backend);
		}

		free(this);
		this = NULL;
	}

	return this;
}

void Decoder_Close(Decoder *this)
{
	this->backend->Close(this->backend_object);

	for (DecoderBackend *backend = this->backend, *next_backend; next_backend != NULL; backend = next_backend)
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
