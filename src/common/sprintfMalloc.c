// sprintfMalloc (because I'm lazy)
// Copyright © 2017 Clownacy

#include "sprintfMalloc.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

char* sprintfMalloc(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	const int string_length = vsnprintf(NULL, 0, format, args) + 1;
	char *string = malloc(string_length);
	vsnprintf(string, string_length, format, args);

	va_end(args);

	return string;
}
