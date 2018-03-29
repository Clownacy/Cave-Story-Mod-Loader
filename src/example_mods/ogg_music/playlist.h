#pragma once

#include <stdbool.h>

typedef struct PlaylistEntry
{
	const char *name;
	bool loops;
	bool split;
	bool is_org;
	void *backend_data;
} PlaylistEntry;

PlaylistEntry* GetFirstPlaylistEntry(void);
PlaylistEntry* GetNextPlaylistEntry(void);

PlaylistEntry* GetPlaylistEntry(unsigned int song_id);

bool InitPlaylist(void);

