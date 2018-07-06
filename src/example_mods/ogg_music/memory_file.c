#include "memory_file.h"

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct MemoryFile
{
	unsigned char *data;
	size_t size;
	long current_offset;
} MemoryFile;

MemoryFile* MemoryFile_fopen_from(unsigned char *data, size_t size)
{
	MemoryFile *memory_file = malloc(sizeof(MemoryFile));

	memory_file->data = data;
	memory_file->size = size;
	memory_file->current_offset = 0;

	return memory_file;
}

MemoryFile* MemoryFile_fopen(const char* const file_path)
{
	MemoryFile *memory_file = NULL;

	FILE *file = fopen(file_path, "rb");
	if (file != NULL)
	{

		fseek(file, 0, SEEK_END);
		const size_t size = ftell(file);
		rewind(file);

		unsigned char *data = malloc(size);

		fread(data, 1, size, file);

		fclose(file);

		memory_file = MemoryFile_fopen_from(data, size);
	}

	return memory_file;
}

int MemoryFile_fclose(MemoryFile *file)
{
	free(file->data);
	free(file);

	return 0;
}

size_t MemoryFile_fread(void *output, size_t size, size_t count, MemoryFile *file)
{
	const unsigned int elements_remaining = (file->size - file->current_offset) / size;

	if (count > elements_remaining)
		count = elements_remaining;

	memcpy(output, file->data + file->current_offset, size * count);

	file->current_offset += size * count;

	return count;
}

int MemoryFile_fseek(MemoryFile *file, long offset, int origin)
{
	switch (origin)
	{
		case SEEK_SET:
		{
			file->current_offset = offset;
			break;
		}
		case SEEK_CUR:
		{
			file->current_offset += offset;
			break;
		}
		case SEEK_END:
		{
			file->current_offset = file->size + offset;
			break;
		}
		default:
		{
			return -1;
		}
	}

	return 0;
}

long MemoryFile_ftell(MemoryFile *file)
{
	return file->current_offset;
}
