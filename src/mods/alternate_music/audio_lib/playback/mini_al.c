// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "../playback.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "../mini_al.h"

typedef struct BackendStream
{
	void (*user_callback)(void*, void*, unsigned long);
	void *user_data;

	mal_device device;
	float volume;
} BackendStream;

static mal_uint32 Callback(mal_device *device, mal_uint32 frames_to_do, void *output_buffer_void)
{
	BackendStream *stream = device->pUserData;
	float *output_buffer = output_buffer_void;

	stream->user_callback(stream->user_data, output_buffer, frames_to_do);

	// Handle volume in software, since mini_al's API doesn't have volume control
	if (stream->volume != 1.0f)
		for (unsigned long i = 0; i < frames_to_do * STREAM_CHANNEL_COUNT; ++i)
			output_buffer[i] *= stream->volume;

	return frames_to_do;
}

bool Backend_Init(void)
{
	return true;
}

void Backend_Deinit(void)
{
	
}

BackendStream* Backend_CreateStream(void (*user_callback)(void*, void*, unsigned long), void *user_data)
{
	mal_device_config config = mal_device_config_init_playback(mal_format_f32, STREAM_CHANNEL_COUNT, STREAM_SAMPLE_RATE, Callback);

	BackendStream *stream = malloc(sizeof(BackendStream));

	if (mal_device_init(NULL, mal_device_type_playback, NULL, &config, stream, &stream->device) == MAL_SUCCESS)
	{
		stream->user_callback = user_callback;
		stream->user_data = user_data;

		stream->volume = 1.0f;
	}
	else
	{
		free(stream);
		stream = NULL;
	}

	return stream;
}

bool Backend_DestroyStream(BackendStream *stream)
{
	if (stream)
	{
		mal_device_uninit(&stream->device);
		free(stream);
	}

	return true;
}

bool Backend_SetVolume(BackendStream *stream, float volume)
{
	if (stream)
		stream->volume = volume * volume;

	return true;
}

bool Backend_PauseStream(BackendStream *stream)
{
	bool success = true;

	if (stream && mal_device_is_started(&stream->device))
		success = mal_device_stop(&stream->device) == MAL_SUCCESS;

	return success;
}

bool Backend_ResumeStream(BackendStream *stream)
{
	bool success = true;

	if (stream && !mal_device_is_started(&stream->device))
		success = mal_device_start(&stream->device) == MAL_SUCCESS;

	return success;
}
