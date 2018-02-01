// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "log.h"
#include "mod_list.h"
#include "redirect_org_files.h"
#include "settings.h"
#include "sprintfMalloc.h"

HMODULE this_hmodule;

void LoadMod(const char* const filename)
{
	PrintDebug("  Loading mod '%s'...\n", filename);

	char *mod_folder_relative = sprintfMalloc("mods\\%s\\", filename);

	size_t mod_folder_length = GetFullPathName(mod_folder_relative, 0, NULL, NULL);
	char *mod_folder = malloc(mod_folder_length);
	GetFullPathName(mod_folder_relative, mod_folder_length, mod_folder, NULL);

	AddToModList(mod_folder);

	char *mod_path = sprintfMalloc("%s%s", mod_folder, filename);

	SetDllDirectory(mod_folder_relative);
	free(mod_folder_relative);

	// Load mod DLL
	HMODULE hmodule = LoadLibrary(mod_path);
	free(mod_path);

	if (hmodule == NULL)
	{
		free(mod_folder);
		LPVOID error_message;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&error_message, 0, NULL);
		PrintError("  Could not load mod '%s'\n  Windows reports: %s\n", filename, error_message);
		LocalFree(error_message);
		return;
	}

	// Get DLL entry point
	void (*ModEntry)(HMODULE, Setting*, const char* const) = (void (*)(HMODULE, Setting*, const char* const))GetProcAddress(hmodule, "ModEntry");
	if (ModEntry == NULL)
	{
		free(mod_folder);
		PrintError("  Mod '%s' did not contain a valid entry point (\"ModEntry\")\n", filename);
		return;
	}

	Setting *settings = ReadSettings(filename);

	PrintDebug("    Mod's debug output:\n\n");

	// Run mod
	ModEntry(this_hmodule, settings, mod_folder);

	PrintDebug("\n    End of mod's debug output\n");

	SetDllDirectory(NULL);

	PrintDebug("  Done loading mod\n");
}

__declspec(dllexport) void init(void)
{
	mod_loader_settings = ReadSettings(NULL);

	InitLogging();
	//RedirectOrgFiles();

	FILE *mod_list = fopen("mods/mods.txt", "r");

	if (mod_list == NULL)
	{
		PrintMessageBoxError("Could not find mods.txt\n");
		return;
	}

	PrintDebug("Started loading mods\n");

	// Scan through mods.txt, loading each DLL listed
	char filename[MAX_PATH];
	while (fgets(filename, MAX_PATH, mod_list) != NULL)
	{
		// Get path of mod DLL
		filename[strcspn(filename, "\r\n")] = '\0';	// Trim newline characters

		if (strcmp(filename, ""))
		{
			LoadMod(filename);
		}
	}

	PrintDebug("Done loading mods\n");

	fclose(mod_list);

	PrintDebug("Mod Loader initialised\n");
}

BOOL APIENTRY DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		this_hmodule = hinstDll;

	return TRUE;
}
