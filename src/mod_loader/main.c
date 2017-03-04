// OGG mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "error.h"
#include "settings.h"

HMODULE this_hmodule;

void LoadMod(const char* const filename)
{
		char *mod_folder = malloc(5 + strlen(filename) + 1 + 1);
		strcpy(mod_folder, "mods/");
		strcat(mod_folder, filename);
		strcat(mod_folder, "/");

		char mod_path[strlen(mod_folder) + strlen(filename) + 1];
		strcpy(mod_path, mod_folder);
		strcat(mod_path, filename);

		// Load mod DLL
		HMODULE hmodule = LoadLibrary(mod_path);
		if (hmodule == NULL)
		{
			PrintError("Could not find mod '%s'\n", filename);
			return;
		}

		// Get DLL entry point
		void (*ModEntry)(HMODULE, void*, const char* const) = (void (*)(HMODULE, void*, const char* const))GetProcAddress(hmodule, "ModEntry");
		if (ModEntry == NULL)
		{
			PrintError("Mod '%s' did not contain a valid entry point (\"ModEntry\")\n", filename);
			return;
		}

		// Run mod
		ModEntry(this_hmodule, ReadSettings(filename), mod_folder);
}

__declspec(dllexport) void init(void)
{
	InitError();

	SetDllDirectory("mods/_deps");

	FILE *mod_list = fopen("mods/mods.txt", "r");

	if (mod_list == NULL)
	{
		PrintError("Could not find mods.txt\n", NULL);
		return;
	}

	// Scan through mods.txt, loading each DLL listed
	char filename[MAX_PATH];
	while (fgets(filename, MAX_PATH, mod_list) != NULL)
	{
		// Get path of mod DLL
		filename[strcspn(filename, "\r\n")] = '\0';	// Trim newline characters
		LoadMod(filename);
	}

	fclose(mod_list);
	SetDllDirectory(NULL);
}

BOOL APIENTRY DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		this_hmodule = hinstDll;

	return TRUE;
}
