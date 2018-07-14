// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "backend.h"

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
	unsigned int volume_int;
} BackendStream;

static void CallbackF32(void *user_data, unsigned char *output_buffer, int bytes_to_do)
{
	BackendStream *stream = (BackendStream*)user_data;

	const unsigned long bytes_done = stream->user_callback(stream->user_data, output_buffer, bytes_to_do);

	// Handle volume in software, since SDL2's API doesn't have volume control
	float *output_buffer_float = (float*)output_buffer;
	if (stream->volume_int != 0x100)
		for (unsigned int i = 0; i < bytes_done / sizeof(float); ++i)
			*output_buffer_float++ *= stream->volume;

	const unsigned long bytes_to_clear = bytes_to_do - bytes_done;

	if (bytes_to_clear)
		memset(output_buffer_float, 0, bytes_to_clear);
}

static void CallbackS16(void *user_data, unsigned char *output_buffer, int bytes_to_do)
{
	BackendStream *stream = (BackendStream*)user_data;

	const unsigned long bytes_done = stream->user_callback(stream->user_data, output_buffer, bytes_to_do);

	// Handle volume in software, since SDL2's API doesn't have volume control
	short *output_buffer_short = (short*)output_buffer;
	if (stream->volume_int != 0x100)
	{
		for (unsigned int i = 0; i < bytes_done / sizeof(short); ++i)
		{
			const short sample = (*output_buffer_short * stream->volume_int) / 0x100;
			*output_buffer_short++ = sample;
		}
	}

	const unsigned long bytes_to_clear = bytes_to_do - bytes_done;

	if (bytes_to_clear)
		memset(output_buffer_short, 0, bytes_to_clear);
}

bool Backend_Init(void)
{
	SDL_Init(SDL_INIT_AUDIO);

	return true;
}

BackendStream* Backend_CreateStream(unsigned int sample_rate, unsigned int channel_count, BackendFormat format, unsigned long (*user_callback)(void*, void*, unsigned long), void *user_data)
{
	BackendStream *stream = malloc(sizeof(BackendStream));

	SDL_AudioSpec want;
	memset(&want, 0, sizeof(want));
	want.freq = sample_rate;
	want.format = (format == BACKEND_FORMAT_F32 ? AUDIO_F32 : AUDIO_S16);
	want.channels = channel_count;
	want.samples = 4096;
	want.callback = (format == BACKEND_FORMAT_F32 ? CallbackF32 : CallbackS16);
	want.userdata = stream;

	SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);

	if (device)
	{
		stream->user_callback = user_callback;
		stream->user_data = user_data;

		stream->device = device;
		stream->volume = 1.0f;
		stream->volume_int = 0x100;
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
	{
		stream->volume = volume;
		stream->volume_int = volume * 0x100;
	}

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
