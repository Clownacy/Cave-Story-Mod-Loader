#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct RAMFile RAMFile;

RAMFile* RAMFile_fopen(char *file_path);
size_t RAMFile_fread(void *output, size_t size, size_t count, RAMFile *file);
int RAMFile_fseek(RAMFile *file, int64_t offset, int origin);
int RAMFile_fclose(RAMFile *file);
long RAMFile_ftell(RAMFile *file);
