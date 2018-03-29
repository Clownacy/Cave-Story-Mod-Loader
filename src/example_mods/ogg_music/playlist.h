#pragma once

#include <stdbool.h>

#include "song_file.h"

typedef struct PlaylistEntry
{
	const char *name;
	bool loops;
	bool split;
	bool is_org;
	SongFile *file;
} PlaylistEntry;

PlaylistEntry* GetFirstPlaylistEntry(void);
PlaylistEntry* GetNextPlaylistEntry(void);

PlaylistEntry* GetPlaylistEntry(unsigned int song_id);

bool InitPlaylist(void);

