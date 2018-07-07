#include "decoder_vorbisfile.h"

#include <stddef.h>
#include <stdlib.h>

#include <vorbis/vorbisfile.h>

#include "memory_file.h"

static int MemoryFile_fseek_wrapper(MemoryFile *file, long long offset, int origin)
{
	return MemoryFile_fseek(file, offset, origin);
}

static const ov_callbacks ov_callback_memory = {
	(size_t (*)(void*, size_t, size_t, void*))  MemoryFile_fread,
	(int (*)(void*, long long, int))            MemoryFile_fseek_wrapper,
	(int (*)(void*))                            MemoryFile_fclose,
	(long (*)(void*))                           MemoryFile_ftell
};

typedef struct DecoderVorbisfile
{
	OggVorbis_File vorbis_file;
} DecoderVorbisfile;

DecoderVorbisfile* Decoder_Vorbisfile_Load(const char* const file_path, unsigned int *channel_count, unsigned int *sample_rate)
{
	DecoderVorbisfile *this = malloc(sizeof(DecoderVorbisfile));

	MemoryFile *file = MemoryFile_fopen(file_path);

	if (file)
	{
		if (ov_open_callbacks(file, &this->vorbis_file, NULL, 0, ov_callback_memory) == 0)
		{
			vorbis_info *info = ov_info(&this->vorbis_file, -1);

			if (channel_count)
				*channel_count = info->channels;

			if (sample_rate)
				*sample_rate = info->rate;
		}
		else
		{
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

void Decoder_Vorbisfile_Close(DecoderVorbisfile *this)
{
	ov_clear(&this->vorbis_file);
}

void Decoder_Vorbisfile_Rewind(DecoderVorbisfile *this)
{
	ov_time_seek(&this->vorbis_file, 0);
}

unsigned long Decoder_Vorbisfile_GetSamples(DecoderVorbisfile *this, void *buffer, unsigned long bytes_to_do)
{
	unsigned long bytes_done_total = 0;

	for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
	{
		bytes_done = ov_read(&this->vorbis_file, buffer + bytes_done_total, bytes_to_do - bytes_done_total, 0, 2, 1, NULL);

		if (bytes_done == 0)
			break;
	}

	return bytes_done_total;
}

unsigned int Decoder_Vorbisfile_GetSize(DecoderVorbisfile *this)
{
	return ov_pcm_total(&this->vorbis_file, -1) * sizeof(short) * ov_info(&this->vorbis_file, -1)->channels;
}
