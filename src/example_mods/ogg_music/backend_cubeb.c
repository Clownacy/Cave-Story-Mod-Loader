// Ogg music mod for Freeware Cave Story
// Copyright © 2018 Clownacy

#include "backend.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include <cubeb/cubeb.h>

typedef struct BackendStream
{
	unsigned long (*callback)(void*, void*, unsigned long);
	void *user_data;

	cubeb_stream *cubeb_stream_pointer;
	unsigned int bytes_per_frame;
} BackendStream;

static cubeb *cubeb_context;

static long data_callback(cubeb_stream *c_stream, void *user_data, void const *input_buffer, void *output_buffer, long frames_to_do)
{
	(void)c_stream;
	(void)input_buffer;

	BackendStream *stream = (BackendStream*)user_data;

	return stream->callback(stream->user_data, output_buffer, frames_to_do * stream->bytes_per_frame) / stream->bytes_per_frame;
}

static void state_callback(cubeb_stream *stream, void *user_data, cubeb_state state)
{
	(void)stream;
	(void)user_data;
	(void)state;
}

bool Backend_Init(void)
{
#ifdef _WIN32
	CoInitializeEx(NULL, COINIT_MULTITHREADED);	// Cubeb needs us to init COM
#endif

	return cubeb_init(&cubeb_context, NULL, NULL) == CUBEB_OK;
}

BackendStream* Backend_CreateStream(unsigned int sample_rate, unsigned int channel_count, unsigned long (*callback)(void*, void*, unsigned long), void *user_data)
{
	BackendStream *stream = NULL;

	cubeb_stream_params output_params;
	output_params.format = CUBEB_SAMPLE_S16LE;
	output_params.rate = sample_rate;
	output_params.prefs = CUBEB_STREAM_PREF_NONE;
	output_params.channels = channel_count;

	if (channel_count == 1 || channel_count == 2)
	{
		if (channel_count == 1)
		{
			output_params.layout = CUBEB_LAYOUT_MONO;
		}
		else// if (channel_count == 2)
		{
			output_params.layout = CUBEB_LAYOUT_STEREO;
		}

		uint32_t latency_frames;

		if (cubeb_get_min_latency(cubeb_context, &output_params, &latency_frames) == CUBEB_OK)
		{
			stream = malloc(sizeof(BackendStream));
			cubeb_stream *cubeb_stream_pointer;

			if (cubeb_stream_init(cubeb_context, &cubeb_stream_pointer, NULL, NULL, NULL, NULL, &output_params, latency_frames, data_callback, state_callback, stream) == CUBEB_OK)
			{
				stream->callback = callback;
				stream->user_data = user_data;

				stream->cubeb_stream_pointer = cubeb_stream_pointer;
				stream->bytes_per_frame = channel_count * sizeof(short);
			}
			else
			{
				free(stream);
				stream = NULL;
			}
		}
	}

	return stream;
}

bool Backend_DestroyStream(BackendStream *stream)
{
	bool success = true;

	if (stream)
	{
		if (cubeb_stream_stop(stream->cubeb_stream_pointer) == CUBEB_OK)
		{
			cubeb_stream_destroy(stream->cubeb_stream_pointer);
			free(stream);
		}
		else
		{
			success = false;
		}
	}

	return success;
}

bool Backend_SetVolume(BackendStream *stream, float volume)
{
	bool success = true;

	if (stream)
		success = cubeb_stream_set_volume(stream->cubeb_stream_pointer, volume) == CUBEB_OK;

	return success;
}

bool Backend_PauseStream(BackendStream *stream)
{
	bool success = true;

	if (stream)
		success = cubeb_stream_stop(stream->cubeb_stream_pointer) == CUBEB_OK;

	return success;
}

bool Backend_ResumeStream(BackendStream *stream)
{
	bool success = true;

	if (stream)
		success = cubeb_stream_start(stream->cubeb_stream_pointer) == CUBEB_OK;

	return success;
}
