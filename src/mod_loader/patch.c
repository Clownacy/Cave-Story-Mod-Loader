// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <windows.h>

#include "patch.h"

__declspec(dllexport) void WriteRelativeAddress(const int instruction_address, const void* const new_destination)
{
	const int relative_address = (int)new_destination - (instruction_address + 4);
	WriteProcessMemory(GetCurrentProcess(), (void*)instruction_address, &relative_address, 4, NULL);
}

__declspec(dllexport) void WriteByte(const int instruction_address, const char value)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)instruction_address, &value, 1, NULL);
}

__declspec(dllexport) void WriteWord(const int instruction_address, const short value)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)instruction_address, &value, 2, NULL);
}

__declspec(dllexport) void WriteLong(const int instruction_address, const int value)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)instruction_address, &value, 4, NULL);
}

__declspec(dllexport) void WriteJump(const int instruction_address, const void* const new_destination)
{
	WriteByte(instruction_address, 0xE9);
	WriteRelativeAddress(instruction_address + 1, new_destination);
}

__declspec(dllexport) void WriteCall(const int instruction_address, const void* const new_destination)
{
	WriteByte(instruction_address, 0xE8);
	WriteRelativeAddress(instruction_address + 1, new_destination);
}

