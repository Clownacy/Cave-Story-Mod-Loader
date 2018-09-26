// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "flac.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <FLAC/stream_decoder.h>

#include "common.h"
#include "memory_file.h"

#undef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct DecoderFLAC
{
	MemoryFile *file;
	FLAC__StreamDecoder *stream_decoder;
	DecoderInfo *info; 

	bool loop;

	bool error;

	unsigned int bits_per_sample;

	unsigned char *block_buffer;
	unsigned long block_buffer_index;
	unsigned long block_buffer_size;
} DecoderFLAC;

static FLAC__StreamDecoderReadStatus MemoryFile_fread_wrapper(const FLAC__StreamDecoder *decoder, FLAC__byte *output, size_t *count, void *user)
{
	(void)decoder;

	DecoderFLAC *this = user;

	FLAC__StreamDecoderReadStatus status = FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;

	*count = MemoryFile_fread(output, sizeof(FLAC__byte), *count, this->file);

	if (*count == 0)
		status = FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;

	return status;
}

static FLAC__StreamDecoderSeekStatus MemoryFile_fseek_wrapper(const FLAC__StreamDecoder *decoder, FLAC__uint64 offset, void *user)
{
	(void)decoder;

	DecoderFLAC *this = user;

	return MemoryFile_fseek(this->file, offset, SEEK_SET) != 0 ? FLAC__STREAM_DECODER_SEEK_STATUS_ERROR : FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}

static FLAC__StreamDecoderTellStatus MemoryFile_ftell_wrapper(const FLAC__StreamDecoder *decoder, FLAC__uint64 *offset, void *user)
{
	(void)decoder;

	DecoderFLAC *this = user;

	*offset = MemoryFile_ftell(this->file);

	return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

static FLAC__StreamDecoderLengthStatus MemoryFile_GetSize(const FLAC__StreamDecoder *decoder, FLAC__uint64 *length, void *user)
{
	(void)decoder;

	DecoderFLAC *this = user;

	const long old_offset = MemoryFile_ftell(this->file);

	MemoryFile_fseek(this->file, 0, SEEK_END);
	*length = MemoryFile_ftell(this->file);

	MemoryFile_fseek(this->file, old_offset, SEEK_SET);

	return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

static FLAC__bool MemoryFile_EOF(const FLAC__StreamDecoder *decoder, void *user)
{
	(void)decoder;

	DecoderFLAC *this = user;

	const long offset = MemoryFile_ftell(this->file);

	MemoryFile_fseek(this->file, 0, SEEK_END);
	const long size = MemoryFile_ftell(this->file);

	MemoryFile_fseek(this->file, offset, SEEK_SET);

	return (offset == size);
}

static FLAC__StreamDecoderWriteStatus WriteCallback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32* const buffer[], void *user)
{
	(void)decoder;

	DecoderFLAC *this = user;

	FLAC__int16 *block_buffer_pointer = (FLAC__int16*)this->block_buffer;
	for (unsigned int i = 0; i < frame->header.blocksize; ++i)
	{
		for (unsigned int j = 0; j < frame->header.channels; ++j)
		{
			const FLAC__int32 sample = buffer[j][i];

			// Downscale/upscale to 16-bit
			if (this->bits_per_sample < 16)
				*block_buffer_pointer++ = sample << (16 - this->bits_per_sample);
			else if (this->bits_per_sample > 16)
				*block_buffer_pointer++ = sample >> (this->bits_per_sample - 16);
			else
				*block_buffer_pointer++ = sample;
		}
	}

	this->block_buffer_index = 0;
	this->block_buffer_size = (void*)block_buffer_pointer - (void*)this->block_buffer;

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void MetadataCallback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *user)
{
	(void)decoder;

	DecoderFLAC *this = user;

	this->info->sample_rate = metadata->data.stream_info.sample_rate;
	this->info->channel_count = metadata->data.stream_info.channels;
	this->info->decoded_size = (metadata->data.stream_info.bits_per_sample / 8) * metadata->data.stream_info.channels * metadata->data.stream_info.total_samples;
	this->info->format = DECODER_FORMAT_S16;	// libFLAC doesn't do float32

	this->bits_per_sample = metadata->data.stream_info.bits_per_sample;

	// Init block buffer
	this->block_buffer = malloc(metadata->data.stream_info.max_blocksize * (metadata->data.stream_info.bits_per_sample / 8) * metadata->data.stream_info.channels);
	this->block_buffer_index = 0;
	this->block_buffer_size = 0;
}

static void ErrorCallback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *user)
{
	(void)decoder;
	(void)status;

	DecoderFLAC *this = user;

	this->error = true;
}

DecoderFLAC* Decoder_FLAC_Open(const char *file_path, bool loop, DecoderFormat format, DecoderInfo *info, LinkedBackend *linked_backend)
{
	(void)linked_backend;
	(void)format;

	DecoderFLAC *this = malloc(sizeof(DecoderFLAC));

	this->stream_decoder = FLAC__stream_decoder_new();

	if (this->stream_decoder)
	{
		this->file = MemoryFile_fopen(file_path);

		if (this->file)
		{
			if (FLAC__stream_decoder_init_stream(this->stream_decoder, MemoryFile_fread_wrapper, MemoryFile_fseek_wrapper, MemoryFile_ftell_wrapper, MemoryFile_GetSize, MemoryFile_EOF, WriteCallback, MetadataCallback, ErrorCallback, this) == FLAC__STREAM_DECODER_INIT_STATUS_OK)
			{
				this->error = false;
				this->info = info;
				FLAC__stream_decoder_process_until_end_of_metadata(this->stream_decoder);

				if (!this->error)
				{
					this->loop = loop;
				}
				else
				{
					FLAC__stream_decoder_finish(this->stream_decoder);
					FLAC__stream_decoder_delete(this->stream_decoder);
					MemoryFile_fclose(this->file);
					free(this);
					this = NULL;
				}

			}
			else
			{
				FLAC__stream_decoder_delete(this->stream_decoder);
				MemoryFile_fclose(this->file);
				free(this);
				this = NULL;
			}
		}
		else
		{
			FLAC__stream_decoder_delete(this->stream_decoder);
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

void Decoder_FLAC_Close(DecoderFLAC *this)
{
	FLAC__stream_decoder_finish(this->stream_decoder);
	FLAC__stream_decoder_delete(this->stream_decoder);
	MemoryFile_fclose(this->file);
	free(this->block_buffer);
	free(this);
}

void Decoder_FLAC_Rewind(DecoderFLAC *this)
{
	FLAC__stream_decoder_seek_absolute(this->stream_decoder, 0);
}

unsigned long Decoder_FLAC_GetSamples(DecoderFLAC *this, void *buffer, unsigned long bytes_to_do)
{
	unsigned long bytes_done_total = 0;

	while (bytes_done_total != bytes_to_do)
	{
		if (this->block_buffer_index == this->block_buffer_size)
		{
			FLAC__stream_decoder_process_single(this->stream_decoder);

			if (FLAC__stream_decoder_get_state(this->stream_decoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
			{
				if (this->loop)
				{
					Decoder_FLAC_Rewind(this);
					continue;
				}
				else
				{
					break;
				}
			}
		}

		const unsigned long block_bytes_to_do = MIN(bytes_to_do - bytes_done_total, this->block_buffer_size - this->block_buffer_index);

		memcpy(buffer + bytes_done_total, this->block_buffer + this->block_buffer_index, block_bytes_to_do);

		this->block_buffer_index += block_bytes_to_do;
		bytes_done_total += block_bytes_to_do;
	}

	return bytes_done_total;
}
