// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "backend.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define MINI_AL_IMPLEMENTATION

//#define MAL_NO_WASAPI
//#define MAL_NO_DSOUND
#define MAL_NO_WINMM
#define MAL_NO_ALSA
#define MAL_NO_PULSEAUDIO
#define MAL_NO_JACK
#define MAL_NO_COREAUDIO
#define MAL_NO_OSS
#define MAL_NO_OPENSL
#define MAL_NO_OPENAL
#define MAL_NO_SDL
#define MAL_NO_NULL

#define MAL_NO_DECODING
#include "mini_al.h"

typedef struct BackendStream
{
	unsigned long (*user_callback)(void*, void*, unsigned long);
	void *user_data;

	mal_device device;
	unsigned int bytes_per_frame;
	float volume;
	unsigned int volume_int;
} BackendStream;

static mal_uint32 CallbackF32(mal_device *device, mal_uint32 frames_to_do, void *output_buffer)
{
	BackendStream *stream = (BackendStream*)device->pUserData;
	const unsigned int bytes_to_do = frames_to_do * stream->bytes_per_frame;

	const unsigned long bytes_done = stream->user_callback(stream->user_data, output_buffer, bytes_to_do);

	// Handle volume in software, since mini_al's API doesn't have volume control
	float *output_buffer_float = output_buffer;
	if (stream->volume_int != 0x100)
		for (unsigned int i = 0; i < bytes_done / sizeof(float); ++i)
			*output_buffer_float++ *= stream->volume;

	return bytes_done / stream->bytes_per_frame;
}

static mal_uint32 CallbackS16(mal_device *device, mal_uint32 frames_to_do, void *output_buffer)
{
	BackendStream *stream = (BackendStream*)device->pUserData;
	const unsigned int bytes_to_do = frames_to_do * stream->bytes_per_frame;

	const unsigned long bytes_done = stream->user_callback(stream->user_data, output_buffer, bytes_to_do);

	// Handle volume in software, since mini_al's API doesn't have volume control
	short *output_buffer_short = output_buffer;
	if (stream->volume_int != 0x100)
	{
		for (unsigned int i = 0; i < bytes_done / sizeof(short); ++i)
		{
			const short sample = (*output_buffer_short * stream->volume_int) / 0x100;
			*output_buffer_short++ = sample;
		}
	}

	return bytes_done / stream->bytes_per_frame;
}

bool Backend_Init(void)
{
	return true;
}

BackendStream* Backend_CreateStream(unsigned int sample_rate, unsigned int channel_count, BackendFormat format, unsigned long (*user_callback)(void*, void*, unsigned long), void *user_data)
{
	mal_device_config config = mal_device_config_init_playback((format == BACKEND_FORMAT_F32 ? mal_format_f32 : mal_format_s16), channel_count, sample_rate, (format == BACKEND_FORMAT_F32 ? CallbackF32 : CallbackS16));

	BackendStream *stream = malloc(sizeof(BackendStream));

	if (mal_device_init(NULL, mal_device_type_playback, NULL, &config, stream, &stream->device) == MAL_SUCCESS)
	{
		stream->user_callback = user_callback;
		stream->user_data = user_data;

		stream->bytes_per_frame = channel_count * (format == BACKEND_FORMAT_F32 ? sizeof(float) : sizeof(short));
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
		mal_device_uninit(&stream->device);
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
	bool success = true;

	if (stream && mal_device_is_started(&stream->device))
		success = mal_device_stop(&stream->device) == MAL_SUCCESS;

	return success;
}

bool Backend_ResumeStream(BackendStream *stream)
{
	bool success = true;

	if (stream)
		success = mal_device_start(&stream->device) == MAL_SUCCESS;

	return success;
}
