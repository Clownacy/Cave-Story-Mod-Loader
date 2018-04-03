// Mod loader for Freeware Cave Story
// Copyright © 2017 Clownacy

#include "mod_loader.h"

#include <stdbool.h>
#include <windows.h>

typedef struct Settings Settings;

extern void InitMod(void);

void (*ModLoader_WriteRelativeAddress)(void* const address, const void* const new_destination);
void (*ModLoader_WriteByte)(void* const address, const unsigned char value);
void (*ModLoader_WriteWord)(void* const address, const unsigned short value);
void (*ModLoader_WriteLong)(void* const address, const unsigned int value);
void (*ModLoader_WriteWordBE)(void* const address, const unsigned short value);
void (*ModLoader_WriteLongBE)(void* const address, const unsigned int value);
void (*ModLoader_WriteJump)(void* const address, const void* const new_destination);
void (*ModLoader_WriteCall)(void* const address, const void* const new_destination);
void (*ModLoader_WriteNOPs)(void* const address, const unsigned int count);
void (*ModLoader_FixDoorEnterBug)(void);
void (*ModLoader_PrintErrorMessageBox)(const char* const format, ...);
void (*ModLoader_PrintError)(const char* const format, ...);
void (*ModLoader_PrintDebug)(const char* const format, ...);

static const char* (*GetSettingString_inner)(const char* const filename, const char* const default_string, const Settings* const settings);
static int (*GetSettingInt_inner)(const char* const filename, const int default_int, const Settings* const settings);
static bool (*GetSettingBool_inner)(const char* const filename, const bool default_bool, const Settings* const settings);

const char *ModLoader_path_to_dll;

static const Settings *settings;

const char* ModLoader_GetSettingString(const char* const setting_name, const char* const default_string)
{
	return GetSettingString_inner(setting_name, default_string, settings);
}

int ModLoader_GetSettingInt(const char* const setting_name, const int default_int)
{
	return GetSettingInt_inner(setting_name, default_int, settings);
}

bool ModLoader_GetSettingBool(const char* const setting_name, const bool default_bool)
{
	return GetSettingBool_inner(setting_name, default_bool, settings);
}

__declspec(dllexport) void ModEntry(const HMODULE mod_loader_hmodule, const Settings* const settings_p, const char* const path_to_dll)
{
	settings = settings_p;
	ModLoader_path_to_dll = path_to_dll;

	ModLoader_WriteRelativeAddress = (void (*)(void* const, const void* const))GetProcAddress(mod_loader_hmodule, "WriteRelativeAddress");
	ModLoader_WriteByte = (void (*)(void* const, const unsigned char))GetProcAddress(mod_loader_hmodule, "WriteByte");
	ModLoader_WriteWord = (void (*)(void* const, const unsigned short))GetProcAddress(mod_loader_hmodule, "WriteWord");
	ModLoader_WriteLong = (void (*)(void* const, const unsigned int))GetProcAddress(mod_loader_hmodule, "WriteLong");
	ModLoader_WriteWordBE = (void (*)(void* const, const unsigned short))GetProcAddress(mod_loader_hmodule, "WriteWordBE");
	ModLoader_WriteLongBE = (void (*)(void* const, const unsigned int))GetProcAddress(mod_loader_hmodule, "WriteLongBE");
	ModLoader_WriteJump = (void (*)(void* const, const void* const))GetProcAddress(mod_loader_hmodule, "WriteJump");
	ModLoader_WriteCall = (void (*)(void* const, const void* const))GetProcAddress(mod_loader_hmodule, "WriteCall");
	ModLoader_WriteNOPs = (void (*)(void* const, const unsigned int))GetProcAddress(mod_loader_hmodule, "WriteNOPs");
	ModLoader_FixDoorEnterBug = (void (*)(void))GetProcAddress(mod_loader_hmodule, "FixDoorEnterBug");

	GetSettingString_inner = (const char* (*)(const char* const, const char* const, const Settings* const))GetProcAddress(mod_loader_hmodule, "GetSettingString");
	GetSettingInt_inner = (int (*)(const char* const, const int, const Settings* const))GetProcAddress(mod_loader_hmodule, "GetSettingInt");
	GetSettingBool_inner = (bool (*)(const char* const, const bool, const Settings* const))GetProcAddress(mod_loader_hmodule, "GetSettingBool");

	ModLoader_PrintErrorMessageBox = (void (*)(const char* const format, ...))GetProcAddress(mod_loader_hmodule, "PrintMessageBoxError");
	ModLoader_PrintError = (void (*)(const char* const format, ...))GetProcAddress(mod_loader_hmodule, "PrintError");
	ModLoader_PrintDebug = (void (*)(const char* const format, ...))GetProcAddress(mod_loader_hmodule, "PrintDebug");

	if (ModLoader_WriteRelativeAddress == NULL ||
	ModLoader_WriteByte == NULL ||
	ModLoader_WriteWord == NULL ||
	ModLoader_WriteLong == NULL ||
	ModLoader_WriteWordBE == NULL ||
	ModLoader_WriteLongBE == NULL ||
	ModLoader_WriteJump == NULL ||
	ModLoader_WriteCall == NULL ||
	ModLoader_WriteNOPs == NULL ||
	ModLoader_FixDoorEnterBug == NULL ||
	GetSettingString_inner == NULL ||
	GetSettingInt_inner == NULL ||
	GetSettingBool_inner == NULL ||
	ModLoader_PrintErrorMessageBox == NULL ||
	ModLoader_PrintError == NULL ||
	ModLoader_PrintDebug == NULL)
	{
		if (ModLoader_PrintErrorMessageBox)
			ModLoader_PrintErrorMessageBox("Fatal error: Mod could not find all Mod Loader exports. Are your mods and mod loader up to date?\n");
	}
	else
	{
		InitMod();
	}
}
