// OGG mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <stdbool.h>
#include <windows.h>

#include "60fps.h"
#include "input.h"
#include "main.h"
#include "music.h"

void WriteRelativeAddress(const int instruction_address, const void* const new_destination)
{
	const int relative_address = (int)new_destination - (instruction_address + 4);
	WriteProcessMemory(GetCurrentProcess(), (void*)instruction_address, &relative_address, 4, NULL);
}

void WriteByte(const int instruction_address, const char value)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)instruction_address, &value, 1, NULL);
}

void WriteJump(const int instruction_address, const void* const new_destination)
{
	WriteByte(instruction_address, 0xE9);
	WriteRelativeAddress(instruction_address + 1, new_destination);
}
void WriteCall(const int instruction_address, const void* const new_destination)
{
	WriteByte(instruction_address, 0xE8);
	WriteRelativeAddress(instruction_address + 1, new_destination);
}

__declspec(dllexport) void init(void)
{
	// Do all init work here
	Init60FPS();
	InitInput();
	InitMusic();
}
