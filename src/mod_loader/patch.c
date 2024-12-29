// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "patch.h"

#include <stdbool.h>

#include "log.h"

#include <windows.h>

static void WriteData(void* const address, const void* const value, const unsigned int size)
{
	static bool got_handle;
	static HANDLE handle;

	if (!got_handle)
	{
		got_handle = true;
		handle = GetCurrentProcess();
	}

	PrintPollution("%d bytes written at %p\n", size, address);
	WriteProcessMemory(handle, address, value, size, NULL);
}

void WriteRelativeAddress(void* const address, const void* const new_destination)
{
	const unsigned int relative_address = (unsigned int)new_destination - ((unsigned int)((char*)address + 4));
	WriteData(address, &relative_address, 4);
}

void WriteByte(void* const address, const unsigned char value)
{
	WriteData(address, &value, 1);
}

void WriteWord(void* const address, const unsigned short value)
{
	WriteData(address, &value, 2);
}

void WriteLong(void* const address, const unsigned int value)
{
	WriteData(address, &value, 4);
}

void WriteWordBE(void* const address, const unsigned short value)
{
	const unsigned short big_endian_value = ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
	WriteData(address, &big_endian_value, 2);
}

void WriteLongBE(void* const address, const unsigned int value)
{
	const unsigned int big_endian_value = ((value & 0xFF) << 24) | ((value & 0xFF00) << 8) | ((value & 0xFF0000) >> 8) | ((value & 0xFF000000) >> 24);
	WriteData(address, &big_endian_value, 4);
}

void WriteJump(void* const address, const void* const new_destination)
{
	WriteByte(address, 0xE9);
	WriteRelativeAddress((char*)address + 1, new_destination);
}

void WriteCall(void* const address, const void* const new_destination)
{
	WriteByte(address, 0xE8);
	WriteRelativeAddress((char*)address + 1, new_destination);
}

void WriteNOPs(void* const address, const unsigned int count)
{
	char *current_address = address;

	for (unsigned int i = 0; i < count / 4; ++i)
	{
		WriteData(current_address, &(unsigned char[4]){0x90, 0x90, 0x90, 0x90}, 4);
		current_address += 4;
	}

	if (count & 2)
	{
		WriteData(current_address, &(unsigned char[2]){0x90, 0x90}, 2);
		current_address += 2;
	}

	if (count & 1)
		WriteData(current_address, &(unsigned char){0x90}, 1);
}
