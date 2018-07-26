// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "log.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

#include "settings.h"
#include "sprintfMalloc.h"

#define ERROR_PATH "mods/error.txt"
#define DEBUG_PATH "mods/debug.txt"
#define POLLUTION_PATH "mods/pollution.txt"

static bool logging_initialised;

static bool console_enabled;
static bool logging_enabled;
static bool pollution_map_enabled;

void InitLogging(void)
{
	logging_initialised = true;

	console_enabled = GetSettingBool("debug_console", false, mod_loader_settings);
	logging_enabled = GetSettingBool("logging", false, mod_loader_settings);
	pollution_map_enabled = GetSettingBool("pollution_map", false, mod_loader_settings);

	remove(ERROR_PATH);
	if (logging_enabled)
		remove(DEBUG_PATH);
	if (pollution_map_enabled)
		remove(POLLUTION_PATH);

	if (console_enabled)
	{
		// Create console window, and direct stdout to it
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
	}

	PrintDebug("Logging initialised - Mod Loader version " MOD_LOADER_VERSION "\n");
}

static void PrintToFile(const char* const format, va_list args, const char* const file_path)
{
	if (logging_initialised)
	{
		FILE *log_file = fopen(file_path, "a");

		vfprintf(log_file, format, args);

		fclose(log_file);
	}
}

__declspec(dllexport) void PrintMessageBoxError(const char* const format, ...)
{
	va_list args, args_copy;
	va_start(args, format);

	if (console_enabled)
	{
		va_copy(args_copy, args);
		vprintf(format, args_copy);
		va_end(args_copy);
	}

	va_copy(args_copy, args);
	PrintToFile(format, args_copy, ERROR_PATH);
	va_end(args_copy);

	if (logging_enabled)
	{
		va_copy(args_copy, args);
		PrintToFile(format, args_copy, DEBUG_PATH);
		va_end(args_copy);
	}

	char *message = vsprintfMalloc(format, args);
	MessageBox(NULL, message, "Mod Loader error", 0);
	free(message);

	va_end(args);
}

__declspec(dllexport) void PrintError(const char* const format, ...)
{
	va_list args, args_copy;
	va_start(args, format);

	if (console_enabled)
	{
		va_copy(args_copy, args);
		vprintf(format, args_copy);
		va_end(args_copy);
	}

	va_copy(args_copy, args);
	PrintToFile(format, args_copy, ERROR_PATH);
	va_end(args_copy);

	if (logging_enabled)
		PrintToFile(format, args, DEBUG_PATH);

	va_end(args);
}

__declspec(dllexport) void PrintDebug(const char* const format, ...)
{
	va_list args, args_copy;
	va_start(args, format);

	if (console_enabled)
	{
		va_copy(args_copy, args);
		vprintf(format, args_copy);
		va_end(args_copy);
	}

	if (logging_enabled)
		PrintToFile(format, args, DEBUG_PATH);

	va_end(args);
}

__declspec(dllexport) void PrintPollution(const char* const format, ...)
{
	if (pollution_map_enabled)
	{
		va_list args;
		va_start(args, format);

		PrintToFile(format, args, POLLUTION_PATH);

		va_end(args);
	}
}
