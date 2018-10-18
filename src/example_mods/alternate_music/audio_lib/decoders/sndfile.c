// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "sndfile.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <sndfile.h>

#include "common.h"
#include "memory_file.h"

typedef struct DecoderData_Sndfile
{
	unsigned char *file_buffer;
	size_t file_size;
	bool loops;
} DecoderData_Sndfile;

typedef struct Decoder_Sndfile
{
	DecoderData_Sndfile *data;
	MemoryFile *file;
	SNDFILE *sndfile;
	DecoderFormat format;
	unsigned int bytes_per_frame;
} Decoder_Sndfile;

static sf_count_t MemoryFile_fread_wrapper(void *output, sf_count_t count, void *user)
{
	return MemoryFile_fread(output, 1, count, user);
}

static sf_count_t MemoryFile_fseek_wrapper(sf_count_t offset, int origin, void *user)
{
	return MemoryFile_fseek(user, offset, origin);
}

static sf_count_t MemoryFile_ftell_wrapper(void *user)
{
	return MemoryFile_ftell(user);
}

static sf_count_t MemoryFile_GetSize(void *user)
{
	const sf_count_t old_offset = MemoryFile_ftell_wrapper(user);

	MemoryFile_fseek_wrapper(0, SEEK_END, user);
	const sf_count_t size = MemoryFile_ftell_wrapper(user);

	MemoryFile_fseek_wrapper(old_offset, SEEK_SET, user);

	return size;
}

static SF_VIRTUAL_IO sfvirtual = {
	MemoryFile_GetSize,
	MemoryFile_fseek_wrapper,
	MemoryFile_fread_wrapper,
	NULL,
	MemoryFile_ftell_wrapper
};

DecoderData_Sndfile* Decoder_Sndfile_LoadData(const char *file_path, bool loops, LinkedBackend *linked_backend)
{
	(void)linked_backend;

	DecoderData_Sndfile *data = NULL;

	size_t file_size;
	unsigned char *file_buffer = MemoryFile_fopen_to(file_path, &file_size);

	if (file_buffer)
	{
		data = malloc(sizeof(DecoderData_Sndfile));
		data->file_buffer = file_buffer;
		data->file_size = file_size;
		data->loops = loops;
	}

	return data;
}

void Decoder_Sndfile_UnloadData(DecoderData_Sndfile *data)
{
	if (data)
	{
		free(data->file_buffer);
		free(data);
	}
}

Decoder_Sndfile* Decoder_Sndfile_Create(DecoderData_Sndfile *data, DecoderInfo *info)
{
	Decoder_Sndfile *decoder = NULL;

	MemoryFile *file = MemoryFile_fopen_from(data->file_buffer, data->file_size, false);

	SF_INFO sf_info;
	memset(&sf_info, 0, sizeof(SF_INFO));

	SNDFILE *sndfile = sf_open_virtual(&sfvirtual, SFM_READ, &sf_info, file);

	if (sndfile)
	{
		decoder = malloc(sizeof(Decoder_Sndfile));
		decoder->data = data;
		decoder->sndfile = sndfile;
		decoder->file = file;
		decoder->bytes_per_frame = sf_info.channels * sizeof(float);

		info->sample_rate = sf_info.samplerate;
		info->channel_count = sf_info.channels;
		info->decoded_size = sf_info.frames * decoder->bytes_per_frame;
		info->format = DECODER_FORMAT_F32;
	}
	else
	{
		MemoryFile_fclose(file);
	}

	return decoder;
}

void Decoder_Sndfile_Destroy(Decoder_Sndfile *decoder)
{
	if (decoder)
	{
		sf_close(decoder->sndfile);
		MemoryFile_fclose(decoder->file);
		free(decoder);
	}
}

void Decoder_Sndfile_Rewind(Decoder_Sndfile *decoder)
{
	sf_seek(decoder->sndfile, 0, SEEK_SET);
}

unsigned long Decoder_Sndfile_GetSamples(Decoder_Sndfile *decoder, void *buffer, unsigned long frames_to_do)
{
	unsigned long frames_done_total = 0;

	for (;;)
	{
		frames_done_total += sf_readf_float(decoder->sndfile, buffer + (frames_done_total * decoder->bytes_per_frame), frames_to_do - frames_done_total);

		if (frames_done_total == frames_to_do || !decoder->data->loops)
			break;

		Decoder_Sndfile_Rewind(decoder);
	}

	return frames_done_total;
}