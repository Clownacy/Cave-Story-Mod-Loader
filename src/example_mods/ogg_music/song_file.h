#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct SongFile SongFile;

SongFile* SongFile_Load(const char* const path, bool loops, bool predecoded);
void SongFile_Unload(SongFile *this);
void SongFile_Reset(SongFile *this);
size_t SongFile_GetSamples(SongFile *this, void *output_buffer, size_t samples_to_do);
unsigned int SongFile_GetChannels(SongFile *this);
unsigned int SongFile_GetSampleRate(SongFile *this);
