// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

typedef enum
{
	BACKEND_FORMAT_S16,
	BACKEND_FORMAT_F32
} BackendFormat;

typedef struct BackendStream BackendStream;

bool Backend_Init(void);
BackendStream* Backend_CreateStream(unsigned int sample_rate, unsigned int channel_count, BackendFormat format, unsigned long (*callback)(void*, void*, unsigned long), void *user_data);
bool Backend_DestroyStream(BackendStream *stream);
bool Backend_SetVolume(BackendStream *stream, float volume);
bool Backend_PauseStream(BackendStream *stream);
bool Backend_ResumeStream(BackendStream *stream);
