// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "audio_lib/mixer.h"

typedef struct PlaylistEntry
{
	const char *name;
	bool loop;
	bool is_org;
	Mixer_Sound *sound;
} PlaylistEntry;

PlaylistEntry* GetFirstPlaylistEntry(void);
PlaylistEntry* GetNextPlaylistEntry(void);

PlaylistEntry* GetPlaylistEntry(unsigned int song_id);

bool InitPlaylist(void);
