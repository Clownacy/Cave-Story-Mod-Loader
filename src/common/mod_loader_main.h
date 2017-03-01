#pragma once

#include <windows.h>

#include "mod_loader.h"

void InitMod(void);

void *settings;
HMODULE mod_loader_hmodule;

const char* const GetSetting(const char* const setting_name)
{
	static char* (*GetSetting_inner)(const char* const filename, void* settings);
	if (GetSetting_inner == NULL)
		GetSetting_inner = (char* (*)(const char* const setting_name, void* settings))GetProcAddress(mod_loader_hmodule, "GetSetting");

	return GetSetting_inner(setting_name, settings);
}

__declspec(dllexport) void ModEntry(HMODULE mod_loader_hmodule_p, void *settings_p)
{
	WriteRelativeAddress = (void (*)(const int, const void* const))GetProcAddress(mod_loader_hmodule_p, "WriteRelativeAddress");
	WriteByte = (void (*)(const int, const char))GetProcAddress(mod_loader_hmodule_p, "WriteByte");
	WriteWord = (void (*)(const int, const short))GetProcAddress(mod_loader_hmodule_p, "WriteWord");
	WriteLong = (void (*)(const int, const int))GetProcAddress(mod_loader_hmodule_p, "WriteLong");
	WriteJump = (void (*)(const int, const void* const))GetProcAddress(mod_loader_hmodule_p, "WriteJump");
	WriteCall = (void (*)(const int, const void* const))GetProcAddress(mod_loader_hmodule_p, "WriteCall");
	FixDoorEnterBug = (void (*)(void))GetProcAddress(mod_loader_hmodule_p, "FixDoorEnterBug");

	settings = settings_p;
	mod_loader_hmodule = mod_loader_hmodule_p;

	InitMod();
}
