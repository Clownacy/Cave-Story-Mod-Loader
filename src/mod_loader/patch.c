// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch.h"

#include <windows.h>

__declspec(dllexport) void WriteRelativeAddress(void* const address, const void* const new_destination)
{
	const int relative_address = (int)new_destination - ((int)address + 4);
	WriteProcessMemory(GetCurrentProcess(), address, &relative_address, 4, NULL);
}

__declspec(dllexport) void WriteByte(void* const address, const char value)
{
	WriteProcessMemory(GetCurrentProcess(), address, &value, 1, NULL);
}

__declspec(dllexport) void WriteWord(void* const address, const short value)
{
	WriteProcessMemory(GetCurrentProcess(), address, &value, 2, NULL);
}

__declspec(dllexport) void WriteLong(void* const address, const int value)
{
	WriteProcessMemory(GetCurrentProcess(), address, &value, 4, NULL);
}

__declspec(dllexport) void WriteWordBE(void* const address, const short value)
{
	short big_endian_value = ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
	WriteProcessMemory(GetCurrentProcess(), address, &big_endian_value, 2, NULL);
}

__declspec(dllexport) void WriteLongBE(void* const address, const int value)
{
	int big_endian_value = ((value & 0xFF) << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) | ((value & 0xFF000000) >> 24);
	WriteProcessMemory(GetCurrentProcess(), address, &big_endian_value, 4, NULL);
}

__declspec(dllexport) void WriteJump(void* const address, const void* const new_destination)
{
	WriteByte(address, 0xE9);
	WriteRelativeAddress(address + 1, new_destination);
}

__declspec(dllexport) void WriteCall(void* const address, const void* const new_destination)
{
	WriteByte(address, 0xE8);
	WriteRelativeAddress(address + 1, new_destination);
}
