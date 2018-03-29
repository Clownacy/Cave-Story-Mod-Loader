#include "ram_file.h"

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct RAMFile
{
	char *data;
	size_t data_size;
	signed long current_offset;
} RAMFile;

RAMFile* RAMFile_fopen(char *file_path)
{
	RAMFile *memory_file = NULL;

	FILE *file = fopen(file_path, "rb");
	if (file != NULL)
	{
		memory_file = malloc(sizeof(RAMFile));

		fseek(file, 0, SEEK_END);
		memory_file->data_size = ftell(file);
		rewind(file);

		memory_file->data = malloc(memory_file->data_size);

		fread(memory_file->data, 1, memory_file->data_size, file);

		fclose(file);

		memory_file->current_offset = 0;
	}

	return memory_file;
}

size_t RAMFile_fread(void *output, size_t size, size_t count, RAMFile *file)
{
	const unsigned int elements_remaining = (file->data_size - file->current_offset) / size;

	if (count > elements_remaining)
		count = elements_remaining;

	memcpy(output, file->data + file->current_offset, size * count);

	file->current_offset += size * count;

	return count;
}

int RAMFile_fseek(RAMFile *file, int64_t offset, int origin)
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
			file->current_offset = file->data_size + offset;
			break;
		}
		default:
		{
			return -1;
		}
	}

	return 0;
}

int RAMFile_fclose(RAMFile *file)
{
	free(file->data);
	free(file);

	return 0;
}

long RAMFile_ftell(RAMFile *file)
{
	return file->current_offset;
}
