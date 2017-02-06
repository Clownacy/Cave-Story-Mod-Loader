// OGG mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdio.h>
#include <string.h>
#include <windows.h>

HMODULE this_hmodule;

void PrintError(FILE **error_log, char *string, char *printf_arg)
{
	if (*error_log == NULL)
		*error_log = fopen("mods/error.txt", "w");

	fprintf(*error_log, string, printf_arg);
}

__declspec(dllexport) void init(void)
{
	SetDllDirectory("mods/deps");
	FILE *mod_list = fopen("mods/mods.txt", "r");

	FILE *error_log = NULL;
	if (mod_list == NULL)
	{
		PrintError(&error_log, "Could not find mods.txt\n", NULL);
		return;
	}

	// Scan through mods.txt, loading each DLL listed
	char filename[MAX_PATH];
	while (fgets(filename, MAX_PATH, mod_list) != NULL)
	{
		// Get path of mod DLL
		filename[strcspn(filename, "\r\n")] = '\0';	// Trim newline characters
		char mod_path[strlen(filename) + 5 + 1];
		strcpy(mod_path, "mods/");
		strcat(mod_path, filename);

		// Load mod DLL
		HMODULE hmodule = LoadLibrary(mod_path);
		if (hmodule == NULL)
		{
			PrintError(&error_log, "Could not find mod '%s'\n", filename);
			continue;
		}

		// Get DLL entry point
		void (*ModEntry)(HMODULE) = (void (*)(HMODULE))GetProcAddress(hmodule, "ModEntry");
		if (ModEntry == NULL)
		{
			PrintError(&error_log, "Mod '%s' did not contain a valid entry point (\"ModEntry\")\n", filename);
			continue;
		}

		// Run mod
		ModEntry(this_hmodule);
	}

	if (error_log != NULL)
		fclose(error_log);

	fclose(mod_list);
	SetDllDirectory(NULL);
}

BOOL APIENTRY DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		this_hmodule = hinstDll;

	return TRUE;
}
