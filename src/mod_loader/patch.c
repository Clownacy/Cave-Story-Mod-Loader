// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch.h"

#include "log.h"

#include <windows.h>

static void WriteData(void* const address, const void* const value, const unsigned int size)
{
	PrintPollution("%d bytes written at %p\n", size, address);
	WriteProcessMemory(GetCurrentProcess(), address, value, size, NULL);
}

__declspec(dllexport) void WriteRelativeAddress(void* const address, const void* const new_destination)
{
	const int relative_address = (int)new_destination - ((int)address + 4);
	WriteData(address, &relative_address, 4);
}

__declspec(dllexport) void WriteByte(void* const address, const char value)
{
	WriteData(address, &value, 1);
}

__declspec(dllexport) void WriteWord(void* const address, const short value)
{
	WriteData(address, &value, 2);
}

__declspec(dllexport) void WriteLong(void* const address, const int value)
{
	WriteData(address, &value, 4);
}

__declspec(dllexport) void WriteWordBE(void* const address, const short value)
{
	short big_endian_value = ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
	WriteData(address, &big_endian_value, 2);
}

__declspec(dllexport) void WriteLongBE(void* const address, const int value)
{
	int big_endian_value = ((value & 0xFF) << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) | ((value & 0xFF000000) >> 24);
	WriteData(address, &big_endian_value, 4);
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

__declspec(dllexport) void WriteNOPs(void* const address, const unsigned int count)
{
	char *nop_buffer = malloc(count);
	for (unsigned int i = 0; i < count; ++i)
		nop_buffer[i] = 0x90;

	WriteData(address, nop_buffer, count);

	free(nop_buffer);
}
