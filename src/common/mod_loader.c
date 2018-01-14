// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_loader.h"

#include <stdbool.h>
#include <windows.h>

extern void InitMod(void);

void (*WriteRelativeAddress)(void* const address, const void* const new_destination);
void (*WriteByte)(void* const address, const unsigned char value);
void (*WriteWord)(void* const address, const unsigned short value);
void (*WriteLong)(void* const address, const unsigned int value);
void (*WriteWordBE)(void* const address, const unsigned short value);
void (*WriteLongBE)(void* const address, const unsigned int value);
void (*WriteJump)(void* const address, const void* const new_destination);
void (*WriteCall)(void* const address, const void* const new_destination);
void (*WriteNOPs)(void* const address, const unsigned int count);
void (*FixDoorEnterBug)(void);
void (*PrintError)(const char* const format, ...);
void (*PrintDebug)(const char* const format, ...);

static const char* (*GetSettingString_inner)(const char* const filename, const char* const default_string, const Setting* const settings);
static int (*GetSettingInt_inner)(const char* const filename, const int default_int, const Setting* const settings);
static bool (*GetSettingBool_inner)(const char* const filename, const bool default_bool, const Setting* const settings);

const char *location_path;

static const Setting *settings;

const char* GetSettingString(const char* const setting_name, const char* const default_string)
{
	return GetSettingString_inner(setting_name, default_string, settings);
}

int GetSettingInt(const char* const setting_name, const int default_int)
{
	return GetSettingInt_inner(setting_name, default_int, settings);
}

bool GetSettingBool(const char* const setting_name, const bool default_bool)
{
	return GetSettingBool_inner(setting_name, default_bool, settings);
}

__declspec(dllexport) void ModEntry(const HMODULE mod_loader_hmodule, const Setting* const settings_p, const char* const location_path_p)
{
	settings = settings_p;
	location_path = location_path_p;

	WriteRelativeAddress = (void (*)(void* const, const void* const))GetProcAddress(mod_loader_hmodule, "WriteRelativeAddress");
	WriteByte = (void (*)(void* const, const unsigned char))GetProcAddress(mod_loader_hmodule, "WriteByte");
	WriteWord = (void (*)(void* const, const unsigned short))GetProcAddress(mod_loader_hmodule, "WriteWord");
	WriteLong = (void (*)(void* const, const unsigned int))GetProcAddress(mod_loader_hmodule, "WriteLong");
	WriteWordBE = (void (*)(void* const, const unsigned short))GetProcAddress(mod_loader_hmodule, "WriteWordBE");
	WriteLongBE = (void (*)(void* const, const unsigned int))GetProcAddress(mod_loader_hmodule, "WriteLongBE");
	WriteJump = (void (*)(void* const, const void* const))GetProcAddress(mod_loader_hmodule, "WriteJump");
	WriteCall = (void (*)(void* const, const void* const))GetProcAddress(mod_loader_hmodule, "WriteCall");
	WriteNOPs = (void (*)(void* const, const unsigned int))GetProcAddress(mod_loader_hmodule, "WriteNOPs");
	FixDoorEnterBug = (void (*)(void))GetProcAddress(mod_loader_hmodule, "FixDoorEnterBug");

	GetSettingString_inner = (const char* (*)(const char* const, const char* const, const Setting* const))GetProcAddress(mod_loader_hmodule, "GetSettingString");
	GetSettingInt_inner = (int (*)(const char* const, const int, const Setting* const))GetProcAddress(mod_loader_hmodule, "GetSettingInt");
	GetSettingBool_inner = (bool (*)(const char* const, const bool, const Setting* const))GetProcAddress(mod_loader_hmodule, "GetSettingBool");

	PrintError = (void (*)(const char* const format, ...))GetProcAddress(mod_loader_hmodule, "PrintError");
	PrintDebug = (void (*)(const char* const format, ...))GetProcAddress(mod_loader_hmodule, "PrintDebug");

	if (WriteRelativeAddress == NULL ||
	WriteByte == NULL ||
	WriteWord == NULL ||
	WriteLong == NULL ||
	WriteWordBE == NULL ||
	WriteLongBE == NULL ||
	WriteJump == NULL ||
	WriteCall == NULL ||
	WriteNOPs == NULL ||
	FixDoorEnterBug == NULL ||
	GetSettingString_inner == NULL ||
	GetSettingInt_inner == NULL ||
	GetSettingBool_inner == NULL ||
	PrintError == NULL ||
	PrintDebug == NULL)
	{
		if (PrintError)
			PrintError("Fatal error: Mod could not find all Mod Loader exports. Are your mods and mod loader up to date?\n");
	}
	else
	{
		InitMod();
	}
}
