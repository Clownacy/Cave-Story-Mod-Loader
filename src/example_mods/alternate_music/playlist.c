// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "playlist.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "mod_loader.h"
#include "sprintfMalloc.h"

typedef struct PlaylistListEntry
{
	struct PlaylistListEntry *next;
	unsigned int id;
	PlaylistEntry playlist_data;
} PlaylistListEntry;

static PlaylistListEntry *playlist_list_head;

static PlaylistListEntry *last_playlist_list_entry;

PlaylistEntry* GetFirstPlaylistEntry(void)
{
	last_playlist_list_entry = playlist_list_head;

	return &last_playlist_list_entry->playlist_data;
}

PlaylistEntry* GetNextPlaylistEntry(void)
{
	PlaylistEntry *playlist_entry = NULL;

	if (last_playlist_list_entry->next)
	{
		last_playlist_list_entry = last_playlist_list_entry->next;

		playlist_entry = &last_playlist_list_entry->playlist_data;
	}

	return playlist_entry;
}

PlaylistEntry* GetPlaylistEntry(unsigned int song_id)
{
	PlaylistEntry *playlist_entry = NULL;

	for (PlaylistListEntry *current_playlist_list_entry = playlist_list_head; current_playlist_list_entry != NULL; current_playlist_list_entry = current_playlist_list_entry->next)
	{
		if (current_playlist_list_entry->id == song_id)
		{
			playlist_entry = &current_playlist_list_entry->playlist_data;
			break;
		}
	}

	return playlist_entry;
}

bool InitPlaylist(void)
{
	bool success = true;

	const char* const playlist_filename = ModLoader_GetSettingString("playlist", NULL);

	if (playlist_filename == NULL)
	{
		success = false;
	}
	else
	{
		char *playlist_folder = sprintfMalloc("%s%s", mod_loader_path_to_dll, playlist_filename);

		char *playlist_path = sprintfMalloc("%s/playlist.txt", playlist_folder);
		FILE *playlist_file = fopen(playlist_path, "r");

		if (playlist_file == NULL)
		{
			ModLoader_PrintError("Could not open playlist.txt at '%s'\n", playlist_path);
			success = false;
		}
		else
		{
			char line[MAX_PATH];
			for (int current_song = 0; fgets(line, MAX_PATH, playlist_file) != NULL; ++current_song)
			{
				char *line_current_position = line;

				line_current_position[strcspn(line_current_position, "\r\n")] = '\0';	// Trim newline characters

				// Get path to ogg file
				size_t path_length = strcspn(line_current_position, ",");

				if (path_length == 0)
					continue;

				char *song_path_rel = malloc(path_length + 1);
				strncpy(song_path_rel, line_current_position, path_length);
				song_path_rel[path_length] = '\0';

				ModLoader_PrintDebug("playlist.txt: song '%s'\n", song_path_rel);

				char *song_path = sprintfMalloc("%s/%s", playlist_folder, song_path_rel);
				free(song_path_rel);

				// Now that we have the path to the ogg file, let's set up the playlist data
				PlaylistListEntry *playlist_list_entry = malloc(sizeof(PlaylistListEntry));
				playlist_list_entry->next = playlist_list_head;
				playlist_list_head = playlist_list_entry;

				playlist_list_entry->id = current_song;

				playlist_list_entry->playlist_data.name = song_path;
				playlist_list_entry->playlist_data.loop = false;
				playlist_list_entry->playlist_data.is_org = false;
				playlist_list_entry->playlist_data.sound = NULL;

				line_current_position += path_length;
				line_current_position += strspn(line_current_position, ", ");

				while (line_current_position[0] != '\0')
				{
					// Get song properties
					size_t property_length = strcspn(line_current_position, ",");

					if (property_length != 0)
					{
						if (!strncmp(line_current_position, "loop", 4))
							playlist_list_entry->playlist_data.loop = true;
						else if (!strncmp(line_current_position, "org", 3))
							playlist_list_entry->playlist_data.is_org = true;

						line_current_position += property_length;
					}

					line_current_position += strspn(line_current_position, ", ");
				}
			}

			fclose(playlist_file);
		}

		free(playlist_path);
		free(playlist_folder);
	}

	return success;
}
