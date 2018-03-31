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

	unsigned int default_current_file;
	unsigned int current_file;

	int current_section[2];

	unsigned int channels;
	bool has_next_part;
	bool playing_intro;
	bool loops;
	unsigned int sample_rate;
} SongFile;

static ov_callbacks ov_callback_memory = {
	(size_t (*)(void *, size_t, size_t, void *))  MemoryFile_fread,
	(int (*)(void *, long long, int))             MemoryFile_fseek,
	(int (*)(void *))                             NULL,
	(long (*)(void *))                            MemoryFile_ftell
};

SongFile* SongFile_Load(const char* const path, bool split, bool loops)
{
	SongFile *song = malloc(sizeof(SongFile));

	if (split)
	{
		// Play split-Ogg music (Cave Story 3D)
		char *file_path = sprintfMalloc("%s_intro.ogg", path);
		song->file[0] = MemoryFile_fopen(file_path);
		free(file_path);

		file_path = sprintfMalloc("%s_loop.ogg", path);
		song->file[1] = MemoryFile_fopen(file_path);
		free(file_path);
	}
	else
	{
		// Play single-Ogg music (Cave Story WiiWare)
		char *file_path = sprintfMalloc("%s.ogg", path);
		song->file[0] = MemoryFile_fopen(file_path);
		free(file_path);

		song->file[1] = NULL;
	}

	if (song->file[0] == NULL && song->file[1] == NULL)
	{
		// Neither file could be opened
		goto Fail2;
	}
	else if (song->file[0] == NULL || song->file[1] == NULL)
	{
		// Only one file could be opened
		song->has_next_part = false;
		song->playing_intro = false;

		if (song->file[0] == NULL)
		{
			song->file[0] = song->file[1];
			song->file[1] = NULL;
		}
	}
	else
	{
		// Both files opened successfully
		song->has_next_part = true;
		song->playing_intro = true;
	}

	song->default_current_file = 0;
	song->current_file = 0;

	if (ov_open_callbacks(song->file[0], &song->vorbis_file[0], NULL, 0, ov_callback_memory) < 0)
	{
		PrintError("ogg_music: Input does not appear to be an Ogg bitstream.\n");

		goto Fail1;
	}

	if (song->has_next_part)
	{
		if (ov_open_callbacks(song->file[1], &song->vorbis_file[1], NULL, 0, ov_callback_memory) < 0)
		{
			PrintError("ogg_music: Input does not appear to be an Ogg bitstream.\n");

			goto Fail1;
		}
	}

	song->channels = ov_info(&song->vorbis_file[song->current_file], -1)->channels;

	song->sample_rate = ov_info(&song->vorbis_file[song->current_file], -1)->rate;

	song->loops = loops;

	return song;

	Fail1:

	if (song->file[0])
		MemoryFile_fclose(song->file[0]);
	if (song->file[1])
		MemoryFile_fclose(song->file[1]);

	Fail2:

	free(song);

	return NULL;
}

void SongFile_Unload(SongFile *this)
{
	ov_clear(&this->vorbis_file[0]);
	if (this->has_next_part)
		ov_clear(&this->vorbis_file[1]);

	if (this->file[0])
		MemoryFile_fclose(this->file[0]);
	if (this->file[1])
		MemoryFile_fclose(this->file[1]);

	free(this);
}

void SongFile_Reset(SongFile *this)
{
	this->playing_intro = this->has_next_part;
	this->current_file = this->default_current_file;
	ov_time_seek(&this->vorbis_file[0], 0);
	if (this->has_next_part)
		ov_time_seek(&this->vorbis_file[1], 0);
}

size_t SongFile_GetSamples(SongFile *this, void *output_buffer, size_t samples_to_do)
{
	const unsigned int BYTES_PER_SAMPLE = this->channels * 2;	// 2 channels, 16-bit
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

unsigned int SongFile_GetRate(SongFile *this)
{
	return this->sample_rate;
}
