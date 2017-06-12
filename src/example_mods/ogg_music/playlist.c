// Ogg music mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "playlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

SongEntry playlist[] = {
	{"data/Ogg/WANPAKU", true, false, true},
	{"data/Ogg/ANZEN", true, false, true},
	{"data/Ogg/GAMEOVER", false, false, true},
	{"data/Ogg/GRAVITY", true, false, true},
	{"data/Ogg/WEED", true, false, true},
	{"data/Ogg/MDOWN2", true, false, true},
	{"data/Ogg/FIREEYE", true, false, true},
	{"data/Ogg/VIVI", true, false, true},
	{"data/Ogg/MURA", true, false, true},
	{"data/Ogg/FANFALE1", false, false, true},
	{"data/Ogg/GINSUKE", true, false, true},
	{"data/Ogg/CEMETERY", true, false, true},
	{"data/Ogg/PLANT", true, false, true},
	{"data/Ogg/KODOU", true, false, true},
	{"data/Ogg/FANFALE3", false, false, true},
	{"data/Ogg/FANFALE2", false, false, true},
	{"data/Ogg/DR", true, false, true},
	{"data/Ogg/ESCAPE", true, false, true},
	{"data/Ogg/JENKA", true, false, true},
	{"data/Ogg/MAZE", true, false, true},
	{"data/Ogg/ACCESS", true, false, true},
	{"data/Ogg/IRONH", true, false, true},
	{"data/Ogg/GRAND", true, false, true},
	{"data/Ogg/Curly", true, false, true},
	{"data/Ogg/OSIDE", true, false, true},
	{"data/Ogg/REQUIEM", true, false, true},
	{"data/Ogg/WANPAK2", true, false, true},
	{"data/Ogg/QUIET", true, false, true},
	{"data/Ogg/LASTCAVE", true, false, true},
	{"data/Ogg/BALCONY", true, false, true},
	{"data/Ogg/LASTBTL", true, false, true},
	{"data/Ogg/LASTBT3", true, false, true},
	{"data/Ogg/ENDING", true, false, true},
	{"data/Ogg/ZONBIE", true, false, true},
	{"data/Ogg/BDOWN", true, false, true},
	{"data/Ogg/HELL", true, false, true},
	{"data/Ogg/JENKA2", true, false, true},
	{"data/Ogg/MARINE", true, false, true},
	{"data/Ogg/BALLOS", true, false, true},
	{"data/Ogg/TOROKO", false, false, true},
	{"data/Ogg/WHITE", true, false, true}
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
			playlist[current_song].name = song_path;
			playlist[current_song].loops = false;
			playlist[current_song].split = false;
			playlist[current_song].is_org = false;

			line_current_position += path_length;
			line_current_position += strspn(line_current_position, ", ");

			while (line_current_position[0] != '\0')
			{
				// Get song properties
				size_t property_length = strcspn(line_current_position, ",");

				if (property_length != 0)
				{
					if (strncmp(line_current_position, "loop", 4) == 0)
						playlist[current_song].loops = true;
					else if (strncmp(line_current_position, "split", 5) == 0)
						playlist[current_song].split = true;
					else if (strncmp(line_current_position, "org", 3) == 0)
						playlist[current_song].is_org = true;

					line_current_position += property_length;
				}

				line_current_position += strspn(line_current_position, ", ");
			}
		}

		fclose(playlist_file);
	}
}
