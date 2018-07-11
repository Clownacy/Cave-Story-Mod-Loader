#include "decoder_split.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "sprintfMalloc.h"

#include "decoder_common.h"

typedef struct DecoderSplit
{
	unsigned int current_file;

	bool is_split;
	bool playing_intro;

	void *decoders[2];

	DecoderBackend *backend;
} DecoderSplit;

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

static int LoadFiles(const char *file_path, bool loop, DecoderInfo *out_info, DecoderBackend *backend, void *decoders[2])
{
	int result;

	DecoderInfo info[2];

	char *file_path_no_extension, *file_extension;
	SplitFileExtension(file_path, &file_path_no_extension, &file_extension);

	// Look for split-file music (Cave Story 3D)
	char* const intro_file_path = sprintfMalloc("%s_intro%s", file_path_no_extension, file_extension);
	decoders[0] = backend->Open(intro_file_path, false, &info[0], backend->backend);
	free(intro_file_path);

	char* const loop_file_path = sprintfMalloc("%s_loop%s", file_path_no_extension, file_extension);
	decoders[1] = backend->Open(loop_file_path, loop, &info[1], backend->backend);
	free(loop_file_path);

	if (decoders[0] == NULL || decoders[1] == NULL)
	{
		result = 0;

		if (decoders[0] == NULL && decoders[1] == NULL)
		{
			// Look for single-file music (Cave Story WiiWare)
			char* const single_file_path = sprintfMalloc("%s%s", file_path_no_extension, file_extension);
			decoders[0] = backend->Open(single_file_path, loop, &info[0], backend->backend);
			free(single_file_path);

			if (decoders[0] == NULL)
			{
				// Neither file could be opened
				result = -1;
			}
		}
		else
		{
			// Only one file could be opened
			if (decoders[0] == NULL)
			{
				// Swap files, since there has to be one in the first slot
				decoders[0] = decoders[1];
				decoders[1] = NULL;
			}
		}
	}
	else
	{
		// Both files opened successfully
		result = 1;
	}

	free(file_path_no_extension);
	free(file_extension);

	if (result == 1 && (info[0].channel_count != info[1].channel_count || info[0].sample_rate != info[1].sample_rate))
	{
		backend->Close(decoders[0]);
		backend->Close(decoders[1]);

		result = -1;
	}

	*out_info = info[0];

	if (result == 1)
		out_info->decoded_size += info[1].decoded_size;

	return result;
}

DecoderSplit* Decoder_Split_Open(const char *path, bool loop, DecoderInfo *info, DecoderBackend *backend)
{
	DecoderSplit *this = NULL;

	void *decoders[2];
	const int format = LoadFiles(path, loop, info, backend, decoders);

	if (format != -1)
	{
		this = malloc(sizeof(DecoderSplit));
		this->backend = backend;

		if (format == 0)
		{
			this->is_split = false;
			this->playing_intro = false;
		}
		else //if (format == 1)
		{
			this->is_split = true;
			this->playing_intro = true;
		}

		this->decoders[0] = decoders[0];
		this->decoders[1] = decoders[1];
		this->current_file = 0;
	}
	else
	{
		free(backend);
	}

	return this;
}

void Decoder_Split_Close(DecoderSplit *this)
{
	for (unsigned int i = 0; i < (this->is_split ? 2 : 1); ++i)
		this->backend->Close(this->decoders[i]);

	free(this->backend);
	free(this);
}

void Decoder_Split_Rewind(DecoderSplit *this)
{
	this->playing_intro = this->is_split;
	this->current_file = 0;

	for (unsigned int i = 0; i < (this->is_split ? 2 : 1); ++i)
		this->backend->Rewind(this->decoders[i]);
}

unsigned long Decoder_Split_GetSamples(DecoderSplit *this, void *output_buffer, unsigned long bytes_to_do)
{
	unsigned long bytes_done_total = 0;

	for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
	{
		bytes_done = this->backend->GetSamples(this->decoders[this->current_file], output_buffer + bytes_done_total, bytes_to_do - bytes_done_total);

		if (bytes_done < bytes_to_do - bytes_done_total)	// EOF
		{
			if (this->playing_intro)
			{
				this->playing_intro = false;
				++this->current_file;
			}
			else
			{
				break;
			}
		}
	}

	return bytes_done_total;
}
