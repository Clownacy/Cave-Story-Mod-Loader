#pragma once

#include <stdbool.h>

typedef struct Mixer_Sound Mixer_Sound;
typedef unsigned int Mixer_SoundInstanceID;

bool Mixer_Init(void);
void Mixer_Deinit(void);
void Mixer_Pause(void);
void Mixer_Unpause(void);
Mixer_Sound* Mixer_LoadSound(const char *file_path, bool loop, bool predecode);
void Mixer_UnloadSound(Mixer_Sound *sound);
Mixer_SoundInstanceID Mixer_PlaySound(Mixer_Sound *sound);
void Mixer_StopSound(Mixer_SoundInstanceID instance);
void Mixer_PauseSound(Mixer_SoundInstanceID instance);
void Mixer_UnpauseSound(Mixer_SoundInstanceID instance);
void Mixer_FadeOutSound(Mixer_SoundInstanceID instance, unsigned int duration);
void Mixer_FadeInSound(Mixer_SoundInstanceID instance, unsigned int duration);
void Mixer_SetSoundVolume(Mixer_SoundInstanceID instance, float volume);
