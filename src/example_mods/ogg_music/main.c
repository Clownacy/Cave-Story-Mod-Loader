// Ogg music mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <cubeb/cubeb.h>

#include "cave_story.h"
#include "mod_loader.h"

#include "playlist.h"
#include "song_file.h"

typedef struct Song {
	cubeb_stream *stream;
	SongFile *file;
	bool is_org;
} Song;

static bool setting_preload;
static bool setting_fade_in_previous_song;

static const Song blank_song;

static Song current_song;
static Song previous_song;

static cubeb *cubeb_context;

static struct
{
	bool active;
	unsigned int counter;
} fade_out, fade_in;

static void LoadSong(PlaylistEntry *playlist_entry)
{
	if (!playlist_entry->is_org)
		playlist_entry->file = SongFile_Load(playlist_entry->name, playlist_entry->loops);
}

static void PreloadSongs(void)
{
	for (PlaylistEntry *playlist_entry = GetFirstPlaylistEntry(); playlist_entry != NULL; playlist_entry = GetNextPlaylistEntry())
	{
		LoadSong(playlist_entry);
	}
}

static long data_cb(cubeb_stream *stream, void *user_data, void const *input_buffer, void *output_buffer, long samples_to_do)
{
	return SongFile_GetSamples(current_song.file, output_buffer, samples_to_do);
}

static void state_cb(cubeb_stream *stream, void *user, cubeb_state state)
{

}

static void UnloadSong(Song *song)
{
	if (song->stream)
	{
		if (cubeb_stream_stop(song->stream) != CUBEB_OK)
		{
			ModLoader_PrintError("ogg_music: Could not stop the stream\n");
		}
		else
		{
			cubeb_stream_destroy(song->stream);

			if (setting_preload)
				SongFile_Reset(song->file);
			else
				SongFile_Unload(song->file);
		}
	}
}

static void PauseSong(void)
{
	if (current_song.stream)
	{
		if (cubeb_stream_stop(current_song.stream) != CUBEB_OK)
		{
			ModLoader_PrintError("ogg_music: Could not stop the stream\n");
		}
	}
}

static void ResumeSong(void)
{
	if (current_song.stream)
	{
		if (cubeb_stream_start(current_song.stream) != CUBEB_OK)
		{
			ModLoader_PrintError("ogg_music: Could not start the stream\n");
		}
	}
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
				cubeb_stream_params output_params;
				output_params.format = CUBEB_SAMPLE_S16LE;
				output_params.rate = SongFile_GetRate(song);
				output_params.prefs = CUBEB_STREAM_PREF_NONE;
				output_params.channels = channels;
				if (channels == 1)
				{
					output_params.layout = CUBEB_LAYOUT_MONO;
				}
				else// if (channels == 2)
				{
					output_params.layout = CUBEB_LAYOUT_STEREO;
				}

				uint32_t latency_frames;

				if (cubeb_get_min_latency(cubeb_context, &output_params, &latency_frames) != CUBEB_OK)
				{
					ModLoader_PrintError("ogg_music: Could not get minimum latency\n");

					if (!setting_preload)
						SongFile_Unload(song);
				}
				else
				{
					if (cubeb_stream_init(cubeb_context, &current_song.stream, "Main Stream", NULL, NULL, NULL, &output_params, latency_frames, data_cb, state_cb, NULL) != CUBEB_OK)
					{
						ModLoader_PrintError("ogg_music: Could not open the stream\n");

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
		cubeb_stream_set_volume(current_song.stream, 1.0f);

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
		cubeb_stream_set_volume(current_song.stream, 0.0f);
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
		if (current_song.stream)
		{
			const float volume_float = fade_out.counter / (60.0f * 5);
			cubeb_stream_set_volume(current_song.stream, volume_float * volume_float	);
		}

		if (fade_out.counter-- == 0)
		{
			fade_out.active = false;
			PauseSong();
		}
	}
	else if (fade_in.active)
	{
		if (current_song.stream)
		{
			const float volume_float = fade_in.counter / (60.0f * 2);
			cubeb_stream_set_volume(current_song.stream, volume_float * volume_float);
		}

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

static bool InitBackend(void)
{
	bool success = true;

	CoInitializeEx(NULL, COINIT_MULTITHREADED);	// Cubeb needs us to init COM

	if (cubeb_init(&cubeb_context, "Ogg player for Cave Story", NULL) != CUBEB_OK)
		success = false;

	return success;
}

void InitMod(void)
{
	setting_preload = ModLoader_GetSettingBool("preload_oggs", false);
	setting_fade_in_previous_song = ModLoader_GetSettingBool("fade_in_previous_song", true);

	if (InitPlaylist() && InitBackend())
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
