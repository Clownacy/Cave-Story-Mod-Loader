// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdlib.h>
#include <string.h>

#include "mod_list.h"

typedef struct ModListEntry
{
	char *path;
	struct ModListEntry *next;
} ModListEntry;

ModListEntry *mod_list_head;
unsigned int mod_count;

void AddToModList(char *mod_path)
{
	ModListEntry *new_mod_list_entry = malloc(sizeof(ModListEntry));
	char *malloc_path = malloc(strlen(mod_path) + 1);
	strcpy(malloc_path, mod_path);
	new_mod_list_entry->path = malloc_path;
	new_mod_list_entry->next = NULL;

	ModListEntry **current_mod_ptr = &mod_list_head;
	while (*current_mod_ptr != NULL)
		 current_mod_ptr = &(*current_mod_ptr)->next;

	*current_mod_ptr = new_mod_list_entry;

	++mod_count;
}

const char* const GetModPath(unsigned int mod_num)
{
	if (mod_list_head == NULL)
		return NULL;

	ModListEntry *current_mod_ptr = mod_list_head;

	for (int i=0; i < mod_num; ++i)
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
