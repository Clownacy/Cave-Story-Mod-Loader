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

typedef struct DecoderData_FLAC
{
	unsigned char *file_buffer;
	size_t file_size;
	bool loops;
} DecoderData_FLAC;

typedef struct Decoder_FLAC
{
	DecoderData_FLAC *data;

	MemoryFile *file;
	FLAC__StreamDecoder *stream_decoder;
	DecoderInfo *info;

	unsigned int channel_count;

	bool error;

	unsigned int bits_per_sample;

	unsigned char *block_buffer;
	unsigned long block_buffer_index;
	unsigned long block_buffer_size;
} Decoder_FLAC;

static FLAC__StreamDecoderReadStatus MemoryFile_fread_wrapper(const FLAC__StreamDecoder *decoder, FLAC__byte *output, size_t *count, void *user)
{
	(void)decoder;

	Decoder_FLAC *this = user;

	FLAC__StreamDecoderReadStatus status = FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;

	*count = MemoryFile_fread(output, sizeof(FLAC__byte), *count, this->file);

	if (*count == 0)
		status = FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;

	return status;
}

static FLAC__StreamDecoderSeekStatus MemoryFile_fseek_wrapper(const FLAC__StreamDecoder *decoder, FLAC__uint64 offset, void *user)
{
	(void)decoder;

	Decoder_FLAC *this = user;

	return MemoryFile_fseek(this->file, offset, SEEK_SET) != 0 ? FLAC__STREAM_DECODER_SEEK_STATUS_ERROR : FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}

static FLAC__StreamDecoderTellStatus MemoryFile_ftell_wrapper(const FLAC__StreamDecoder *decoder, FLAC__uint64 *offset, void *user)
{
	(void)decoder;

	Decoder_FLAC *this = user;

	*offset = MemoryFile_ftell(this->file);

	return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

static FLAC__StreamDecoderLengthStatus MemoryFile_GetSize(const FLAC__StreamDecoder *decoder, FLAC__uint64 *length, void *user)
{
	(void)decoder;

	Decoder_FLAC *this = user;

	const long old_offset = MemoryFile_ftell(this->file);

	MemoryFile_fseek(this->file, 0, SEEK_END);
	*length = MemoryFile_ftell(this->file);

	MemoryFile_fseek(this->file, old_offset, SEEK_SET);

	return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

static FLAC__bool MemoryFile_EOF(const FLAC__StreamDecoder *decoder, void *user)
{
	(void)decoder;

	Decoder_FLAC *this = user;

	const long offset = MemoryFile_ftell(this->file);

	MemoryFile_fseek(this->file, 0, SEEK_END);
	const long size = MemoryFile_ftell(this->file);

	MemoryFile_fseek(this->file, offset, SEEK_SET);

	return (offset == size);
}

static FLAC__StreamDecoderWriteStatus WriteCallback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32* const buffer[], void *user)
{
	(void)decoder;

	Decoder_FLAC *this = user;

	FLAC__int32 *block_buffer_pointer = (FLAC__int32*)this->block_buffer;
	for (unsigned int i = 0; i < frame->header.blocksize; ++i)
	{
		for (unsigned int j = 0; j < frame->header.channels; ++j)
		{
			const FLAC__int32 sample = buffer[j][i];

			// Downscale/upscale to 32-bit
			if (this->bits_per_sample < 32)
				*block_buffer_pointer++ = sample << (32 - this->bits_per_sample);
			else if (this->bits_per_sample > 32)
				*block_buffer_pointer++ = sample >> (this->bits_per_sample - 32);
			else
				*block_buffer_pointer++ = sample;
		}
	}

	this->block_buffer_index = 0;
	this->block_buffer_size = (block_buffer_pointer - (FLAC__int32*)this->block_buffer) / this->channel_count;

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void MetadataCallback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *user)
{
	(void)decoder;
	Decoder_FLAC *this = user;

	this->info->sample_rate = metadata->data.stream_info.sample_rate;
	this->info->channel_count = this->channel_count = metadata->data.stream_info.channels;
	this->info->decoded_size = (metadata->data.stream_info.bits_per_sample / 8) * metadata->data.stream_info.channels * metadata->data.stream_info.total_samples;
	this->info->format = DECODER_FORMAT_S32;	// libFLAC doesn't do float32

	this->bits_per_sample = metadata->data.stream_info.bits_per_sample;

	// Init block buffer
	this->block_buffer = malloc(metadata->data.stream_info.max_blocksize * sizeof(long) * metadata->data.stream_info.channels);
	this->block_buffer_index = 0;
	this->block_buffer_size = 0;
}

static void ErrorCallback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *user)
{
	(void)decoder;
	(void)status;

	Decoder_FLAC *this = user;

	this->error = true;
}

DecoderData_FLAC* Decoder_FLAC_LoadData(const char *file_path, bool loops, LinkedBackend *linked_backend)
{
	(void)linked_backend;

	DecoderData_FLAC *data = NULL;

	size_t file_size;
	unsigned char *file_buffer = MemoryFile_fopen_to(file_path, &file_size);

	if (file_buffer)
	{
		data = malloc(sizeof(DecoderData_FLAC));
		data->file_buffer = file_buffer;
		data->file_size = file_size;
		data->loops = loops;
	}

	return data;
}

void Decoder_FLAC_UnloadData(DecoderData_FLAC *data)
{
	if (data)
	{
		free(data->file_buffer);
		free(data);
	}
}

Decoder_FLAC* Decoder_FLAC_Create(DecoderData_FLAC *data, DecoderInfo *info)
{
	Decoder_FLAC *decoder = malloc(sizeof(Decoder_FLAC));

	decoder->stream_decoder = FLAC__stream_decoder_new();

	if (decoder->stream_decoder)
	{
		decoder->file = MemoryFile_fopen_from(data->file_buffer, data->file_size, false);

		if (decoder->file)
		{
			if (FLAC__stream_decoder_init_stream(decoder->stream_decoder, MemoryFile_fread_wrapper, MemoryFile_fseek_wrapper, MemoryFile_ftell_wrapper, MemoryFile_GetSize, MemoryFile_EOF, WriteCallback, MetadataCallback, ErrorCallback, decoder) == FLAC__STREAM_DECODER_INIT_STATUS_OK)
			{
				decoder->data = data;
				decoder->error = false;
				decoder->info = info;
				FLAC__stream_decoder_process_until_end_of_metadata(decoder->stream_decoder);

				if (decoder->error)
				{
					FLAC__stream_decoder_finish(decoder->stream_decoder);
					FLAC__stream_decoder_delete(decoder->stream_decoder);
					MemoryFile_fclose(decoder->file);
					free(decoder);
					decoder = NULL;
				}

			}
			else
			{
				FLAC__stream_decoder_delete(decoder->stream_decoder);
				MemoryFile_fclose(decoder->file);
				free(decoder);
				decoder = NULL;
			}
		}
		else
		{
			FLAC__stream_decoder_delete(decoder->stream_decoder);
			free(decoder);
			decoder = NULL;
		}
	}
	else
	{
		free(decoder);
		decoder = NULL;
	}

	return decoder;
}

void Decoder_FLAC_Destroy(Decoder_FLAC *decoder)
{
	if (decoder)
	{
		FLAC__stream_decoder_finish(decoder->stream_decoder);
		FLAC__stream_decoder_delete(decoder->stream_decoder);
		MemoryFile_fclose(decoder->file);
		free(decoder->block_buffer);
		free(decoder);
	}
}

void Decoder_FLAC_Rewind(Decoder_FLAC *decoder)
{
	FLAC__stream_decoder_seek_absolute(decoder->stream_decoder, 0);
}

unsigned long Decoder_FLAC_GetSamples(Decoder_FLAC *decoder, void *buffer_void, unsigned long frames_to_do)
{
	unsigned char *buffer = buffer_void;

	unsigned long frames_done_total = 0;

	while (frames_done_total != frames_to_do)
	{
		if (decoder->block_buffer_index == decoder->block_buffer_size)
		{
			FLAC__stream_decoder_process_single(decoder->stream_decoder);

			if (FLAC__stream_decoder_get_state(decoder->stream_decoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
			{
				if (decoder->data->loops)
				{
					Decoder_FLAC_Rewind(decoder);
					continue;
				}
				else
				{
					break;
				}
			}
		}

		const unsigned int SIZE_OF_FRAME = sizeof(long) * decoder->channel_count;

		const unsigned long block_frames_to_do = MIN(frames_to_do - frames_done_total, decoder->block_buffer_size - decoder->block_buffer_index);

		memcpy(buffer + (frames_done_total * SIZE_OF_FRAME), decoder->block_buffer + (decoder->block_buffer_index * SIZE_OF_FRAME), block_frames_to_do * SIZE_OF_FRAME);

		decoder->block_buffer_index += block_frames_to_do;
		frames_done_total += block_frames_to_do;
	}

	return frames_done_total;
}
