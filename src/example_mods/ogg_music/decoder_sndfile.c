#include "decoder_sndfile.h"

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
	(void*)NULL,
	MemoryFile_ftell_wrapper
};

DecoderSndfile* Decoder_Sndfile_Open(const char *file_path, DecoderInfo *info, DecoderBackend *backend)
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
			sf_command(this->sndfile, SFC_SET_SCALE_FLOAT_INT_READ, NULL, SF_TRUE);

			if (info)
			{
				info->sample_rate = sf_info.samplerate;
				info->channel_count = sf_info.channels;
				info->decoded_size = sf_info.frames * sf_info.channels * sizeof(short);
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

void Decoder_Sndfile_Loop(DecoderSndfile *this)
{
	Decoder_Sndfile_Rewind(this);
}

unsigned long Decoder_Sndfile_GetSamples(DecoderSndfile *this, void *buffer, unsigned long bytes_to_do)
{
	return sf_read_short(this->sndfile, buffer, bytes_to_do / sizeof(short)) * sizeof(short);
}
