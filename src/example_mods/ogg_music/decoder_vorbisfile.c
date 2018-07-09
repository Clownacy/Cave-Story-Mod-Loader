#include "decoder_vorbisfile.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <vorbis/vorbisfile.h>

#include "decoder_common.h"
#include "memory_file.h"

static int MemoryFile_fseek_wrapper(MemoryFile *file, ogg_int64_t offset, int origin)
{
	return MemoryFile_fseek(file, offset, origin);
}

static const ov_callbacks ov_callback_memory = {
	(size_t (*)(void*, size_t, size_t, void*))  MemoryFile_fread,
	(int (*)(void*, ogg_int64_t, int))          MemoryFile_fseek_wrapper,
	(int (*)(void*))                            MemoryFile_fclose,
	(long (*)(void*))                           MemoryFile_ftell
};

typedef struct DecoderVorbisfile
{
	OggVorbis_File vorbis_file;
	bool loop;
} DecoderVorbisfile;

DecoderVorbisfile* Decoder_Vorbisfile_Open(const char* const file_path, bool loop, DecoderInfo *info)
{
	DecoderVorbisfile *this = malloc(sizeof(DecoderVorbisfile));

	MemoryFile *file = MemoryFile_fopen(file_path);

	if (file)
	{
		if (ov_open_callbacks(file, &this->vorbis_file, NULL, 0, ov_callback_memory) == 0)
		{
			this->loop = loop;

			vorbis_info *v_info = ov_info(&this->vorbis_file, -1);

			if (info)
			{
				info->sample_rate = v_info->rate;
				info->channel_count = v_info->channels;
				info->decoded_size = ov_pcm_total(&this->vorbis_file, -1) * v_info->channels * sizeof(short);
			}
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

void Decoder_Vorbisfile_Close(DecoderVorbisfile *this)
{
	ov_clear(&this->vorbis_file);
	free(this);
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
		{
			if (this->loop)
				Decoder_Vorbisfile_Rewind(this);
			else
				break;
		}
	}

	return bytes_done_total;
}
