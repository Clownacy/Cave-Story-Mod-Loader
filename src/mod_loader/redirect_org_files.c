// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "redirect_org_files.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "mod_list.h"
#include "patch.h"

__stdcall char* LoadOrg(char *filename)
{
	char *org_memory;
	FILE *file = NULL;

	// Find highest-priority mod with matching Org file
	for (unsigned int current_mod = GetModCount(); current_mod > 0; --current_mod)
	{
		const char* const mod_path = GetModPath(current_mod - 1);
		if (mod_path != NULL)
		{
			char file_path[strlen(mod_path) + 9 + strlen(filename) + 4 + 1];
			sprintf(file_path, "%sdata/Org/%s.org", mod_path, filename);

			file = fopen(file_path, "rb");

			if (file != NULL)
				break;
		}
	}

	if (file != NULL)
	{
		// Org file found
		fseek(file, 0, SEEK_END);
		size_t filesize = ftell(file);
		rewind(file);

		org_memory = malloc(filesize);

		fread(org_memory, 1, filesize, file);

		fclose(file);
	}
	else
	{
		// No Org file found in mods, fallback on built-in files
		HRSRC resource_block = FindResourceA(NULL, filename, (char*)0x48C6D4);

		if (resource_block == NULL)
			return NULL;

		HGLOBAL resource_handle = LoadResource(NULL, resource_block);
		org_memory = LockResource(resource_handle);
	}

	return org_memory;
}

__asm(
"_sub_41BAD0_new_segment:\n"
"	pushl	8(%ebp)\n"
"	call	_LoadOrg@4\n"
"	test	%eax, %eax\n"
"	jnz	0x41BB20\n"
"	jmp	0x41C094\n"
);
void sub_41BAD0_new_segment(void);

void RedirectOrgFiles(void)
{
//	WriteRelativeAddress(0x41C709 + 1, LoadOrg);
	WriteJump(0x41BAEC, sub_41BAD0_new_segment);	// Skip all the resource stuff
//	WriteCall(0x41BB1A, GetOrgFileBuffer);
//	WriteByte(0x41BB1A + 5, 0x90);
//	WriteByte(0x41BB19 + 0, 0xB8);
//	WriteLong(0x41BB19 + 1, file_memory);
//	WriteWord(0x41BB19 + 5, 0x008B);
}
