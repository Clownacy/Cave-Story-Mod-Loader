// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include <stdbool.h>
#include <stddef.h>

#include "cave_story.h"
#include "mod_loader.h"

#include "playlist.h"
#include "audio_lib/mixer.h"

typedef struct Song
{
	Mixer_SoundInstanceID instance;
	Mixer_Sound *sound;
	bool is_org;
} Song;

static bool setting_preload;
static bool setting_predecode;
static bool setting_fade_in_previous_song;
static float setting_volume;

static Song current_song;
static Song previous_song;

static void LoadSong(PlaylistEntry *playlist_entry)
{
	if (!playlist_entry->is_org)
		playlist_entry->sound = Mixer_LoadSound(playlist_entry->name, playlist_entry->loop, setting_predecode);
}

static void PreloadSongs(void)
{
	for (PlaylistEntry *playlist_entry = GetFirstPlaylistEntry(); playlist_entry != NULL; playlist_entry = GetNextPlaylistEntry())
		LoadSong(playlist_entry);
}

static void UnloadSong(Song *song)
{
	Mixer_StopSound(song->instance);

	if (!setting_preload)
		Mixer_UnloadSound(song->sound);
}

static bool PlayOggMusic(const int song_id)
{
	bool success = false;

	PlaylistEntry *playlist_entry = GetPlaylistEntry(song_id);

	if (playlist_entry)
	{
		if (!setting_preload)
			LoadSong(playlist_entry);

		Mixer_Sound *sound = playlist_entry->sound;

		if (sound)
		{
			current_song.sound = sound;
			current_song.instance = Mixer_PlaySound(sound);
			Mixer_SetSoundVolume(current_song.instance, setting_volume);
			Mixer_UnpauseSound(current_song.instance);

			success = true;
		}
		else
		{
			ModLoader_PrintError("ogg_music: Could not load sound\n");
		}
	}

	return success;
}

static void PlayOrgMusic(const int music_id)
{
	CS_previous_song_last_position = CS_GetOrgMusicPosition();
	CS_sub_41C7F0();
	CS_LoadOrgMusic(CS_org_playlist[music_id]);
	CS_SetOrgVolume(100);
	CS_SetOrgMusicPosition(0);
	CS_StartOrgPlayback();
}

static void PlayPreviousOrgMusic(void)
{
	CS_sub_41C7F0();
	CS_LoadOrgMusic(CS_org_playlist[CS_previous_music]);
	CS_SetOrgMusicPosition(CS_previous_song_last_position);
	CS_SetOrgVolume(100);
	CS_StartOrgPlayback();
}

static void PlayMusic_new(const int music_id)
{
	if (music_id == 0 || music_id != CS_current_music)
	{
		CS_previous_music = CS_current_music;

		Mixer_PauseSound(current_song.instance);
		UnloadSong(&previous_song);
		previous_song = current_song;
		current_song = (Song){};

		if (music_id != 0 && PlayOggMusic(music_id - 1))
		{
			// Ogg music played successfully,
			// silence any org music that might be playing
			PlayOrgMusic(0);
			current_song.is_org = false;
		}
		else
		{
			// Ogg music failed to play,
			// play Org instead
			PlayOrgMusic(music_id);
			current_song.is_org = true;
		}
		CS_current_music = music_id;
	}
}

static void PlayPreviousMusic_new(void)
{
	UnloadSong(&current_song);
	current_song = previous_song;
	previous_song = (Song){};

	if (!current_song.is_org)
	{
		// Silence any Org music that might be playing
		PlayOrgMusic(0);

		if (setting_fade_in_previous_song)
			Mixer_FadeInSound(current_song.instance, 2 * 1000);

		Mixer_UnpauseSound(current_song.instance);
	}
	else
	{
		// Play Org instead
		PlayPreviousOrgMusic();
	}
	CS_current_music = CS_previous_music;
}

static void WindowFocusGained_new(void)
{
	Mixer_Unpause();
	CS_sub_41C7F0();	// The instruction we hijacked to get here
}

static void WindowFocusLost_new(void)
{
	Mixer_Pause();
	CS_sub_41C7F0();	// The instruction we hijacked to get here
}

static void FadeMusic_new(void)
{
	CS_music_fade_flag = 1;
	Mixer_FadeOutSound(current_song.instance, 5 * 1000);
}

void InitMod(void)
{
	setting_preload = ModLoader_GetSettingBool("preload_songs", false);
	setting_predecode = ModLoader_GetSettingBool("predecode_songs", false);
	setting_fade_in_previous_song = ModLoader_GetSettingBool("fade_in_previous_song", true);
	const bool setting_pause_when_focus_lost = ModLoader_GetSettingBool("pause_when_focus_lost", true);

	int volume = ModLoader_GetSettingInt("volume", 100);
	if (volume < 0)
		volume = 0;
	setting_volume = volume / 100.0f;

	if (InitPlaylist())
	{
		if (Mixer_Init())
		{
			if (setting_preload)
				PreloadSongs();
			// Replace PlayMusic and PlayPreviousMusic with our custom Ogg ones
			ModLoader_WriteJump(CS_PlayMusic, PlayMusic_new);
			ModLoader_WriteJump(CS_PlayPreviousMusic, PlayPreviousMusic_new);
			// We also need to replace the music pausing/resuming when the window focus changes
			if (setting_pause_when_focus_lost)
			{
				ModLoader_WriteRelativeAddress((void*)0x412BD6 + 1, WindowFocusLost_new);
				ModLoader_WriteRelativeAddress((void*)0x412C06 + 1, WindowFocusGained_new);
			}
			// Patch fading
			ModLoader_WriteJump(CS_FadeMusic, FadeMusic_new);
		}
		else
		{
			ModLoader_PrintError("ogg_music: Could not initialise backend\n");
		}
	}
}
