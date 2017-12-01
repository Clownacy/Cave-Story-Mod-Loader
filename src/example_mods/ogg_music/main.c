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

typedef struct VorbisMemoryFile
{
	char *data;
	size_t data_size;
	signed long current_offset;
} VorbisMemoryFile;

typedef struct Song {
	VorbisMemoryFile *file[2];
	OggVorbis_File vorbis_file[2];
	char *file_buffer[2];

	unsigned int current_file;

	int current_section[2];
	unsigned int channels;
	bool has_next_part;
	bool loops;
	bool is_org;
	unsigned int sample_rate;

	cubeb_stream *stream;
} Song;

static const Song song_blank;

static Song song;
static Song song_backup;

static cubeb *cubeb_context;

static int current_loop_setting;

static struct
{
	bool active;
	unsigned int counter;
	unsigned int volume;
	uint16_t nonlinear_volume_table[100];
} fade;

static VorbisMemoryFile* VorbisMemoryFile_FOpen(char *file_path)
{
	VorbisMemoryFile *memory_file = NULL;

	FILE *file = fopen(file_path, "rb");
	if (file != NULL)
	{
		memory_file = malloc(sizeof(VorbisMemoryFile));

		fseek(file, 0, SEEK_END);
		memory_file->data_size = ftell(file);
		rewind(file);

		memory_file->data = malloc(memory_file->data_size);

		fread(memory_file->data, 1, memory_file->data_size, file);

		fclose(file);

		memory_file->current_offset = 0;
	}

	return memory_file;
}

static size_t VorbisMemoryFile_FRead(void *output, size_t size, size_t count, VorbisMemoryFile *file)
{
	const unsigned int elements_remaining = (file->data_size - file->current_offset) / size;

	if (count > elements_remaining)
		count = elements_remaining;

	memcpy(output, file->data + file->current_offset, size * count);

	file->current_offset += size * count;

	return count;
}

static int VorbisMemoryFile_FSeek(VorbisMemoryFile *file, ogg_int64_t offset, int origin)
{
	switch (origin)
	{
		case SEEK_SET:
		{
			file->current_offset = offset;
			break;
		}
		case SEEK_CUR:
		{
			file->current_offset += offset;
			break;
		}
		case SEEK_END:
		{
			file->current_offset = file->data_size + offset;
			break;
		}
		default:
		{
			return -1;
		}
	}

	return 0;
}

static int VorbisMemoryFile_FClose(VorbisMemoryFile *file)
{
	free(file->data);
	free(file);

	return 0;
}

static long VorbisMemoryFile_FTell(VorbisMemoryFile *file)
{
	return file->current_offset;
}

static ov_callbacks ov_callback_memory = {
  (size_t (*)(void *, size_t, size_t, void *))  VorbisMemoryFile_FRead,
  (int (*)(void *, ogg_int64_t, int))           VorbisMemoryFile_FSeek,
  (int (*)(void *))                             VorbisMemoryFile_FClose,
  (long (*)(void *))                            VorbisMemoryFile_FTell
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

	int16_t *output_buffer_short = (int16_t*)output_buffer;
	unsigned long samples_done = bytes_done_total / BYTES_PER_SAMPLE;

	for (unsigned int i = 0; i < samples_done; ++i)
	{
		for (unsigned int j = 0; j < song.channels; ++j)
		{
			int16_t sample = (signed int)((*output_buffer_short) * fade.nonlinear_volume_table[fade.volume - 1]) / 100;
			*output_buffer_short++ = sample;
		}

		if (fade.active)
		{
			if (fade.counter-- == 0)
			{
				if (--fade.volume == 0)
				{
					fade.active = false;
				}
				else
				{
					fade.counter = (song.sample_rate * 5) / 100;
				}
			}
		}
	}

	return samples_done;
}

static void state_cb(cubeb_stream * stm, void * user, cubeb_state state)
{

}

static void UnloadSong(Song *song)
{
	if (song->stream)
	{
		if (cubeb_stream_stop(song->stream) != CUBEB_OK)
		{
			PrintError("ogg_music: Could not stop the stream\n");
			return;
		}
		cubeb_stream_destroy(song->stream);

		ov_clear(&song->vorbis_file[0]);
		ov_clear(&song->vorbis_file[1]);

		free(song->file_buffer[0]);
		free(song->file_buffer[1]);
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

static bool LoadSong(char *intro_file_path, char *loop_file_path, bool loops)
{
	if (intro_file_path == NULL && loop_file_path == NULL)
	{
		// WTF, why did you call this in the first place?
		return false;
	}

	song.file[0] = intro_file_path ? VorbisMemoryFile_FOpen(intro_file_path) : NULL;
	song.file[1] = loop_file_path ? VorbisMemoryFile_FOpen(loop_file_path) : NULL;

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

		if (song.file[0] != NULL)
			VorbisMemoryFile_FClose(song.file[0]);
		if (song.file[1] != NULL)
			VorbisMemoryFile_FClose(song.file[1]);

		return false;
	}

	if (song.has_next_part)
	{
		if (ov_open_callbacks(song.file[1], &song.vorbis_file[1], NULL, 0, ov_callback_memory) < 0)
		{
			PrintError("ogg_music: Input does not appear to be an Ogg bitstream.\n");

			if (song.file[0] != NULL)
				VorbisMemoryFile_FClose(song.file[0]);
			if (song.file[1] != NULL)
				VorbisMemoryFile_FClose(song.file[1]);

			return false;
		}
	}

	song.channels = ov_info(&song.vorbis_file[song.current_file], -1)->channels;

	cubeb_stream_params output_params;
	output_params.format = CUBEB_SAMPLE_S16LE;
	output_params.rate = song.sample_rate = ov_info(&song.vorbis_file[song.current_file], -1)->rate;
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

		if (song.file[0] != NULL)
			VorbisMemoryFile_FClose(song.file[0]);
		if (song.file[1] != NULL)
			VorbisMemoryFile_FClose(song.file[1]);

		return false;
	}

	uint32_t latency_frames;

	if (cubeb_get_min_latency(cubeb_context, &output_params, &latency_frames) != CUBEB_OK)
	{
		PrintError("ogg_music: Could not get minimum latency\n");

		if (song.file[0] != NULL)
			VorbisMemoryFile_FClose(song.file[0]);
		if (song.file[1] != NULL)
			VorbisMemoryFile_FClose(song.file[1]);

		return false;
	}

	if (cubeb_stream_init(cubeb_context, &song.stream, "Example Stream 1", NULL, NULL, NULL, &output_params, latency_frames, data_cb, state_cb, NULL) != CUBEB_OK)
	{
		PrintError("ogg_music: Could not open the stream\n");

		if (song.file[0] != NULL)
			VorbisMemoryFile_FClose(song.file[0]);
		if (song.file[1] != NULL)
			VorbisMemoryFile_FClose(song.file[1]);

		return false;
	}

	song.loops = loops;

	return true;
}

static bool PlayOggMusic(const int song_id)
{
	const char* const song_name = playlist[song_id].name;
	char *song_intro_file_path, *song_loop_file_path;

	if (playlist[song_id].split)
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

	if (!LoadSong(song_intro_file_path, song_loop_file_path, playlist[song_id].loops))
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

static void PlayOrgMusic(const int music_id)
{
	*previous_song_last_position = GetOrgMusicPosition();
	sub_41C7F0();
	LoadOrgMusic(org_playlist[music_id]);
	SetOrgVolume(100);
	SetOrgMusicPosition(0);
	StartOrgPlayback();
}

static void PlayPreviousOrgMusic(void)
{
	sub_41C7F0();
	LoadOrgMusic(org_playlist[*previous_music]);
	SetOrgMusicPosition(*previous_song_last_position);
	SetOrgVolume(100);
	StartOrgPlayback();
}

static void PlayMusic_new(const int music_id)
{
	if (music_id == 0 || music_id != *current_music)
	{
		*previous_music = *current_music;

		StopSong();

		UnloadSong(&song_backup);
		song_backup = song;
		song = song_blank;

		if (music_id == 0 || (!playlist[music_id - 1].is_org && PlayOggMusic(music_id - 1)))
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

		fade.volume = 100;
		fade.active = false;
	}
}

static void PlayPreviousMusic_new(void)
{
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
	*current_music = *previous_music;

	fade.volume = 100;
	fade.active = false;
}

static void WindowFocusGained_new(void)
{
	StartSong();
	sub_41C7F0();	// The instruction we hijacked to get here
}

static void WindowFocusLost_new(void)
{
	StopSong();
	sub_41C7F0();	// The instruction we hijacked to get here
}

static void FadeMusic_new(void)
{
	*music_fade_flag = 1;
	fade.counter = (song.sample_rate * 5) / 100;
	fade.active = true;
}

void InitMod(void)
{
	const char* const playlist_filename = GetSettingString("playlist");
	if (playlist_filename != NULL)
	{
		for (unsigned int i = 0; i < 100; ++i)
			fade.nonlinear_volume_table[i] = ((i + 1) * (i + 1)) / 100;

		char playlist_path[strlen(location_path) + strlen(playlist_filename) + 1];
		strcpy(playlist_path, location_path);
		strcat(playlist_path, playlist_filename);
		LoadPlaylist(playlist_path);

		// Setup music system
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
}
