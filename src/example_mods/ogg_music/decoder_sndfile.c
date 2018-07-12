#include "decoder_sndfile.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <sndfile.h>

#include "decoder_common.h"
#include "memory_file.h"

typedef struct DecoderSndfile
{
	MemoryFile *file;
	SNDFILE *sndfile;
	bool loop;
	DecoderFormat format;
	unsigned int bytes_per_frame;
} DecoderSndfile;

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

DecoderSndfile* Decoder_Sndfile_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, DecoderBackend *backend)
{
	(void)backend;

	DecoderSndfile *this = malloc(sizeof(DecoderSndfile));

	this->file = MemoryFile_fopen(file_path);

	if (this->file)
	{
		SF_INFO sf_info;
		memset(&sf_info, 0, sizeof(SF_INFO));

		this->sndfile = sf_open_virtual(&sfvirtual, SFM_READ, &sf_info, this->file);

		if (this->sndfile)
		{
			// Set scaling to prevent weird clipping
			if (format == DECODER_FORMAT_S16)
				sf_command(this->sndfile, SFC_SET_SCALE_FLOAT_INT_READ, NULL, SF_TRUE);

			this->loop = loop;
			this->format = format;
			this->bytes_per_frame = sf_info.channels * DECODER_GET_FORMAT_SIZE(format);

			if (info)
			{
				info->sample_rate = sf_info.samplerate;
				info->channel_count = sf_info.channels;
				info->decoded_size = sf_info.frames * this->bytes_per_frame;
				info->format = format;
			}
		}
		else
		{
			MemoryFile_fclose(this->file);
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

void Decoder_Sndfile_Close(DecoderSndfile *this)
{
	sf_close(this->sndfile);
	MemoryFile_fclose(this->file);
	free(this);
}

void Decoder_Sndfile_Rewind(DecoderSndfile *this)
{
	sf_seek(this->sndfile, 0, SEEK_SET);
}

unsigned long Decoder_Sndfile_GetSamples(DecoderSndfile *this, void *buffer, unsigned long bytes_to_do)
{
	unsigned long bytes_done_total = 0;

	if (this->format == DECODER_FORMAT_F32)
	{
		for (;;)
		{
			bytes_done_total += sf_readf_float(this->sndfile, buffer, bytes_to_do / this->bytes_per_frame) * this->bytes_per_frame;

			if (bytes_done_total == bytes_to_do || !this->loop)
				break;

			Decoder_Sndfile_Rewind(this);
		}
	}
	else if (this->format == DECODER_FORMAT_S16)
	{
		for (;;)
		{
			bytes_done_total += sf_readf_short(this->sndfile, buffer, bytes_to_do / this->bytes_per_frame) * this->bytes_per_frame;

			if (bytes_done_total == bytes_to_do || !this->loop)
				break;

			Decoder_Sndfile_Rewind(this);
		}
	}

	return bytes_done_total;
}
