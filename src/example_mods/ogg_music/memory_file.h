#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct MemoryFile MemoryFile;

MemoryFile* MemoryFile_fopen(char *file_path);
size_t MemoryFile_fread(void *output, size_t size, size_t count, MemoryFile *file);
int MemoryFile_fseek(MemoryFile *file, long long offset, int origin);
int MemoryFile_fclose(MemoryFile *file);
long MemoryFile_ftell(MemoryFile *file);
