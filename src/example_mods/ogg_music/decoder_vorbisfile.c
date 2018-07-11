#include "decoder_vorbisfile.h"

#include <stddef.h>
#include <stdlib.h>

#include <vorbis/vorbisfile.h>

#include "decoder_common.h"
#include "memory_file.h"

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

typedef struct DecoderVorbisfile
{
	OggVorbis_File vorbis_file;
} DecoderVorbisfile;

DecoderVorbisfile* Decoder_Vorbisfile_Open(const char *file_path, DecoderInfo *info, DecoderBackend *backend)
{
	(void)backend;

	DecoderVorbisfile *this = malloc(sizeof(DecoderVorbisfile));

	MemoryFile *file = MemoryFile_fopen(file_path);

	if (file)
	{
		if (ov_open_callbacks(file, &this->vorbis_file, NULL, 0, ov_callback_memory) == 0)
		{
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

void Decoder_Vorbisfile_Loop(DecoderVorbisfile *this)
{
	Decoder_Vorbisfile_Rewind(this);
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
