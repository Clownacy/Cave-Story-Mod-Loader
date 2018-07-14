// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "decoder.h"

typedef struct PlaylistEntry
{
	const char *name;
	bool loop;
	bool is_org;
	Decoder *decoder;
	DecoderInfo decoder_info;
} PlaylistEntry;

PlaylistEntry* GetFirstPlaylistEntry(void);
PlaylistEntry* GetNextPlaylistEntry(void);

PlaylistEntry* GetPlaylistEntry(unsigned int song_id);

bool InitPlaylist(void);
