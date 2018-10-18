#define MINI_AL_IMPLEMENTATION

#define MAL_NO_DECODING

#ifndef MINI_AL_ENABLE_DEVICE_IO
 #define MAL_NO_DEVICE_IO
#else
 //#define MAL_NO_WASAPI
 //#define MAL_NO_DSOUND
 #define MAL_NO_WINMM
 #define MAL_NO_ALSA
 #define MAL_NO_PULSEAUDIO
 #define MAL_NO_JACK
 #define MAL_NO_COREAUDIO
 #define MAL_NO_SNDIO
 #define MAL_NO_AUDIO4
 #define MAL_NO_OSS
 #define MAL_NO_OPENSL
 #define MAL_NO_OPENAL
 #define MAL_NO_SDL
 #define MAL_NO_NULL
#endif

#include "mini_al.h"
