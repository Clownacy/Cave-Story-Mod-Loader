// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "decoder_ivorbisfile.h"

#include <stddef.h>
#include <stdlib.h>

#include <tremor/ivorbisfile.h>

#include "decoder_common.h"
#include "memory_file.h"

typedef struct DecoderIVorbisfile
{
	OggVorbis_File vorbis_file;
	bool loop;
	unsigned int bytes_per_frame;
	unsigned int channel_count;
} DecoderIVorbisfile;

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

DecoderIVorbisfile* Decoder_IVorbisfile_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, LinkedBackend *linked_backend)
{
	(void)linked_backend;
	(void)format;

	DecoderIVorbisfile *this = malloc(sizeof(DecoderIVorbisfile));

	MemoryFile *file = MemoryFile_fopen(file_path);

	if (file)
	{
		if (ov_open_callbacks(file, &this->vorbis_file, NULL, 0, ov_callback_memory) == 0)
		{
			this->loop = loop;

			vorbis_info *v_info = ov_info(&this->vorbis_file, -1);

			this->channel_count = v_info->channels;
			this->bytes_per_frame = v_info->channels * DECODER_GET_FORMAT_SIZE(DECODER_FORMAT_S16);

			info->sample_rate = v_info->rate;
			info->channel_count = v_info->channels;
			info->decoded_size = ov_pcm_total(&this->vorbis_file, -1) * this->bytes_per_frame;
			info->format = DECODER_FORMAT_S16;
		}
		else
		{
			MemoryFile_fclose(file);
			free(this);
			this = NULL;
		}
	}
	else
	{
		free(this);
		this = NULL;
	}

	return this;
}

void Decoder_IVorbisfile_Close(DecoderIVorbisfile *this)
{
	ov_clear(&this->vorbis_file);
	free(this);
}

void Decoder_IVorbisfile_Rewind(DecoderIVorbisfile *this)
{
	ov_time_seek(&this->vorbis_file, 0);
}

unsigned long Decoder_IVorbisfile_GetSamples(DecoderIVorbisfile *this, void *buffer, unsigned long bytes_to_do)
{
	unsigned long bytes_done_total = 0;


	for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
	{
		bytes_done = ov_read(&this->vorbis_file, buffer + bytes_done_total, bytes_to_do - bytes_done_total, NULL);

		if (bytes_done == 0)
		{
			if (this->loop)
				Decoder_IVorbisfile_Rewind(this);
			else
				break;
		}
	}

	return bytes_done_total;
}
