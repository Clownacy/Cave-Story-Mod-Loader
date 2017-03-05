// Ogg music mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdbool.h>
#include <string.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"

#include "mod_loader_main.h"

#include "playlist.h"

// Variables
int* const current_music = (int* const)0x4A57F4;
int* const previous_song_last_position = (int* const)0x4A57F8;
int* const previous_music = (int* const)0x4A57FC;

// String array
const char* const * const org_playlist = (const char* const * const)0x4981E8;

// Functions
void (*LoadOrgMusic)(const char* const) = (void(*)(const char* const))0x41C6F0;
void (*SetOrgMusicPosition)(int) = (void(*)(int))0x41C730;
int (*GetOrgMusicPosition)(void) = (int(*)(void))0x41C770;
void (*sub_41C790)(void) = (void(*)(void))0x41C790;
void (*SetOrgVolume)(int) = (void(*)(int))0x41C7C0;
void (*sub_41C7F0)(void) = (void(*)(void))0x41C7F0;

bool intro_playing;
int current_loop_setting;
Mix_Music *music_intro, *music_loop;

void UnloadMusic(Mix_Music **music)
{
	Mix_HaltMusic();
	Mix_FreeMusic(*music);
	*music = NULL;
}

void OggMusicEnded(void)
{
	if (intro_playing == true)
	{
		intro_playing = false;
		UnloadMusic(&music_intro);
		Mix_PlayMusic(music_loop, current_loop_setting);
	}
	else
	{
		UnloadMusic(&music_loop);
	}
}

bool PlayOggMusic(const int song_id)
{
	// Kill current music
	UnloadMusic(&music_intro);
	UnloadMusic(&music_loop);

	if (song_id == 0)
	{
		// Song is XXXX, which doesn't have a file in either Ogg soundtrack
		return true;
	}

	if (playlist[song_id - 1].song_flags & SONG_SPLIT)
	{
		// Play split-Ogg music (Cave Story 3D)
		// Get filenames
		const char* const song_name = playlist[song_id - 1].song_name;
		char song_base_file_path[strlen(song_name)+1];
		strcpy(song_base_file_path, song_name);
		char song_intro_file_path[strlen(song_base_file_path)+6+4+1];
		char song_loop_file_path[strlen(song_base_file_path)+5+4+1];
		strcpy(song_intro_file_path, song_base_file_path);
		strcpy(song_loop_file_path, song_base_file_path);
		strcat(song_intro_file_path, "_intro.ogg");
		strcat(song_loop_file_path, "_loop.ogg");

		// Load intro and loop
		music_intro = Mix_LoadMUS(song_intro_file_path);
		music_loop = Mix_LoadMUS(song_loop_file_path);

		if (music_intro == NULL && music_loop == NULL)
			return false;

		intro_playing = true;
		current_loop_setting = (playlist[song_id - 1].song_flags & SONG_LOOP) ? -1 : 0;

		// Play intro
		Mix_PlayMusic(music_intro, 0);
	}
	else
	{
		// Play single-Ogg music (Cave Story WiiWare)
		const char* const song_name = playlist[song_id - 1].song_name;
		char song_file_path[strlen(song_name)+4+1];
		strcpy(song_file_path, song_name);
		strcat(song_file_path, ".ogg");

		music_loop = Mix_LoadMUS(song_file_path);

		if (music_loop == NULL)
			return false;

		intro_playing = false;

		Mix_PlayMusic(music_loop, (playlist[song_id - 1].song_flags & SONG_LOOP) ? -1 : 0);
	}

	return true;
}

void PlayOrgMusic(const int music_id)
{
	*previous_song_last_position = GetOrgMusicPosition();
	sub_41C7F0();
	LoadOrgMusic(org_playlist[music_id]);
	SetOrgVolume(100);
	SetOrgMusicPosition(0);
	sub_41C790();
}

void PlayPreviousOrgMusic(void)
{
	sub_41C7F0();
	LoadOrgMusic(org_playlist[*previous_song_last_position]);
	SetOrgMusicPosition(*previous_song_last_position);
	SetOrgVolume(100);
	sub_41C790();
}

void __cdecl PlayMusic_new(const int music_id)
{
	if (music_id == 0 || music_id != *current_music)
	{
		*previous_music = *current_music;
		if (PlayOggMusic(music_id))
		{
			// Ogg music played successfully,
			// silence any org music that might be playing
			PlayOrgMusic(0);
		}
		else
		{
			// Ogg music failed to play,
			// play Org instead
			PlayOrgMusic(music_id);
		}
		*current_music = music_id;
	}
}

void __cdecl PlayPreviousMusic_new(void)
{
	if (PlayOggMusic(*previous_music))
	{
		// Ogg music played successfully,
		// silence any org music that might be playing
		PlayOrgMusic(0);
	}
	else
	{
		// Ogg music failed to play,
		// play Org instead
		PlayPreviousOrgMusic();
	}
	*current_music = *previous_music;
}

void __cdecl WindowFocusGained_new(void)
{
	Mix_ResumeMusic();
	sub_41C7F0();	// The instruction we hijacked to get here
}

void __cdecl WindowFocusLost_new(void)
{
	Mix_PauseMusic();
	sub_41C7F0();	// The instruction we hijacked to get here
}

void __cdecl FadeMusic_new(void)
{
	int* const music_fade_flag = (void*)0x4A4E10;

	*music_fade_flag = 1;
	intro_playing = false;	// A bit of a hack, but we can't have a new song kick in just because we faded out
	Mix_FadeOutMusic(1000 * 5);
}

void InitMod(void)
{
	const char* const playlist_filename = GetSetting("playlist");
	if (playlist_filename != NULL)
	{
		char playlist_path[strlen(location_path) + strlen(playlist_filename) + 1];
		strcpy(playlist_path, location_path);
		strcat(playlist_path, playlist_filename);
		LoadPlaylist(playlist_path);
	}

	// Setup music system
	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Init(MIX_INIT_OGG);
	Mix_HookMusicFinished(OggMusicEnded);
	// Replace PlayMusic and PlayPreviousMusic with our custom Ogg ones
	WriteRelativeAddress(0x40D833 + 1, PlayMusic_new);
	WriteRelativeAddress(0x40F756 + 1, PlayMusic_new);
	WriteRelativeAddress(0x40FE81 + 1, PlayMusic_new);
	WriteRelativeAddress(0x40FE96 + 1, PlayMusic_new);
	WriteRelativeAddress(0x40FEAB + 1, PlayMusic_new);
	WriteRelativeAddress(0x40FEC0 + 1, PlayMusic_new);
	WriteRelativeAddress(0x40FECC + 1, PlayMusic_new);
	WriteRelativeAddress(0x41038C + 1, PlayMusic_new);
	WriteRelativeAddress(0x41D3FF + 1, PlayMusic_new);
	WriteRelativeAddress(0x424330 + 1, PlayMusic_new);
	WriteRelativeAddress(0x4243DF + 1, PlayPreviousMusic_new);
	// We also need to replace the music pausing/resuming when the window focus changes
	WriteRelativeAddress(0x412BD6 + 1, WindowFocusLost_new);
	WriteRelativeAddress(0x412C06 + 1, WindowFocusGained_new);
	// Patch fading
	WriteRelativeAddress(0x40D84F + 1, FadeMusic_new);
	WriteRelativeAddress(0x42438A + 1, FadeMusic_new);
}
