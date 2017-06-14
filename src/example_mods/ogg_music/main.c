// Ogg music mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <cubeb/cubeb.h>
#include <vorbis/vorbisfile.h>

#include "cave_story.h"
#include "mod_loader.h"
#include "sprintfMalloc.h"

#include "playlist.h"

typedef struct Song {
	FILE *file[2];
	OggVorbis_File vorbis_file[2];

	unsigned int current_file;

	int current_section[2];
	unsigned int channels;
	bool has_next_part;
	bool loops;
	bool is_org;

	cubeb_stream *stream;
} Song;

const Song song_blank;

Song song;
Song song_backup;

cubeb *cubeb_context;

int current_loop_setting;

long data_cb(cubeb_stream *stream, void *user_data, void const *input_buffer, void *output_buffer, long samples_to_do)
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

void state_cb(cubeb_stream * stm, void * user, cubeb_state state)
{
	if (state == CUBEB_STATE_STARTED)
		printf("state = CUBEB_STATE_STARTED\n");
	else if (state == CUBEB_STATE_STOPPED)
		printf("state = CUBEB_STATE_STOPPED\n");
	else if (state == CUBEB_STATE_DRAINED)
	{
		printf("state = CUBEB_STATE_DRAINED\n");
	}
	else if (state == CUBEB_STATE_ERROR)
		printf("state = CUBEB_STATE_ERROR\n");
}

void UnloadSong(Song *song)
{
	if (song->stream)
	{
		if (cubeb_stream_stop(song->stream) != CUBEB_OK)
		{
			printf("Could not stop the stream");
			return;
		}
		cubeb_stream_destroy(song->stream);

		ov_clear(&song->vorbis_file[0]);
		fclose(song->file[0]);
		ov_clear(&song->vorbis_file[1]);
		fclose(song->file[1]);
	}
}

void StopSong(void)
{
	if (song.stream)
	{
		if (cubeb_stream_stop(song.stream) != CUBEB_OK)
		{
			printf("Could not stop the stream");
		}
	}
}

void StartSong(void)
{
	if (song.stream)
	{
		if (cubeb_stream_start(song.stream) != CUBEB_OK)
		{
			printf("Could not start the stream");
		}
	}
}

bool LoadSong(char *intro_file_path, char *loop_file_path, bool loops)
{
	if (intro_file_path == NULL && loop_file_path == NULL)
	{
		// WTF, why did you call this in the first place?
		return false;
	}

	song.file[0] = intro_file_path ? fopen(intro_file_path, "rb") : NULL;
	song.file[1] = loop_file_path ? fopen(loop_file_path, "rb") : NULL;

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

	if (ov_open_callbacks(song.file[song.current_file], &song.vorbis_file[song.current_file], NULL, 0, OV_CALLBACKS_NOCLOSE) < 0)
	{
		printf("Input does not appear to be an Ogg bitstream.\n");

		if (song.file[0] != NULL)
			fclose(song.file[0]);
		if (song.file[1] != NULL)
			fclose(song.file[1]);

		return false;
	}

	if (song.has_next_part)
	{
		if (ov_open_callbacks(song.file[1], &song.vorbis_file[1], NULL, 0, OV_CALLBACKS_NOCLOSE) < 0)
		{
			printf("Input does not appear to be an Ogg bitstream.\n");

			if (song.file[0] != NULL)
				fclose(song.file[0]);
			if (song.file[1] != NULL)
				fclose(song.file[1]);

			return false;
		}
	}

	song.channels = ov_info(&song.vorbis_file[song.current_file], -1)->channels;

	cubeb_stream_params output_params;
	output_params.format = CUBEB_SAMPLE_S16LE;
	output_params.rate = ov_info(&song.vorbis_file[song.current_file], -1)->rate;
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
		printf("Unsupported channel count\n");

		if (song.file[0] != NULL)
			fclose(song.file[0]);
		if (song.file[1] != NULL)
			fclose(song.file[1]);

		return false;
	}

	uint32_t latency_frames;

	if (cubeb_get_min_latency(cubeb_context, output_params, &latency_frames) != CUBEB_OK)
	{
		printf("Could not get minimum latency");

		if (song.file[0] != NULL)
			fclose(song.file[0]);
		if (song.file[1] != NULL)
			fclose(song.file[1]);

		return false;
	}

	if (cubeb_stream_init(cubeb_context, &song.stream, "Example Stream 1", NULL, NULL, NULL, &output_params, latency_frames, data_cb, state_cb, NULL) != CUBEB_OK)
	{
		printf("Could not open the stream");

		if (song.file[0] != NULL)
			fclose(song.file[0]);
		if (song.file[1] != NULL)
			fclose(song.file[1]);

		return false;
	}

	song.loops = loops;

	return true;
}

bool PlayOggMusic(const int song_id)
{
	if (song_id == 0)
	{
		// This is basically a 'stop music' command.
		// In the Org soundtrack, this is implemented with a dummy 'XXXX' file.
		// We don't need to cheat, so we just kill the current music and be
		// done with it.
		return true;
	}

	const char* const song_name = playlist[song_id - 1].name;
	char *song_intro_file_path, *song_loop_file_path;

	if (playlist[song_id - 1].split)
	{
		// Play split-Ogg music (Cave Story 3D)
		song_intro_file_path = sprintfMalloc("%s_intro.ogg", song_name);
		song_loop_file_path = sprintfMalloc("%s_loop.ogg", song_name);
	}
	else
	{
		// Play single-Ogg music (Cave Story WiiWare)
		song_intro_file_path = sprintfMalloc("%s.ogg", song_name);
		song_loop_file_path = NULL;
	}

	if (!LoadSong(song_intro_file_path, song_loop_file_path, playlist[song_id - 1].loops))
	{
		if (song_intro_file_path != NULL)
			free(song_intro_file_path);
		if (song_loop_file_path != NULL)
			free(song_loop_file_path);

		return false;
	}

	if (song_intro_file_path != NULL)
		free(song_intro_file_path);
	if (song_loop_file_path != NULL)
		free(song_loop_file_path);

	StartSong();

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

		StopSong();

		UnloadSong(&song_backup);
		song_backup = song;
		song = song_blank;

		if (!playlist[music_id - 1].is_org && PlayOggMusic(music_id))
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
		*current_music = music_id;
	}
}

void __cdecl PlayPreviousMusic_new(void)
{
	if (!song_backup.is_org)
	{
		// Ogg music played successfully,
		// silence any org music that might be playing
		PlayOrgMusic(0);

		UnloadSong(&song);
		song = song_backup;
		song_backup = song_blank;
		
		StartSong();
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
	StartSong();
	sub_41C7F0();	// The instruction we hijacked to get here
}

void __cdecl WindowFocusLost_new(void)
{
	StopSong();
	sub_41C7F0();	// The instruction we hijacked to get here
}

void __cdecl FadeMusic_new(void)
{
	*music_fade_flag = 1;
//	intro_playing = false;	// A bit of a hack, but we can't have a new song kick in just because we faded out
//	Mix_FadeOutMusic(1000 * 5);
}

void InitMod(void)
{
	const char* const playlist_filename = GetSettingString("playlist");
	if (playlist_filename == NULL)
	{
		return;
	}

	char playlist_path[strlen(location_path) + strlen(playlist_filename) + 1];
	strcpy(playlist_path, location_path);
	strcat(playlist_path, playlist_filename);
	LoadPlaylist(playlist_path);

	// Setup music system
	cubeb_context;
	cubeb_init(&cubeb_context, "Ogg player for Cave Story", NULL);
	// Replace PlayMusic and PlayPreviousMusic with our custom Ogg ones
	WriteJump(PlayMusic, PlayMusic_new);
	WriteJump(PlayPreviousMusic, PlayPreviousMusic_new);
	// We also need to replace the music pausing/resuming when the window focus changes
	WriteRelativeAddress((void*)0x412BD6 + 1, WindowFocusLost_new);
	WriteRelativeAddress((void*)0x412C06 + 1, WindowFocusGained_new);
	// Patch fading
	WriteJump(FadeMusic, FadeMusic_new);
}
