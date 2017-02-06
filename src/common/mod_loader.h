#pragma once

#include <windows.h>

void (*WriteRelativeAddress)(const int instruction_address, const void* const new_destination);
void (*WriteByte)(const int instruction_address, const char value);
void (*WriteJump)(const int instruction_address, const void* const new_destination);
void (*WriteCall)(const int instruction_address, const void* const new_destination);
void (*FixDoorEnterBug)(void);

void GetModLoaderFunctions(HMODULE mod_loader_hmodule)
{
	WriteRelativeAddress = (void (*)(const int, const void* const))GetProcAddress(mod_loader_hmodule, "WriteRelativeAddress");
	WriteByte = (void (*)(const int, const char))GetProcAddress(mod_loader_hmodule, "WriteByte");
	WriteJump = (void (*)(const int, const void* const))GetProcAddress(mod_loader_hmodule, "WriteJump");
	WriteCall = (void (*)(const int, const void* const))GetProcAddress(mod_loader_hmodule, "WriteCall");
	FixDoorEnterBug = (void (*)(void))GetProcAddress(mod_loader_hmodule, "FixDoorEnterBug");
}
