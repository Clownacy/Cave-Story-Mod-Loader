#include "song_file.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <vorbis/vorbisfile.h>

#include "mod_loader.h"
#include "sprintfMalloc.h"

#include "memory_file.h"

typedef struct SongFile {
	MemoryFile *file[2];
	OggVorbis_File vorbis_file[2];

	unsigned int current_file;

	int current_section[2];

	int channels;
	int sample_rate;
	bool is_split;
	bool playing_intro;
	bool loops;
} SongFile;

static ov_callbacks ov_callback_memory = {
	(size_t (*)(void *, size_t, size_t, void *))  MemoryFile_fread,
	(int (*)(void *, long long, int))             MemoryFile_fseek,
	(int (*)(void *))                             MemoryFile_fclose,
	(long (*)(void *))                            MemoryFile_ftell
};

SongFile* SongFile_Load(const char* const path, bool loops)
{
	SongFile *song = malloc(sizeof(SongFile));

	// Look for split-Ogg music (Cave Story 3D)
	char *file_path = sprintfMalloc("%s_intro.ogg", path);
	song->file[0] = MemoryFile_fopen(file_path);
	free(file_path);

	file_path = sprintfMalloc("%s_loop.ogg", path);
	song->file[1] = MemoryFile_fopen(file_path);
	free(file_path);

	if (song->file[0] == NULL && song->file[1] == NULL)
	{
		// Look for single-Ogg music (Cave Story WiiWare)
		file_path = sprintfMalloc("%s.ogg", path);
		song->file[0] = MemoryFile_fopen(file_path);
		free(file_path);

		if (song->file[0] == NULL)
		{
			// Neither file could be opened
			goto Fail1;
		}
	}

	if (song->file[0] == NULL || song->file[1] == NULL)
	{
		// Only one file could be opened
		song->is_split = false;
		song->playing_intro = false;

		if (song->file[0] == NULL)
		{
			// Swap files, since there has to be one in the first slot
			song->file[0] = song->file[1];
			song->file[1] = NULL;
		}
	}
	else
	{
		// Both files opened successfully
		song->is_split = true;
		song->playing_intro = true;
	}

	song->current_file = 0;

	if (ov_open_callbacks(song->file[0], &song->vorbis_file[0], NULL, 0, ov_callback_memory) < 0)
	{
		ModLoader_PrintError("ogg_music: Error - '%s' does not appear to be an Ogg bitstream\n", path);

		goto Fail2;
	}

	song->channels = ov_info(&song->vorbis_file[0], -1)->channels;

	song->sample_rate = ov_info(&song->vorbis_file[0], -1)->rate;

	if (song->is_split)
	{
		if (ov_open_callbacks(song->file[1], &song->vorbis_file[1], NULL, 0, ov_callback_memory) < 0)
		{
			ModLoader_PrintError("ogg_music: Error - '%s' loop file does not appear to be an Ogg bitstream\n", path);

			goto Fail2;
		}

		if (ov_info(&song->vorbis_file[1], -1)->channels != song->channels)
		{
			ModLoader_PrintError("ogg_music: Error - The files for '%s' don't have the same channel count\n", path);
			SongFile_Unload(song);
			return NULL;
		}

		if (ov_info(&song->vorbis_file[1], -1)->rate != song->sample_rate)
		{
			ModLoader_PrintError("ogg_music: Error - The files for '%s' don't have the same sample rate\n", path);
			SongFile_Unload(song);
			return NULL;
		}
	}

	song->loops = loops;

	return song;

	Fail2:

	MemoryFile_fclose(song->file[0]);

	if (song->is_split)
		MemoryFile_fclose(song->file[1]);

	Fail1:

	free(song);

	return NULL;
}

void SongFile_Unload(SongFile *this)
{
	ov_clear(&this->vorbis_file[0]);

	if (this->is_split)
		ov_clear(&this->vorbis_file[1]);

	free(this);
}

void SongFile_Reset(SongFile *this)
{
	this->playing_intro = this->is_split;
	this->current_file = 0;

	ov_time_seek(&this->vorbis_file[0], 0);

	if (this->is_split)
		ov_time_seek(&this->vorbis_file[1], 0);
}

size_t SongFile_GetSamples(SongFile *this, void *output_buffer, size_t samples_to_do)
{
	const unsigned int BYTES_PER_SAMPLE = this->channels * 2;	// 16-bit
	const unsigned long bytes_to_do = samples_to_do * BYTES_PER_SAMPLE;

	unsigned long bytes_done_total = 0;

	for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
	{
		bytes_done = ov_read(&this->vorbis_file[this->current_file], output_buffer + bytes_done_total, bytes_to_do - bytes_done_total, 0, 2, 1, &this->current_section[this->current_file]);

		if (bytes_done == 0)	// EOF
		{
			if (this->playing_intro)
			{
				this->playing_intro = false;
				++this->current_file;
			}
			else
			{
				if (this->loops)
					ov_time_seek(&this->vorbis_file[this->current_file], 0);
				else
					break;
			}
		}
	}

	return bytes_done_total / BYTES_PER_SAMPLE;
}

unsigned int SongFile_GetChannels(SongFile *this)
{
	return this->channels;
}

unsigned int SongFile_GetSampleRate(SongFile *this)
{
	return this->sample_rate;
}
