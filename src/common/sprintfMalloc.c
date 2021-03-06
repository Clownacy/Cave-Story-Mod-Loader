// sprintfMalloc (because I'm lazy)
// Public domain

#include "sprintfMalloc.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

char* vsprintfMalloc(const char *format, va_list args)
{
	va_list args_copy;

	va_copy(args_copy, args);
	const int string_length = vsnprintf(NULL, 0, format, args_copy) + 1;
	va_end(args_copy);

	char *string = (char*)malloc(string_length);

	vsnprintf(string, string_length, format, args);

	return string;
}

char* sprintfMalloc(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	char *string = vsprintfMalloc(format, args);

	va_end(args);

	return string;
}
