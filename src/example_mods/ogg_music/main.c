// Ogg music mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdbool.h>
#include <string.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"

#include "cave_story.h"
#include "mod_loader.h"

#include "playlist.h"

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
	StartOrgPlayback();
}

void PlayPreviousOrgMusic(void)
{
	sub_41C7F0();
	LoadOrgMusic(org_playlist[*previous_music]);
	SetOrgMusicPosition(*previous_song_last_position);
	SetOrgVolume(100);
	StartOrgPlayback();
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
	WriteJump((int)PlayMusic, PlayMusic_new);
	WriteJump((int)PlayPreviousMusic, PlayPreviousMusic_new);
	// We also need to replace the music pausing/resuming when the window focus changes
	WriteRelativeAddress(0x412BD6 + 1, WindowFocusLost_new);
	WriteRelativeAddress(0x412C06 + 1, WindowFocusGained_new);
	// Patch fading
	WriteJump((int)FadeMusic, FadeMusic_new);
}
