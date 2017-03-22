// Ogg music mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "playlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

SongEntry playlist[] = {
	{"data/Ogg/WANPAKU", SONG_LOOP},
	{"data/Ogg/ANZEN", SONG_LOOP},
	{"data/Ogg/GAMEOVER", 0},
	{"data/Ogg/GRAVITY", SONG_LOOP},
	{"data/Ogg/WEED", SONG_LOOP},
	{"data/Ogg/MDOWN2", SONG_LOOP},
	{"data/Ogg/FIREEYE", SONG_LOOP},
	{"data/Ogg/VIVI", SONG_LOOP},
	{"data/Ogg/MURA", SONG_LOOP},
	{"data/Ogg/FANFALE1", 0},
	{"data/Ogg/GINSUKE", SONG_LOOP},
	{"data/Ogg/CEMETERY", SONG_LOOP},
	{"data/Ogg/PLANT", SONG_LOOP},
	{"data/Ogg/KODOU", SONG_LOOP},
	{"data/Ogg/FANFALE3", 0},
	{"data/Ogg/FANFALE2", 0},
	{"data/Ogg/DR", SONG_LOOP},
	{"data/Ogg/ESCAPE", SONG_LOOP},
	{"data/Ogg/JENKA", SONG_LOOP},
	{"data/Ogg/MAZE", SONG_LOOP},
	{"data/Ogg/ACCESS", SONG_LOOP},
	{"data/Ogg/IRONH", SONG_LOOP},
	{"data/Ogg/GRAND", SONG_LOOP},
	{"data/Ogg/Curly", SONG_LOOP},
	{"data/Ogg/OSIDE", SONG_LOOP},
	{"data/Ogg/REQUIEM", SONG_LOOP},
	{"data/Ogg/WANPAK2", SONG_LOOP},
	{"data/Ogg/QUIET", SONG_LOOP},
	{"data/Ogg/LASTCAVE", SONG_LOOP},
	{"data/Ogg/BALCONY", SONG_LOOP},
	{"data/Ogg/LASTBTL", SONG_LOOP},
	{"data/Ogg/LASTBT3", SONG_LOOP},
	{"data/Ogg/ENDING", SONG_LOOP},
	{"data/Ogg/ZONBIE", SONG_LOOP},
	{"data/Ogg/BDOWN", SONG_LOOP},
	{"data/Ogg/HELL", SONG_LOOP},
	{"data/Ogg/JENKA2", SONG_LOOP},
	{"data/Ogg/MARINE", SONG_LOOP},
	{"data/Ogg/BALLOS", SONG_LOOP},
	{"data/Ogg/TOROKO", 0},
	{"data/Ogg/WHITE", SONG_LOOP}
};

void LoadPlaylist(const char* const playlist_path)
{
	FILE *playlist_file = fopen(playlist_path, "r");

	if (playlist_file != NULL)
	{
		char line[MAX_PATH];
		for (int current_song = 0; fgets(line, MAX_PATH, playlist_file) != NULL; ++current_song)
		{
			char *line_current_position = line;

			line_current_position[strcspn(line_current_position, "\r\n")] = '\0';	// Trim newline characters

			// Get file path
			size_t path_length = strcspn(line_current_position, ",");

			// No song path
			if (path_length == 0)
				continue;

			char *song_path = malloc(path_length + 1);
			strncpy(song_path, line_current_position, path_length);
			song_path[path_length] = '\0';

			line_current_position += path_length;
			line_current_position += strspn(line_current_position, ", ");

			char flags = 0;

			while (line_current_position[0] != '\0')
			{
				// Get song properties
				size_t property_length = strcspn(line_current_position, ",");

				if (property_length != 0)
				{
					if (strncmp(line_current_position, "loop", 4) == 0)
						flags |= SONG_LOOP;
					else if (strncmp(line_current_position, "split", 5) == 0)
						flags |= SONG_SPLIT;

					line_current_position += property_length;
				}

				line_current_position += strspn(line_current_position, ", ");
			}

			playlist[current_song].song_name = song_path;
			playlist[current_song].song_flags = flags;
		}

		fclose(playlist_file);
	}
}
