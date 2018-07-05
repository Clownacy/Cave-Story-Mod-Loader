// Ogg music mod for Freeware Cave Story
// Copyright © 2018 Clownacy

#include "backend.h"

#include <stdbool.h>
#include <stddef.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include <cubeb/cubeb.h>

#include "mod_loader.h"

typedef struct BackendStream
{
	cubeb_stream *cubeb_stream_pointer;
	unsigned int bytes_per_frame;
	void *user_data;
} BackendStream;

static cubeb *cubeb_context;

static long (*UserDataCallback)(void*, void*, long);

static long data_cb(cubeb_stream *c_stream, void *user_data, void const *input_buffer, void *output_buffer, long frames_to_do)
{
	BackendStream *stream = (BackendStream*)user_data;

	return UserDataCallback(stream->user_data, output_buffer, frames_to_do * stream->bytes_per_frame) / stream->bytes_per_frame;
}

static void state_cb(cubeb_stream *stream, void *user_data, cubeb_state state)
{

}

bool Backend_Init(long int (*callback)(void*, void*, long))
{
	bool success = false;

#ifdef _WIN32
	CoInitializeEx(NULL, COINIT_MULTITHREADED);	// Cubeb needs us to init COM
#endif

	if (cubeb_init(&cubeb_context, "Ogg player for Cave Story", NULL) == CUBEB_OK)
	{
		UserDataCallback = callback;

		success = true;
	}

	return success;
}

BackendStream* Backend_CreateStream(unsigned int sample_rate, unsigned int channel_count, void *user_data)
{
	BackendStream *stream = NULL;

	cubeb_stream_params output_params;
	output_params.format = CUBEB_SAMPLE_S16LE;
	output_params.rate = sample_rate;
	output_params.prefs = CUBEB_STREAM_PREF_NONE;
	output_params.channels = channel_count;

	if (channel_count < 1 || channel_count > 2)
	{
		ModLoader_PrintError("ogg_music: Invalid channel count (must be 1 or 2)\n");
	}
	else
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

		if (cubeb_get_min_latency(cubeb_context, &output_params, &latency_frames) != CUBEB_OK)
		{
			ModLoader_PrintError("ogg_music: Could not get minimum latency\n");
		}
		else
		{
			stream = malloc(sizeof(BackendStream));
			cubeb_stream *cubeb_stream_pointer;

			if (cubeb_stream_init(cubeb_context, &cubeb_stream_pointer, "Main Stream", NULL, NULL, NULL, &output_params, latency_frames, data_cb, state_cb, stream) != CUBEB_OK)
			{
				ModLoader_PrintError("ogg_music: Could not open the stream\n");
				free(stream);
				stream = NULL;
			}
			else
			{
				stream->cubeb_stream_pointer = cubeb_stream_pointer;
				stream->bytes_per_frame = channel_count * sizeof(short);
				stream->user_data = user_data;
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
		if (cubeb_stream_stop(stream->cubeb_stream_pointer) != CUBEB_OK)
		{
			ModLoader_PrintError("ogg_music: Could not stop the stream\n");
			success = false;
		}
		else
		{
			cubeb_stream_destroy(stream->cubeb_stream_pointer);
			free(stream);
		}
	}

	return success;
}

bool Backend_SetVolume(BackendStream *stream, float volume)
{
	bool success = true;

	if (stream)
	{
		if (cubeb_stream_set_volume(stream->cubeb_stream_pointer, volume) != CUBEB_OK)
		{
			ModLoader_PrintError("ogg_music: Could not set the stream's volume\n");
			success = false;
		}
	}

	return success;
}

bool Backend_PauseStream(BackendStream *stream)
{
	bool success = true;

	if (stream)
	{
		if (cubeb_stream_stop(stream->cubeb_stream_pointer) != CUBEB_OK)
		{
			ModLoader_PrintError("ogg_music: Could not stop the stream\n");
			success = false;
		}
	}

	return success;
}

bool Backend_ResumeStream(BackendStream *stream)
{
	bool success = true;

	if (stream)
	{
		if (cubeb_stream_start(stream->cubeb_stream_pointer) != CUBEB_OK)
		{
			ModLoader_PrintError("ogg_music: Could not start the stream\n");
			success = false;
		}
	}

	return success;
}
