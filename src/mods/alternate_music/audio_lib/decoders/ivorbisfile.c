// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "ivorbisfile.h"

#include <stddef.h>
#include <stdlib.h>

#include <tremor/ivorbisfile.h>

#include "common.h"
#include "memory_file.h"

typedef struct DecoderData_IVorbisfile
{
	unsigned char *file_buffer;
	size_t file_size;
	bool loops;
} DecoderData_IVorbisfile;

typedef struct Decoder_IVorbisfile
{
	DecoderData_IVorbisfile *data;
	OggVorbis_File vorbis_file;
	unsigned int bytes_per_frame;
	unsigned int channel_count;
} Decoder_IVorbisfile;

static size_t MemoryFile_fread_wrapper(void *output, size_t size, size_t count, void *file)
{
	return MemoryFile_fread(output, size, count, file);
}

static int MemoryFile_fseek_wrapper(void *file, ogg_int64_t offset, int origin)
{
	return MemoryFile_fseek(file, offset, origin);
}

static int MemoryFile_fclose_wrapper(void *file)
{
	return MemoryFile_fclose(file);
}

static long MemoryFile_ftell_wrapper(void *file)
{
	return MemoryFile_ftell(file);
}

static const ov_callbacks ov_callback_memory = {
	MemoryFile_fread_wrapper,
	MemoryFile_fseek_wrapper,
	MemoryFile_fclose_wrapper,
	MemoryFile_ftell_wrapper
};

DecoderData_IVorbisfile* Decoder_IVorbisfile_LoadData(const char *file_path, bool loops, LinkedBackend *linked_backend)
{
	(void)linked_backend;

	DecoderData_IVorbisfile *data = NULL;

	size_t file_size;
	unsigned char *file_buffer = MemoryFile_fopen_to(file_path, &file_size);

	if (file_buffer)
	{
		data = malloc(sizeof(DecoderData_IVorbisfile));
		data->file_buffer = file_buffer;
		data->file_size = file_size;
		data->loops = loops;
	}

	return data;
}

void Decoder_IVorbisfile_UnloadData(DecoderData_IVorbisfile *data)
{
	if (data)
	{
		free(data->file_buffer);
		free(data);
	}
}

Decoder_IVorbisfile* Decoder_IVorbisfile_Create(DecoderData_IVorbisfile *data, DecoderInfo *info)
{
	Decoder_IVorbisfile *decoder = NULL;

	MemoryFile *file = MemoryFile_fopen_from(data->file_buffer, data->file_size, false);

	if (file)
	{
		OggVorbis_File vorbis_file;

		if (ov_open_callbacks(file, &vorbis_file, NULL, 0, ov_callback_memory) == 0)
		{
			vorbis_info *v_info = ov_info(&vorbis_file, -1);

			decoder = malloc(sizeof(Decoder_IVorbisfile));

			decoder->data = data;
			decoder->vorbis_file = vorbis_file;
			decoder->channel_count = v_info->channels;
			decoder->bytes_per_frame = v_info->channels * sizeof(short);

			info->sample_rate = v_info->rate;
			info->channel_count = v_info->channels;
			info->decoded_size = ov_pcm_total(&vorbis_file, -1) * decoder->bytes_per_frame;
			info->format = DECODER_FORMAT_S16;
		}
		else
		{
			MemoryFile_fclose(file);
		}
	}

	return decoder;
}

void Decoder_IVorbisfile_Destroy(Decoder_IVorbisfile *decoder)
{
	if (decoder)
	{
		ov_clear(&decoder->vorbis_file);
		free(decoder);
	}
}

void Decoder_IVorbisfile_Rewind(Decoder_IVorbisfile *decoder)
{
	ov_time_seek(&decoder->vorbis_file, 0);
}

unsigned long Decoder_IVorbisfile_GetSamples(Decoder_IVorbisfile *decoder, void *buffer_void, unsigned long frames_to_do)
{
	char *buffer = buffer_void;

	const unsigned long bytes_to_do = frames_to_do * decoder->bytes_per_frame;

	unsigned long bytes_done_total = 0;

	for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
	{
		bytes_done = ov_read(&decoder->vorbis_file, buffer + bytes_done_total, bytes_to_do - bytes_done_total, NULL);

		if (bytes_done == 0)
		{
			if (decoder->data->loops)
				Decoder_IVorbisfile_Rewind(decoder);
			else
				break;
		}
	}

	return bytes_done_total / decoder->bytes_per_frame;
}
