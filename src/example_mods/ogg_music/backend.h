#pragma once

#include <stdbool.h>

typedef struct BackendStream BackendStream;

bool Backend_Init(long int(*callback)(void*, long));
BackendStream* Backend_CreateStream(unsigned int sample_rate, unsigned int channel_count);
bool Backend_DestroyStream(BackendStream *stream);
void Backend_SetVolume(BackendStream *stream, float volume);
bool Backend_PauseStream(BackendStream *stream);
bool Backend_ResumeStream(BackendStream *stream);
