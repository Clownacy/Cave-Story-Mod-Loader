#include "mixer.h"

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "mini_al.h"

#include "decoder.h"
#include "playback.h"

typedef struct Channel
{
	struct Channel *next;

	bool paused;
	float volume;
	Decoder *decoder;
	mal_dsp dsp;
	Mixer_SoundInstanceID instance;

	unsigned int fade_out_counter_max;
	unsigned int fade_in_counter_max;
	unsigned int fade_counter;
} Channel;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static Channel *channel_list_head;
static BackendStream *stream;

static Channel* FindChannel(Mixer_SoundInstanceID instance)
{
	for (Channel *channel = channel_list_head; channel != NULL; channel = channel->next)
		if (channel->instance == instance)
			return channel;

	return NULL;
}

static unsigned long CallbackStream(void *user_data, void *output_buffer_void, unsigned long frames_to_do)
{
	(void)user_data;

	float (*output_buffer)[frames_to_do][STREAM_CHANNEL_COUNT] = output_buffer_void;

	memset(output_buffer, 0, frames_to_do * sizeof(float) * STREAM_CHANNEL_COUNT);

	pthread_mutex_lock(&mutex);
	Channel **channel_pointer = &channel_list_head;
	while (*channel_pointer != NULL)
	{
		Channel *channel = *channel_pointer;

		if (channel->paused == false)
		{
			float read_buffer[frames_to_do][STREAM_CHANNEL_COUNT];

			mal_uint64 frames_read = mal_dsp_read(&channel->dsp, frames_to_do, read_buffer, channel->decoder);

			for (mal_uint64 i = 0; i < frames_read; ++i)
			{	
				for (unsigned int j = 0; j < STREAM_CHANNEL_COUNT; ++j)
					(*output_buffer)[i][j] += read_buffer[i][j] * channel->volume;

				if (channel->fade_out_counter_max)
				{
					const float fade_out_volume = channel->fade_counter / (float)channel->fade_out_counter_max;

					for (unsigned int j = 0; j < STREAM_CHANNEL_COUNT; ++j)
						(*output_buffer)[i][j] *= (fade_out_volume * fade_out_volume);

					if (channel->fade_counter)
						--channel->fade_counter;
				}

				if (channel->fade_in_counter_max)
				{
					const float fade_in_volume = (channel->fade_in_counter_max - channel->fade_counter) / (float)channel->fade_in_counter_max;

					for (unsigned int j = 0; j < STREAM_CHANNEL_COUNT; ++j)
						(*output_buffer)[i][j] *= (fade_in_volume * fade_in_volume);

					if (!--channel->fade_counter)
						channel->fade_in_counter_max = 0;
				}
			}

			if (frames_read < frames_to_do)	// Sound finished
			{
				Decoder_Destroy(channel->decoder);
				*channel_pointer = channel->next;
				free(channel);
				continue;
			}
		}

		channel_pointer = &(*channel_pointer)->next;
	}
	pthread_mutex_unlock(&mutex);

	return frames_to_do;
}

static mal_uint32 CallbackDSP(mal_dsp *dsp, mal_uint32 frames_to_do, void *output_buffer, void *user_data)
{
	(void)dsp;

	return Decoder_GetSamples((Decoder*)user_data, output_buffer, frames_to_do);
}

bool Mixer_Init(void)
{
	bool success = false;

	if (Backend_Init())
	{
		stream = Backend_CreateStream(CallbackStream, NULL);

		if (stream)
		{
			if (Backend_ResumeStream(stream))
				success = true;
			else
				Mixer_Deinit();
		}
		else
		{
			Backend_Deinit();
		}
	}

	return success;
}

void Mixer_Deinit(void)
{
	Backend_DestroyStream(stream);
	Backend_Deinit();
}

void Mixer_Pause(void)
{
	Backend_PauseStream(stream);
}

void Mixer_Unpause(void)
{
	Backend_ResumeStream(stream);
}

Mixer_Sound* Mixer_LoadSound(const char *file_path, bool loop, bool predecode)
{
	return (Mixer_Sound*)Decoder_LoadData(file_path, loop, predecode);
}

void Mixer_UnloadSound(Mixer_Sound *sound)
{
	Decoder_UnloadData((DecoderData*)sound);
}

Mixer_SoundInstanceID Mixer_PlaySound(Mixer_Sound *sound)
{
	static Mixer_SoundInstanceID instance_allocator;

	Mixer_SoundInstanceID instance = 0;

	DecoderInfo info;
	Decoder *decoder = Decoder_Create((DecoderData*)sound, &info);

	if (decoder)
	{
		instance = ++instance_allocator;

		Channel *channel = malloc(sizeof(Channel));

		channel->decoder = decoder;
		channel->volume = 1.0f;
		channel->instance = instance;
		channel->paused = true;
		channel->fade_out_counter_max = 0;
		channel->fade_in_counter_max = 0;

		mal_format format;
		if (info.format == DECODER_FORMAT_S16)
			format = mal_format_s16;
		else if (info.format == DECODER_FORMAT_S32)
			format = mal_format_s32;
		else //if (info.format == DECODER_FORMAT_F32)
			format = mal_format_f32;

		const mal_dsp_config config = mal_dsp_config_init(format, info.channel_count, info.sample_rate, mal_format_f32, STREAM_CHANNEL_COUNT, STREAM_SAMPLE_RATE, CallbackDSP, NULL);
		mal_dsp_init(&config, &channel->dsp);

		pthread_mutex_lock(&mutex);
		channel->next = channel_list_head;
		channel_list_head = channel;
		pthread_mutex_unlock(&mutex);
	}

	return instance;
}

void Mixer_StopSound(Mixer_SoundInstanceID instance)
{
	Channel *channel = NULL;

	pthread_mutex_lock(&mutex);

	for (Channel **channel_pointer = &channel_list_head; *channel_pointer != NULL; channel_pointer = &(*channel_pointer)->next)
	{
		if ((*channel_pointer)->instance == instance)
		{
			channel = *channel_pointer;
			*channel_pointer = channel->next;
			break;
		}
	}

	pthread_mutex_unlock(&mutex);

	if (channel)
	{
		Decoder_Destroy(channel->decoder);
		free(channel);
	}
}

void Mixer_PauseSound(Mixer_SoundInstanceID instance)
{
	pthread_mutex_lock(&mutex);

	Channel *channel = FindChannel(instance);

	if (channel)
		channel->paused = true;

	pthread_mutex_unlock(&mutex);
}

void Mixer_UnpauseSound(Mixer_SoundInstanceID instance)
{
	pthread_mutex_lock(&mutex);

	Channel *channel = FindChannel(instance);

	if (channel)
		channel->paused = false;

	pthread_mutex_unlock(&mutex);
}

void Mixer_FadeOutSound(Mixer_SoundInstanceID instance, unsigned int duration)
{
	pthread_mutex_lock(&mutex);

	Channel *channel = FindChannel(instance);

	if (channel)
	{
		channel->fade_out_counter_max = (STREAM_SAMPLE_RATE * duration) / 1000;
		channel->fade_counter = channel->fade_in_counter_max ? (((unsigned long long)channel->fade_in_counter_max - channel->fade_counter) * channel->fade_out_counter_max) / channel->fade_in_counter_max : channel->fade_out_counter_max;
		channel->fade_in_counter_max = 0;
	}

	pthread_mutex_unlock(&mutex);
}

void Mixer_FadeInSound(Mixer_SoundInstanceID instance, unsigned int duration)
{
	pthread_mutex_lock(&mutex);

	Channel *channel = FindChannel(instance);

	if (channel)
	{
		channel->fade_in_counter_max = (STREAM_SAMPLE_RATE * duration) / 1000;
		channel->fade_counter = channel->fade_out_counter_max ? (((unsigned long long)channel->fade_out_counter_max - channel->fade_counter) * channel->fade_in_counter_max) / channel->fade_out_counter_max : channel->fade_in_counter_max;
		channel->fade_out_counter_max = 0;
	}

	pthread_mutex_unlock(&mutex);
}

void Mixer_SetSoundVolume(Mixer_SoundInstanceID instance, float volume)
{
	pthread_mutex_lock(&mutex);

	Channel *channel = FindChannel(instance);

	if (channel)
		channel->volume = volume * volume;

	pthread_mutex_unlock(&mutex);
}
