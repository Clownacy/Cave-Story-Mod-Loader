// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "../playback.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

typedef struct BackendStream
{
	unsigned long (*user_callback)(void*, void*, unsigned long);
	void *user_data;

	SDL_AudioDeviceID device;
	float volume;

	unsigned int bytes_per_frame;
} BackendStream;

static void Callback(void *user_data, unsigned char *output_buffer, int bytes_to_do)
{
	BackendStream *stream = (BackendStream*)user_data;

	const unsigned long bytes_done = stream->user_callback(stream->user_data, output_buffer, bytes_to_do / stream->bytes_per_frame) * stream->bytes_per_frame;

	// Handle volume in software, since SDL2's API doesn't have volume control
	float *output_buffer_float = (float*)output_buffer;
	if (stream->volume != 1.0f)
		for (unsigned int i = 0; i < bytes_done / sizeof(float); ++i)
			*output_buffer_float++ *= stream->volume;

	const unsigned long bytes_to_clear = bytes_to_do - bytes_done;

	if (bytes_to_clear)
		memset(output_buffer_float, 0, bytes_to_clear);
}

bool Backend_Init(void)
{
	SDL_Init(SDL_INIT_AUDIO);

	return true;
}

void Backend_Deinit(void)
{
	SDL_Quit();
}

BackendStream* Backend_CreateStream(unsigned long (*user_callback)(void*, void*, unsigned long), void *user_data)
{
	BackendStream *stream = malloc(sizeof(BackendStream));

	SDL_AudioSpec want;
	memset(&want, 0, sizeof(want));
	want.freq = STREAM_SAMPLE_RATE;
	want.format = AUDIO_F32;
	want.channels = STREAM_CHANNEL_COUNT;
	want.samples = 4096;
	want.callback = Callback;
	want.userdata = stream;

	SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);

	if (device)
	{
		stream->user_callback = user_callback;
		stream->user_data = user_data;

		stream->device = device;
		stream->volume = 1.0f;

		stream->bytes_per_frame = sizeof(float) * STREAM_CHANNEL_COUNT;
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
		SDL_CloseAudioDevice(stream->device);
		free(stream);
	}

	return true;
}

bool Backend_SetVolume(BackendStream *stream, float volume)
{
	if (stream)
		stream->volume = volume;

	return true;
}

bool Backend_PauseStream(BackendStream *stream)
{
	if (stream)
		SDL_PauseAudioDevice(stream->device, -1);

	return true;
}

bool Backend_ResumeStream(BackendStream *stream)
{
	if (stream)
		SDL_PauseAudioDevice(stream->device, 0);

	return true;
}
