// OGG mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "mod_loader.h"

__declspec(dllexport) void WriteRelativeAddress(const int instruction_address, const void* const new_destination)
{
	const int relative_address = (int)new_destination - (instruction_address + 4);
	WriteProcessMemory(GetCurrentProcess(), (void*)instruction_address, &relative_address, 4, NULL);
}

__declspec(dllexport) void WriteByte(const int instruction_address, const char value)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)instruction_address, &value, 1, NULL);
}

__declspec(dllexport) void WriteJump(const int instruction_address, const void* const new_destination)
{
	WriteByte(instruction_address, 0xE9);
	WriteRelativeAddress(instruction_address + 1, new_destination);
}

__declspec(dllexport) void WriteCall(const int instruction_address, const void* const new_destination)
{
	WriteByte(instruction_address, 0xE8);
	WriteRelativeAddress(instruction_address + 1, new_destination);
}

void PrintError(FILE **error_log, char *string, char *printf_arg)
{
	if (*error_log == NULL)
		*error_log = fopen("error.txt", "w");

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
		void (*ModInit)(void) = (void (*)(void))GetProcAddress(hmodule, "InitMod");
		if (ModInit == NULL)
		{
			PrintError(&error_log, "Mod '%s' did not contain a valid entry point (\"InitMod\")\n", filename);
			continue;
		}

		// Run mod
		ModInit();
	}

	if (error_log != NULL)
		fclose(error_log);

	fclose(mod_list);
	SetDllDirectory(NULL);
}
