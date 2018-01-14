#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATCH_1_SIZE (5)
#define PATCH_2_SIZE (0x90)

unsigned char vanilla_code_1[PATCH_1_SIZE] = {0xA1, 0x20, 0x8B, 0x49, 0x00};
unsigned char patched_code_1[PATCH_1_SIZE] = {0xE8, 0xE2, 0xE7, 0xFF, 0xFF};

unsigned char vanilla_code_2[PATCH_2_SIZE] = {
	0x55, 0x8B, 0xEC, 0x81, 0xEC, 0x10, 0x01, 0x00, 0x00, 0xA1, 0x20, 0x8B, 0x49, 0x00, 0x89, 0x45,
	0xF8, 0x68, 0x28, 0xE3, 0x49, 0x00, 0x68, 0x70, 0xC4, 0x48, 0x00, 0x8D, 0x85, 0xF0, 0xFE, 0xFF,
	0xFF, 0x50, 0xE8, 0xD9, 0x03, 0x07, 0x00, 0x83, 0xC4, 0x0C, 0x68, 0x80, 0xC4, 0x48, 0x00, 0x8D,
	0x8D, 0xF0, 0xFE, 0xFF, 0xFF, 0x51, 0xE8, 0xB2, 0x03, 0x07, 0x00, 0x83, 0xC4, 0x08, 0x89, 0x45,
	0xFC, 0x83, 0x7D, 0xFC, 0x00, 0x75, 0x04, 0x33, 0xC0, 0xEB, 0x32, 0x8B, 0x55, 0x14, 0x52, 0x8B,
	0x45, 0x10, 0x50, 0x8B, 0x4D, 0x0C, 0x51, 0x8B, 0x55, 0x08, 0x52, 0x68, 0x84, 0xC4, 0x48, 0x00,
	0x8B, 0x45, 0xFC, 0x50, 0xE8, 0xA3, 0x0B, 0x07, 0x00, 0x83, 0xC4, 0x18, 0x8B, 0x4D, 0xFC, 0x51,
	0xE8, 0x96, 0x01, 0x07, 0x00, 0x83, 0xC4, 0x04, 0xB8, 0x01, 0x00, 0x00, 0x00, 0x8B, 0x4D, 0xF8,
	0xE8, 0x2C, 0x01, 0x07, 0x00, 0x8B, 0xE5, 0x5D, 0xC3, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
};

unsigned char patched_code_2[PATCH_2_SIZE] = {
	0x55, 0x8B, 0xEC, 0x68, 0x3B, 0x0C, 0x41, 0x00, 0xFF, 0x15, 0xAC, 0xC0, 0x48, 0x00, 0x85, 0xC0,
	0x74, 0x12, 0x68, 0x4B, 0x0C, 0x41, 0x00, 0x50, 0xFF, 0x15, 0x34, 0xC1, 0x48, 0x00, 0x85, 0xC0,
	0x74, 0x02, 0xFF, 0xD0, 0xA1, 0x20, 0x8B, 0x49, 0x00, 0x5D, 0xC3, 0x6D, 0x6F, 0x64, 0x73, 0x2F,
	0x6D, 0x6F, 0x64, 0x5F, 0x6C, 0x6F, 0x61, 0x64, 0x65, 0x72, 0x00, 0x69, 0x6E, 0x69, 0x74, 0x00,
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
};

void DoPatch(unsigned char *vanilla, unsigned char *patch, unsigned char *exe, size_t address, size_t size)
{
	if (memcmp(vanilla, &exe[address], size) != 0)
	{
		printf("Warning: The code at 0x%X is modified, the mod loader may not work!\n", address);
	}

	for (unsigned int i = 0; i < size; ++i)
	{
		exe[address + i] = patch[i];
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("This is an automated EXE patcher to install Clownacy's Cave Story mod loader.\n\nTo patch an EXE, drag-and-drop it onto this program (or pass it on the command line as a parameter).\n\n");
	}
	else
	{
		FILE *in_file = fopen(argv[1], "rb");
		if (in_file == NULL)
		{
			printf("Could not open file '%s'\n", argv[1]);
		}
		else
		{
			fseek(in_file, 0, SEEK_END);
			size_t file_size = ftell(in_file);
			rewind(in_file);

			unsigned char *file_memory = malloc(file_size);
			fread(file_memory, 1, file_size, in_file);
			fclose(in_file);

			if (!memcmp(patched_code_1, &file_memory[0x12429], PATCH_1_SIZE) && !memcmp(patched_code_2, &file_memory[0x10C10], PATCH_2_SIZE))
			{
				printf("Error: EXE has already been patched\n\n");
			}
			else
			{
				DoPatch(vanilla_code_1, patched_code_1, file_memory, 0x12429, PATCH_1_SIZE);
				DoPatch(vanilla_code_2, patched_code_2, file_memory, 0x10C10, PATCH_2_SIZE);

				FILE *out_file = fopen("Doukutsu_mod_loader.exe", "wb");
				fwrite(file_memory, 1, file_size, out_file);
				fclose(out_file);

				printf("Patched EXE created as 'Doukutsu_mod_loader.exe'\n\n");
			}

			free(file_memory);
		}
	}
	printf("Press enter to close this window...\n");
	getchar();
	return 0;
}
