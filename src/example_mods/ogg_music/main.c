// Ogg music mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <cubeb/cubeb.h>
#include <vorbis/vorbisfile.h>

#include "cave_story.h"
#include "mod_loader.h"
#include "sprintfMalloc.h"

#include "playlist.h"
#include "ram_file.h"

typedef struct Song {
	RAMFile *file[2];
	OggVorbis_File vorbis_file[2];

	unsigned int current_file;

	int current_section[2];
	unsigned int channels;
	bool has_next_part;
	bool loops;
	bool is_org;
	unsigned int sample_rate;

	cubeb_stream *stream;
} Song;

static bool setting_preload;
static bool setting_fade_in_previous_song;

static const Song song_blank;

static Song song;
static Song song_backup;

static cubeb *cubeb_context;

static struct
{
	bool active;
	unsigned int counter;
} fade_out, fade_in;

typedef struct PreloadData
{
	RAMFile *file_intro;
	RAMFile *file_loop;
} PreloadData;

static void PreloadSongs(void)
{
	if (setting_preload)
	{
		for (PlaylistEntry *playlist_entry = GetFirstPlaylistEntry(); playlist_entry != NULL; playlist_entry = GetNextPlaylistEntry())
		{
			playlist_entry->backend_data = malloc(sizeof(PreloadData));

			if (playlist_entry->split)
			{
				// Play split-Ogg music (Cave Story 3D)
				char *file_path;
				PreloadData *preload_data;

				file_path = sprintfMalloc("%s_intro.ogg", playlist_entry->name);
				preload_data = playlist_entry->backend_data;
				preload_data->file_intro = RAMFile_fopen(file_path);
				free(file_path);

				file_path = sprintfMalloc("%s_loop.ogg", playlist_entry->name);
				preload_data = playlist_entry->backend_data;
				preload_data->file_loop = RAMFile_fopen(file_path);
				free(file_path);
			}
			else
			{
				// Play single-Ogg music (Cave Story WiiWare)
				char *file_path = sprintfMalloc("%s.ogg", playlist_entry->name);
				PreloadData *preload_data = playlist_entry->backend_data;
				preload_data->file_intro = RAMFile_fopen(file_path);
				free(file_path);
			}
		}
	}
}

static ov_callbacks ov_callback_memory = {
	(size_t (*)(void *, size_t, size_t, void *))  RAMFile_fread,
	(int (*)(void *, ogg_int64_t, int))           RAMFile_fseek,
	(int (*)(void *))                             NULL,
	(long (*)(void *))                            RAMFile_ftell
};

static long data_cb(cubeb_stream *stream, void *user_data, void const *input_buffer, void *output_buffer, long samples_to_do)
{
	const unsigned int BYTES_PER_SAMPLE = song.channels * 2;	// 2 channels, 16-bit
	const unsigned long bytes_to_do = samples_to_do * BYTES_PER_SAMPLE;

	unsigned long bytes_done_total = 0;

	for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
	{
		bytes_done = ov_read(&song.vorbis_file[song.current_file], output_buffer + bytes_done_total, bytes_to_do - bytes_done_total, 0, 2, 1, &song.current_section[song.current_file]);

		if (bytes_done == 0)	// EOF
		{
			if (song.has_next_part)
			{
				song.has_next_part = false;
				++song.current_file;
			}
			else
			{
				if (song.loops)
					ov_time_seek(&song.vorbis_file[song.current_file], 0);
				else
					break;
			}
		}
	}

	return bytes_done_total / BYTES_PER_SAMPLE;
}

static void state_cb(cubeb_stream *stm, void *user, cubeb_state state)
{

}

static void UnloadSong(Song *song)
{
	if (song->stream)
	{
		if (cubeb_stream_stop(song->stream) != CUBEB_OK)
		{
			PrintError("ogg_music: Could not stop the stream\n");
		}
		else
		{
			cubeb_stream_destroy(song->stream);

			ov_clear(&song->vorbis_file[0]);
			ov_clear(&song->vorbis_file[1]);

			if (setting_preload)
			{
				if (song->file[0])
					RAMFile_fseek(song->file[0], 0, SEEK_SET);
				if (song->file[1])
					RAMFile_fseek(song->file[1], 0, SEEK_SET);
			}
			else
			{
				if (song->file[0])
					RAMFile_fclose(song->file[0]);
				if (song->file[1])
					RAMFile_fclose(song->file[1]);
			}
		}
	}
}

static void StopSong(void)
{
	if (song.stream)
	{
		if (cubeb_stream_stop(song.stream) != CUBEB_OK)
		{
			PrintError("ogg_music: Could not stop the stream\n");
		}
	}
}

static void StartSong(void)
{
	if (song.stream)
	{
		if (cubeb_stream_start(song.stream) != CUBEB_OK)
		{
			PrintError("ogg_music: Could not start the stream\n");
		}
	}
}

static bool LoadSong(const int song_id)
{
	PlaylistEntry *playlist_entry = GetPlaylistEntry(song_id);

	if (playlist_entry == NULL)
	{
		return false;
	}
	else
	{
		if (setting_preload)
		{
			PreloadData *preload_data = GetPlaylistEntry(song_id)->backend_data;

			song.file[0] = preload_data->file_intro;
			song.file[1] = preload_data->file_loop;
		}
		else
		{
			const char* const song_name = playlist_entry->name;

			if (playlist_entry->split)
			{
				// Play split-Ogg music (Cave Story 3D)
				char *file_path = sprintfMalloc("%s_intro.ogg", song_name);
				song.file[0] = RAMFile_fopen(file_path);
				free(file_path);

				file_path = sprintfMalloc("%s_loop.ogg", song_name);
				song.file[1] = RAMFile_fopen(file_path);
				free(file_path);
			}
			else
			{
				// Play single-Ogg music (Cave Story WiiWare)
				char *file_path = sprintfMalloc("%s.ogg", song_name);
				song.file[0] = RAMFile_fopen(file_path);
				free(file_path);

				song.file[1] = NULL;
			}
		}

		if (song.file[0] == NULL && song.file[1] == NULL)
		{
			// Neither file could be opened
			return false;
		}
		else if ((song.file[0] == NULL) != (song.file[1] == NULL))
		{
			// Only one file could be opened
			song.has_next_part = false;
			song.current_file = song.file[0] == NULL ? 1 : 0;
		}
		else
		{
			// Both files opened successfully
			song.has_next_part = true;
			song.current_file = 0;
		}

		if (ov_open_callbacks(song.file[song.current_file], &song.vorbis_file[song.current_file], NULL, 0, ov_callback_memory) < 0)
		{
			PrintError("ogg_music: Input does not appear to be an Ogg bitstream.\n");

			goto Fail;
		}

		if (song.has_next_part)
		{
			if (ov_open_callbacks(song.file[1], &song.vorbis_file[1], NULL, 0, ov_callback_memory) < 0)
			{
				PrintError("ogg_music: Input does not appear to be an Ogg bitstream.\n");

				goto Fail;
			}
		}

		song.channels = ov_info(&song.vorbis_file[song.current_file], -1)->channels;

		cubeb_stream_params output_params;
		output_params.format = CUBEB_SAMPLE_S16LE;
		output_params.rate = song.sample_rate = ov_info(&song.vorbis_file[song.current_file], -1)->rate;
		output_params.prefs = CUBEB_STREAM_PREF_NONE;
		if (song.channels == 1)
		{
			output_params.channels = 1;
			output_params.layout = CUBEB_LAYOUT_MONO;
		}
		else if (song.channels == 2)
		{
			output_params.channels = 2;
			output_params.layout = CUBEB_LAYOUT_STEREO;
		}
		else
		{
			// Unsupported channel count
			PrintError("ogg_music: Unsupported channel count\n");

			goto Fail;
		}

		uint32_t latency_frames;

		if (cubeb_get_min_latency(cubeb_context, &output_params, &latency_frames) != CUBEB_OK)
		{
			PrintError("ogg_music: Could not get minimum latency\n");

			goto Fail;
		}

		if (cubeb_stream_init(cubeb_context, &song.stream, "Main Stream", NULL, NULL, NULL, &output_params, latency_frames, data_cb, state_cb, NULL) != CUBEB_OK)
		{
			PrintError("ogg_music: Could not open the stream\n");

			goto Fail;
		}

		song.loops = playlist_entry->loops;

		return true;

		Fail:
		if (!setting_preload)
		{
			if (song.file[0] != NULL)
				RAMFile_fclose(song.file[0]);
			if (song.file[1] != NULL)
				RAMFile_fclose(song.file[1]);
		}

		return false;
	}
}

static bool PlayOggMusic(const int song_id)
{
	bool success;

	if (LoadSong(song_id))
	{
		StartSong();

		success = true;
	}
	else
	{
		success = false;
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

		StopSong();

		UnloadSong(&song_backup);
		song_backup = song;
		song = song_blank;

		PlaylistEntry *playlist_entry = GetPlaylistEntry(music_id - 1);

		if (music_id == 0 || (playlist_entry && !playlist_entry->is_org && PlayOggMusic(music_id - 1)))
		{
			// Ogg music played successfully,
			// silence any org music that might be playing
			PlayOrgMusic(0);
		}
		else
		{
			// Ogg music failed to play,
			// play Org instead
			song.is_org = true;
			PlayOrgMusic(music_id);
		}
		CS_current_music = music_id;

		fade_out.active = false;
		fade_in.active = false;
	}
}

static void PlayPreviousMusic_new(void)
{
	cubeb_stream_set_volume(song.stream, 0.0f);

	if (!song_backup.is_org)
	{
		// Silence any Org music that might be playing
		PlayOrgMusic(0);

		UnloadSong(&song);
		song = song_backup;
		song_backup = song_blank;

		StartSong();
	}
	else
	{
		// Play Org instead
		PlayPreviousOrgMusic();
	}
	CS_current_music = CS_previous_music;

	fade_out.active = false;
	if (setting_fade_in_previous_song)
	{
		fade_in.active = true;
		fade_in.counter = 0;
	}
}

static void WindowFocusGained_new(void)
{
	StartSong();
	CS_sub_41C7F0();	// The instruction we hijacked to get here
}

static void WindowFocusLost_new(void)
{
	StopSong();
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
		if (song.stream)
		{
			const float volume_float = fade_out.counter / (60.0f * 5);
			cubeb_stream_set_volume(song.stream, volume_float * volume_float	);
		}

		if (fade_out.counter-- == 0)
		{
			fade_out.active = false;
			StopSong();
		}
	}
	else if (fade_in.active)
	{
		if (song.stream)
		{
			const float volume_float = fade_in.counter / (60.0f * 2);
			cubeb_stream_set_volume(song.stream, volume_float * volume_float);
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
	setting_preload = GetSettingBool("preload_oggs", false);
	setting_fade_in_previous_song = GetSettingBool("fade_in_previous_song", true);

	if (InitPlaylist() && InitBackend())
	{
		PreloadSongs();
		// Replace PlayMusic and PlayPreviousMusic with our custom Ogg ones
		WriteJump(CS_PlayMusic, PlayMusic_new);
		WriteJump(CS_PlayPreviousMusic, PlayPreviousMusic_new);
		// We also need to replace the music pausing/resuming when the window focus changes
		WriteRelativeAddress((void*)0x412BD6 + 1, WindowFocusLost_new);
		WriteRelativeAddress((void*)0x412C06 + 1, WindowFocusGained_new);
		// Patch fading
		WriteJump(CS_FadeMusic, FadeMusic_new);
		// Insert hook for per-frame fade updating
		WriteJump((void*)0x40B44B, &UpdateMusicFade_asm);
	}
}
