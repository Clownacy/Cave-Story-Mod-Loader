#include <stdio.h>

#include "error.h"

#define ERROR_PATH "mods/error.txt"

void InitError(void)
{
	remove(ERROR_PATH);
}

void PrintError(const char* const string, const char* const printf_arg)
{
	FILE *error_log = fopen(ERROR_PATH, "a");

	fprintf(error_log, string, printf_arg);

	fclose(error_log);
}
