// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "log.h"

#include <stdarg.h>
#include <stdio.h>

#define ERROR_PATH "mods/error.txt"
#define DEBUG_PATH "mods/debug.txt"

void InitLogging(void)
{
	remove(ERROR_PATH);
	remove(DEBUG_PATH);

	PrintDebug("Logging initialised\n");
}

void PrintToFile(const char* const format, va_list args, const char* const file_path)
{
	FILE *log_file = fopen(file_path, "a");

	vfprintf(log_file, format, args);

	fclose(log_file);
}

void PrintError(const char* const format, ...)
{
	va_list args;
	va_start(args, format);

	PrintToFile(format, args, ERROR_PATH);
	PrintToFile(format, args, DEBUG_PATH);

	va_end(args);
}

void PrintDebug(const char* const format, ...)
{
	va_list args;
	va_start(args, format);

	PrintToFile(format, args, DEBUG_PATH);

	va_end(args);
}
