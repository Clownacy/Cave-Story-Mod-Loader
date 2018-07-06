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

	bool fade_in_active;
	bool fade_out_active;
	unsigned int fade_counter;
} Song;

static bool setting_preload;
static bool setting_predecode;
static bool setting_fade_in_previous_song;

static const Song blank_song;

static Song current_song;
static Song previous_song;

static unsigned long StreamCallback(void *user_data, void *output_buffer, unsigned long bytes_to_do)
{
	Song *song = user_data;

	unsigned long bytes_done = SongFile_GetSamples(song->file, output_buffer, bytes_to_do);

	if (song->fade_out_active)
	{
		const unsigned int channels = SongFile_GetChannelCount(song->file);
		const unsigned int fade_counter_max = SongFile_GetSampleRate(song->file) * 5;

		short *output_buffer_short = output_buffer;
		for (unsigned int i = 0; i < bytes_done / channels / sizeof(short); ++i)
		{
			const float volume_float = song->fade_counter / (float)fade_counter_max;

			for (unsigned int i = 0; i < channels; ++i)
				*output_buffer_short++ *= volume_float * volume_float;

			if (song->fade_counter)
				--song->fade_counter;
		}
	}
	else if (song->fade_in_active)
	{
		const unsigned int channels = SongFile_GetChannelCount(song->file);
		const unsigned int fade_counter_max = SongFile_GetSampleRate(song->file) * 2;

		short *output_buffer_short = output_buffer;
		for (unsigned int i = 0; i < bytes_done / channels / sizeof(short); ++i)
		{
			const float volume_float = song->fade_counter / (float)fade_counter_max;

			for (unsigned int i = 0; i < channels; ++i)
				*output_buffer_short++ *= volume_float * volume_float;

			if (++song->fade_counter == fade_counter_max)
			{
				song->fade_in_active = false;
				break;
			}
		}
	}

	return bytes_done;
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
	else
	{
		ModLoader_PrintError("ogg_music: Could not destroy the stream\n");
	}
}

static void PauseSong(BackendStream *stream)
{
	if (!Backend_PauseStream(stream))
		ModLoader_PrintError("ogg_music: Could not pause the stream\n");
}

static void ResumeSong(BackendStream *stream)
{
	if (!Backend_ResumeStream(stream))
		ModLoader_PrintError("ogg_music: Could not resume the stream\n");
}

static bool PlayOggMusic(const int song_id)
{
	bool success = false;

	PlaylistEntry *playlist_entry = GetPlaylistEntry(song_id);

	if (playlist_entry)
	{
		if (!setting_preload)
			LoadSong(playlist_entry);

		SongFile *song_file = playlist_entry->file;

		if (song_file)
		{
			unsigned int channels = SongFile_GetChannelCount(song_file);

			if (channels != 1 && channels != 2)
			{
				// Unsupported channel count
				ModLoader_PrintError("ogg_music: Unsupported channel count\n");

				if (!setting_preload)
					SongFile_Unload(song_file);
			}
			else
			{
				current_song.stream = Backend_CreateStream(SongFile_GetSampleRate(song_file), channels, StreamCallback, &current_song);

				if (current_song.stream == NULL)
				{
					ModLoader_PrintError("ogg_music: Could not create the stream\n");

					if (!setting_preload)
						SongFile_Unload(song_file);
				}
				else
				{
					current_song.file = song_file;

					ResumeSong(current_song.stream);

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
		CS_previous_music = CS_current_music;

		PauseSong(current_song.stream);
		UnloadSong(&previous_song);
		previous_song = current_song;
		current_song = blank_song;

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

		current_song.fade_out_active = false;
		current_song.fade_in_active = false;
	}
}

static void PlayPreviousMusic_new(void)
{
	UnloadSong(&current_song);
	current_song = previous_song;
	previous_song = blank_song;

	if (!current_song.is_org)
	{
		// Silence any Org music that might be playing
		PlayOrgMusic(0);

		if (setting_fade_in_previous_song)
		{
			current_song.fade_counter = 0;
			current_song.fade_in_active = true;
		}

		ResumeSong(current_song.stream);
	}
	else
	{
		// Play Org instead
		PlayPreviousOrgMusic();
	}
	CS_current_music = CS_previous_music;

	current_song.fade_out_active = false;
}

static void WindowFocusGained_new(void)
{
	ResumeSong(current_song.stream);
	CS_sub_41C7F0();	// The instruction we hijacked to get here
}

static void WindowFocusLost_new(void)
{
	PauseSong(current_song.stream);
	CS_sub_41C7F0();	// The instruction we hijacked to get here
}

static void FadeMusic_new(void)
{
	CS_music_fade_flag = 1;
	current_song.fade_counter = (SongFile_GetSampleRate(current_song.file) * 5) - 1;
	current_song.fade_out_active = true;
}

void InitMod(void)
{
	setting_preload = ModLoader_GetSettingBool("preload_songs", false);
	setting_predecode = ModLoader_GetSettingBool("predecode_songs", false);
	setting_fade_in_previous_song = ModLoader_GetSettingBool("fade_in_previous_song", true);

	if (InitPlaylist())
	{
		if (Backend_Init())
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
		}
		else
		{
			ModLoader_PrintError("ogg_music: Could not initialise backend\n");
		}
	}
}
