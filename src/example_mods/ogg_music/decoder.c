#include "decoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <vorbis/vorbisfile.h>

#include "memory_file.h"

typedef struct Decoder
{
	bool predecoded;
	unsigned int channel_count;
	unsigned int sample_rate;

	MemoryFile *file;
	MemoryFile *decoded_file;
	OggVorbis_File vorbis_file;
} Decoder;

int MemoryFile_fseek_wrapper(MemoryFile *file, long long offset, int origin)
{
	return MemoryFile_fseek(file, offset, origin);
}

static const ov_callbacks ov_callback_memory = {
	(size_t (*)(void*, size_t, size_t, void*))  MemoryFile_fread,
	(int (*)(void*, long long, int))            MemoryFile_fseek_wrapper,
	(int (*)(void*))                            NULL,
	(long (*)(void*))                           MemoryFile_ftell
};

static unsigned long ReadOggToBuffer(Decoder *this, void *buffer, unsigned long bytes_to_do)
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

static unsigned long ReadPCMToBuffer(Decoder *this, char *buffer, unsigned long bytes_to_do)
{
	return MemoryFile_fread(buffer, 1, bytes_to_do, this->decoded_file);
}

Decoder* Decoder_Open(const char* const file_path, bool predecode)
{
	Decoder *this = malloc(sizeof(Decoder));

	this->file = MemoryFile_fopen(file_path);

	if (this->file)
	{
		if (ov_open_callbacks(this->file, &this->vorbis_file, NULL, 0, ov_callback_memory) == 0)
		{
			this->predecoded = predecode;

			vorbis_info *info = ov_info(&this->vorbis_file, -1);
			this->channel_count = info->channels;
			this->sample_rate = info->rate;

			if (predecode)
			{
				size_t size = ov_pcm_total(&this->vorbis_file, -1) * sizeof(short) * ov_info(&this->vorbis_file, -1)->channels;
				unsigned char *data = malloc(size);

				ReadOggToBuffer(this, data, size);

				ov_clear(&this->vorbis_file);
				MemoryFile_fclose(this->file);

				this->decoded_file = MemoryFile_fopen_from(data, size);
			}
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

void Decoder_Close(Decoder *this)
{
	if (this)
	{
		if (this->predecoded)
		{
			MemoryFile_fclose(this->decoded_file);
		}
		else
		{
			ov_clear(&this->vorbis_file);
			MemoryFile_fclose(this->file);
		}
	}
}

void Decoder_Rewind(Decoder *this)
{
	if (this)
	{
		if (this->predecoded)
			MemoryFile_fseek(this->decoded_file, 0, SEEK_SET);
		else
			ov_time_seek(&this->vorbis_file, 0);
	}
}

unsigned long Decoder_GetSamples(Decoder *this, void *output_buffer, unsigned long bytes_to_do)
{
	unsigned long bytes_done;

	if (this->predecoded)
		bytes_done = ReadPCMToBuffer(this, output_buffer, bytes_to_do);
	else
		bytes_done = ReadOggToBuffer(this, output_buffer, bytes_to_do);

	return bytes_done;
}

unsigned int Decoder_GetChannelCount(Decoder *this)
{
	return this ? this->channel_count : 0;
}

unsigned int Decoder_GetSampleRate(Decoder *this)
{
	return this ? this->sample_rate : 0;
}
