// Ogg music mod for Freeware Cave Story
// Copyright © 2018 Clownacy

#include "backend.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "mod_loader.h"

typedef struct BackendStream
{
	SDL_AudioDeviceID device;
	unsigned int volume;
	void *user_data;
} BackendStream;

static unsigned long (*UserDataCallback)(void*, void*, unsigned long);

static void DataCallbackWrapper(void *user_data, unsigned char *output_buffer, int bytes_to_do)
{
	BackendStream *stream = (BackendStream*)user_data;

	const unsigned long bytes_done = UserDataCallback(stream->user_data, output_buffer, bytes_to_do);

	// Handle volume in software, since SDL2's API doesn't have volume control
	short *output_buffer_short = (short*)output_buffer;
	if (stream->volume != 0x100)
	{
		for (unsigned int i = 0; i < bytes_done / sizeof(short); ++i)
		{
			const short sample = (*output_buffer_short * stream->volume) / 0x100;
			*output_buffer_short++ = sample;
		}
	}

	const unsigned long bytes_to_clear = bytes_to_do - bytes_done;

	if (bytes_to_clear)
		memset(output_buffer_short, 0, bytes_to_clear);
}

bool Backend_Init(unsigned long (*callback)(void*, void*, unsigned long))
{
	SDL_Init(SDL_INIT_AUDIO);

	UserDataCallback = callback;

	return true;
}

BackendStream* Backend_CreateStream(unsigned int sample_rate, unsigned int channel_count, void *user_data)
{
	BackendStream *stream = malloc(sizeof(BackendStream));

	SDL_AudioSpec want;
	memset(&want, 0, sizeof(want));
	want.freq = sample_rate;
	want.format = AUDIO_S16;
	want.channels = channel_count;
	want.samples = 4096;
	want.callback = DataCallbackWrapper;
	want.userdata = stream;

	SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);

	if (device)
	{
		stream->device = device;
		stream->volume = 0x100;
		stream->user_data = user_data;
	}
	else
	{
		ModLoader_PrintError("ogg_music: Could not open the stream\n");
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
		stream->volume = volume * 0x100;

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
