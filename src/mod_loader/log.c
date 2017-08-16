// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "log.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

#include "settings.h"

#define VERSION "v1.3+"

#define ERROR_PATH "mods/error.txt"
#define DEBUG_PATH "mods/debug.txt"

bool console_enabled;

void InitLogging(void)
{
	remove(ERROR_PATH);
	remove(DEBUG_PATH);

	console_enabled = GetSettingBool("debug_console", mod_loader_settings);

	if (console_enabled)
	{
		// Create console window, and direct stdout to it
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
	}

	PrintDebug("Logging initialised - Mod Loader version %s\n", VERSION);
}

void PrintToFile(const char* const format, va_list args, const char* const file_path)
{
	FILE *log_file = fopen(file_path, "a");

	vfprintf(log_file, format, args);

	fclose(log_file);
}

__declspec(dllexport) void PrintError(const char* const format, ...)
{
	va_list args;
	va_start(args, format);

	if (console_enabled)
		vprintf(format, args);

	PrintToFile(format, args, ERROR_PATH);
	PrintToFile(format, args, DEBUG_PATH);

	va_end(args);
}

__declspec(dllexport) void PrintDebug(const char* const format, ...)
{
	va_list args;
	va_start(args, format);

	if (console_enabled)
		vprintf(format, args);

	PrintToFile(format, args, DEBUG_PATH);

	va_end(args);
}
