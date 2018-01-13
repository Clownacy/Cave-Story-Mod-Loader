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

typedef struct VorbisMemoryFile
{
	char *data;
	size_t data_size;
	signed long current_offset;
} VorbisMemoryFile;

typedef struct Song {
	VorbisMemoryFile *file[2];
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

typedef struct SongEntry
{
	const char *name;
	bool loops;
	bool split;
	bool is_org;
	VorbisMemoryFile *file_intro;
	VorbisMemoryFile *file_loop;
} SongEntry;

static bool setting_preload;

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

static SongEntry playlist[] = {
	{"data/Ogg/WANPAKU", true, false, true, NULL, NULL},
	{"data/Ogg/ANZEN", true, false, true, NULL, NULL},
	{"data/Ogg/GAMEOVER", false, false, true, NULL, NULL},
	{"data/Ogg/GRAVITY", true, false, true, NULL, NULL},
	{"data/Ogg/WEED", true, false, true, NULL, NULL},
	{"data/Ogg/MDOWN2", true, false, true, NULL, NULL},
	{"data/Ogg/FIREEYE", true, false, true, NULL, NULL},
	{"data/Ogg/VIVI", true, false, true, NULL, NULL},
	{"data/Ogg/MURA", true, false, true, NULL, NULL},
	{"data/Ogg/FANFALE1", false, false, true, NULL, NULL},
	{"data/Ogg/GINSUKE", true, false, true, NULL, NULL},
	{"data/Ogg/CEMETERY", true, false, true, NULL, NULL},
	{"data/Ogg/PLANT", true, false, true, NULL, NULL},
	{"data/Ogg/KODOU", true, false, true, NULL, NULL},
	{"data/Ogg/FANFALE3", false, false, true, NULL, NULL},
	{"data/Ogg/FANFALE2", false, false, true, NULL, NULL},
	{"data/Ogg/DR", true, false, true, NULL, NULL},
	{"data/Ogg/ESCAPE", true, false, true, NULL, NULL},
	{"data/Ogg/JENKA", true, false, true, NULL, NULL},
	{"data/Ogg/MAZE", true, false, true, NULL, NULL},
	{"data/Ogg/ACCESS", true, false, true, NULL, NULL},
	{"data/Ogg/IRONH", true, false, true, NULL, NULL},
	{"data/Ogg/GRAND", true, false, true, NULL, NULL},
	{"data/Ogg/Curly", true, false, true, NULL, NULL},
	{"data/Ogg/OSIDE", true, false, true, NULL, NULL},
	{"data/Ogg/REQUIEM", true, false, true, NULL, NULL},
	{"data/Ogg/WANPAK2", true, false, true, NULL, NULL},
	{"data/Ogg/QUIET", true, false, true, NULL, NULL},
	{"data/Ogg/LASTCAVE", true, false, true, NULL, NULL},
	{"data/Ogg/BALCONY", true, false, true, NULL, NULL},
	{"data/Ogg/LASTBTL", true, false, true, NULL, NULL},
	{"data/Ogg/LASTBT3", true, false, true, NULL, NULL},
	{"data/Ogg/ENDING", true, false, true, NULL, NULL},
	{"data/Ogg/ZONBIE", true, false, true, NULL, NULL},
	{"data/Ogg/BDOWN", true, false, true, NULL, NULL},
	{"data/Ogg/HELL", true, false, true, NULL, NULL},
	{"data/Ogg/JENKA2", true, false, true, NULL, NULL},
	{"data/Ogg/MARINE", true, false, true, NULL, NULL},
	{"data/Ogg/BALLOS", true, false, true, NULL, NULL},
	{"data/Ogg/TOROKO", false, false, true, NULL, NULL},
	{"data/Ogg/WHITE", true, false, true, NULL, NULL}
};

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
  (int (*)(void *))                             NULL,
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

		if (setting_preload)
		{
			if (song->file[0])
				VorbisMemoryFile_FSeek(song->file[0], 0, SEEK_SET);
			if (song->file[1])
				VorbisMemoryFile_FSeek(song->file[1], 0, SEEK_SET);
		}
		else
		{
			if (song->file[0])
				VorbisMemoryFile_FClose(song->file[0]);
			if (song->file[1])
				VorbisMemoryFile_FClose(song->file[1]);
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
	if (setting_preload)
	{
		song.file[0] = playlist[song_id].file_intro;
		song.file[1] = playlist[song_id].file_loop;
	}
	else
	{
		const char* const song_name = playlist[song_id].name;
		char *intro_file_path, *loop_file_path;

		if (playlist[song_id].split)
		{
			// Play split-Ogg music (Cave Story 3D)
			intro_file_path = sprintfMalloc("%s_intro.ogg", song_name);
			loop_file_path = sprintfMalloc("%s_loop.ogg", song_name);
		}
		else
		{
			// Play single-Ogg music (Cave Story WiiWare)
			intro_file_path = sprintfMalloc("%s.ogg", song_name);
			loop_file_path = NULL;
		}

		song.file[0] = VorbisMemoryFile_FOpen(intro_file_path);
		song.file[1] = loop_file_path ? VorbisMemoryFile_FOpen(loop_file_path) : NULL;

		free(intro_file_path);
		if (loop_file_path)
			free(loop_file_path);
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

	if (cubeb_stream_init(cubeb_context, &song.stream, "Example Stream 1", NULL, NULL, NULL, &output_params, latency_frames, data_cb, state_cb, NULL) != CUBEB_OK)
	{
		PrintError("ogg_music: Could not open the stream\n");

		goto Fail;
	}

	song.loops = playlist[song_id].loops;

	return true;

	Fail:
	if (!setting_preload)
	{
		if (song.file[0] != NULL)
			VorbisMemoryFile_FClose(song.file[0]);
		if (song.file[1] != NULL)
			VorbisMemoryFile_FClose(song.file[1]);
	}

	return false;
}

static bool PlayOggMusic(const int song_id)
{
	if (!LoadSong(song_id))
	{
		return false;
	}

	StartSong();

	return true;
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
		CS_current_music = music_id;

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
	CS_current_music = CS_previous_music;

	fade.volume = 100;
	fade.active = false;
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
	fade.counter = (song.sample_rate * 5) / 100;
	fade.active = true;
}

void LoadPlaylist(const char* const playlist_folder)
{
	char *playlist_path = sprintfMalloc("%s/playlist.txt", playlist_folder);
	FILE *playlist_file = fopen(playlist_path, "r");

	if (playlist_file == NULL)
	{
		PrintError("Could not open playlist.txt at '%s'\n", playlist_path);
	}
	else
	{
		char line[MAX_PATH];
		for (int current_song = 0; fgets(line, MAX_PATH, playlist_file) != NULL; ++current_song)
		{
			char *line_current_position = line;

			line_current_position[strcspn(line_current_position, "\r\n")] = '\0';	// Trim newline characters

			// Get file path
			size_t path_length = strcspn(line_current_position, ",");

			// No song path
			if (path_length == 0)
				continue;

			char *song_path_rel = malloc(path_length + 1);
			strncpy(song_path_rel, line_current_position, path_length);
			song_path_rel[path_length] = '\0';

			PrintDebug("playlist.txt: song '%s'\n", song_path_rel);

			char *song_path = sprintfMalloc("%s/%s", playlist_folder, song_path_rel);
			free(song_path_rel);
			playlist[current_song].name = song_path;
			playlist[current_song].loops = false;
			playlist[current_song].split = false;
			playlist[current_song].is_org = false;

			line_current_position += path_length;
			line_current_position += strspn(line_current_position, ", ");

			while (line_current_position[0] != '\0')
			{
				// Get song properties
				size_t property_length = strcspn(line_current_position, ",");

				if (property_length != 0)
				{
					if (strncmp(line_current_position, "loop", 4) == 0)
						playlist[current_song].loops = true;
					else if (strncmp(line_current_position, "split", 5) == 0)
						playlist[current_song].split = true;
					else if (strncmp(line_current_position, "org", 3) == 0)
						playlist[current_song].is_org = true;

					line_current_position += property_length;
				}

				line_current_position += strspn(line_current_position, ", ");
			}

			if (setting_preload)
			{
				const char* const song_name = playlist[current_song].name;
				char *intro_file_path, *loop_file_path;

				if (playlist[current_song].split)
				{
					// Play split-Ogg music (Cave Story 3D)
					intro_file_path = sprintfMalloc("%s_intro.ogg", song_name);
					loop_file_path = sprintfMalloc("%s_loop.ogg", song_name);
				}
				else
				{
					// Play single-Ogg music (Cave Story WiiWare)
					intro_file_path = sprintfMalloc("%s.ogg", song_name);
					loop_file_path = NULL;
				}

				playlist[current_song].file_intro = VorbisMemoryFile_FOpen(intro_file_path);
				playlist[current_song].file_loop = loop_file_path ? VorbisMemoryFile_FOpen(loop_file_path) : NULL;

				free(intro_file_path);
				if (loop_file_path)
					free(loop_file_path);
			}
		}

		fclose(playlist_file);
	}

	free(playlist_path);
}

void InitMod(void)
{
	const char* const playlist_filename = GetSettingString("playlist");
	if (playlist_filename != NULL)
	{
		setting_preload = GetSettingBool("preload_oggs");

		for (unsigned int i = 0; i < 100; ++i)
			fade.nonlinear_volume_table[i] = ((i + 1) * (i + 1)) / 100;

		char playlist_path[strlen(location_path) + strlen(playlist_filename) + 1];
		strcpy(playlist_path, location_path);
		strcat(playlist_path, playlist_filename);
		LoadPlaylist(playlist_path);

		// Setup music system
		cubeb_init(&cubeb_context, "Ogg player for Cave Story", NULL);
		// Replace PlayMusic and PlayPreviousMusic with our custom Ogg ones
		WriteJump(CS_PlayMusic, PlayMusic_new);
		WriteJump(CS_PlayPreviousMusic, PlayPreviousMusic_new);
		// We also need to replace the music pausing/resuming when the window focus changes
		WriteRelativeAddress((void*)0x412BD6 + 1, WindowFocusLost_new);
		WriteRelativeAddress((void*)0x412C06 + 1, WindowFocusGained_new);
		// Patch fading
		WriteJump(CS_FadeMusic, FadeMusic_new);
	}
}
