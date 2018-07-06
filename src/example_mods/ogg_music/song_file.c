#include "song_file.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <vorbis/vorbisfile.h>

#include "mod_loader.h"
#include "sprintfMalloc.h"

#include "memory_file.h"

typedef struct SongFile {

	unsigned int current_file;

	int channel_count;
	int sample_rate;
	bool is_split;
	bool playing_intro;
	bool loops;
	bool predecoded;

	struct
	{
		OggVorbis_File vorbis_file;
		unsigned int size;
		char *buffer;
		size_t index;
	} part[2];
} SongFile;

static ov_callbacks ov_callback_memory = {
	(size_t (*)(void *, size_t, size_t, void *))  MemoryFile_fread,
	(int (*)(void *, long long, int))             MemoryFile_fseek,
	(int (*)(void *))                             MemoryFile_fclose,
	(long (*)(void *))                            MemoryFile_ftell
};

static unsigned long ReadOggToBuffer(OggVorbis_File *vorbis_file, void *buffer, size_t bytes_to_do)
{
	unsigned long bytes_done_total = 0;

	for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
	{
		bytes_done = ov_read(vorbis_file, buffer + bytes_done_total, bytes_to_do - bytes_done_total, 0, 2, 1, NULL);

		if (bytes_done == 0)
			break;
	}

	return bytes_done_total;
}

static unsigned long ReadPCMToBuffer(SongFile *this, char *buffer, size_t bytes_to_do)
{
	const size_t bytes_left = this->part[this->current_file].size - this->part[this->current_file].index;

	bytes_to_do = bytes_left < bytes_to_do ? bytes_left : bytes_to_do;

	memcpy(buffer, this->part[this->current_file].buffer + this->part[this->current_file].index, bytes_to_do);

	this->part[this->current_file].index += bytes_to_do;

	return bytes_to_do;
}

SongFile* SongFile_Load(const char* const path, bool loops, bool predecoded)
{
	SongFile *song = malloc(sizeof(SongFile));

	MemoryFile *file[2];

	// Look for split-Ogg music (Cave Story 3D)
	char *file_path = sprintfMalloc("%s_intro.ogg", path);
	file[0] = MemoryFile_fopen(file_path);
	free(file_path);

	file_path = sprintfMalloc("%s_loop.ogg", path);
	file[1] = MemoryFile_fopen(file_path);
	free(file_path);

	if (file[0] == NULL && file[1] == NULL)
	{
		// Look for single-Ogg music (Cave Story WiiWare)
		file_path = sprintfMalloc("%s.ogg", path);
		file[0] = MemoryFile_fopen(file_path);
		free(file_path);

		if (file[0] == NULL)
		{
			// Neither file could be opened
			goto Fail1;
		}
	}

	if (file[0] == NULL || file[1] == NULL)
	{
		// Only one file could be opened
		song->is_split = false;
		song->playing_intro = false;

		if (file[0] == NULL)
		{
			// Swap files, since there has to be one in the first slot
			file[0] = file[1];
			file[1] = NULL;
		}
	}
	else
	{
		// Both files opened successfully
		song->is_split = true;
		song->playing_intro = true;
	}

	song->current_file = 0;

	if (ov_open_callbacks(file[0], &song->part[0].vorbis_file, NULL, 0, ov_callback_memory) < 0)
	{
		ModLoader_PrintError("ogg_music: Error - '%s' does not appear to be an Ogg bitstream\n", path);

		goto Fail2;
	}

	song->channel_count = ov_info(&song->part[0].vorbis_file, -1)->channels;

	song->sample_rate = ov_info(&song->part[0].vorbis_file, -1)->rate;

	if (song->is_split)
	{
		if (ov_open_callbacks(file[1], &song->part[1].vorbis_file, NULL, 0, ov_callback_memory) < 0)
		{
			ModLoader_PrintError("ogg_music: Error - '%s' loop file does not appear to be an Ogg bitstream\n", path);

			goto Fail2;
		}

		if (ov_info(&song->part[1].vorbis_file, -1)->channels != song->channel_count)
		{
			ModLoader_PrintError("ogg_music: Error - The files for '%s' don't have the same channel count\n", path);
			goto Fail3;
		}

		if (ov_info(&song->part[1].vorbis_file, -1)->rate != song->sample_rate)
		{
			ModLoader_PrintError("ogg_music: Error - The files for '%s' don't have the same sample rate\n", path);
			goto Fail3;
		}
	}

	song->loops = loops;
	song->predecoded = predecoded;

	if (predecoded)
	{
		for (unsigned int i = 0; i < (song->is_split ? 2 : 1); ++i)
		{
			song->part[i].size = ov_pcm_total(&song->part[i].vorbis_file, -1) * 2 * song->channel_count;
			song->part[i].buffer = malloc(song->part[i].size);
			song->part[i].index = 0;

			unsigned long bytes_done = ReadOggToBuffer(&song->part[i].vorbis_file, song->part[i].buffer, song->part[i].size);

			if (bytes_done != song->part[i].size)
			{
				ModLoader_PrintError("ogg_music - Ogg reported bad size\n");
			}

			ov_clear(&song->part[i].vorbis_file);
		}
	}

	return song;

	Fail3:

	for (unsigned int i = 0; i < (song->is_split ? 2 : 1); ++i)
		ov_clear(&song->part[i].vorbis_file);

	goto Fail1;

	Fail2:

	for (unsigned int i = 0; i < (song->is_split ? 2 : 1); ++i)
		MemoryFile_fclose(file[i]);

	//goto Fail1;

	Fail1:

	free(song);

	return NULL;
}

void SongFile_Unload(SongFile *this)
{
	if (this)
	{
		for (unsigned int i = 0; i < (this->is_split ? 2 : 1); ++i)
		{
			if (this->predecoded)
				free(this->part[i].buffer);
			else
				ov_clear(&this->part[i].vorbis_file);
		}

		free(this);
	}
}

void SongFile_Reset(SongFile *this)
{
	if (this)
	{
		this->playing_intro = this->is_split;
		this->current_file = 0;

		for (unsigned int i = 0; i < (this->is_split ? 2 : 1); ++i)
		{
			if (this->predecoded)
				this->part[i].index = 0;
			else
				ov_time_seek(&this->part[i].vorbis_file, 0);
		}
	}
}

unsigned long SongFile_GetSamples(SongFile *this, void *output_buffer, unsigned long bytes_to_do)
{
	unsigned long bytes_done_total = 0;

	if (this->predecoded)
	{
		for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
		{
			bytes_done = ReadPCMToBuffer(this, output_buffer + bytes_done_total, bytes_to_do - bytes_done_total);

			if (bytes_done < bytes_to_do - bytes_done_total)	// EOF
			{
				if (this->playing_intro)
				{
					this->playing_intro = false;
					++this->current_file;
				}
				else
				{
					if (this->loops)
						this->part[this->current_file].index = 0;
					else
						break;
				}
			}
		}
	}
	else
	{
		for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
		{
			bytes_done = ReadOggToBuffer(&this->part[this->current_file].vorbis_file, output_buffer + bytes_done_total, bytes_to_do - bytes_done_total);

			if (bytes_done < bytes_to_do - bytes_done_total)	// EOF
			{
				if (this->playing_intro)
				{
					this->playing_intro = false;
					++this->current_file;
				}
				else
				{
					if (this->loops)
						ov_time_seek(&this->part[this->current_file].vorbis_file, 0);
					else
						break;
				}
			}
		}
	}

	return bytes_done_total;
}

unsigned int SongFile_GetChannels(SongFile *this)
{
	return this ? this->channel_count : 0;
}

unsigned int SongFile_GetSampleRate(SongFile *this)
{
	return this ? this->sample_rate : 0;
}
