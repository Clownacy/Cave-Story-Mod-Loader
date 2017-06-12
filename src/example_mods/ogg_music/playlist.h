// Ogg music mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#pragma once

#include <stdbool.h>

#define SONG_LOOP (1<<0)
#define SONG_SPLIT (1<<1)

typedef struct SongEntry
{
	const char *name;
	bool loops;
	bool split;
	bool is_org;
} SongEntry;

SongEntry playlist[41];
void LoadPlaylist(const char* const playlist_path);
