// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <windows.h>

#include "mod_loader.h"

extern void InitMod(void);

void (*WriteRelativeAddress)(const int instruction_address, const void* const new_destination);
void (*WriteByte)(const int instruction_address, const char value);
void (*WriteWord)(const int instruction_address, const short value);
void (*WriteLong)(const int instruction_address, const int value);
void (*WriteJump)(const int instruction_address, const void* const new_destination);
void (*WriteCall)(const int instruction_address, const void* const new_destination);
void (*FixDoorEnterBug)(void);

static char* (*GetSetting_inner)(const char* const filename, const void* const settings);

const char* location_path;

static HMODULE mod_loader_hmodule;
static const void* settings;

const char* const GetSetting(const char* const setting_name)
{
	return GetSetting_inner(setting_name, settings);
}

__declspec(dllexport) void ModEntry(const HMODULE mod_loader_hmodule_p, const void* const settings_p, const char* const location_path_p)
{
	WriteRelativeAddress = (void (*)(const int, const void* const))GetProcAddress(mod_loader_hmodule_p, "WriteRelativeAddress");
	WriteByte = (void (*)(const int, const char))GetProcAddress(mod_loader_hmodule_p, "WriteByte");
	WriteWord = (void (*)(const int, const short))GetProcAddress(mod_loader_hmodule_p, "WriteWord");
	WriteLong = (void (*)(const int, const int))GetProcAddress(mod_loader_hmodule_p, "WriteLong");
	WriteJump = (void (*)(const int, const void* const))GetProcAddress(mod_loader_hmodule_p, "WriteJump");
	WriteCall = (void (*)(const int, const void* const))GetProcAddress(mod_loader_hmodule_p, "WriteCall");
	FixDoorEnterBug = (void (*)(void))GetProcAddress(mod_loader_hmodule_p, "FixDoorEnterBug");
	GetSetting_inner = (char* (*)(const char* const, const void* const))GetProcAddress(mod_loader_hmodule, "GetSetting");

	mod_loader_hmodule = mod_loader_hmodule_p;
	settings = settings_p;
	location_path = location_path_p;

	InitMod();
}
