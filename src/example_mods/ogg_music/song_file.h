#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct SongFile SongFile;

SongFile* SongFile_Load(const char* const path, bool loops, bool predecode);
void SongFile_Unload(SongFile *this);
void SongFile_Reset(SongFile *this);
unsigned long SongFile_GetSamples(SongFile *this, void *output_buffer, unsigned long samples_to_do);
unsigned int SongFile_GetChannelCount(SongFile *this);
unsigned int SongFile_GetSampleRate(SongFile *this);
