// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_list.h"

#include <stdlib.h>
#include <string.h>

typedef struct ModListEntry
{
	char *path;
	struct ModListEntry *next;
} ModListEntry;

static ModListEntry *mod_list_head;
static unsigned int mod_count;

void AddToModList(char *mod_path)
{
	ModListEntry *new_mod_list_entry = malloc(sizeof(ModListEntry));
	char *malloc_path = malloc(strlen(mod_path) + 1);
	strcpy(malloc_path, mod_path);
	new_mod_list_entry->path = malloc_path;

	new_mod_list_entry->next = mod_list_head;
	mod_list_head = new_mod_list_entry;

	++mod_count;
}

char* GetModPath(unsigned int mod_num)
{
	if (mod_list_head == NULL)
		return NULL;

	ModListEntry *current_mod_ptr = mod_list_head;

	for (unsigned int i = 0; i < mod_num; ++i)
	{
		current_mod_ptr = current_mod_ptr->next;

		if (current_mod_ptr == NULL)
			return NULL;
	}

	return current_mod_ptr->path;
}

unsigned int GetModCount(void)
{
	return mod_count;
}
