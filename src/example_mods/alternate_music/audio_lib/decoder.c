// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "decoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "decoders/common.h"
#include "decoders/misc_utilities.h"
#include "decoders/predecode.h"
#include "decoders/split.h"

#ifdef USE_VORBISFILE
#include "decoders/vorbisfile.h"
#endif
#ifdef USE_IVORBISFILE
#include "decoders/ivorbisfile.h"
#endif
#ifdef USE_FLAC
#include "decoders/flac.h"
#endif
#ifdef USE_SNDFILE
#include "decoders/sndfile.h"
#endif
#ifdef USE_OPENMPT
#include "decoders/openmpt.h"
#endif
#ifdef USE_SPC
#include "decoders/spc.h"
#endif
#ifdef USE_PXTONE
#include "decoders/pxtone.h"
#endif

typedef struct DecoderData
{
	LinkedBackend *linked_backend;
	void *backend_data;
} DecoderData;

typedef struct Decoder
{
	void *backend_object;
	DecoderData *data;
} Decoder;

static const struct
{
	const char **file_extensions;
	const DecoderBackend decoder;
	bool can_be_predecoded;
	bool can_be_split;
} backends[] = {
#ifdef USE_VORBISFILE
	{
		(const char*[]){".ogg", NULL},
		{
			(void*)Decoder_Vorbisfile_LoadData,
			(void*)Decoder_Vorbisfile_UnloadData,
			(void*)Decoder_Vorbisfile_Create,
			(void*)Decoder_Vorbisfile_Destroy,
			(void*)Decoder_Vorbisfile_Rewind,
			(void*)Decoder_Vorbisfile_GetSamples
		},
		true, true
	},
#endif
#ifdef USE_IVORBISFILE
	{
		(const char*[]){".ogg", NULL},
		{
			(void*)Decoder_IVorbisfile_LoadData,
			(void*)Decoder_IVorbisfile_UnloadData,
			(void*)Decoder_IVorbisfile_Create,
			(void*)Decoder_IVorbisfile_Destroy,
			(void*)Decoder_IVorbisfile_Rewind,
			(void*)Decoder_IVorbisfile_GetSamples
		},
		true, true
	},
#endif
#ifdef USE_FLAC
	{
		(const char*[]){".flac", NULL},
		{
			(void*)Decoder_FLAC_LoadData,
			(void*)Decoder_FLAC_UnloadData,
			(void*)Decoder_FLAC_Create,
			(void*)Decoder_FLAC_Destroy,
			(void*)Decoder_FLAC_Rewind,
			(void*)Decoder_FLAC_GetSamples
		},
		true, true
	},
#endif
#ifdef USE_SNDFILE
	{
		(const char*[]){".ogg", ".flac", ".wav", ".aiff", NULL},
		{
			(void*)Decoder_Sndfile_LoadData,
			(void*)Decoder_Sndfile_UnloadData,
			(void*)Decoder_Sndfile_Create,
			(void*)Decoder_Sndfile_Destroy,
			(void*)Decoder_Sndfile_Rewind,
			(void*)Decoder_Sndfile_GetSamples
		},
		true, true
	},
#endif
#ifdef USE_OPENMPT
	{
		(const char*[]){".mod", ".s3m", ".xm", ".it", ".mptm", NULL},
		{
			(void*)Decoder_OpenMPT_LoadData,
			(void*)Decoder_OpenMPT_UnloadData,
			(void*)Decoder_OpenMPT_Create,
			(void*)Decoder_OpenMPT_Destroy,
			(void*)Decoder_OpenMPT_Rewind,
			(void*)Decoder_OpenMPT_GetSamples
		},
		false, false
	},
#endif
#ifdef USE_SPC
	{
		(const char*[]){".spc", NULL},
		{
			(void*)Decoder_SPC_LoadData,
			(void*)Decoder_SPC_UnloadData,
			(void*)Decoder_SPC_Create,
			(void*)Decoder_SPC_Destroy,
			(void*)Decoder_SPC_Rewind,
			(void*)Decoder_SPC_GetSamples
		},
		false, false
	},
#endif
#ifdef USE_PXTONE
	{
		(const char*[]){".ptcop", ".pttune", NULL},
		{
			(void*)Decoder_Pxtone_LoadData,
			(void*)Decoder_Pxtone_UnloadData,
			(void*)Decoder_Pxtone_Create,
			(void*)Decoder_Pxtone_Destroy,
			(void*)Decoder_Pxtone_Rewind,
			(void*)Decoder_Pxtone_GetSamples
		},
		false, false
	},
#endif
};

static void* TryOpen(const DecoderBackend *backend, LinkedBackend **out_linked_backend, const char *file_path, bool loop, bool predecode, bool split)
{
	LinkedBackend *last_backend;
	LinkedBackend *linked_backend = malloc(sizeof(LinkedBackend));
	linked_backend->next = NULL;
	linked_backend->backend = backend;

	if (predecode)
	{
		static const DecoderBackend backend = {
			(void*)Decoder_Predecode_LoadData,
			(void*)Decoder_Predecode_UnloadData,
			(void*)Decoder_Predecode_Create,
			(void*)Decoder_Predecode_Destroy,
			(void*)Decoder_Predecode_Rewind,
			(void*)Decoder_Predecode_GetSamples
		};

		last_backend = linked_backend;
		linked_backend = malloc(sizeof(LinkedBackend));
		linked_backend->next = last_backend;
		linked_backend->backend = &backend;
	}

	if (split)
	{
		static const DecoderBackend backend = {
			(void*)Decoder_Split_LoadData,
			(void*)Decoder_Split_UnloadData,
			(void*)Decoder_Split_Create,
			(void*)Decoder_Split_Destroy,
			(void*)Decoder_Split_Rewind,
			(void*)Decoder_Split_GetSamples
		};

		last_backend = linked_backend;
		linked_backend = malloc(sizeof(LinkedBackend));
		linked_backend->next = last_backend;
		linked_backend->backend = &backend;
	}

	void *backend_object = linked_backend->backend->LoadData(file_path, loop, linked_backend->next);

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

DecoderData* Decoder_LoadData(const char *file_path, bool loop, bool predecode)
{
	void *backend_data = NULL;
	for (unsigned int i = 0; i < sizeof(backends) / sizeof(backends[0]); ++i)
	{
		char *extension;
		DecoderUtil_SplitFileExtension(file_path, NULL, &extension);

		for (unsigned int j = 0; backends[i].file_extensions[j] != NULL; ++j)
		{
			if (!strcmp(extension, backends[i].file_extensions[j]))
			{
				free(extension);

				LinkedBackend *linked_backend = NULL;
				backend_data = TryOpen(&backends[i].decoder, &linked_backend, file_path, loop, predecode && backends[i].can_be_predecoded, backends[i].can_be_split);

				DecoderData *this = NULL;

				if (backend_data)
				{
					this = malloc(sizeof(DecoderData));
					this->backend_data = backend_data;
					this->linked_backend = linked_backend;
				}

				return this;
			}
		}

		free(extension);
	}

	return NULL;
}

void Decoder_UnloadData(DecoderData *data)
{
	if (data)
	{
		data->linked_backend->backend->UnloadData(data->backend_data);

		for (LinkedBackend *current_backend = data->linked_backend, *next_backend; current_backend != NULL; current_backend = next_backend)
		{
			next_backend = current_backend->next;
			free(current_backend);
		}

		free(data);
	}
}

Decoder* Decoder_Create(DecoderData *data, DecoderInfo *info)
{
	Decoder *decoder = NULL;

	if (data && data->backend_data)
	{
		void *backend_object = data->linked_backend->backend->Create(data->backend_data, info);

		if (backend_object)
		{
			decoder = malloc(sizeof(Decoder));
			decoder->backend_object = backend_object;
			decoder->data = data;
		}
	}

	return decoder;
}

void Decoder_Destroy(Decoder *decoder)
{
	if (decoder)
	{
		decoder->data->linked_backend->backend->Destroy(decoder->backend_object);
		free(decoder);
	}
}

void Decoder_Rewind(Decoder *decoder)
{
	decoder->data->linked_backend->backend->Rewind(decoder->backend_object);
}

unsigned long Decoder_GetSamples(Decoder *decoder, void *output_buffer, unsigned long frames_to_do)
{
	return decoder->data->linked_backend->backend->GetSamples(decoder->backend_object, output_buffer, frames_to_do);
}
