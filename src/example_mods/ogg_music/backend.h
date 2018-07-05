#pragma once

#include <stdbool.h>

typedef struct BackendStream BackendStream;

bool Backend_Init(unsigned long (*callback)(void*, void*, unsigned long));
BackendStream* Backend_CreateStream(unsigned int sample_rate, unsigned int channel_count, void *user_data);
bool Backend_DestroyStream(BackendStream *stream);
bool Backend_SetVolume(BackendStream *stream, float volume);
bool Backend_PauseStream(BackendStream *stream);
bool Backend_ResumeStream(BackendStream *stream);
