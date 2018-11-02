// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "../playback.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include <malloc.h>
#endif

#include "SDL.h"

typedef struct BackendStream
{
	void (*user_callback)(void*, void*, unsigned long);
	void *user_data;

	float volume;

	unsigned int bytes_per_frame;
} BackendStream;

static void Callback(void *user_data, Uint8 *output_buffer_uint8, int bytes_to_do)
{
	BackendStream *stream = user_data;
	const unsigned long frames_to_do = bytes_to_do / stream->bytes_per_frame;
	short *output_buffer = (short*)output_buffer_uint8;

#ifdef _MSC_VER
	float *read_buffer = _malloca(frames_to_do * STREAM_CHANNEL_COUNT * sizeof(float));
#else
	float read_buffer[frames_to_do * STREAM_CHANNEL_COUNT];
#endif

	stream->user_callback(stream->user_data, read_buffer, frames_to_do);

	for (unsigned long i = 0; i < frames_to_do * STREAM_CHANNEL_COUNT; ++i)
	{
		if (read_buffer[i] > 1.0f)
			read_buffer[i] = 1.0f;
		else if (read_buffer[i] < -1.0f)
			read_buffer[i] = -1.0f;

		output_buffer[i] = read_buffer[i] * stream->volume * 32767;
	}
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

BackendStream* Backend_CreateStream(void (*user_callback)(void*, void*, unsigned long), void *user_data)
{
	BackendStream *stream = malloc(sizeof(BackendStream));

	SDL_AudioSpec want;
	memset(&want, 0, sizeof(want));
	want.freq = STREAM_SAMPLE_RATE;
	want.format = AUDIO_S16;
	want.channels = STREAM_CHANNEL_COUNT;
	want.samples = (STREAM_SAMPLE_RATE * 25) / 1000;
	want.callback = Callback;
	want.userdata = stream;

	if (!SDL_OpenAudio(&want, NULL))
	{
		stream->user_callback = user_callback;
		stream->user_data = user_data;

		stream->volume = 1.0f;

		stream->bytes_per_frame = sizeof(short) * STREAM_CHANNEL_COUNT;
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
		SDL_CloseAudio();
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
	if (stream)
		SDL_PauseAudio(-1);

	return true;
}

bool Backend_ResumeStream(BackendStream *stream)
{
	if (stream)
		SDL_PauseAudio(0);

	return true;
}
