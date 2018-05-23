#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

extern char* vsprintfMalloc(const char *format, va_list args);
extern char* sprintfMalloc(const char *format, ...);

#ifdef __cplusplus
}
#endif
