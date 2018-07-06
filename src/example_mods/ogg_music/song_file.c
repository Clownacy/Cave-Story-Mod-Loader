#include "song_file.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "mod_loader.h"
#include "sprintfMalloc.h"

#include "decoder.h"

typedef struct SongFile
{
	unsigned int current_file;

	bool is_split;
	bool playing_intro;
	bool loops;

	Decoder *decoder[2];
} SongFile;

SongFile* SongFile_Load(const char* const path, bool loops, bool predecode)
{
	SongFile *song = malloc(sizeof(SongFile));

	// Look for split-Ogg music (Cave Story 3D)
	char *file_path = sprintfMalloc("%s_intro.ogg", path);
	song->decoder[0] = Decoder_Open(file_path, predecode);
	free(file_path);

	file_path = sprintfMalloc("%s_loop.ogg", path);
	song->decoder[1] = Decoder_Open(file_path, predecode);
	free(file_path);

	if (song->decoder[0] == NULL && song->decoder[1] == NULL)
	{
		// Look for single-Ogg music (Cave Story WiiWare)
		file_path = sprintfMalloc("%s.ogg", path);
		song->decoder[0] = Decoder_Open(file_path, predecode);
		free(file_path);

		if (song->decoder[0] == NULL)
		{
			// Neither file could be opened
			goto Fail;
		}
	}

	if (song->decoder[0] == NULL || song->decoder[1] == NULL)
	{
		// Only one file could be opened
		song->is_split = false;
		song->playing_intro = false;

		if (song->decoder[0] == NULL)
		{
			// Swap files, since there has to be one in the first slot
			song->decoder[0] = song->decoder[1];
			song->decoder[1] = NULL;
		}
	}
	else
	{
		// Both files opened successfully
		song->is_split = true;
		song->playing_intro = true;
	}

	if (song->is_split && (Decoder_GetChannelCount(song->decoder[0]) != Decoder_GetChannelCount(song->decoder[1]) || Decoder_GetSampleRate(song->decoder[0]) != Decoder_GetSampleRate(song->decoder[1])))
		goto Fail;

	song->current_file = 0;
	song->loops = loops;

	return song;

	Fail:

	free(song);

	return NULL;
}

void SongFile_Unload(SongFile *this)
{
	if (this)
	{
		for (unsigned int i = 0; i < (this->is_split ? 2 : 1); ++i)
			Decoder_Close(this->decoder[i]);

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
			Decoder_Rewind(this->decoder[i]);
	}
}

unsigned long SongFile_GetSamples(SongFile *this, void *output_buffer, unsigned long bytes_to_do)
{
	unsigned long bytes_done_total = 0;

	for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
	{
		bytes_done = Decoder_GetSamples(this->decoder[this->current_file], output_buffer + bytes_done_total, bytes_to_do - bytes_done_total);

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
					Decoder_Rewind(this->decoder[this->current_file]);
				else
					break;
			}
		}
	}

	return bytes_done_total;
}

unsigned int SongFile_GetChannelCount(SongFile *this)
{
	return Decoder_GetChannelCount(this->decoder[0]);
}

unsigned int SongFile_GetSampleRate(SongFile *this)
{
	return Decoder_GetSampleRate(this->decoder[0]);
}
