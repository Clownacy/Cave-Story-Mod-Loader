// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "error.h"

#include <stdarg.h>
#include <stdio.h>

#define ERROR_PATH "mods/error.txt"

void InitError(void)
{
	remove(ERROR_PATH);
}

void PrintError(const char* const format, ...)
{
	va_list args;
	va_start(args, format);

	FILE *error_log = fopen(ERROR_PATH, "a");

	vfprintf(error_log, format, args);

	fclose(error_log);

	va_end(args);
}
