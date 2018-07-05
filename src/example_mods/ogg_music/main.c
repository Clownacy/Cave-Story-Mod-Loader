// Ogg music mod for Freeware Cave Story
// Copyright © 2018 Clownacy

#include <stdbool.h>
#include <stddef.h>

#include "cave_story.h"
#include "mod_loader.h"

#include "backend.h"
#include "playlist.h"
#include "song_file.h"

typedef struct Song {
	BackendStream *stream;
	SongFile *file;
	bool is_org;
} Song;

static bool setting_preload;
static bool setting_predecode;
static bool setting_fade_in_previous_song;

static const Song blank_song;

static Song current_song;
static Song previous_song;

static struct
{
	bool active;
	unsigned int counter;
} fade_out, fade_in;

// Just here so we can implcitly cast the user_data parameter without a warning
static unsigned long StreamCallback(void *user_data, void *output_buffer, unsigned long samples_to_do)
{
	return SongFile_GetSamples(user_data, output_buffer, samples_to_do);
}

static void LoadSong(PlaylistEntry *playlist_entry)
{
	if (!playlist_entry->is_org)
		playlist_entry->file = SongFile_Load(playlist_entry->name, playlist_entry->loops, setting_predecode);
}

static void PreloadSongs(void)
{
	for (PlaylistEntry *playlist_entry = GetFirstPlaylistEntry(); playlist_entry != NULL; playlist_entry = GetNextPlaylistEntry())
	{
		LoadSong(playlist_entry);
	}
}

static void UnloadSong(Song *song)
{
	if (Backend_DestroyStream(song->stream))
	{
		if (setting_preload)
			SongFile_Reset(song->file);
		else
			SongFile_Unload(song->file);
	}
}

static void PauseSong(void)
{
	Backend_PauseStream(current_song.stream);
}

static void ResumeSong(void)
{
	Backend_ResumeStream(current_song.stream);
}

static bool PlayOggMusic(const int song_id)
{
	bool success = false;

	PlaylistEntry *playlist_entry = GetPlaylistEntry(song_id);

	if (playlist_entry)
	{
		if (!setting_preload)
			LoadSong(playlist_entry);

		SongFile *song = playlist_entry->file;

		if (song)
		{
			unsigned int channels = SongFile_GetChannels(song);

			if (channels != 1 && channels != 2)
			{
				// Unsupported channel count
				ModLoader_PrintError("ogg_music: Unsupported channel count\n");

				if (!setting_preload)
					SongFile_Unload(song);
			}
			else
			{
				current_song.stream = Backend_CreateStream(SongFile_GetSampleRate(song), channels, song);

				if (current_song.stream == NULL)
				{
					if (!setting_preload)
						SongFile_Unload(song);
				}
				else
				{
					current_song.file = song;

					ResumeSong();

					success = true;
				}
			}
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
		Backend_SetVolume(current_song.stream, 1.0f);

		CS_previous_music = CS_current_music;

		PauseSong();

		UnloadSong(&previous_song);
		previous_song = current_song;
		current_song = blank_song;

		if (music_id == 0 || PlayOggMusic(music_id - 1))
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

		fade_out.active = false;
		fade_in.active = false;
	}
}

static void PlayPreviousMusic_new(void)
{
	if (setting_fade_in_previous_song)
	{
		Backend_SetVolume(current_song.stream, 0.0f);
		fade_in.active = true;
		fade_in.counter = 0;
	}

	if (!previous_song.is_org)
	{
		// Silence any Org music that might be playing
		PlayOrgMusic(0);

		UnloadSong(&current_song);
		current_song = previous_song;
		previous_song = blank_song;

		ResumeSong();
	}
	else
	{
		// Play Org instead
		PlayPreviousOrgMusic();
	}
	CS_current_music = CS_previous_music;

	fade_out.active = false;
}

static void WindowFocusGained_new(void)
{
	ResumeSong();
	CS_sub_41C7F0();	// The instruction we hijacked to get here
}

static void WindowFocusLost_new(void)
{
	PauseSong();
	CS_sub_41C7F0();	// The instruction we hijacked to get here
}

static void FadeMusic_new(void)
{
	CS_music_fade_flag = 1;
	fade_out.counter = 60 * 5;
	fade_out.active = true;
}

void UpdateMusicFade(void)
{
	if (fade_out.active)
	{
		const float volume_float = fade_out.counter / (60.0f * 5);
		Backend_SetVolume(current_song.stream, volume_float * volume_float);

		if (fade_out.counter-- == 0)
		{
			fade_out.active = false;
			PauseSong();
		}
	}
	else if (fade_in.active)
	{
		const float volume_float = fade_in.counter / (60.0f * 2);
		Backend_SetVolume(current_song.stream, volume_float * volume_float);

		if (fade_in.counter++ == 60 * 2)
		{
			fade_in.active = false;
		}
	}
}

__asm
(
"_UpdateMusicFade_asm:\n"
"	push	%eax\n"
"	call	_UpdateMusicFade\n"
"	pop	%eax\n"
"	ret\n"
);
extern char UpdateMusicFade_asm;

void InitMod(void)
{
	setting_preload = ModLoader_GetSettingBool("preload_songs", false);
	setting_predecode = ModLoader_GetSettingBool("predecode_songs", false);
	setting_fade_in_previous_song = ModLoader_GetSettingBool("fade_in_previous_song", true);

	if (InitPlaylist() && Backend_Init(StreamCallback))
	{
		if (setting_preload)
			PreloadSongs();
		// Replace PlayMusic and PlayPreviousMusic with our custom Ogg ones
		ModLoader_WriteJump(CS_PlayMusic, PlayMusic_new);
		ModLoader_WriteJump(CS_PlayPreviousMusic, PlayPreviousMusic_new);
		// We also need to replace the music pausing/resuming when the window focus changes
		ModLoader_WriteRelativeAddress((void*)0x412BD6 + 1, WindowFocusLost_new);
		ModLoader_WriteRelativeAddress((void*)0x412C06 + 1, WindowFocusGained_new);
		// Patch fading
		ModLoader_WriteJump(CS_FadeMusic, FadeMusic_new);
		// Insert hook for per-frame fade updating
		ModLoader_WriteJump((void*)0x40B44B, &UpdateMusicFade_asm);
	}
}
