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
		char mod_path[strlen(filename) + 1 + strlen(filename) + 5 + 1];
		strcpy(mod_path, "mods/");
		strcat(mod_path, filename);
		strcat(mod_path, "/");
		strcat(mod_path, filename);

		// Load mod DLL
		HMODULE hmodule = LoadLibrary(mod_path);
		if (hmodule == NULL)
		{
			PrintError("Could not find mod '%s'\n", filename);
			return;
		}

		// Get DLL entry point
		void (*ModEntry)(HMODULE, void*) = (void (*)(HMODULE, void*))GetProcAddress(hmodule, "ModEntry");
		if (ModEntry == NULL)
		{
			PrintError("Mod '%s' did not contain a valid entry point (\"ModEntry\")\n", filename);
			return;
		}

		// Run mod
		ModEntry(this_hmodule, ReadSettings(filename));
}

__declspec(dllexport) void init(void)
{
	InitError();

	SetDllDirectory("mods/deps");

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
