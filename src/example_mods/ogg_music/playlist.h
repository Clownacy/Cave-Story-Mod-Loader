#pragma once

#include <stdbool.h>

#include "dual_decoder.h"

typedef struct PlaylistEntry
{
	const char *name;
	bool loop;
	bool is_org;
	DualDecoder *dual_decoder;
	DecoderInfo decoder_info;
} PlaylistEntry;

PlaylistEntry* GetFirstPlaylistEntry(void);
PlaylistEntry* GetNextPlaylistEntry(void);

PlaylistEntry* GetPlaylistEntry(unsigned int song_id);

bool InitPlaylist(void);

